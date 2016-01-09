// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef  SNSTARGETSTEELCYLINDER_HH
#define  SNSTARGETSTEELCYLINDER_HH

#include "VBaseDetectorPart.hh"
#include "globals.hh"
#include "G4Colour.hh"

class G4LogicalVolume;


class SNSTargetSteelCylinder : public VBaseDetectorPart
{
  public:
    SNSTargetSteelCylinder(G4String, G4Colour, G4bool);
    virtual ~SNSTargetSteelCylinder();
        
    virtual G4LogicalVolume* ConstructPart();
	
  private:
	SNSTargetSteelCylinder(const SNSTargetSteelCylinder&);
	SNSTargetSteelCylinder& operator=(const SNSTargetSteelCylinder&);	
	
    G4Colour     fColor;
    G4bool      fDraw; 
};
 
#endif