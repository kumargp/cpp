//AlphaDetector
#ifndef _ALPHADETECTOR_HH
#define _ALPHADETECTOR_HH

//---------------------------------------------------------------------------//

#include "globals.hh"
#include "geometry/MGGeometryDetector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include <vector>

//---------------------------------------------------------------------------//

using namespace std;
class AlphaMessenger;
class G4Material;
class G4LogicalVolume;
class G4VisAttributes;
class G4VSolid;


class AlphaDetector : public MGGeometryDetector
{
public:
  AlphaDetector( G4String serNum );
  AlphaDetector(const AlphaDetector &);
  ~AlphaDetector();

  void ConstructDetector();
  void Dump();
  void SetDetMaterialName( G4String name ) { fDetMaterialName = name; }
  void SetSampleMaterialName( G4String name ) { fSampleMaterialName = name; }
  void SetChamberMaterialName( G4String name ) { fChamberMaterialName = name; }
  void SetDeadLMaterialName( G4String name ) { fDeadLMaterialName = name; }

  void SetDetHeight ( G4double height ) { fDetHeight = height; }
  void SetDetRadius ( G4double  radius ) { fDetRadius =radius; }
  void SetSampleHeight ( G4double height ) { fSampleHeight = height; }
  void SetSampleRadius ( G4double radius ) { fSampleRadius = radius; }
  void SetSampleLength  ( G4double  length ) { fSampleLength  =  length; }
  void SetSampleWidth  ( G4double width  ) { fSampleWidth  =  width; }
  void SetGroove  ( G4double groove  ) { fGroove  =  groove; }
  void SetSpot  ( G4double spot  ) { fSpot  =  spot; }
  void SetSlot( G4double dist ) { fSlot= dist; }
  void SetDeadThick ( G4double thick ) { fDeadThick = thick; }
  void SetBoundThick ( G4double thick ) { fDetBoundThick = thick; }


protected:
  G4double      fDetHeight;
  G4double      fDetRadius;
  G4double      fSampleHeight;
  G4double      fSampleRadius;
  G4double      fSampleLength;
  G4double      fSampleWidth;
  G4double      fGroove;
  G4double      fSpot;
  G4double	fDeadThick;
  G4double	fDetBoundThick;
  G4double      fSlot;
  G4double	fGap;
  G4double	fVacPressure;

  G4VSolid* 	fChamberBox;
  G4Material*   fDetMaterial;
  G4String      fDetMaterialName;
  G4Material*   fSampleMaterial;
  G4String      fSampleMaterialName;
  G4Material*   fChamberMaterial;
  G4String      fChamberMaterialName;
  G4Material*   fDeadLMaterial;
  G4String      fDeadLMaterialName;

  vector<G4VPhysicalVolume*> fPhysicalVolumeVec;
  vector<G4LogicalVolume*> fLogicalVolumeVec;
  vector<G4VSolid*> fSolidVec;

//private:

  AlphaMessenger   *fMessenger;
  G4VisAttributes* fDetVisAtt;
  G4VisAttributes* fDLayerVisAtt;
  G4VisAttributes* fSampleVisAtt;
  G4VisAttributes* fBoundaryVisAtt;
};
#endif
