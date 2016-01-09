// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

#include "StackingActionMessenger.hh"
#include "StackingAction.hh"

#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

StackingActionMessenger::StackingActionMessenger(StackingAction* stackAction)
:fStackingAction(stackAction)
{
	fStackDir = new G4UIdirectory("/Sim/Kill/");
	
	fNeutronCmd = new G4UIcmdWithADoubleAndUnit("/Sim/Kill/SetNeutronThreshold", this);
	fNeutronCmd->SetGuidance("Kill Neutron Tracks below Threshold");
	fNeutronCmd->SetDefaultUnit("MeV");
	
	fGammaCmd = new G4UIcmdWithADoubleAndUnit("/Sim/Kill/SetGammaThreshold", this);
	fGammaCmd->SetGuidance("Kill Gamma Tracks below Threshold");
	fGammaCmd->SetDefaultUnit("MeV");
	
}

StackingActionMessenger::~StackingActionMessenger()
{
	delete fStackDir;
	delete fNeutronCmd;
	delete fGammaCmd;
}


void StackingActionMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
	if(command == fNeutronCmd)
	{
		fStackingAction->SetNeutronThreshold(fNeutronCmd->GetNewDoubleValue(newValue)); 
	}
	else if( command == fGammaCmd )
	{ 
		fStackingAction->SetGammaThreshold(fGammaCmd->GetNewDoubleValue(newValue)); 
	}
}