// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "VBaseDetectorPart.hh"
#include "SNSTargetSteelCylinder.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4Trap.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"  
#include "G4SystemOfUnits.hh"

 

SNSTargetSteelCylinder::SNSTargetSteelCylinder(G4String physName, 
 G4Colour color, G4bool draw)
: VBaseDetectorPart(physName), fColor(color), fDraw(draw)
{
}


SNSTargetSteelCylinder::~SNSTargetSteelCylinder()
{}


G4LogicalVolume* SNSTargetSteelCylinder::ConstructPart()
{
    G4LogicalVolumeStore *logStorePtr = G4LogicalVolumeStore::GetInstance();
         
	G4String partLogName = fPartPhysName + "Log";

	G4LogicalVolume* partLog = logStorePtr->GetVolume(partLogName, false);
    
    if(partLog == NULL)
    {
		//inside concrete
		G4double steelR1   =  50.*cm;
		G4double steelR2   =  53.*cm; 
		G4double steelHt   =  100.*cm; //200/2
   
		G4Tubs* cyl = new G4Tubs ("SteelCylSolid", steelR1, steelR2,steelHt, 0, twopi);
		G4Material* Steel = G4NistManager::Instance()->FindOrBuildMaterial("Steel");    
		
		
		//beam entrance hole box1 
		G4double cutBoxHalfX1 	= 15.*cm; 
		G4double cutBoxHalfY1 	= (steelR2-steelR1)/2. + 1.*cm; //outer to inner rad +extra
		G4double cutBoxHalfZ1 	= 10.*cm; 

		G4Box* box1 = new G4Box("ConcreteHole1", cutBoxHalfX1, cutBoxHalfY1, cutBoxHalfZ1);
		G4RotationMatrix* rot1 = new G4RotationMatrix;
		G4ThreeVector  trans1(0., -(steelR1+(steelR2-steelR1)/2.), 0.);  
		G4SubtractionSolid* cylSub2 = new G4SubtractionSolid("CylSub2", cyl, 
														 box1, rot1, trans1);
		
		
		//beam opposite hole box2 
		G4double cutBoxHalfX2 	= 25.*cm; //width/2
		G4double cutBoxHalfY2 	= (steelR2-steelR1)/2. + 1.*cm; //outer to inner rad +extra
		G4double cutBoxHalfZ2 	= 15.*cm; 
		G4Box* box2 = new G4Box("ConcreteHole2", cutBoxHalfX2, cutBoxHalfY2, cutBoxHalfZ2);
		//same as box1 ???
		G4RotationMatrix* rot2 = new G4RotationMatrix;
		G4ThreeVector  trans2(0., steelR1+(steelR2-steelR1)/2., 0.);  
		G4SubtractionSolid* cylSub22 = new G4SubtractionSolid("CylSub22", cylSub2, 
															  box2, rot2, trans2);
		
		
		
		partLog = new G4LogicalVolume(cylSub22,Steel, partLogName);
                  
        G4VisAttributes* visAtt = new G4VisAttributes (fColor);
        //visAtt->SetVisibility(true);  
        //visAtt->SetForceSolid(fDraw);
        partLog->SetVisAttributes(visAtt);
    }
    return partLog;
    
}


