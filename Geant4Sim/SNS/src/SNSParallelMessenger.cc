// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

// SNSParallelMessenger.cc


#include "SNSParallelMessenger.hh"
#include "SNSParallelTargetDetector.hh"

#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "Analysis.hh"

//----------------------------------------------------------------------------//


SNSParallelMessenger::SNSParallelMessenger(SNSParallelTargetDetector* det)
: G4UImessenger(), fSNSParDetector(det) 
{
	G4cout << "constructed SNSParallelMessenger" << G4endl;

	fDetDirectory = new G4UIdirectory("/Sim/parallel/");
	fDetDirectory->SetGuidance("UI commands specific to parallel geometry");
	
	fNumVolCmd = new G4UIcmdWithAnInteger("/Sim/parallel/parallelNum", this);
	fNumVolCmd->SetGuidance( "number of SNS parallel basement volumes" );
	fNumVolCmd->SetParameterName( "numVol", false );
	fNumVolCmd->AvailableForStates( G4State_PreInit, G4State_Idle );
							
	fIsVariableStepCmd =  new G4UIcmdWithABool("/Sim/parallel/IsVariablestep", this);
	fIsVariableStepCmd->SetGuidance("If variable Parallel Step");
	fIsVariableStepCmd->SetParameterName("isVariable", false);
	fIsVariableStepCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
		
}
//----------------------------------------------------------------------------//

SNSParallelMessenger::~SNSParallelMessenger()
{
	delete fDetDirectory;
	delete fNumVolCmd;
	delete fIsVariableStepCmd;
}
//----------------------------------------------------------------------------//

void SNSParallelMessenger::SetNewValue( G4UIcommand *command, G4String newValue )
{
	
	//G4cout << "in SetNewValue of messenger .." <<G4endl;
	if( command == fNumVolCmd)
	{ 
		G4cout << "Setting number of parallel volumes " <<G4endl;
		fSNSParDetector->SetNumVolumes( fNumVolCmd->GetNewIntValue(newValue));
	}
	else if (command == fIsVariableStepCmd) 
	{
		G4cout << " Setting variable steps for parallel volumes"<< G4endl;
		fSNSParDetector->SetIsVariableStep(fIsVariableStepCmd->GetNewBoolValue(newValue));
	}
	
}
