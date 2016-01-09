

#ifndef STACKINGACTIONMESSENGER_HH
#define STACKINGACTIONMESSENGER_HH

#include "globals.hh"
#include "G4UImessenger.hh"

class StackingAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

//..............................................................................

class StackingActionMessenger: public G4UImessenger
{
public:
	StackingActionMessenger(StackingAction*);
	virtual ~StackingActionMessenger();
	
	virtual void SetNewValue(G4UIcommand*, G4String);
	//virtual G4String GetCurrentValue(G4UIcommand * command);
	
private:
	StackingActionMessenger (const StackingActionMessenger&);
	StackingActionMessenger& operator= (const StackingActionMessenger&);	
	
	StackingAction*       			fStackingAction;
	
	G4UIdirectory*        			fStackDir;
	G4UIcmdWithADoubleAndUnit*   	fNeutronCmd;
	G4UIcmdWithADoubleAndUnit*   	fGammaCmd;	
};

//.............................................................................

#endif

