// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef VBASEDETECTORPART_HH
#define VBASEDETECTORPART_HH

#include "globals.hh"
//#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"


class G4LogicalVolume;
class G4PVPlacement;

//base class for part.


class VBaseDetectorPart
{
  
  public:
	VBaseDetectorPart(G4String );
    virtual ~VBaseDetectorPart();
	
	//G4LogicalVolume cannot be copy constructed.
	VBaseDetectorPart(const VBaseDetectorPart& );
	VBaseDetectorPart& operator=(const VBaseDetectorPart& );
	
	
  public:
	
    virtual G4LogicalVolume* ConstructPart() = 0;// {return 0;}

    void Place(G4RotationMatrix* rot, G4ThreeVector* trans,
			G4LogicalVolume* motherLogVol);
 
	
    G4String   GetPartPhysName() { return fPartPhysName; }  
    G4LogicalVolume* GetPartLog() { return fPartLog; }   
    
  protected:

    G4String fPartPhysName;  
    G4LogicalVolume* fPartLog;
  
};

#endif


