#ifndef COMPTON_IO_H
#define COMPTON_IO_H

#include <fun4all/SubsysReco.h>

#include <map>
#include <set>
#include <string>
#include <vector>

// Forward declerations
class PHCompositeNode;
class TFile;
class TTree;

struct comptonTruth_t{
  int fDetNr;
  double fE,fX,fY,fZ,fpX,fpY,fpZ,fTime;
  int fPDGid,fTrackID,fParentID;
};

class ComptonIO : public SubsysReco
{
 public:
  //! constructor
  ComptonIO(const std::string &name = "ComptonIO", const std::string &filename = "o_ComptonOut.root");

  //! destructor
  virtual ~ComptonIO();

  //! full initialization
  int Init(PHCompositeNode *);

  //! event processing method
  int process_event(PHCompositeNode *);

  //! end of run method
  int End(PHCompositeNode *);

  void AddNode(const std::string &name, const int detid = 0,  const int oDetid = 0);

 protected:
  std::string _filename;
  std::set<std::string> _node_postfix;
  std::map<std::string, int> _detid;
  std::map<std::string, int> oDetid;

  int evNr;
  TTree *t;
  TFile *outfile;

  void resetValues();
  int fDetNr;
  double fE,fX,fY,fZ,fpX,fpY,fpZ,fTime;
  int fPDGid,fTrackID,fParentID;

};

#endif
