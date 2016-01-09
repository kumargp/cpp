// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


// DetectorMessenger.cc


#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "Analysis.hh"

//----------------------------------------------------------------------------//


DetectorMessenger::DetectorMessenger(DetectorConstruction* det)
: G4UImessenger(),
  fDetectorConstruction(det)
{
	G4cout << "constructed DetMessenger" << G4endl;
	
    fSimDirectory = new G4UIdirectory("/Sim/");
    fSimDirectory->SetGuidance("UI commands specific to this simulation.");
    
    fDetDirectory = new G4UIdirectory("/Sim/det/");
    fDetDirectory->SetGuidance("UI commands specific to this detector.");
    
    fSetDetCmd = new G4UIcmdWithAString("/Sim/det/detector", this);
    fSetDetCmd->SetGuidance( "Select detector type" );
    fSetDetCmd->SetParameterName( "detName", false );
	//fSetDetCmd->SetDefaultValue("SNSDet");
	G4String detAvailable = "CosIDet SNSDet SNSTargetDetector SimpleSNSDet";
    fSetDetCmd->SetCandidates(detAvailable);
    fSetDetCmd->AvailableForStates( G4State_PreInit, G4State_Idle );
	if(fSetDetCmd) G4cout << "in messenger constructor.." <<G4endl;
	
    fWorldHLenCmd = new G4UIcmdWithADoubleAndUnit("/Sim/det/setWorldHalfLength", this);
    fWorldHLenCmd->SetGuidance("Half length of world");
    fWorldHLenCmd->SetParameterName("length", false);
    fWorldHLenCmd->SetUnitCategory("Length");
    fWorldHLenCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetFileNameCmd = new G4UIcmdWithAString("/Sim/FileNamePattern", this);
	fSetFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	
}
//----------------------------------------------------------------------------//


DetectorMessenger::~DetectorMessenger()
{
    delete fSimDirectory;
    delete fDetDirectory;
	delete fSetDetCmd;
	delete fWorldHLenCmd;
	delete fSetFileNameCmd;
}

//----------------------------------------------------------------------------//


void DetectorMessenger::SetNewValue( G4UIcommand *command, G4String newValue )
{
		
	//G4cout << "in SetNewValue of messenger .." <<G4endl;
    if( command == fSetDetCmd)
    { 
		G4cout << "Setting the Detector " <<G4endl;
		fDetectorConstruction->SetDetector( newValue );
    }
    else if (command == fWorldHLenCmd) 
	{
        G4double halfLength = fWorldHLenCmd->GetNewDoubleValue(newValue);
        fDetectorConstruction->SetWorldHalfLen(halfLength);
    }
    else if (command == fSetFileNameCmd)
	{
		Analysis* man = Analysis::GetInstance();
		man->SetFileNamePattern( newValue );
	}

}
