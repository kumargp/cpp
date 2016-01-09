// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "VBaseDetectorPart.hh"
#include "SNSHousingAl.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh" 
#include "Materials.hh"
#include "G4NistManager.hh"// 
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4UnionSolid.hh"

SNSHousingAl::SNSHousingAl(G4String  physName) 
: VBaseDetectorPart(physName)
{;}


SNSHousingAl::~SNSHousingAl()
{;}


G4LogicalVolume* SNSHousingAl::ConstructPart()
{
    //Scintillator Housing
	G4String logName = fPartPhysName+ "Log";
	G4LogicalVolume* partLog = G4LogicalVolumeStore::GetInstance()->GetVolume(logName, false);
	if(! partLog)
	{
        //G4double delta = 0.0;//001*cm;
        G4double thick 			= 1.252*mm;
		G4double length 		= 167.4*mm;
        G4double bottomRad     = 43.25*mm;
        G4double topRad        = 63.6*mm;     //43.25+tan(6.94)*167
        
		G4double zPositions[] 	= {-1/2.*length, 1/2.*length};
		G4double rInner[] 		= {bottomRad-thick/2., topRad-thick/2.};
		G4double rOuter[]		= {bottomRad + thick/2., topRad+thick/2.};
		G4Polyhedra * housingCurved = new G4Polyhedra("Housingcurved", 0, 2*pi, 6, 2, 
									zPositions, rInner, rOuter);
                                    
        G4double lengthEnd      = thick;
		G4double zPositions2[]   = {-1/2.*lengthEnd, 1/2.*lengthEnd};
        G4double rInner2[]       = {0, 0};
        G4double rOuter2[]       = {bottomRad+thick/2., bottomRad+thick/2.};
        G4Polyhedra * housingBottom  = new G4Polyhedra("HousingBottom", 0, 2*pi, 6, 2, 
                                    zPositions2, rInner2, rOuter2);    

      /*  G4double zPositions3[]   = {-1/2.*lengthEnd, 1/2.*lengthEnd};
		G4double rInner3[]       = {0, 0};
		G4double rOuter3[]       = {topRad+thick/2., topRad+thick/2.};
        G4Polyhedra* housingTop = new G4Polyhedra("HousingTop", 0, 2*pi, 6, 2, 
                                    zPositions3, rInner3, rOuter3);
	*/
        G4RotationMatrix *rot1 = new G4RotationMatrix;
        G4ThreeVector transBottom (0,0,-length/2.-lengthEnd/2.);
        //G4ThreeVector transTop (0,0,length/2.+lengthEnd/2.);
        
        G4UnionSolid*  housing = new G4UnionSolid("HousingSolid", housingCurved, 
            housingBottom, rot1, transBottom );
        //G4UnionSolid*  housing = new G4UnionSolid("housing", housingAndBottom, 
        //    housingTop, rot, transTop );            
                                    
		//get material
		G4Material* material = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
		partLog = new G4LogicalVolume(housing, material, logName);
				
	}
	return partLog;
}

