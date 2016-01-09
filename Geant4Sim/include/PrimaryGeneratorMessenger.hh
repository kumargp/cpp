
// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef PRIMARYGENERATORMESSENGER_HH
#define PRIMARYGENERATORMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"
#include "PrimaryGeneratorAction.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
//class G4UIcmdWithoutParameter;
//class G4UIcmdWithAnInteger; 
//class G4UIcmdWithADouble;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
//-----------------------------------------------------------------------------

class PrimaryGeneratorMessenger: public G4UImessenger
{
  public:
    PrimaryGeneratorMessenger(PrimaryGeneratorAction*);
   ~PrimaryGeneratorMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    PrimaryGeneratorAction*     fGenAction;
    
    G4UIdirectory*              fGunDir;
    G4UIcmdWithAString*         fParticleCmd;
    G4UIcmdWithADoubleAndUnit*  fPartEnergyCmd;
    G4UIcmdWith3VectorAndUnit*  fPartPosCmd;  
    G4UIcmdWithADoubleAndUnit*  fPartDistrSphereCmd;
    G4UIcmdWith3VectorAndUnit*  fPartDistrBoxCmd;
    G4UIcmdWithAString*         fPartDirectToVolCmd;
    G4UIcmdWith3Vector*         fPartMomentumCmd;
    G4UIcmdWithAString*         fPartConfineCmd;
    G4UIcmdWithoutParameter*    fPartRandomBeamDirCmd;
};

//----------------------------------------------------------------------------

#endif
