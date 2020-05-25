#include "ComptonTruthDetector.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4Detector.h>

#include <Geant4/G4Box.hh>
#include <Geant4/G4Color.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4VisAttributes.hh>

#include <cmath>
#include <iostream>

class G4VSolid;
class PHCompositeNode;

using namespace std;

//____________________________________________________________________________..
ComptonTruthDetector::ComptonTruthDetector(PHG4Subsystem *subsys,
					   PHCompositeNode *Node,
					   PHParameters *parameters,
					   const std::string &dnam)
  : PHG4Detector(subsys, Node, dnam)
  , m_Params(parameters)
{
}

//_______________________________________________________________
int ComptonTruthDetector::IsInDetector(G4VPhysicalVolume *volume) const
{
  set<G4VPhysicalVolume *>::const_iterator iter = m_PhysicalVolumesSet.find(volume);
  if (iter != m_PhysicalVolumesSet.end())
    {
      return 1;
    }
  return 0;
}

//_______________________________________________________________
void ComptonTruthDetector::ConstructMe(G4LogicalVolume *logicWorld)
{

  // Do not forget to multiply the parameters with their respective CLHEP/G4 unit !
  double xdim = m_Params->get_double_param("size_x") * cm;
  double ydim = m_Params->get_double_param("size_y") * cm;
  double zdim = m_Params->get_double_param("size_z") * cm;
  G4VSolid *solidbox = new G4Box("ComptonTruthSolid", xdim / 2., ydim / 2., zdim / 2.);
  G4LogicalVolume *logical = new G4LogicalVolume(solidbox, G4Material::GetMaterial(m_Params->get_string_param("material")), "ComptonTruthLogical");

  G4VisAttributes *vis = new G4VisAttributes(G4Color(G4Colour::Magenta())); 
  vis->SetForceSolid(true);
  logical->SetVisAttributes(vis);
  G4RotationMatrix *rotm = new G4RotationMatrix();
  rotm->rotateX(m_Params->get_double_param("rot_x") * deg);
  rotm->rotateY(m_Params->get_double_param("rot_y") * deg);
  rotm->rotateZ(m_Params->get_double_param("rot_z") * deg);

  G4VPhysicalVolume *phy = new G4PVPlacement(
					     rotm,
					     G4ThreeVector(m_Params->get_double_param("place_x") * cm,
							   m_Params->get_double_param("place_y") * cm,
							   m_Params->get_double_param("place_z") * cm),
					     logical, "ComptonTruth", logicWorld, 0, false, OverlapCheck());
  // add it to the list of placed volumes so the IsInDetector method
  // picks them up
  m_PhysicalVolumesSet.insert(phy);

  return;
}

//_______________________________________________________________
void ComptonTruthDetector::Print(const std::string &what) const
{
  cout << "ComptonTruth Detector:" << endl;
  if (what == "ALL" || what == "VOLUME")
    {
      cout << "Version 0.1" << endl;
      cout << "Parameters:" << endl;
      m_Params->Print();
    }
  return;
}
