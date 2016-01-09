// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "VBaseDetPartAssembly.hh"
//#include "G4PVPlacement.hh"
#include "G4AffineTransform.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
//#include "G4UnitsTable.hh"
//#include "G4SystemOfUnits.hh"


VBaseDetPartAssembly::VBaseDetPartAssembly(G4String physName)
:fAssemblyBaseName(physName)
{
    
    //new G4UnitDefinition ( "inch", "inch", "Length", 2.54*cm );
    //G4UnitDefinition::BuildUnitsTable(); 
}

VBaseDetPartAssembly::~VBaseDetPartAssembly()
{;}


VBaseDetPartAssembly::VBaseDetPartAssembly(const VBaseDetPartAssembly& rhs )
:fAssemblyBaseName(rhs.fAssemblyBaseName)
{;}


VBaseDetPartAssembly& VBaseDetPartAssembly::operator=(const VBaseDetPartAssembly& rhs)
{
	if(this != &rhs)
	{
		fAssemblyBaseName = rhs.fAssemblyBaseName;
	}
	return *this;
}


G4RotationMatrix* VBaseDetPartAssembly::NetRotation(G4RotationMatrix* rotAssem, 
	G4ThreeVector posAssem, G4RotationMatrix* rot, G4ThreeVector pos)
{
	G4AffineTransform* affine1 = new G4AffineTransform(rotAssem, posAssem);
	G4AffineTransform* affine2 = new G4AffineTransform(rot, pos);	
	*affine1 *= *affine2;

	G4RotationMatrix* netRot = new G4RotationMatrix(affine1->NetRotation());
	
	//G4ThreeVector* netPos = G4ThreeVector(affine1->NetTranslation());
	
	return netRot;
}

G4ThreeVector* VBaseDetPartAssembly::NetTrans(G4RotationMatrix* rotAssem, 
				G4ThreeVector posAssem, G4RotationMatrix* rot, G4ThreeVector pos)
{
	G4AffineTransform* affine1 = new G4AffineTransform(rotAssem, posAssem);
	G4AffineTransform* affine2 = new G4AffineTransform(rot, pos);	
	*affine1 *= *affine2;
	
	//G4RotationMatrix* netRot = new G4RotationMatrix(affine1->NetRotation());
	G4ThreeVector* netPos = new G4ThreeVector(affine1->NetTranslation());
	
	return netPos;
}