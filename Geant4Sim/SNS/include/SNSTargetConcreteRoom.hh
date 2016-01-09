// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#ifndef SNSTARGETCONCRETEROOM_HH 
#define SNSTARGETCONCRETEROOM_HH

#include "VBaseDetectorPart.hh"
#include "globals.hh"
#include "G4Colour.hh"

class G4LogicalVolume;


class SNSTargetConcreteRoom : public VBaseDetectorPart
{
  public:
    SNSTargetConcreteRoom(G4String, G4Colour, G4bool);
    virtual ~SNSTargetConcreteRoom();
        
	virtual G4LogicalVolume* ConstructPart();
	
  private:
	SNSTargetConcreteRoom (const SNSTargetConcreteRoom &);
	SNSTargetConcreteRoom& operator= (const SNSTargetConcreteRoom &);
	
    G4Colour     fColor;
    G4bool      fDraw;
 
};
 
#endif