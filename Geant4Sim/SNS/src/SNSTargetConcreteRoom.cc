// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "VBaseDetectorPart.hh"
#include "SNSTargetConcreteRoom.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"  
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh" 


SNSTargetConcreteRoom::SNSTargetConcreteRoom(G4String physName, 
        G4Colour color, G4bool draw)
: VBaseDetectorPart(physName), fColor(color), fDraw(draw)
{
}


SNSTargetConcreteRoom::~SNSTargetConcreteRoom()
{}


G4LogicalVolume* SNSTargetConcreteRoom::ConstructPart()
{
    G4LogicalVolumeStore *logStorePtr = G4LogicalVolumeStore::GetInstance();
         
	G4String partLogName = fPartPhysName + "Log";

	G4LogicalVolume* partLog = logStorePtr->GetVolume(partLogName, false);
    
    if(partLog == NULL)
    {
		
//leave 3cm thick cylinder space between lids and the other parts
		G4double rMin   		= 53.*cm;
		G4double rMax   		= 97.*cm;
		G4double height 		= 100.*cm; //200/2
		
		G4Material* concrete 	= G4NistManager::Instance()->FindOrBuildMaterial("Concrete");
		
		G4Tubs* fullCyl 	= new G4Tubs ("FullCyl", rMin, rMax,height, 0, twopi);
		
		//beam entrance hole box1 
		G4double cutBoxHalfX1 = 15.*cm + 2.*cm; //thickness of steel inside concrete ???
		G4double cutBoxHalfY1 = (rMax-rMin)/2. + 2.*cm; //outer to inner rad + extra
		G4double cutBoxHalfZ1 = 15.*cm + 2.*cm; //thickness of steel inside concrete ???
		
		G4Box* box1 = new G4Box("ConcreteHole1", cutBoxHalfX1, cutBoxHalfY1, cutBoxHalfZ1);
		G4RotationMatrix* rot2 = new G4RotationMatrix;
		G4ThreeVector  trans2(0., -(rMin+(rMax-rMin)/2.), 0.);  
		G4SubtractionSolid* cylSub2 = new G4SubtractionSolid("CylSub2", fullCyl, 
																box1, rot2, trans2);
		
		//todo: fit the boxes  suitably ??
		
		//beam opposite hole box2 
		G4double cutBoxHalfX2 	= 25.*cm; //width/2
		G4double cutBoxHalfY2 	= (rMax-rMin)/2. + 2.*cm; //outer to inner rad +extra
		G4double cutBoxHalfZ2 	= 15.*cm; 
		G4Box* box2 = new G4Box("ConcreteHole2", cutBoxHalfX2, cutBoxHalfY2, cutBoxHalfZ2);
		//same as box1 ???
		G4RotationMatrix* rot22 = new G4RotationMatrix;
		G4ThreeVector  trans22(0., rMin+(rMax-rMin)/2., 0.);  
		G4SubtractionSolid* cylSub22 = new G4SubtractionSolid("CylSub22", cylSub2, 
															box2, rot22, trans22);
			
	/*	
		//Top Concrete
		G4double topLidR	= 50. *cm;
		G4double topLidHalfHt	= 17.5 *cm; //65 to 100
		G4double topLidStart = 65.*cm;
		G4Tubs* topLidCyl = new G4Tubs ("TopLid", 0., topLidR, topLidHalfHt, 0, twopi);	
		G4RotationMatrix* rot3 = new G4RotationMatrix;
		G4ThreeVector  trans3(0., 0., topLidStart+topLidHalfHt ); 	
		G4UnionSolid* cylPlusTopLid = new G4UnionSolid("CylPlusTopLid", 
												cylSub22, topLidCyl, rot3, trans3);
		
		//Bottom Concrete
		G4RotationMatrix* rot4 = new G4RotationMatrix;
		G4ThreeVector  trans4(0., 0., -topLidStart-topLidHalfHt); 	
		G4UnionSolid* cylPlusLids = new G4UnionSolid("cylPlusLids", 
									cylPlusTopLid, topLidCyl, rot4, trans4);		
	

		//bottomplug 
		G4double bottomPlugHalfHt1 = 24.*cm; //17 to 65
		G4double start1 	    = -17.*cm;
		G4double botPlugRMin1  = 35. *cm;
		G4double botPlugRMax1  = 50. *cm; 		
		G4Tubs* bottomPlugCyl1 = new G4Tubs ("BottomPlugConcrCyl1", botPlugRMin1, 
									botPlugRMax1, bottomPlugHalfHt1, 0, twopi);			
		G4RotationMatrix* rot5 = new G4RotationMatrix;
		G4ThreeVector trans5(0, 0, start1-bottomPlugHalfHt1);  
		G4UnionSolid* cylPlusLidsPlug1 = new G4UnionSolid("cylPlusLidsPug1", 
		cylPlusLids, bottomPlugCyl1, rot5, trans5);	
	*/	
		//steel cyl inside concrete
		
		partLog = new G4LogicalVolume(cylSub22, concrete, partLogName);

        G4VisAttributes* visAtt = new G4VisAttributes (fColor); 
		//partLog->SetVisAttributes(G4VisAttributes::Invisible);
        //visAtt->SetForceSolid(fDraw);
        partLog->SetVisAttributes(visAtt);
    }
    return partLog;
}

