// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "VBaseDetectorPart.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"


VBaseDetectorPart::VBaseDetectorPart(G4String physName)
:fPartPhysName(physName)
{;}

VBaseDetectorPart::~VBaseDetectorPart()
{;}

VBaseDetectorPart::VBaseDetectorPart(const VBaseDetectorPart & rhs):
  fPartPhysName(rhs.fPartPhysName), 
  fPartLog(rhs.fPartLog)  
{;}

VBaseDetectorPart& VBaseDetectorPart::operator=(const VBaseDetectorPart & rhs)
{
	if(this != & rhs)
	{
		fPartPhysName 	= rhs.fPartPhysName;  
		fPartLog		= rhs.fPartLog;  
	}
	return *this;
}


void VBaseDetectorPart::Place(G4RotationMatrix* rot, G4ThreeVector* trans,
     G4LogicalVolume* motherLogVol)
{
    //no need to check if placed, as it can be placed in multiple volumes
    fPartLog = this->ConstructPart();
		
    new G4PVPlacement( rot, *trans,  fPartLog, fPartPhysName, 
                        motherLogVol, false, 0, true);
}
