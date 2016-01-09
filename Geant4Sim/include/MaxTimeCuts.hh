// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef MaxTimeCuts_h
#define MaxTimeCuts_h 1

#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.hh"


class MaxTimeCuts : public SpecialCuts
{
  public:     

     MaxTimeCuts(const G4String& processName ="MaxTimeCuts" );

     virtual ~MaxTimeCuts();

     // PostStep GPIL
     virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double   previousStepSize,
			     G4ForceCondition* condition
			    );
            
			    
  private:
  
  // hide assignment operator as private 
      MaxTimeCuts(const MaxTimeCuts&);
      MaxTimeCuts& operator=(const MaxTimeCuts& right);

};

#endif

