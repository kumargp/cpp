// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


//VBaseDetPartAssembly.hh

#ifndef VBASEDETPARTASSEMBLY_HH
#define VBASEDETPARTASSEMBLY_HH

#include "G4LogicalVolume.hh"
#include "G4AffineTransform.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
//All the detector part assemblies have to be derived from this baseclass

//have at least 1 method written is source file, to avoid error about vtable


static const G4double inch = 2.54*cm;


class VBaseDetPartAssembly
{
    public:
		VBaseDetPartAssembly(G4String);
		virtual ~VBaseDetPartAssembly();
		
		VBaseDetPartAssembly ( const VBaseDetPartAssembly& );
		VBaseDetPartAssembly& operator=( const VBaseDetPartAssembly& );
		
		//to actually construct geometry. Note, no volume is returned.
		virtual void Place( G4RotationMatrix*, G4ThreeVector*,
							G4LogicalVolume*) = 0;
		G4RotationMatrix* NetRotation(G4RotationMatrix*, G4ThreeVector, 
										  G4RotationMatrix*, G4ThreeVector);
		G4ThreeVector* NetTrans(G4RotationMatrix*, G4ThreeVector, 
										  G4RotationMatrix*, G4ThreeVector);
        inline G4String GetAssemblyName(){ return fAssemblyBaseName;}

    protected:
		G4String fAssemblyBaseName;
    
};

#endif