/*
G4double rMin   		= 53.*cm;
G4double rMax   		= 97.*cm;
G4double height 		= 200*cm;

G4Material* concrete 	= G4NistManager::Instance()->FindOrBuildMaterial("Cement");

G4Tubs* fullCyl 	= new G4Tubs ("FullCyl", rMin, rMax,height, 0, twopi);

//top inner cyl to subtract
G4double subCylRMin  	= 53.*cm;
G4double subCylRMax   	= 60.*cm;
G4double subCylHt 		= 62.*cm; //38 to 100
G4double startSubCyl	= 38.*cm;
G4double subCylTransZ  = startSubCyl + subCylHt/2.*cm;// 69 = 38 + 62/2	
G4Tubs* topSubCyl	= new G4Tubs ("TopSubCyl", subCylRMin, subCylRMax,
								  subCylHt, 0, twopi);
G4RotationMatrix* rot1 = new G4RotationMatrix;
G4ThreeVector  trans1(0., 0., subCylTransZ ); 
G4SubtractionSolid* cylSub1 = new G4SubtractionSolid("CylSub1", 
													 fullCyl, topSubCyl, rot1, trans1);

//beam entrance hole box1 or pipe ?
G4double cutBoxHalfX1 	= 15.*cm; 
G4double cutBoxHalfY1 	= (rMax-rMin)/2. + 0.001; //outer to inner rad
G4double cutBoxHalfZ1 	= 10.*cm; 

G4Box* box1 = new G4Box("ConcreteHole1", cutBoxHalfX1, cutBoxHalfY1, cutBoxHalfZ1);
G4RotationMatrix* rot2 = new G4RotationMatrix;
G4ThreeVector  trans2(0., -(rMin+(rMax-rMin)/2.), 0.);  
G4SubtractionSolid* cylSub2 = new G4SubtractionSolid("CylSub2", cylSub1, 
													 box1, rot2, trans2);


//beam entrance hole box2 or pipe ?
G4double cutBoxHalfX2 	= 25.*cm; //width/2
G4double cutBoxHalfY2 	= (rMax-rMin)/2. + 0.001; //outer to inner rad
G4double cutBoxHalfZ2 	= 15.*cm; 
G4Box* box2 = new G4Box("ConcreteHole2", cutBoxHalfX2, cutBoxHalfY2, cutBoxHalfZ2);
//same as box1 ???
G4RotationMatrix* rot22 = new G4RotationMatrix;
G4ThreeVector  trans22(0., +(rMin+(rMax-rMin)/2.), 0.);  
G4SubtractionSolid* cylSub22 = new G4SubtractionSolid("CylSub22", cylSub2, 
													  box2, rot22, trans22);


//Top Concrete
G4double topLidR	= 60. *cm;
G4double topLidHt	= 35. *cm;
G4Tubs* topLidCyl = new G4Tubs ("TopLid", 0., topLidR, topLidHt, 0, twopi);	
G4RotationMatrix* rot3 = new G4RotationMatrix;
G4ThreeVector  trans3(0., 0., 82.5*cm ); 	
G4UnionSolid* cylPlusTopLid = new G4UnionSolid("CylPlusTopLid", 
											   cylSub22, topLidCyl, rot3, trans3);

//Bottom Concrete
G4double botLidR	= 50. *cm;
G4double botLidHt	= 35. *cm;
G4Tubs* bottomLidcyl = new G4Tubs ("BottomLidcyl", 0., botLidR, botLidHt, 0, twopi);	
G4RotationMatrix* rot4 = new G4RotationMatrix;
G4ThreeVector  trans4(0., 0., -82.5*cm ); 	
G4UnionSolid* cylPlusLids = new G4UnionSolid("cylPlusLids", 
											 cylPlusTopLid, bottomLidcyl, rot4, trans4);		

//bottomplug Upper Part
G4double bottomPlugHt1 = 37.*cm; //15 to 52 
G4double start1 	    = 15.*cm;
G4double botPlugRMin1  = 35. *cm;
G4double botPlugRMax1  = 50. *cm; 		
G4Tubs* bottomPlugCyl1 = new G4Tubs ("BottomPlugConcrCyl1", botPlugRMin1, 
									 botPlugRMax1, bottomPlugHt1, 0, twopi);			
G4RotationMatrix* rot5 = new G4RotationMatrix;
G4ThreeVector trans5(0, 0, -(start1+ bottomPlugHt1/2.)); //15+37/2 
G4UnionSolid* cylPlusLidsPlug1 = new G4UnionSolid("cylPlusLidsPug1", 
												  cylPlusLids, bottomPlugCyl1, rot5, trans5);	

//bottom plug lower part
G4double bottomPlugHt2 = 13.*cm; //52 to 65 
G4double start2 	  	= 52.*cm;
G4double botPlugRMin2  = 33. *cm;
G4double botPlugRMax2  = 43. *cm; 		
G4Tubs* bottomPlugCyl2 = new G4Tubs ("BottomPlugConcrCyl2", botPlugRMin2, 
									 botPlugRMax2, bottomPlugHt2, 0, twopi);			
G4RotationMatrix* rot6 = new G4RotationMatrix;
G4ThreeVector trans6(0, 0, -(start2+ bottomPlugHt2/2.)); //15+37/2 
G4UnionSolid* cylPlusLidsPlug = new G4UnionSolid("cylPlusLidsPug", 
												 cylPlusLidsPlug1, bottomPlugCyl2, rot6, trans6);		
*/

