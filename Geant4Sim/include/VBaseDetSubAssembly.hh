//VBaseDetPartAssembly.hh

#ifndef VBASEDETPARTASSEMBLY_HH
#define VBASEDETPARTASSEMBLY_HH

#include "G4LogicalVolume.hh"

//All the detectors have to be derived from this baseclass, so that they can be
//selected and constructed in the DetectorConstruction class.

//have at least 1 method written is source file, to avoid error about vtable

class VBaseDetPartAssembly
{
    public:
        VBaseDetPartAssembly();
        virtual ~VBaseDetPartAssembly();
          
        //to actually construct geometry. Note, no volume is returned.
        virtual void ConstructAssembly() = 0;
        virtual void Place( G4RotationMatrix*, G4ThreeVector,
							G4LogicalVolume*) = 0;
        
        virtual G4LogicalVolume* GetSDLogVol(){;}
        virtual G4String GetDetLogName() {;}
        virtual G4LogicalVolume* GetDetLogical(){;}
          
    private:

    
    
};

#endif

