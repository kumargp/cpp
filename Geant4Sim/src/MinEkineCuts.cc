// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#include "MinEkineCuts.hh"

#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"
#include "G4LossTableManager.hh"

MinEkineCuts::MinEkineCuts(const G4String& aName)
  : SpecialCuts(aName)
{
    if (verboseLevel>1) {
    G4cout << GetProcessName() << " is created "<< G4endl;
   }
  SetProcessType(fUserDefined);
}

MinEkineCuts::~MinEkineCuts()
{}

MinEkineCuts::MinEkineCuts(const MinEkineCuts&)
  : SpecialCuts()
{}


G4double MinEkineCuts::PostStepGetPhysicalInteractionLength(
                             const G4Track& aTrack,
			     G4double ,
			     G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  
  G4double     proposedStep = DBL_MAX;
  // get the pointer to UserLimits
  G4UserLimits* pUserLimits = aTrack.GetVolume()->GetLogicalVolume()->GetUserLimits();
  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  G4ParticleDefinition* aParticleDef = aTrack.GetDefinition();
  
  if (pUserLimits && aParticleDef->GetPDGCharge() != 0.0) {
    //min kinetic energy
    G4double temp = DBL_MAX;
    G4double    eKine     = aParticle->GetKineticEnergy();
    const G4MaterialCutsCouple* couple = aTrack.GetMaterialCutsCouple();
    G4double eMin = pUserLimits->GetUserMinEkine(aTrack);

    if (eKine < eMin)
      return 0.;

    G4double    rangeNow = DBL_MAX;
      
    G4LossTableManager* lossManager = G4LossTableManager::Instance();
    rangeNow = lossManager->GetRange(aParticleDef,eKine,couple);

    // charged particles only      
    G4double rangeMin = lossManager->GetRange(aParticleDef,eMin,couple); 
    temp = rangeNow - rangeMin;
    if (proposedStep > temp) proposedStep = temp;  
  }
  return proposedStep;
}
