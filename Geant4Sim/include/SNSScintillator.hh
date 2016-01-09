// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

#ifndef SNSSCINTILLATOR_HH
#define SNSSCINTILLATOR_HH

#include "VBaseDetectorPart.hh"
#include "globals.hh"

class G4LogicalVolume;


class SNSScintillator : public VBaseDetectorPart
{
  public:
	SNSScintillator(G4String);
    virtual ~SNSScintillator();
	       
    virtual G4LogicalVolume* ConstructPart();
	
  private:
	  SNSScintillator(const SNSScintillator&);
	  SNSScintillator& operator=( const SNSScintillator& );	  

 
};
 
#endif