/*
 // Top Concrete
 G4Material* concrete = G4NistManager::Instance()->FindOrBuildMaterial("Cement");
 G4Tubs* concreteTop = new G4Tubs ("ConcreteSolidTop", 0., 57.*cm, 35.*cm, 0, twopi);	
 G4LogicalVolume* concreteTopLog = new G4LogicalVolume(concreteTop, concrete, "ConcreteTop");	
 G4VisAttributes* visConcrTop = new G4VisAttributes(G4Colour::Cyan()); 	
 concreteTopLog->SetVisAttributes(visConcrTop);
 G4RotationMatrix* rotConcrTop = new G4RotationMatrix;
 G4ThreeVector  transConcrTop(0., 0., 82.5*cm ); 	
 new G4PVPlacement( rotConcrTop,transConcrTop, concreteTopLog, "ConcreteSolidTop",
 fDetLogical, false, 0 , true);
 
 
 
 //Bottom Concrete
 G4Tubs* concreteBottom = new G4Tubs ("ConcreteSolidBot", 0., 50.*cm, 35.*cm, 0, twopi);	
 G4LogicalVolume* concreteBottomLog = new G4LogicalVolume(concreteBottom, concrete, "ConcreteBottom");	
 G4VisAttributes* visConcrBot = new G4VisAttributes(G4Colour::Cyan()); 	
 concreteBottomLog->SetVisAttributes(visConcrBot);
 G4RotationMatrix* rotConcrBot = new G4RotationMatrix;
 G4ThreeVector  transConcrBot(0., 0., -82.5*cm ); 	
 new G4PVPlacement( rotConcrBot,transConcrBot, concreteBottomLog, "ConcreteSolidBottom",
 fDetLogical, false, 0 , true);
 */
