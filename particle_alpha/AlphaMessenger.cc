
#include "globals.hh"
#include "G4ios.hh"
#include "G4ApplicationState.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "io/MGLogger.hh"

//---------------------------------------------------------------------------//
#include "geometry/AlphaDetector.hh"
#include "geometry/AlphaMessenger.hh"

//---------------------------------------------------------------------------//

AlphaMessenger::AlphaMessenger(AlphaDetector *adet):fAlphaDet(adet)
{

  fAlphaDirectory = new G4UIdirectory("/MG/geometry/AlphaDet/");

  fSampleMaterialCmd = new G4UIcmdWithAString("/MG/geometry/AlphaDet/sampleMaterial", this);
  fSampleMaterialCmd->SetGuidance("Set Geant4 name of sample material ");
  fSampleMaterialCmd->AvailableForStates(G4State_PreInit);

  fDetMaterialCmd = new G4UIcmdWithAString("/MG/geometry/AlphaDet/detMaterial", this);
  fDetMaterialCmd->SetGuidance("Set Geant4 name of det material ");
  fDetMaterialCmd->AvailableForStates(G4State_PreInit);


  fChamberMaterialCmd = new G4UIcmdWithAString("/MG/geometry/AlphaDet/ChamberMaterial", this);
  fChamberMaterialCmd->SetGuidance("Set Chamber material.");
  fChamberMaterialCmd->AvailableForStates(G4State_PreInit);

  fDeadMaterialCmd = new G4UIcmdWithAString("/MG/geometry/AlphaDet/deadMaterial", this);
  fDeadMaterialCmd->SetGuidance("Set deadlayer material.");
  fDeadMaterialCmd->AvailableForStates(G4State_PreInit);



  fSampleRadiusCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/sampleRadius",
					     this);
  fSampleRadiusCmd->SetGuidance("Set outer radius of Sample.");
  fSampleRadiusCmd->AvailableForStates(G4State_PreInit);
  fSampleRadiusCmd->SetDefaultUnit("mm");
  fSampleRadiusCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");

  fSampleHeightCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/sampleHeight",
					     this);
  fSampleHeightCmd->SetGuidance("Set height of Sample.");
  fSampleHeightCmd->AvailableForStates(G4State_PreInit);
  fSampleHeightCmd->SetDefaultUnit("mm");
  fSampleHeightCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");


  fSampleLengthCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/sampleLength",
					     this);
  fSampleLengthCmd->SetGuidance("Set length of Sample.");
  fSampleLengthCmd->AvailableForStates(G4State_PreInit);
  fSampleLengthCmd->SetDefaultUnit("mm");
  fSampleLengthCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");

  fSampleWidthCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/sampleWidth",
					     this);
  fSampleWidthCmd->SetGuidance("Set width of Sample.");
  fSampleWidthCmd->AvailableForStates(G4State_PreInit);
  fSampleWidthCmd->SetDefaultUnit("mm");
  fSampleWidthCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");

  fGrooveCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/groove", this);
  fGrooveCmd->SetGuidance("Set groove size on top surface of Sample.");
  fGrooveCmd->AvailableForStates(G4State_PreInit);
  fGrooveCmd->SetDefaultUnit("micrometer");
  fGrooveCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");

  fSpotCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/spot", this);
  fSpotCmd->SetGuidance("Set spot size on top surface of Sample.");
  fSpotCmd->AvailableForStates(G4State_PreInit);
  fSpotCmd->SetDefaultUnit("micrometer");
  fSpotCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");


  fDetRadiusCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/detRadius",
					     this);
  fDetRadiusCmd->SetGuidance("Set outer radius of detector.");
  fDetRadiusCmd->AvailableForStates(G4State_PreInit);
  fDetRadiusCmd->SetDefaultUnit("mm");
  fDetRadiusCmd->SetUnitCandidates("mm cm m micrometer  nanometer angstrom");

  fDetHeightCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/detHeight",
					     this);
  fDetHeightCmd->SetGuidance("Set height of detector.");
  fDetHeightCmd->AvailableForStates(G4State_PreInit);
  fDetHeightCmd->SetDefaultUnit("mm");
  fDetHeightCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");

  fSlotCmd = new G4UIcmdWithAnInteger("/MG/geometry/AlphaDet/slotNum", this);
  fSlotCmd->SetGuidance("Set slot Number.");
  fSlotCmd->AvailableForStates(G4State_PreInit);


  fDeadThickCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/deadThick",
					     this);
  fDeadThickCmd->SetGuidance("Set dead layer thickness of detector.");
  fDeadThickCmd->AvailableForStates(G4State_PreInit);
  fDeadThickCmd->SetDefaultUnit("mm");
  fDeadThickCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");

  fDetBoundThickCmd = new G4UIcmdWithADoubleAndUnit("/MG/geometry/AlphaDet/detBoundary",
					     this);
  fDetBoundThickCmd->SetGuidance("Set dead layer thickness of detector boundary.");
  fDetBoundThickCmd->AvailableForStates(G4State_PreInit);
  fDetBoundThickCmd->SetDefaultUnit("mm");
  fDetBoundThickCmd->SetUnitCandidates("mm cm m micrometer  nanometer  angstrom ");


}

