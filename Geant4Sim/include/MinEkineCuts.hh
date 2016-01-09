// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#ifndef MinEkineCuts_h
#define MinEkineCuts_h 1

#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.hh"


class MinEkineCuts : public SpecialCuts
{
  public:     

     MinEkineCuts(const G4String& processName ="MinEkineCuts" );

     virtual ~MinEkineCuts();

     // PostStep GPIL
     virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double   previousStepSize,
			     G4ForceCondition* condition
			    );
            
			    
  private:
  
  // hide assignment operator as private 
      MinEkineCuts(const MinEkineCuts&);
      MinEkineCuts& operator=(const MinEkineCuts& right);

};

#endif

