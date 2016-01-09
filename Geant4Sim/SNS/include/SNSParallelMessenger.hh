
#ifndef SNSPARALLELMESSENGER_HH
#define SNSPARALLELMESSENGER_HH

#include "globals.hh"
#include "G4UImessenger.hh"

class SNSParallelTargetDetector;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;


//----------------------------------------------------------------------------//

class SNSParallelMessenger : public G4UImessenger 
{
public:
	SNSParallelMessenger( SNSParallelTargetDetector* );
	virtual ~SNSParallelMessenger();
	
	virtual void SetNewValue( G4UIcommand*, G4String );
	
private:
	SNSParallelMessenger(const SNSParallelMessenger& );
	SNSParallelMessenger& operator=(const SNSParallelMessenger& );
	
	SNSParallelTargetDetector*  fSNSParDetector;
	G4UIdirectory*              fDetDirectory;
	G4UIcmdWithAnInteger*       fNumVolCmd;
	G4UIcmdWithABool*  			 fIsVariableStepCmd;

};

//----------------------------------------------------------------------------

#endif