//---------------------------------------------------------------------------//

AlphaMessenger::~AlphaMessenger()
{

  delete fAlphaDirectory;
  delete fSampleMaterialCmd;
  delete fDetMaterialCmd;
  delete fChamberMaterialCmd;
  delete fDeadMaterialCmd;
  delete fSampleRadiusCmd;
  delete fSampleHeightCmd;
  delete fSampleLengthCmd;
  delete fSampleWidthCmd;
  delete fGrooveCmd;
  delete fSpotCmd;
  delete fDetRadiusCmd;
  delete fDetHeightCmd;
  delete fSlotCmd;
  delete fDeadThickCmd;
  delete fDetBoundThickCmd;
}

//---------------------------------------------------------------------------//

void AlphaMessenger::SetNewValue(G4UIcommand *cmd, G4String option)
{
  if(cmd == fSampleMaterialCmd)
    fAlphaDet->SetSampleMaterialName(option);
  else if(cmd == fDetMaterialCmd)
    fAlphaDet->SetDetMaterialName(option);
  else if(cmd == fChamberMaterialCmd)
    fAlphaDet->SetChamberMaterialName(option);
  else if(cmd == fDeadMaterialCmd)
   fAlphaDet->SetDeadLMaterialName(option);
  else if(cmd == fSampleRadiusCmd)
    fAlphaDet->SetSampleRadius(fSampleRadiusCmd->GetNewDoubleValue(option));
  else if(cmd == fDetRadiusCmd)
    fAlphaDet->SetDetRadius(fDetRadiusCmd->GetNewDoubleValue(option));
  else if(cmd == fSampleHeightCmd)
    fAlphaDet->SetSampleHeight(fSampleHeightCmd->GetNewDoubleValue(option));
  else if(cmd == fSampleLengthCmd)
    fAlphaDet->SetSampleLength(fSampleLengthCmd->GetNewDoubleValue(option));
  else if(cmd == fSampleWidthCmd)
    fAlphaDet->SetSampleWidth(fSampleWidthCmd->GetNewDoubleValue(option));
  else if(cmd == fGrooveCmd)
    fAlphaDet->SetGroove(fGrooveCmd->GetNewDoubleValue(option));
  else if(cmd == fSpotCmd)
    fAlphaDet->SetSpot(fSpotCmd->GetNewDoubleValue(option));
  else if(cmd == fDetHeightCmd)
    fAlphaDet->SetDetHeight(fDetHeightCmd->GetNewDoubleValue(option));
  else if(cmd == fSlotCmd)
    fAlphaDet->SetSlot(fSlotCmd->GetNewIntValue(option));
  else if(cmd == fDeadThickCmd)
    fAlphaDet->SetDeadThick(fDeadThickCmd->GetNewDoubleValue(option));
  else if(cmd == fDetBoundThickCmd)
    fAlphaDet->SetBoundThick(fDetBoundThickCmd->GetNewDoubleValue(option));
}
