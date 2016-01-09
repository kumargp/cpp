// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "globals.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
//#include "G4UIcmdWithAnInteger.hh"
//#include "G4UIcmdWithADouble.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithoutParameter.hh"
//---------------------------------------------------------------------------

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(
                  PrimaryGeneratorAction* generatorAction)
:G4UImessenger(),fGenAction(generatorAction)
 {
  fGunDir = new G4UIdirectory("/Sim/particle/");
  fGunDir->SetGuidance("gun control");
  
  fParticleCmd = new G4UIcmdWithAString("/Sim/particle/name",this);
  fParticleCmd->SetGuidance("Set particle to be generated.");
  fParticleCmd->SetGuidance(" (neutron is default)");
  
  fPartEnergyCmd = new G4UIcmdWithADoubleAndUnit("/Sim/particle/energy",this);
  fPartEnergyCmd->SetGuidance("Set kinetic energy.");
  fPartEnergyCmd->SetDefaultUnit("MeV");
  
  fPartPosCmd = new G4UIcmdWith3VectorAndUnit("/Sim/particle/position",this);
  fPartPosCmd->SetGuidance("Set starting position of the particle.");
  fPartPosCmd->SetDefaultUnit("cm");
  fPartPosCmd->SetUnitCandidates("mm cm m");
  
  fPartDistrSphereCmd = new G4UIcmdWithADoubleAndUnit("/Sim/particle/DistrSphereRadius",this);
  fPartDistrSphereCmd->SetDefaultUnit("cm");
  fPartDistrSphereCmd->SetUnitCandidates("mm cm m");
  
  fPartDistrBoxCmd = new G4UIcmdWith3VectorAndUnit("/Sim/particle/DistrBoxSize",this);
  
  fPartDirectToVolCmd = new G4UIcmdWithAString("/Sim/particle/DirectToVolume",this);
  
  fPartMomentumCmd = new G4UIcmdWith3Vector("/Sim/particle/direction",this);
  fPartMomentumCmd->SetGuidance("Set momentum direction.");
  
  fPartConfineCmd = new G4UIcmdWithAString("/Sim/particle/confine",this);
  fPartConfineCmd->SetGuidance("Confine source to volume (NULL to unset).");
  fPartConfineCmd->SetGuidance("usage: confine VolName");
  fPartConfineCmd->SetParameterName("VolName",true,true);
  fPartConfineCmd->SetDefaultValue("NULL");
  
  fPartRandomBeamDirCmd = 
            new G4UIcmdWithoutParameter("/Sim/particle/SetRandomBeamDir",this);
}

//--------------------------------------------------------------------------

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fGunDir;
  delete fParticleCmd;
  delete fPartEnergyCmd;
  delete fPartPosCmd;
  delete fPartDistrSphereCmd;
  delete fPartDistrBoxCmd; 
  delete fPartDirectToVolCmd;
  delete fPartMomentumCmd;
  delete fPartConfineCmd;
  delete fPartRandomBeamDirCmd;
}

//-----------------------------------------------------------------------------

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,
                                               G4String newValue)
{ 
  if (command == fParticleCmd)
   fGenAction->SetParticleName(newValue);
  
  else if (command == fPartEnergyCmd)
   fGenAction->SetParticleEnergy(fPartEnergyCmd->GetNewDoubleValue(newValue)); 
  
  else if (command == fPartPosCmd)
   fGenAction->SetParticlePos( fPartPosCmd->GetNew3VectorValue(newValue)); 
  
  else if (command == fPartRandomBeamDirCmd)
    fGenAction->SetRandomBeamDir();
    
  else if (command == fPartMomentumCmd )
   fGenAction->SetParticleMomentum ( fPartMomentumCmd->GetNew3VectorValue(newValue));

  else if (command == fPartDistrSphereCmd)
   fGenAction->DistributeSourceOnSphere(fPartDistrSphereCmd->GetNewDoubleValue(newValue));
  
  else if (command == fPartDistrBoxCmd )
   fGenAction->DistributeSourceInABox( fPartDistrBoxCmd->GetNew3VectorValue(newValue));
   
  else if (command ==  fPartDirectToVolCmd )
   fGenAction->DirectParticleToVolume (newValue);
   
  else if ( command ==  fPartConfineCmd)
   fGenAction->ConfineSourceToVolume (newValue);
  
}

//----------------------------------------------------------------------------
