// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef COMPTONTRUTHSTEPPINGACTION_H
#define COMPTONTRUTHSTEPPINGACTION_H

#include <g4main/PHG4SteppingAction.h>

class ComptonTruthDetector;

class G4Step;
class G4VPhysicalVolume;
class PHCompositeNode;
class PHG4Hit;
class PHG4HitContainer;
class PHParameters;
//class PHG4TruthInfoContainer;//fixme add evAction

class ComptonTruthSteppingAction : public PHG4SteppingAction
{
 public:
  //! constructor
  ComptonTruthSteppingAction(ComptonTruthDetector*, const PHParameters* parameters, int trackLevel);

  //! destructor
  virtual ~ComptonTruthSteppingAction();

  //! stepping action
  virtual bool UserSteppingAction(const G4Step*, bool);

  //! reimplemented from base class
  virtual void SetInterfacePointers(PHCompositeNode*);
  
 private:
  //! pointer to the detector
  ComptonTruthDetector* m_Detector;
  const PHParameters* m_Params;
  //! pointer to hit container
  PHG4HitContainer* m_HitContainer;
  PHG4Hit* m_Hit;
  //PHG4TruthInfoContainer* m_truthContainer;//fixme
  PHG4HitContainer* m_SaveHitContainer;
  G4VPhysicalVolume* m_SaveVolPre;
  G4VPhysicalVolume* m_SaveVolPost;

  int m_SaveTrackId;
  int m_SavePreStepStatus;
  int m_SavePostStepStatus;
  int m_ActiveFlag;
  int m_BlackHoleFlag;
  double m_EdepSum;
  double m_EionSum;

  //0 (default) = all; 1=primary only (trackID 1,2)
  int m_TrackLevel;
};

#endif // COMPTONTRUTHSTEPPINGACTION_H
