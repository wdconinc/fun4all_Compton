#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include "GlobalVariables.C"
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <g4histos/G4HitNtuple.h>
#include <g4eval/PHG4DSTReader.h>
#include <g4main/PHG4ParticleGun.h>
#include <g4main/HepMCNodeReader.h>
#include <g4main/ReadEICFiles.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4detectors/BeamLineMagnetSubsystem.h>
#include <g4detectors/PHG4BlockSubsystem.h>
#include <phool/recoConsts.h>

#include <fun4all_compton/ComptonTruthSubsystem.h>
#include <fun4all_compton/ComptonIO.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libg4histos.so)
R__LOAD_LIBRARY(libCompton.so)

#endif

#include <set>

//bool verbose = true;
bool verbose = false;

void Fun4All_G4_IP12Compton(
			    int nEvents = -1, 
			    const std::string finNm="./milouIn.root", 
			    const std::string foutNm="o_ComptonTst.root",
			    const bool testGun = false,
			    const int eBeam = 18)
{

  if(eBeam !=5 && eBeam !=12 && eBeam !=18){
    cout<<"wrong beam energy! only 5,12,18 supported"<<endl;
    cout<<"\tyou provided "<<eBeam<<endl;
    return;
  }

  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");  
  gSystem->Load("libg4histos");
  gSystem->Load("libg4eval");
  gSystem->Load("libCompton.so");

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  if(nEvents<5 && nEvents>0)
    se->Verbosity(2);
  recoConsts *rc = recoConsts::instance();

  if(!testGun){
    ReadEICFiles *eicfile = new ReadEICFiles();
    if(nEvents<5 && nEvents>0)
      eicfile->Verbosity(2);

    eicfile->set_vertex_distribution_mean(0,0,0,0);//cm, ns
    //eicfile->set_vertex_distribution_width(4.3e-2,3.3e-2,0.9,0);//cm, ns -> electron bunch
    //eicfile->set_vertex_distribution_width(1e-2,1e-2,0.4,0);//cm, ns -> photon bunch
    eicfile->set_vertex_distribution_width(0,0,0,0);//cm, ns
    eicfile->OpenInputFile(finNm);
    se->registerSubsystem(eicfile);
    
    HepMCNodeReader *hr = new HepMCNodeReader();
    if(nEvents<5 && nEvents>0)
      hr->Verbosity(2);
    se->registerSubsystem(hr);
  }else{
    PHG4ParticleGun *gun = new PHG4ParticleGun();
    gun->set_pid(11);
    //gun->set_name("chargedgeantino");//positive charge!!
    //TVector3 gMom(-0.252569,2.76806e-05,-13.147);
    //gun->set_name("geantino");
    //TVector3 gMom(-0.09301,-2.76806e-05,-4.84968);

    // //rotation test
    TVector3 gMom(0,0,-eBeam);
    //gMom.RotateY(0.0183897);
    gMom.RotateY(0.0192);

    gun->set_vtx(0, 0, 0);
    gun->set_mom(gMom.X(), gMom.Y(), gMom.Z());
    if(nEvents<5)
      cout<<"Momentum "<<gMom.X()<<" "<<gMom.Y()<<" "<<gMom.Z()<<endl;
    se->registerSubsystem(gun);
  }

  // make magnet active volume if you want to study the hits
  bool magnet_active=false;
  int absorberactive = 0;

  // if you insert numbers it only displays those magnets, do not comment out the set declaration
  set<int> magnetlist;
  //  magnetlist.insert(1);
  //  magnetlist.insert(3);

  //
  // Geant4 setup
  //
  PHG4Reco* g4Reco = new PHG4Reco();
  if(nEvents<5 && verbose && nEvents>0)
    g4Reco->Verbosity(2);

  // setup of G4: 
  //   no saving of geometry: it takes time and we do not do tracking
  //   so we do not need the geometry
  g4Reco->save_DST_geometry(false);
  g4Reco->set_field(0);
  g4Reco->SetWorldMaterial("G4_Galactic");

  map<int,double> gammaFactor;
  gammaFactor[5] =98.91782353*0.55;//with fudge factor to get a "straight line"
  gammaFactor[12]=153.2428333*0.87;
  gammaFactor[18]=187.6833741*1.05;//with fudge factor to get a "straight line"
  const double gFactor = gammaFactor[eBeam]/187.6833741;

  BeamLineMagnetSubsystem *bl = nullptr;
  std::ifstream infile("ip12_magnetV2.dat");
  if (infile.is_open())
    {
      double biggest_z = 0.;
      int imagnet = 0;
      std::string line;
      while (std::getline(infile, line))
	{
	  //	cout << line << endl;
	  if (! line.compare(0,1,"D") || 
	      ! line.compare(0,1,"Q") ||
	      ! line.compare(0,1,"S"))
	    {
	      std::istringstream iss(line);
	      string magname;
	      double x;
	      double y;
	      double z;
	      double inner_radius_zin;
	      double inner_radius_zout;
	      double outer_magnet_diameter;
	      double length;
	      double angle;
	      double dipole_field_x;
	      double fieldgradient;
	      if (!(iss >> magname >> x >> y >> z 
		    >> inner_radius_zin >> inner_radius_zout
		    >> outer_magnet_diameter >> length
		    >> angle >> dipole_field_x >> fieldgradient))
		{
		  cout << "coud not decode " << line << endl;
		  gSystem->Exit(1);
		}
	      else
		{
		  string magtype;
		  if (inner_radius_zin != inner_radius_zout)
		    {
		      cout << "inner radius at front of magnet " << inner_radius_zin
			   << " not equal radius at back of magnet " << inner_radius_zout
			   << " needs change in code (replace tube by cone for beamline)" << endl;
		      gSystem->Exit(1);
		    }
		  if(verbose  || (nEvents<5 && nEvents>0)){
		    cout << endl << endl << "\tID number "<<imagnet<<endl;
		    cout << "magname: " << magname << endl;
		    cout << "x: " << x << endl;
		    cout << "y: " << y << endl;
		    cout << "z: " << z << endl;
		    cout << "inner_radius_zin: " << inner_radius_zin << endl;
		    cout << "inner_radius_zout: " << inner_radius_zout << endl;
		    cout << "outer_magnet_diameter: " << outer_magnet_diameter << endl;
		    cout << "length: " << length << endl;
		    cout << "angle: " << angle << endl;
		    cout << "dipole_field_x: " << dipole_field_x << endl;
		    cout << "fieldgradient: " << fieldgradient << endl;
		  }
		  if (! magname.compare(0,1,"D"))
		    {
		      magtype = "DIPOLE";
		    }
		  else if (! magname.compare(0,1,"Q"))
		    {
		      magtype = "QUADRUPOLE";
		    }
		  else if (! magname.compare(0,1,"S"))
		    {
		      magtype = "SEXTUPOLE";
		    }
		  else
		    {
		      cout << "cannot decode magnet name " << magname << endl;
		      gSystem->Exit(1);
		    }
		  // convert to our units (cm, deg)
		  x *= 100.;
		  y *= 100.;
		  z *= 100.;
		  length *= 100.;
		  inner_radius_zin *= 100.;
		  outer_magnet_diameter *= 100.;
		  angle = (angle/TMath::Pi()*360.)/1000.; // given in mrad

		  dipole_field_x *= gFactor;
		  fieldgradient *= gFactor;//FIXME xcheck if you need this

		  if (magnetlist.empty() || magnetlist.find(imagnet) != magnetlist.end())
		    {
		      bl = new BeamLineMagnetSubsystem("BEAMLINEMAGNET",imagnet);
		      bl->set_double_param("field_y",dipole_field_x);
		      bl->set_double_param("field_x",0.);
		      bl->set_double_param("fieldgradient",fieldgradient);
		      bl->set_string_param("magtype",magtype);
		      bl->set_double_param("length",length);
		      bl->set_double_param("place_x",x);
		      bl->set_double_param("place_y",y);
		      bl->set_double_param("place_z",z);
		      bl->set_double_param("rot_y",angle);
		      bl->set_double_param("inner_radius",inner_radius_zin);
		      bl->set_double_param("outer_radius", outer_magnet_diameter/2.);
		      bl->SetActive(magnet_active);
		      //bl->BlackHole();
		      if (absorberactive)  
			{
			  bl->SetAbsorberActive();
			}
		      bl->OverlapCheck(overlapcheck);
		      bl->SuperDetector("BEAMLINEMAGNET");
		      g4Reco->registerSubsystem(bl);
		    }
		  imagnet++;
		  if (fabs(z)+length > biggest_z)
		    {
		      biggest_z = fabs(z)+length;
		    }
		}
	    }
	}
      infile.close();
      if (biggest_z*2. > g4Reco->GetWorldSizeZ())
	{
	  g4Reco->SetWorldSizeZ((biggest_z+100.)*2); // leave 1m on both sides
	}
    }
  g4Reco->SetWorldSizeZ(3000*2); //in cm

  //1=primaries only (trackID 1,2); 0=all particles
  const int trackingLevel = 1;
  //Simple flat detector
  auto *genDet = new ComptonTruthSubsystem("gen");
  genDet->set_double_param("place_x",0);
  genDet->set_double_param("place_y",0);
  genDet->set_double_param("place_z",-5);
  genDet->set_double_param("size_x",100);
  genDet->set_double_param("size_y",100);
  genDet->set_double_param("size_z",0.1);
  genDet->set_string_param("material","G4_Galactic");
  genDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    genDet->Verbosity(4);
  genDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(genDet);

  auto *dipoleExitDet = new ComptonTruthSubsystem("dExit");
  dipoleExitDet->set_double_param("place_x",0);
  dipoleExitDet->set_double_param("place_y",0);
  dipoleExitDet->set_double_param("place_z",-500);
  dipoleExitDet->set_double_param("size_x",100);
  dipoleExitDet->set_double_param("size_y",100);
  dipoleExitDet->set_double_param("size_z",0.1);
  dipoleExitDet->set_string_param("material","G4_Galactic");
  dipoleExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    dipoleExitDet->Verbosity(4);
  dipoleExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(dipoleExitDet);

  auto *qf11ExitDet = new ComptonTruthSubsystem("qf11Exit");
  qf11ExitDet->set_double_param("place_x",0);
  qf11ExitDet->set_double_param("place_y",0);
  qf11ExitDet->set_double_param("place_z",-650);
  qf11ExitDet->set_double_param("size_x",100);
  qf11ExitDet->set_double_param("size_y",100);
  qf11ExitDet->set_double_param("size_z",0.1);
  qf11ExitDet->set_string_param("material","G4_Galactic");
  qf11ExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qf11ExitDet->Verbosity(4);
  qf11ExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qf11ExitDet);

  auto *qd10EntranceDet = new ComptonTruthSubsystem("qd10Enter");
  qd10EntranceDet->set_double_param("place_x",0);
  qd10EntranceDet->set_double_param("place_y",0);
  qd10EntranceDet->set_double_param("place_z",-1600);
  qd10EntranceDet->set_double_param("size_x",100);
  qd10EntranceDet->set_double_param("size_y",100);
  qd10EntranceDet->set_double_param("size_z",0.1);
  qd10EntranceDet->set_string_param("material","G4_Galactic");
  qd10EntranceDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qd10EntranceDet->Verbosity(4);
  qd10EntranceDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qd10EntranceDet);

  auto *qd10ExitDet = new ComptonTruthSubsystem("qd10Exit");
  qd10ExitDet->set_double_param("place_x",0);
  qd10ExitDet->set_double_param("place_y",0);
  qd10ExitDet->set_double_param("place_z",-1670);
  qd10ExitDet->set_double_param("size_x",100);
  qd10ExitDet->set_double_param("size_y",100);
  qd10ExitDet->set_double_param("size_z",0.1);
  qd10ExitDet->set_string_param("material","G4_Galactic");
  qd10ExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qd10ExitDet->Verbosity(4);
  qd10ExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qd10ExitDet);

  auto *det25m = new ComptonTruthSubsystem("det25m");
  det25m->set_double_param("place_x",0);
  det25m->set_double_param("place_y",0);
  det25m->set_double_param("place_z",-2500);
  det25m->set_double_param("size_x",100);
  det25m->set_double_param("size_y",100);
  det25m->set_double_param("size_z",0.1);
  det25m->set_string_param("material","G4_Galactic");
  det25m->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    det25m->Verbosity(4);
  det25m->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(det25m);

  auto *qf9EntranceDet = new ComptonTruthSubsystem("qf9Enter");
  qf9EntranceDet->set_double_param("place_x",0);
  qf9EntranceDet->set_double_param("place_y",0);
  qf9EntranceDet->set_double_param("place_z",-2700);
  qf9EntranceDet->set_double_param("size_x",100);
  qf9EntranceDet->set_double_param("size_y",100);
  qf9EntranceDet->set_double_param("size_z",0.1);
  qf9EntranceDet->set_string_param("material","G4_Galactic");
  qf9EntranceDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qf9EntranceDet->Verbosity(4);
  qf9EntranceDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qf9EntranceDet);

  auto *qf9ExitDet = new ComptonTruthSubsystem("qf9Exit");
  qf9ExitDet->set_double_param("place_x",0);
  qf9ExitDet->set_double_param("place_y",0);
  qf9ExitDet->set_double_param("place_z",-2765);
  qf9ExitDet->set_double_param("size_x",100);
  qf9ExitDet->set_double_param("size_y",100);
  qf9ExitDet->set_double_param("size_z",0.1);
  qf9ExitDet->set_string_param("material","G4_Galactic");
  qf9ExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qf9ExitDet->Verbosity(4);
  qf9ExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qf9ExitDet);

  if(verbose || nEvents<5)
    cout<<"World size: "<<g4Reco->GetWorldSizeX()<<" "<<g4Reco->GetWorldSizeY()<<" "<<g4Reco->GetWorldSizeZ()<<" "<<endl;

  se->registerSubsystem(g4Reco);

  if (nEvents>0){
    PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
    g4Reco->registerSubsystem(truth);

    ComptonIO *cmtOut = new ComptonIO("ComptonIO",foutNm);
    if(verbose || (nEvents<5 && nEvents>0))
      cmtOut->Verbosity(4);
    cmtOut->AddNode("gen",0,0);
    cmtOut->AddNode("dExit",0,1);
    cmtOut->AddNode("qf11Exit",0,2);
    cmtOut->AddNode("qd10Enter",0,3);
    cmtOut->AddNode("qd10Exit",0,4);
    cmtOut->AddNode("det25m",0,5);
    cmtOut->AddNode("qf9Enter",0,6);
    cmtOut->AddNode("qf9Exit",0,7);
    se->registerSubsystem(cmtOut);
  }

  // this (dummy) input manager just drives the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager( "Dummy");
  se->registerInputManager( in );

  if (nEvents <= 0)
    {
      return 0;
    }
  se->run(nEvents);
  se->End();
  delete se;
  gSystem->Exit(0);
}
