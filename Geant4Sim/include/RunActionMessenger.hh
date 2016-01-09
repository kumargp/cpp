// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#ifndef RUNACTIONMESSENGER_HH
#define RUNACTIONMESSENGER_HH

#include "globals.hh"
#include "G4UImessenger.hh"

class RunAction;
class G4UIdirectory;
class G4UIcmdWithAString;

//..............................................................................

class RunActionMessenger: public G4UImessenger
{
public:
  RunActionMessenger(RunAction*);
  virtual ~RunActionMessenger();
    
  virtual void SetNewValue(G4UIcommand*, G4String);
    
private:
	RunActionMessenger(const RunActionMessenger& );
	RunActionMessenger& operator=(const RunActionMessenger& );	
	
  RunAction*            fRun;
    
  G4UIdirectory*        fRunDir;
  G4UIcmdWithAString*   fSeedCmd;

};

//.............................................................................

#endif