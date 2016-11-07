#ifndef _ALPHAMESSENGER_HH
#define _ALPHAMESSENGER_HH

//---------------------------------------------------------------------------//

#include "globals.hh"
#include "G4UImessenger.hh"

class AlphaDetector;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;


//---------------------------------------------------------------------------//

class AlphaMessenger : public G4UImessenger
{
public:

  //default constructor
  AlphaMessenger(AlphaDetector*);

  //destructor
  ~AlphaMessenger();

  void SetNewValue(G4UIcommand*, G4String);

  //protected members
protected:


private:
  G4UIdirectory         *fAlphaDirectory;
  AlphaDetector		*fAlphaDet;

  G4UIcmdWithAString  *fSampleMaterialCmd;
  G4UIcmdWithAString  *fDetMaterialCmd;
  G4UIcmdWithAString  *fChamberMaterialCmd;
  G4UIcmdWithAString  *fDeadMaterialCmd;
  G4UIcmdWithADoubleAndUnit *fSampleRadiusCmd;
  G4UIcmdWithADoubleAndUnit *fSampleHeightCmd;
  G4UIcmdWithADoubleAndUnit *fDetRadiusCmd;
  G4UIcmdWithADoubleAndUnit *fDetHeightCmd;
  G4UIcmdWithADoubleAndUnit *fSampleLengthCmd;
  G4UIcmdWithADoubleAndUnit *fSampleWidthCmd;
  G4UIcmdWithADoubleAndUnit *fGrooveCmd;
  G4UIcmdWithADoubleAndUnit *fSpotCmd;
  G4UIcmdWithAnInteger *fSlotCmd;
  G4UIcmdWithADoubleAndUnit *fDeadThickCmd;
  G4UIcmdWithADoubleAndUnit *fDetBoundThickCmd;

};
#endif
