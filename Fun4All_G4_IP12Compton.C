#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include "GlobalVariables.C"
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <g4detectors/PHG4BlockSubsystem.h>
#include <g4histos/G4HitNtuple.h>
#include <g4main/PHG4ParticleGun.h>
#include <g4main/PHG4Reco.h>
#include <phool/recoConsts.h>
#include <g4detectors/BeamLineMagnetSubsystem.h>
#include <g4main/PHG4Reco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libg4histos.so)

#endif

#include <set>

void Fun4All_G4_IP12Compton(int nEvents = -1)
{
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  bool verbose = false;

  // this adds a particle gun in front of every magnet which shoots charged geantinos into them
  bool add_pgun = false;

  // make magnet active volume if you want to study the hits
  bool magnet_active=false;
  int absorberactive = 1;

  // if you insert numbers it only displays those magnets, do not comment out the set declaration
  set<int> magnetlist;
  //  magnetlist.insert(1);
  //  magnetlist.insert(3);

  // ParticleGun shoots straight into the magnets, if you want an angle, set this here
  double px = 0.;
  double py = 0.;
  double pz = 1.;
  PHG4ParticleGun *gun = new PHG4ParticleGun();
  gun->set_name("chargedgeantino");
  gun->set_vtx(20, 0, -950); //cm
  gun->set_mom(0, 0, -10);
  // gun->AddParticle("chargedgeantino",0,2,pz);
  // gun->AddParticle("chargedgeantino",0,3,pz);
  se->registerSubsystem(gun);

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
		      if (add_pgun)
			{
			  PHG4ParticleGun *gun = new PHG4ParticleGun();
			  //   gun->set_name("pi-");
			  gun->set_name("chargedgeantino");
			  gun->set_vtx(x, y, z-length/2.-0.1); 
			  gun->set_mom(px, py, pz);
			  se->registerSubsystem(gun);
			}
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
  dipoleExitDet->set_double_param("place_x",20);
  dipoleExitDet->set_double_param("place_y",0);
  dipoleExitDet->set_double_param("place_z",-1000);
  dipoleExitDet->set_double_param("size_x",100);
  dipoleExitDet->set_double_param("size_y",100);
  dipoleExitDet->set_double_param("size_z",0.1);
  dipoleExitDet->SetActive(1);
  dipoleExitDet->set_string_param("material","G4_Galactic");
  g4Reco->registerSubsystem(dipoleExitDet);
  //FIXME do i need to set a maximum width for the world here?!
  //FIXME why is vis.mac axis not drawn?

  if(verbose)
    cout<<"World size: "<<g4Reco->GetWorldSizeX()<<" "<<g4Reco->GetWorldSizeY()<<" "<<g4Reco->GetWorldSizeZ()<<" "<<endl;

  se->registerSubsystem(g4Reco);

  G4HitNtuple *hits = new G4HitNtuple("hits","o_tst.root");
  hits->AddNode("dExit",0);
  se->registerSubsystem(hits);

  // this (dummy) input manager just drives the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager( "Dummy");
  se->registerInputManager( in );

  // events = 0 => run forever, default is -1, do not run event loop
  if (nEvents <= 0)
    {
      return 0;
    }
  se->run(nEvents);
  se->End();
  delete se;
  gSystem->Exit(0);
}
