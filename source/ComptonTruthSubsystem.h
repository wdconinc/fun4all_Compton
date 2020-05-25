// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef COMPTONTRUTHSUBSYSTEM_H
#define COMPTONTRUTHSUBSYSTEM_H

#include <g4detectors/PHG4DetectorSubsystem.h>

class PHCompositeNode;
class PHG4Detector;
class ComptonTruthDetector;
class PHG4SteppingAction;

/**
   * \brief Detector Subsystem module
   *
   * The detector is constructed and registered via ComptonTruthDetector
   *
   *
   * \see ComptonTruthDetector
   * \see ComptonTruthSubsystem
   *
   */
class ComptonTruthSubsystem : public PHG4DetectorSubsystem
{
 public:
  //! constructor
  ComptonTruthSubsystem(const std::string& name = "ComptonTruth");

  //! destructor
  virtual ~ComptonTruthSubsystem() {}

  /*!
  creates relevant hit nodes that will be populated by the stepping action and stored in the output DST
  */
  int InitRunSubsystem(PHCompositeNode*) override;

  //! event processing
  /*!
  get all relevant nodes from top nodes (namely hit list)
  and pass that to the stepping action
  */
  int process_event(PHCompositeNode*) override;

  //! accessors (reimplemented)
  PHG4Detector* GetDetector() const override;

  PHG4SteppingAction* GetSteppingAction() const override { return m_SteppingAction; }
  //! Print info (from SubsysReco)
  void Print(const std::string& what = "ALL") const override;

  void SetTrackingLevel(int val){m_TrackLevel=val;}
 protected:
  // \brief Set default parameter values
  void SetDefaultParameters() override;

 private:
  //! detector construction
  /*! derives from PHG4Detector */
  ComptonTruthDetector  *m_Detector;

  //! particle tracking "stepping" action
  /*! derives from PHG4SteppingActions */
  PHG4SteppingAction *m_SteppingAction;
  int m_TrackLevel;
};

#endif // COMPTONTRUTHSUBSYSTEM_H
