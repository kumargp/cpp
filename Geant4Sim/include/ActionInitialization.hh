// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef ACTIONINITIALIZATION_HH
#define ACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class DetectorConstruction;
/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(DetectorConstruction* );
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
    
  private:	  
	ActionInitialization(const ActionInitialization&);
	ActionInitialization& operator=(const ActionInitialization& );
    DetectorConstruction* fDetector;
};


#endif