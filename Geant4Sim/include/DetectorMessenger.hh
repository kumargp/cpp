// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#ifndef DETECTORMESSENGER_HH
#define DETECTORMESSENGER_HH

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;


//----------------------------------------------------------------------------//

class DetectorMessenger : public G4UImessenger 
{
  public:
    DetectorMessenger( DetectorConstruction* );
    virtual ~DetectorMessenger();

    virtual void SetNewValue( G4UIcommand*, G4String );

 private:
	DetectorMessenger(const DetectorMessenger&);
	DetectorMessenger& operator=(const DetectorMessenger&);
	
    DetectorConstruction*       fDetectorConstruction;
    G4UIdirectory*              fSimDirectory;
    G4UIdirectory*              fDetDirectory;
    G4UIcmdWithAString*         fSetDetCmd;
    G4UIcmdWithADoubleAndUnit*  fWorldHLenCmd;
	G4UIcmdWithAString*			fSetFileNameCmd;

};

#endif