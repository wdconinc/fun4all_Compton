#include "ComptonIO.h"

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/SubsysReco.h>           // for SubsysReco

#include <phool/getClass.h>

#include <TFile.h>
#include <TH1.h>
#include <TNtuple.h>

#include <sstream>
#include <utility>                        // for pair

using namespace std;

ComptonIO::ComptonIO(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , _filename(filename)
  , evNr(-1)
  , t(nullptr)
  , outfile(nullptr)
{
  resetValues();
}

void ComptonIO::resetValues(){
  fDetNr = std::numeric_limits<int>::quiet_NaN();
  fPDGid = std::numeric_limits<int>::quiet_NaN();
  fTrackID = std::numeric_limits<int>::quiet_NaN();
  fParentID = std::numeric_limits<int>::quiet_NaN();
  fE = std::numeric_limits<double>::quiet_NaN();
  fTime = std::numeric_limits<double>::quiet_NaN();
  fX = std::numeric_limits<double>::quiet_NaN();
  fY = std::numeric_limits<double>::quiet_NaN();
  fZ = std::numeric_limits<double>::quiet_NaN();
  fpX = std::numeric_limits<double>::quiet_NaN();
  fpY = std::numeric_limits<double>::quiet_NaN();
  fpZ = std::numeric_limits<double>::quiet_NaN();
}


ComptonIO::~ComptonIO()
{
  if(t){
    delete t;
    t = nullptr;
  }
  if(outfile){
    delete outfile;
    outfile = nullptr;
  }
}

int ComptonIO::Init(PHCompositeNode *)
{
  outfile = new TFile(_filename.c_str(), "RECREATE");
  t = new TTree("t","Fun4all truth hit info");
  t->Branch("evNr", &evNr);
  t->Branch("detNr", &fDetNr);
  t->Branch("pdgID", &fPDGid);
  t->Branch("trackID", &fTrackID);
  t->Branch("parentID", &fParentID);
  t->Branch("E", &fE);
  t->Branch("x", &fX);
  t->Branch("y", &fY);
  t->Branch("z", &fZ);
  t->Branch("pX", &fpX);
  t->Branch("pY", &fpY);
  t->Branch("pZ", &fpZ);
  t->Branch("time", &fTime);

  return 0;
}

int ComptonIO::process_event(PHCompositeNode *topNode)
{
  evNr++;
  resetValues();
  ostringstream nodename;
  auto *fTruthContainer = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  assert(fTruthContainer);

  set<string>::const_iterator iter;
  for (iter = _node_postfix.begin(); iter != _node_postfix.end(); ++iter)
  {
    int detID = (_detid.find(*iter))->second;
    int oDetID = (oDetid.find(*iter))->second;
    nodename.str("");
    nodename << "G4HIT_" << *iter<< "_"<<detID;
    if(Verbosity()>2){
      cout<<__PRETTY_FUNCTION__<<" : " <<__LINE__<<endl;
      cout<<"\tLooking for: "<<nodename.str()<<endl;
    }

    PHG4HitContainer *hits = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());
    if (hits){
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      if(Verbosity()>2)
	cout<<"\tfound node with ?FIXME? hits"<<endl;
      for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++){
	PHG4Particle *hit_particle = fTruthContainer->GetParticle(hit_iter->second->get_trkid());
	assert(hit_particle);
	PHG4Particle *primary_particle = fTruthContainer->GetParticle(hit_particle->get_primary_id());
	assert(primary_particle);

	fDetNr = oDetID;
	fE = primary_particle->get_e();

	fX = hit_iter->second->get_x(0);
	fY = hit_iter->second->get_y(0);
	fZ = hit_iter->second->get_z(0);

	fpX = hit_iter->second->get_px(0);
	fpY = hit_iter->second->get_py(0);
	fpZ = hit_iter->second->get_pz(0);

	fTime = hit_iter->second->get_t(0);
	fPDGid = primary_particle->get_pid();
	fTrackID = hit_iter->second->get_trkid();
	fParentID = primary_particle->get_parent_id();
	
	if(Verbosity()>3){
	  cout<<"\tHit for : "<<fPDGid<<" with E "<<fE<<" tid "<<fTrackID<<endl;
	}
	t->Fill();
      }
    }
  }
  return 0;
}

int ComptonIO::End(PHCompositeNode *topNode)
{
  if(Verbosity()>3){
    cout<<__PRETTY_FUNCTION__<<" : "<<__LINE__<<endl;
    cout<<"\t writing out truth tree with "<<t->GetEntries()<<" entries"<<endl;
    // cout<<"Tree print:"<<endl;
    // t->Print();
  }
  outfile->cd();
  t->Write("t",TObject::kOverwrite);
  outfile->Write();
  outfile->Close();
  delete outfile;

  return 0;
}

void ComptonIO::AddNode(const std::string &name, const int detid, const int oDetID)
{
  _node_postfix.insert(name);
  _detid[name] = detid;
  oDetid[name] = oDetID;
  return;
}
