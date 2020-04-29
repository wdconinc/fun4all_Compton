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
//#include <g4main/ReadEICFiles.h>
#include <fun4all_compton/ReadEICFilesCompton.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4detectors/BeamLineMagnetSubsystem.h>
#include <g4detectors/PHG4BlockSubsystem.h>
#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libg4histos.so)

#endif

#include <set>

void Fun4All_G4_IP12Compton(
			    int nEvents = -1, 
			    const std::string finNm="../comptonRad/tst.root", 
			    const std::string foutNm="o_tst")
{
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");  
  gSystem->Load("libg4histos");
  gSystem->Load("libg4eval");

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  if(nEvents>0){
    ReadEICFilesCompton *eicfile = new ReadEICFilesCompton();
    eicfile->OpenInputFile(finNm);
    se->registerSubsystem(eicfile);
    
    HepMCNodeReader *hr = new HepMCNodeReader();
    se->registerSubsystem(hr);
  }else{
    PHG4ParticleGun *gun = new PHG4ParticleGun();
    gun->set_name("chargedgeantino");
    gun->set_vtx(0, 0, 0);
    gun->set_mom(0, 0, 10);
    se->registerSubsystem(gun);
  }

  bool verbose = false;

  // make magnet active volume if you want to study the hits
  bool magnet_active=false;
  int absorberactive = 1;

  // if you insert numbers it only displays those magnets, do not comment out the set declaration
  set<int> magnetlist;
  //  magnetlist.insert(1);
  //  magnetlist.insert(3);

  //
  // Geant4 setup
  //
  PHG4Reco* g4Reco = new PHG4Reco();
  // setup of G4: 
  //   no saving of geometry: it takes time and we do not do tracking
  //   so we do not need the geometry
  g4Reco->save_DST_geometry(false);
  g4Reco->set_field(0);
  g4Reco->SetWorldMaterial("G4_Galactic");

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
		  if(verbose){
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

		  if (magnetlist.empty() || magnetlist.find(imagnet) != magnetlist.end())
		    {
		      bl = new BeamLineMagnetSubsystem("BEAMLINEMAGNET",imagnet);
		      bl->set_double_param("field_x",dipole_field_x);
		      bl->set_double_param("field_y",0.);
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

  //Simple flat detector
  auto *dipoleExitDet = new PHG4BlockSubsystem("dExit");
  dipoleExitDet->set_double_param("place_x",0);
  dipoleExitDet->set_double_param("place_y",0);
  dipoleExitDet->set_double_param("place_z",500);
  dipoleExitDet->set_double_param("size_x",100);
  dipoleExitDet->set_double_param("size_y",100);
  dipoleExitDet->set_double_param("size_z",0.1);
  dipoleExitDet->set_string_param("material","G4_Galactic");
  dipoleExitDet->SetActive();
  g4Reco->registerSubsystem(dipoleExitDet);

  auto *genDet = new PHG4BlockSubsystem("gen");
  genDet->set_double_param("place_x",0);
  genDet->set_double_param("place_y",0);
  genDet->set_double_param("place_z",5);
  genDet->set_double_param("size_x",100);
  genDet->set_double_param("size_y",100);
  genDet->set_double_param("size_z",0.1);
  genDet->SetActive();
  genDet->set_string_param("material","G4_Galactic");
  g4Reco->registerSubsystem(genDet);

  if(verbose)
    cout<<"World size: "<<g4Reco->GetWorldSizeX()<<" "<<g4Reco->GetWorldSizeY()<<" "<<g4Reco->GetWorldSizeZ()<<" "<<endl;

  se->registerSubsystem(g4Reco);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  if (nEvents>0){
    Fun4AllOutputManager *out = new Fun4AllDstOutputManager("DSTOUT",foutNm.c_str());
    se->registerOutputManager(out);

    // save a comprehensive  evaluation file
    PHG4DSTReader* ana = new PHG4DSTReader(Form("%s_DSTReader.root",foutNm.c_str()));
    ana->set_save_particle(true);
    ana->set_load_all_particle(false);
    ana->set_load_active_particle(true);
    ana->set_save_vertex(true);
    if (nEvents > 0 && nEvents < 2){
      ana->Verbosity(2);
    }
    ana->AddNode("dExit_0");
    ana->AddNode("gen_0");
    se->registerSubsystem(ana);
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
