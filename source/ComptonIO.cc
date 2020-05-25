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
  , t(nullptr)
  , outfile(nullptr)
{
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
  t->Branch("tru", &fTruthHit);
  return 0;
}

int ComptonIO::process_event(PHCompositeNode *topNode)
{
  ostringstream nodename;
  fTruthHit.clear();
  comptonTruth_t truHit;
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

	truHit.fDetNr = oDetID;
	truHit.fE = primary_particle->get_e();

	truHit.fX = hit_iter->second->get_x(0);
	truHit.fY = hit_iter->second->get_y(0);
	truHit.fZ = hit_iter->second->get_z(0);

	truHit.fpX = hit_iter->second->get_px(0);
	truHit.fpY = hit_iter->second->get_py(0);
	truHit.fpZ = hit_iter->second->get_pz(0);

	truHit.fTime = hit_iter->second->get_t(0);
	truHit.fPDGid = primary_particle->get_pid();
	truHit.fTrackID = hit_iter->second->get_trkid();
	truHit.fTrackID = primary_particle->get_parent_id();
	
	if(Verbosity()>3){
	  cout<<"\tHit: "<<truHit.fPDGid<<" "<<truHit.fE<<endl;
	}

	fTruthHit.push_back(truHit);
      }
    }
  }
  if(Verbosity()>2)
    cout<<"\tFilling "<<fTruthHit.size()<<" hits"<<endl;
  
  if(fTruthHit.size()>0){
    if(Verbosity()>2)
      cout<<"\tFilling "<<fTruthHit.size()<<" events"<<endl;
    t->Fill();
  }
  return 0;
}

int ComptonIO::End(PHCompositeNode *topNode)
{
  if(Verbosity()>3){
    cout<<__PRETTY_FUNCTION__<<" : "<<__LINE__<<endl;
    cout<<"\t writing out truth tree with "<<t->GetEntries()<<" entries"<<endl;
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
