// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "MaxTimeCuts.hh"

#include "G4PhysicalConstants.hh"
#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"

MaxTimeCuts::MaxTimeCuts(const G4String& aName)
  : SpecialCuts(aName)
{
   if (verboseLevel>1) {
     G4cout << GetProcessName() << " is created "<< G4endl;
   }
   SetProcessType(fUserDefined);
}

MaxTimeCuts::~MaxTimeCuts()
{}

MaxTimeCuts::MaxTimeCuts(const MaxTimeCuts&)
 : SpecialCuts()
{}

 
G4double MaxTimeCuts::PostStepGetPhysicalInteractionLength(
                             const G4Track& aTrack,
			     G4double ,
			     G4ForceCondition* condition
			    )
{
  // condition is set to "Not Forced"
  *condition = NotForced;

   G4double     proposedStep = DBL_MAX;
   // get the pointer to UserLimits
   G4UserLimits* pUserLimits = aTrack.GetVolume()->GetLogicalVolume()->GetUserLimits();
   const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();

   // can apply cuts for specific particles - use if(particleDef):
   //   G4ParticleDefinition* aParticleDef = aTrack.GetDefinition();

   //   G4cout << " Time: " << pUserLimits->GetUserMaxTime(aTrack) << G4endl;
  
   if (pUserLimits) {
     G4double temp = DBL_MAX;
     //max time limit
     G4double dTime= (pUserLimits->GetUserMaxTime(aTrack) - aTrack.GetGlobalTime());
     if (dTime < 0. ) {
       proposedStep = 0.;
     } else {  
       G4double beta = (aParticle->GetTotalMomentum())/(aParticle->GetTotalEnergy());
       temp = beta*c_light*dTime;
       if (proposedStep > temp) proposedStep = temp;                  
     }

   }
   return proposedStep;
}
