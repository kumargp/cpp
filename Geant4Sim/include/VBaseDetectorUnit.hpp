
#ifndef VBASEDETECTORPART_HH
#define VBASEDETECTORPART_HH

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class G4LogicalVolume;
class G4PVPlacement;

//base class for part

class VBaseDetectorPart
{
  
  public:
	VBaseDetectorPart( G4String physName );
    virtual ~VBaseDetectorPart();
    VBaseDetectorPart(const VBaseDetectorPart&);
    
    virtual G4LogicalVolume* ConstructPart() {return 0;}

    void Place(G4RotationMatrix* rot, G4ThreeVector* trans,
			G4LogicalVolume* motherLogVol);
            
    G4String   GetPartPhysName() { return fPartPhysName; }  
    G4LogicalVolume* GetPartLog() { return fPartLog; }   
    
  protected:
    G4String fPartPhysName;  
    G4LogicalVolume* fPartLog;
  
};

#endif


