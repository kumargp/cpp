#include "SNSTargetDetector.hh"
#include "globals.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Trd.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Torus.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4UnitsTable.hh"
#include "G4NistManager.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManagerKernel.hh"
#include "G4ProductionCuts.hh"
#include "G4UserLimits.hh"
#include "SNSTargetConcreteRoom.hh"
#include "SNSTargetSteelCylinder.hh"
#include "SNSTargetOuterProtonBox.hh"



SNSTargetDetector::SNSTargetDetector(G4String detName )
:VBaseDetector(detName)
{
    G4cout << "constructed SNSTargetDetector" <<G4endl;
}

SNSTargetDetector::~SNSTargetDetector()
{
    //delete fMessenger;
}


void SNSTargetDetector::ConstructDet()
{
    enum draw { wire=0, solid=1, invisible=2 };
    // expt volume
    G4NistManager* nist = G4NistManager::Instance();
    //G4Material* expMat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* expMat = nist->FindOrBuildMaterial("G4_Galactic");

    G4double halfX = 55.*m;
    G4double halfY = 55.*m;//7.*m;
    G4double halfZ = 55.*m;//60.*m;
    G4Box* snsTargetBox = new G4Box("SNSexptBox", halfX, halfY, halfZ);
    G4String detLogName = "SNSTargetLogical";
    fDetLogical = new G4LogicalVolume(snsTargetBox, expMat, detLogName );

	G4ThreeVector* posCenter = new G4ThreeVector(0, 0, 0);  
	G4RotationMatrix* rotCenter = new G4RotationMatrix;	
	G4Material* steel = G4NistManager::Instance()->FindOrBuildMaterial("Steel");   
	G4Material* concrete = G4NistManager::Instance()->FindOrBuildMaterial("Concrete"); 
	G4Material* concreteHDC = G4NistManager::Instance()->FindOrBuildMaterial("HDConcrete");
	G4Material* water = nist->FindOrBuildMaterial("G4_WATER"); 
	G4Material* SteelD2O = G4NistManager::Instance()->FindOrBuildMaterial("SteelD2O"); 
	G4Material* Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"); 
	
    //TARGET HG box   //red
    G4double HgHalfHt = 4.9*cm; //9.8/2
    G4double HgHalfX = 21.*cm; //42/2
    G4double HgHalfY = 20.3*cm; 
    G4Box* HgBox = new G4Box("HgBox", HgHalfX, HgHalfY, HgHalfHt); //HALF
    G4Material* Hg = G4NistManager::Instance()->FindOrBuildMaterial("G4_Hg");   
    G4LogicalVolume* HgTargetBoxLog = new G4LogicalVolume(HgBox, Hg, "HgTargetBoxLog");
    new G4PVPlacement( rotCenter, *posCenter, HgTargetBoxLog, "HgTarget",
                       fDetLogical, false, 0 , true);
    HgTargetBoxLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));

    //HG STEELBOX steel plate surrounding Hg target.  thick =0.7cm
    G4double tHgSt = 0.7*cm; //all around
    G4double HgSteelhalfX = HgHalfX+tHgSt;
	G4double HgSteelhalfY = HgHalfY+tHgSt;
	G4double HgSteelhalfZ = HgHalfHt+tHgSt; //5.6
	G4Box* steelHgBoxfull = new G4Box("SteelHgBoxFull", HgSteelhalfX,HgSteelhalfY, HgSteelhalfZ);
    G4SubtractionSolid* steelHgBox = new G4SubtractionSolid("SteelHgBox", 
                                   steelHgBoxfull, HgBox, rotCenter, *posCenter);
    G4LogicalVolume* HgSteelBoxLog = new G4LogicalVolume(steelHgBox, steel, "HgSteelBoxLog");
    HgSteelBoxLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
    new G4PVPlacement( rotCenter, *posCenter, HgSteelBoxLog, "HgSteelBox",
                       fDetLogical, false, 0 , true);



    //INNER REFLECTOR PLUG  0 to 49.5 cm rad, ht =  0 to 5.6+5 both ways
    G4double IRPRad =  49.5*cm;
    G4double IRPhalfHt =  10.6*cm;
    G4Tubs* IRPFull = new G4Tubs ("innerRefPlug", 0, IRPRad, IRPhalfHt, 0, twopi);
	G4Box* IRPcutBox = new G4Box("IRPcutBox",HgSteelhalfX, IRPRad,HgSteelhalfZ);
    G4SubtractionSolid* IRPsolidSub = new G4SubtractionSolid("IRPsolidSub", 
                                IRPFull, IRPcutBox, rotCenter, *posCenter);
    //placing later

    //BE PLUG
    //rad=0. to 33 . Ht=  from IRPhalfHt to IRPhalfHt+35. cm
    G4double BePlugRad  = 33.*cm;
	G4double BePlugHalfHt = 17.5*cm;//35/2  from HgSteelhalfZ up/down
    G4Tubs* BePlugCyl = new G4Tubs ("BePlugCyl", 0.*cm, BePlugRad, BePlugHalfHt, 0, twopi);
    //placing later

	
    //MODERATORS
    //ht 5.6 to 5.6+25cm?.    rad upto corner of box 21 half size =29.8cm
    //coupled H Top //red
	G4double lH1halfX = 10.*cm; //20/2
	G4double lH1halfY = 5.*cm; //10/2
	G4double lH1halfZ = 12.5*cm;//25/2
	G4double lH1posYhalf = 10.*cm;
	G4Box* lHBox = new G4Box("liquidHBox1",lH1halfX, lH1halfY, lH1halfZ);
    //G4Tubs* liquidHCyl1 = new G4Tubs ("LiquidHCyl1", 0, 5.*cm, 15.*cm, 0, twopi);
	G4ThreeVector posLH1(0, lH1posYhalf, HgSteelhalfZ + lH1halfZ);  //z=5.6 to 30.6
    G4RotationMatrix* rotLH1 = new G4RotationMatrix; 
    rotLH1->rotateZ(90.*degree);
    G4Material* matLHydr = G4NistManager::Instance()->FindOrBuildMaterial("LiquidHydrogen");  
    G4LogicalVolume* liquidHLog1 = new G4LogicalVolume(lHBox, matLHydr, "LiquidH1Log");
    liquidHLog1->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotLH1, posLH1, liquidHLog1, "LiquidHydr1",
                       fDetLogical, false, 0 , true);   

	G4SubtractionSolid* IRPsolidSub1 = new G4SubtractionSolid("IRPsolidSub1", 
										IRPsolidSub, lHBox, rotLH1, posLH1);
	G4ThreeVector posLH1_BeTop(0, lH1posYhalf, HgSteelhalfZ+lH1halfZ -(HgSteelhalfZ+BePlugHalfHt));
    G4SubtractionSolid* BePlugTopSub1 = new G4SubtractionSolid("BePlugTopSub1", 
										BePlugCyl, lHBox, rotLH1, posLH1_BeTop);

								
    //coupled H Bottom
	G4ThreeVector posLH2(0, lH1posYhalf, -(HgSteelhalfZ + lH1halfZ));      
    G4RotationMatrix* rotLH2 = new G4RotationMatrix; 
    rotLH2->rotateZ(90.*degree);
    G4LogicalVolume* liquidHLog2 = new G4LogicalVolume(lHBox, matLHydr, "LiquidH2Log");
    liquidHLog2->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotLH2, posLH2, liquidHLog2, "LiquidHydr2",
                       fDetLogical, false, 0 , true);  
 

	G4SubtractionSolid* IRPsolidSub2 = new G4SubtractionSolid("IRPsolidSub2", 
											IRPsolidSub1, lHBox, rotLH2, posLH2);
	G4ThreeVector posLH1_BeBot(0, lH1posYhalf, -(HgSteelhalfZ+lH1halfZ)+(HgSteelhalfZ+BePlugHalfHt));
    G4SubtractionSolid* BePlugBotSub1 = new G4SubtractionSolid("BePlugBotSub1", 
										BePlugCyl, lHBox, rotLH2, posLH1_BeBot);

	
    // liquid hydrogen decoupled moderator 3 Top
	G4double lH3posXhalf = -10.*cm;
	G4double lH3posYhalf = -10.*cm;	
	G4ThreeVector posLH3(lH3posXhalf, lH3posYhalf, HgSteelhalfZ + lH1halfZ);      
    G4RotationMatrix* rotLH3 = new G4RotationMatrix; 
    rotLH3->rotateZ(45.*degree);
	G4LogicalVolume* liquidHLog3 = new G4LogicalVolume(lHBox, matLHydr, "LiquidH3Log");
    new G4PVPlacement( rotLH3, posLH3, liquidHLog3, "LiquidHydr3",
                       fDetLogical, false, 0 , true);  
   
	G4SubtractionSolid* IRPsolidSub3 = new G4SubtractionSolid("IRPsolidSub3", 
											IRPsolidSub2, lHBox, rotLH3, posLH3);
	G4ThreeVector posLH3_BeTop(lH3posXhalf, lH3posYhalf, 
							   HgSteelhalfZ+lH1halfZ -(HgSteelhalfZ+BePlugHalfHt));
    G4SubtractionSolid* BePlugTopSub2 = new G4SubtractionSolid("BePlugTopSub2", 
										BePlugTopSub1, lHBox, rotLH3, posLH3_BeTop);

    //water moderator  decoupled   Bottom  
	G4ThreeVector posWatermod(-lH3posXhalf, lH3posYhalf, -(HgSteelhalfZ+lH1halfZ));  
    G4RotationMatrix* rotLH4     = new G4RotationMatrix; 
    rotLH4->rotateZ(135.*degree);
    G4LogicalVolume* waterModLog = new G4LogicalVolume(lHBox, water, "WaterModeratorLog");
    waterModLog->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotLH4, posWatermod, waterModLog, "WaterModerator",
                       fDetLogical, false, 0 , true);   

	G4SubtractionSolid* IRPsolidSub4 = new G4SubtractionSolid("IRPsolidSub4", 
										IRPsolidSub3, lHBox, rotLH4, posWatermod);
	G4ThreeVector posLH4_BeBot(-lH3posXhalf, lH3posYhalf, 
							   -(HgSteelhalfZ+lH1halfZ)+(HgSteelhalfZ+BePlugHalfHt));
    G4SubtractionSolid* BePlugBotSub2 = new G4SubtractionSolid("BePlugBotsub2", 
									BePlugBotSub1, lHBox, rotLH4, posLH4_BeBot);




    //IRP plug placement
	G4Material* BeD2O = G4NistManager::Instance()->FindOrBuildMaterial("BeD2O"); 
	G4LogicalVolume* IRPLog = new G4LogicalVolume(IRPsolidSub4, Al, "InnerRefPlugLog");
	IRPLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
    new G4PVPlacement( rotCenter, *posCenter, IRPLog, "InnerRefPlug",
                       fDetLogical, false, 0 , true);  
 
    //Top Be Plug placement
	G4LogicalVolume* BePlugTopLog = new G4LogicalVolume(BePlugTopSub2, BeD2O, "UpperBePlugLog");
    G4ThreeVector posUpperBePlug(0, 0, IRPhalfHt+BePlugHalfHt);
    BePlugTopLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));

    new G4PVPlacement( rotCenter, posUpperBePlug, BePlugTopLog, "UpperBePlug",
												fDetLogical, false, 0 , true); 
	//Bottom Be plug placement
    G4LogicalVolume* BePlugBotLog = new G4LogicalVolume(BePlugBotSub2, BeD2O, "LowerBePlugLog");
    BePlugBotLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));	
    G4ThreeVector posLowerBePlug(0, 0, -(IRPhalfHt+BePlugHalfHt));  
	new G4PVPlacement( rotCenter, posLowerBePlug, BePlugBotLog, "BePlugBottom",
                       fDetLogical, false, 0 , true);   



	//INNER STEEL upper cylinder BePlugRad=33 to 49 cm rad, ht =  from n plug gap upto Beplug  
	G4double innerSteelRad1 = BePlugRad;
	G4double innerSteelRad2 = IRPRad;
	G4double innerSteelGap  =  BePlugHalfHt;
	G4double innerSteelHalfHt   = (2.*BePlugHalfHt -innerSteelGap)/2.; //subtract gap for n plugs
	G4Tubs* innerSteelD2Ocyl = new G4Tubs ("innerSteelD2Ocyl", innerSteelRad1, 
									innerSteelRad2, innerSteelHalfHt, 0, twopi);
	G4LogicalVolume* innerUpperSteelD2OLog = new G4LogicalVolume(innerSteelD2Ocyl, 
												SteelD2O, "InnerUpperSteelD2OLog");
	G4ThreeVector posInUpSteel(0, 0, IRPhalfHt+innerSteelGap+innerSteelHalfHt);
	innerUpperSteelD2OLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
	new G4PVPlacement( rotCenter, posInUpSteel, innerUpperSteelD2OLog, "InnerUpperSteelD2",
					   fDetLogical, false, 0 , true);	
	
	//INNER STEEL cylinder BOTTOM  
	G4LogicalVolume* innerBotSteelD2OLog = new G4LogicalVolume(innerSteelD2Ocyl, 
												SteelD2O, "InnerBotSteelD2OLog");
	G4ThreeVector posInBotSteel(0, 0, -(IRPhalfHt+innerSteelGap+innerSteelHalfHt));
	innerBotSteelD2OLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
	new G4PVPlacement( rotCenter, posInBotSteel, innerBotSteelD2OLog, "InnerBottomSteelD2O",
                       fDetLogical, false, 0 , true);   				
			
			
	//OUTER STEEL CYLINDER from 49 to 95cm. ht =219cm
	G4double outerSteelR1   =  IRPRad;
	G4double outerSteelR2   =  95.*cm; 
	G4double outerSteelHt   =  109.5*cm; //219/2
	G4Tubs* outerSteelCyl = new G4Tubs ("outerSteelCyl", outerSteelR1, 
									outerSteelR2, outerSteelHt, 0, twopi);
	G4Material* Steel = G4NistManager::Instance()->FindOrBuildMaterial("Steel");   
	//Proton cut
	G4Box* cutPBox1 = new G4Box("cutPBox", HgSteelhalfX, outerSteelR2, HgSteelhalfZ);
	G4ThreeVector posCutPBox1(0,-outerSteelR1-(outerSteelR2 -outerSteelR1)/2., 0);
	G4SubtractionSolid* outerSteelCylSub = new G4SubtractionSolid("outerSteelCylSub",
										outerSteelCyl, 	cutPBox1, rotCenter, posCutPBox1);
	G4LogicalVolume* outerSteelLog = new G4LogicalVolume(outerSteelCylSub, 
														 Steel, "OuterSteelLog");	
	outerSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));	
	new G4PVPlacement( rotCenter, *posCenter, outerSteelLog, "OuterSteelCylinder",
					   fDetLogical, false, 0 , true);   
	
    //MONOLITH STEEL cylinder
    G4double monoSteelRmax    = 5.*m;
    G4double monoSteelRmin    = 1.*m;
    G4double monoSteelHalfHt  = 6.25*m; //12.5/2 
    G4double protonBeamCutXZ  = 0.3*m; //42/2 cm is target in transverse 
    G4double transMonoZ       = 1.75*m;//6.25+1.75=8m bove; 6.25-1.75=4.5mbelow
    //minumum 1m ht below target is needed for innerSteelD2Ocyl2
    G4double monoLidTopHalfHt    = 0.5*m; // full ht=1m 
    G4double monoLidBotHalfHt    = 6.*inch; // 1 foot/2 
    G4Tubs* monoSteel1        = new G4Tubs ("MonoSteel1",monoSteelRmin, 
                                      monoSteelRmax, monoSteelHalfHt, 0, twopi);
    G4Tubs* monoSteelLidTop       = new G4Tubs ("monoSteelLidTop",0, monoSteelRmin,
                                                    monoLidTopHalfHt, 0, twopi);
    G4Tubs* monoSteelLidBot       = new G4Tubs ("monoSteelLidBot",0, monoSteelRmin,
                                                   monoLidBotHalfHt, 0, twopi);
    G4ThreeVector transMonoLidTop(0, 0,  monoSteelHalfHt-monoLidTopHalfHt);
    G4ThreeVector transMonoLidBot(0, 0, -monoSteelHalfHt+monoLidBotHalfHt);
    G4UnionSolid* monoSteel2  = new G4UnionSolid("MonoSteel2",
                         monoSteel1, monoSteelLidTop, rotCenter, transMonoLidTop);
    G4UnionSolid* monoSteel3  = new G4UnionSolid("MonoSteel3",
                         monoSteel2, monoSteelLidBot, rotCenter, transMonoLidBot);

   //beam entrance hole box1 
    G4double cutPBoxHalfY1  = (monoSteelRmax - monoSteelRmin)/2. + 0.3*m;  //extra
    G4Box* monoSteelCutBox1   = new G4Box("MonoSteelCutBox", protonBeamCutXZ, 
                                        cutPBoxHalfY1, protonBeamCutXZ);
    G4ThreeVector transBoxMonoSteel1(0., -monoSteelRmin-(monoSteelRmax-monoSteelRmin)/2.,-transMonoZ);  
    G4SubtractionSolid* monoSteel4 = new G4SubtractionSolid("MonoSteel4", 
                monoSteel3, monoSteelCutBox1, rotCenter, transBoxMonoSteel1);

    G4ThreeVector  transMonoSteel(0.,0., transMonoZ);// shift Y +0.5m
    G4LogicalVolume* monoSteelLog = new G4LogicalVolume(monoSteel4, steel, "MonoSteelLog");
    monoSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
    new G4PVPlacement( rotCenter,  transMonoSteel, monoSteelLog, "MonolithSteelCyl",
                                                fDetLogical, false, 0 , true); 
	
    
    //PROTON BEAM BOX outer of concrete and steel
    G4double protonBoxOuterHalfXZ = 2.0*m;//3.52*m; //NOTE
    G4double protonBoxOuterHalfY  = 15.*m; //NOTE: this will be moved out another 21 m
    SNSTargetOuterProtonBox* outerProtonBox = new SNSTargetOuterProtonBox("OuterProtonBox"
    ,protonBoxOuterHalfXZ, protonBoxOuterHalfY, protonBoxOuterHalfXZ, G4Colour::Cyan(), wire);
    G4ThreeVector* posOuterProtonBox = new G4ThreeVector(0, -protonBoxOuterHalfY-monoSteelRmax, 0);
    outerProtonBox->Place(rotCenter, posOuterProtonBox, fDetLogical); 


	//////////////////////////////////
    //################################
    //BUILDING, BASEMENT and DETECTORS
	//
	// BASEMENT road base stone(gravel), or granite loose
    G4double tunnelWall        = 18.*inch;
	G4double basementRad       = 22.3*m - tunnelWall;//rad 23.8-1.5m -wall. distLOS=25m
    //x=18.3, y=-15.2 =>rad 23.8.;  z=7.8 (8.1m-0.3 for det 1foot above ground)=>25m LOS
    G4double detToTargetFullZ  = 7.8*m;//8.1m-0.3(=1foot) 
	G4double detToWallGap	   = 1.5*m;
	G4double detAxialDist      = basementRad + tunnelWall+detToWallGap;
    G4double instrFloorTop2Tgt = protonBoxOuterHalfXZ;//2.*m;
    G4double instrFloorHalfHt  = 9.*inch;//18"/2
    G4double basementTop2Tgt   = instrFloorTop2Tgt + 2.*instrFloorHalfHt;
    G4double basementHalfHt    = (detToTargetFullZ - basementTop2Tgt+3.*m)/2.;   
    G4double basementPushZ     = basementHalfHt + basementTop2Tgt; 
	G4double monoBase		   = 1.*m; //instrFloorHalfHt;

    //FLOOR SLAB concrete 18"
    G4ThreeVector instrFloorPos(0,0, -instrFloorTop2Tgt-instrFloorHalfHt );
    G4Tubs* instrFloorFull     = new G4Tubs ("instrFloorCyl", 0, 
									basementRad, instrFloorHalfHt, 0, twopi);
    G4Tubs* floorMonoCut       = new G4Tubs ("floorMonoCut", 0,  
							monoSteelRmax+monoBase, instrFloorHalfHt, 0, twopi);
    G4SubtractionSolid* instrFloorCyl = new G4SubtractionSolid("basementCyl", 
                        instrFloorFull, floorMonoCut, rotCenter, *posCenter);
    G4LogicalVolume* instrFloorLog    = new G4LogicalVolume(instrFloorCyl, 
                                                   concrete, "InstrFloorLog"); 
    new G4PVPlacement( rotCenter, instrFloorPos, instrFloorLog, "InstrFloor", 
                                                    fDetLogical, false, 0, true);  



	//basement Inner
    G4double basementOuterLayer   = 4.*m;
    G4double basementRadInnerDisk = basementRad - basementOuterLayer;

	G4Tubs*  basementInSolid    = new G4Tubs ("basementInSolid", 0, basementRadInnerDisk, 
													basementHalfHt, 0, twopi);
	G4double monoCutHalfHt	  = (monoSteelHalfHt-transMonoZ-basementTop2Tgt+monoBase)/2.;
	G4Tubs* basementMonoCut	  = new G4Tubs ("BasementMonoCut", 0, 
					monoSteelRmax+ monoBase, monoCutHalfHt, 0, twopi);
	G4ThreeVector posMonoCut(0, 0, basementHalfHt-monoCutHalfHt); 
	G4SubtractionSolid* basementInCyl = new G4SubtractionSolid("basementInCyl", 
						basementInSolid, basementMonoCut, rotCenter, posMonoCut);
	 
    //G4Material* vac = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");	 
	G4Material* stone = G4NistManager::Instance()->FindOrBuildMaterial("RoadStone");	
	G4String materialType 		 = 										"RoadStone";
	G4LogicalVolume* basementInLog = new G4LogicalVolume(basementInCyl, stone, "BasementInLog"); 

	G4ThreeVector basementPos (0, 0, -basementPushZ);//cylinder
	basementInLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
	new G4PVPlacement( rotCenter, basementPos, basementInLog, "BasementIn", 
												fDetLogical, false, 0 , true);  
    //basement Outer layer
    G4Tubs*  basementLayerCyl = new G4Tubs ("basementLayerSolid", 
                basementRadInnerDisk, basementRad, basementHalfHt, 0, twopi);
    G4LogicalVolume* basementLayerLog = new G4LogicalVolume(basementLayerCyl, 
                                                        stone, "BasementLayerLog"); 
    basementLayerLog->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotCenter, basementPos, basementLayerLog, "BasementLayer", 
                                                fDetLogical, false, 0 , true);  


	//MONOLITH BASE
	G4double monoBaseCutHalf = monoCutHalfHt+instrFloorHalfHt;
	G4Tubs*  monoBaseCyl     = new G4Tubs ("monoBaseCyl", monoSteelRmax, 
							monoSteelRmax+monoBase,	 monoBaseCutHalf, 0, twopi);
	G4Tubs*  monoBaseBot     = new G4Tubs ("monoBaseBot", 0, 
								monoSteelRmax,	monoBase/2., 0, twopi);	
	G4ThreeVector monoBaseBotPos(0,0,-monoBaseCutHalf+monoBase/2.);
	G4UnionSolid* monoBaseSolid = new G4UnionSolid("monoBaseSolid", monoBaseCyl,
									monoBaseBot, rotCenter, monoBaseBotPos); 	
	G4LogicalVolume* monoBaseLog = new G4LogicalVolume(monoBaseSolid,
												concreteHDC, "MonoBaseLog"); 
	G4ThreeVector monoBasePos(0,0, -instrFloorTop2Tgt-monoBaseCutHalf);
	new G4PVPlacement( rotCenter, monoBasePos, monoBaseLog, "MonoBase", 
											fDetLogical, false, 0 , true);  	
	
    //TUNNEL WALL
    G4Tubs*  tunnelWallCyl          = new G4Tubs ("tunnelWall", basementRad, 
                          basementRad+tunnelWall, basementHalfHt+instrFloorHalfHt, 0, twopi);
    G4LogicalVolume* tunnelWallLog = new G4LogicalVolume(tunnelWallCyl, concrete, "TunnelWallLog"); 
    G4ThreeVector tunnelWallPos(0,0,-basementPushZ+instrFloorHalfHt);
    new G4PVPlacement( rotCenter, tunnelWallPos, tunnelWallLog, "TunnelWall", 
                                                    fDetLogical, false, 0 , true);  


    //BASEMENT HALL ROOF
    G4double baseRoofTopHalf     = 9.*inch;//18/2 instrument floor slab
    G4double baseRoofStoneHalf   = 2.5*12.*inch; //5/2 feet
    G4double baseRoofInHalf      = 9.*inch;//18/2  
    G4double baseRoofGravel       = 4.*m+tunnelWall; // 
    G4ThreeVector posHallRoofTop   (0,0, -instrFloorTop2Tgt-baseRoofTopHalf);
    G4ThreeVector posHallRoofStone (0,0, -instrFloorTop2Tgt-2.*baseRoofTopHalf
                                         -baseRoofStoneHalf); 
    G4ThreeVector posHallRoofIn    (0,0, -instrFloorTop2Tgt-2.*baseRoofTopHalf
                                         -2.*baseRoofStoneHalf-baseRoofInHalf);

    G4Tubs*  basementRoofTop   = new G4Tubs ("basementRoof1", basementRad+tunnelWall, 
                          basementRad+baseRoofGravel, baseRoofTopHalf, 0, twopi);
    G4LogicalVolume* basementRoofTopLog = new G4LogicalVolume( basementRoofTop,
                                        concrete, "basementRoofTopLog");
    new G4PVPlacement( rotCenter, posHallRoofTop, basementRoofTopLog,
                          "BasementRoofTop", fDetLogical, false, 0 , true); 

    G4Tubs*  basementRoofStone  = new G4Tubs ("basementRoof2", basementRad+tunnelWall, 
                          basementRad+baseRoofGravel, baseRoofStoneHalf, 0, twopi);
//G4Material* stone          = G4NistManager::Instance()->FindOrBuildMaterial("Stone");

    G4LogicalVolume* basementRoofStoneLog = new G4LogicalVolume( basementRoofStone,
                                        stone, "basementRoofStoneLog");
    new G4PVPlacement( rotCenter, posHallRoofStone, basementRoofStoneLog,
                          "BasementRoofStone", fDetLogical, false, 0 , true);  

    G4Tubs*  basementRoofIn     = new G4Tubs ("basementRoof3", basementRad+tunnelWall, 
                          basementRad+baseRoofGravel, baseRoofInHalf, 0, twopi);
    G4LogicalVolume* basementRoofInLog = new G4LogicalVolume( basementRoofIn,
                                        concrete, "basementRoofInLog");
    new G4PVPlacement( rotCenter, posHallRoofIn, basementRoofInLog, 
                         "BasementRoofIn", fDetLogical, false, 0 , true);  


    //BULK SHIELD HDC outer concrete from monolith to 10m
    G4double bulkConcShieldRmin    = monoSteelRmax;// 5.*m;
    G4double bulkConcShieldRmax    = 10.*m; 
    G4double bulkConcShieldHalfHt  = (instrFloorTop2Tgt+ 1.5*m)/2.;//1.5m above
    G4Tubs* bulkConcShieldCyl      = new G4Tubs ("BulkNBeamConcShieldCyl",
        bulkConcShieldRmin, bulkConcShieldRmax, bulkConcShieldHalfHt,0, twopi);
    G4double cutBoxBulkShieldHalfY  = (bulkConcShieldRmax-
                        bulkConcShieldRmin)/2. + 1.*m;  //extra 
    G4Box* bulkShieldPCutBox        = new G4Box("bulkHDCProtonCutBox1", 
        protonBoxOuterHalfXZ, cutBoxBulkShieldHalfY, protonBoxOuterHalfXZ);
                                        
    //G4double posBulkProtCutZ = bulkConcShieldHalfHt-(monoSteelHalfHt+2.*monConcLidHalfHt - transMonoZ);
    G4double posBulkProtCutZ = bulkConcShieldHalfHt-instrFloorTop2Tgt;//protonBoxOuterHalfXZ;
    
    G4ThreeVector  transBulkShieldCut(0., -bulkConcShieldRmin-(bulkConcShieldRmax
            -bulkConcShieldRmin)/2. + 0.6*m, -posBulkProtCutZ); //adjust extra Y
                                                
    G4SubtractionSolid* bulkConcShieldSub = new G4SubtractionSolid("bulkConcShieldSub", 
        bulkConcShieldCyl, bulkShieldPCutBox, rotCenter, transBulkShieldCut);

    G4LogicalVolume* bulkConcShieldLog  = new G4LogicalVolume( bulkConcShieldSub,
                                        concreteHDC, "BulkNBeamConcShieldLog");
    bulkConcShieldLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));
    //bulkConcShieldLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
    G4ThreeVector posBulkShieldHDC(0, 0, posBulkProtCutZ);
    new G4PVPlacement( rotCenter, posBulkShieldHDC, bulkConcShieldLog, "BulkNBeamConcShield",
                          fDetLogical, false, 0 , true);  
    

	////
	//DETECTOR, annulus
    G4double detCsIRad          = 5.5*cm;
    G4double detCsIRadInner     = detAxialDist - detCsIRad;
    G4double detCsIRadOuter     = detAxialDist + detCsIRad; 
    G4double detCsIHalfHt       = 16.5*cm;//33/2 
    G4ThreeVector posDetector   = G4ThreeVector(0,0, -detToTargetFullZ ); //NOTE
    G4Tubs* detCsICyl = new G4Tubs ("DetCsICyl", detCsIRadInner, 
                                    detCsIRadOuter, detCsIHalfHt,0, twopi);
    G4Material* csI     = G4NistManager::Instance()->FindOrBuildMaterial("CsI"); //d=4.5g/cc
    G4Material* lead    = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
    G4Material* cadmium = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cd");
    //G4Material* poly  = G4NistManager::Instance()->FindOrBuildMaterial("HDPE");
    G4LogicalVolume* detCsILog = new G4LogicalVolume(detCsICyl, csI, "DetCsILog");
    detCsILog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta())); 
    new G4PVPlacement( rotCenter, posDetector, detCsILog, "SNSDetectorCsI", 
                                                fDetLogical, false, 0 , true);

    G4double detPbRad           = detCsIRad +18.*cm;
    G4double detPbRadInner      = detAxialDist - detPbRad;
    G4double detPbRadOuter      = detAxialDist + detPbRad; 
    G4double detPbHalfHt        = detCsIHalfHt + 18.*cm; 
    G4Tubs* detPbCylFull        = new G4Tubs ("DetPbCylFull", detPbRadInner, 
                                        detPbRadOuter, detPbHalfHt,0, twopi);
    G4SubtractionSolid* detPbcyl = new G4SubtractionSolid("detPbcyl", 
                            detPbCylFull, detCsICyl, rotCenter, *posCenter);
    G4LogicalVolume* detPbLog = new G4LogicalVolume(detPbcyl, lead, "DetPbLog");
    detPbLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));    
    new G4PVPlacement( rotCenter, posDetector, detPbLog, "DetLeadShield", 
                                                fDetLogical, false, 0 , true); 

    G4double detCdRad           = detPbRad + 0.5*mm;
    G4double detCdInner         = detAxialDist - detCdRad;
    G4double detCdOuter         = detAxialDist + detCdRad; 
    G4double detCdHalfHt        = detPbHalfHt + 0.5*mm; 
    G4Tubs* detCdCylFull        = new G4Tubs ("DetCdCylFull", detCdInner, 
                                        detCdOuter, detCdHalfHt, 0, twopi);
    G4SubtractionSolid* detCdCyl= new G4SubtractionSolid("detCdCyl", 
                            detCdCylFull, detPbCylFull, rotCenter, *posCenter);
    G4LogicalVolume* detCdLog = new G4LogicalVolume(detCdCyl, cadmium, "DetCdLog");
    detCdLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey())); 
    new G4PVPlacement( rotCenter, posDetector, detCdLog, "DetCdShield", 
                                                fDetLogical, false, 0 , true);

    G4double detWaterRad        = detCdRad + 12.*2.54*cm;
    G4double detWaterInner      = detAxialDist - detWaterRad;
    G4double detWaterOuter      = detAxialDist + detWaterRad; 
    G4double detWaterHalfHt     = detPbHalfHt + 12.*2.54*cm; 
    G4Tubs*  detWaterCylFull    = new G4Tubs ("DetPbCylFull", detWaterInner, 
                                        detWaterOuter, detWaterHalfHt, 0, twopi);
    G4SubtractionSolid* detWaterCyl = new G4SubtractionSolid("detWaterCyl", 
                            detWaterCylFull, detCdCylFull, rotCenter, *posCenter);

    G4LogicalVolume* detWaterLog = new G4LogicalVolume(detWaterCyl, water, "DetWaterLog");
    detWaterLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red())); 
    new G4PVPlacement( rotCenter, posDetector, detWaterLog, "DetWaterShield", 
                                                fDetLogical, false, 0 , true);

    /*
    G4double detPolyRad   = detPbRad + 3.*2.54*cm;
    G4double detPolyInner = detAxialDist - detPolyRad;
    G4double detPolyOuter = detAxialDist + detPolyRad; 
    G4double detPolyHalfHt  = detPbHalfHt + 3.*2.54*cm; 
    G4Tubs* detPolyCylFull     = new G4Tubs ("DetPbCylFull", detPolyInner, 
                                        detPolyOuter, detPolyHalfHt, 0, twopi);
    G4SubtractionSolid* detPolyCyl = new G4SubtractionSolid("detPolyCyl", 
                            detPolyCylFull, detPbCylFull, rotCenter, *posCenter);

    detPolyLog = new G4LogicalVolume(detPolyCyl, poly, "DetPolyLog");
    detPolyLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red())); 
    new G4PVPlacement( rotCenter, posDetector, detPolyLog, "DetPolyShield", 
                                                fDetLogical, false, 0 , true);
    */


    //cuts. NOTE: don't define region without setting default cut/or in macro
    //parallel world has cut of the world by default. Don't set it, otherwise it will
     // be taken for the default.
    //G4RunManagerKernel::GetRunManagerKernel()->SetVerboseLevel(2);
    G4Region* regBase = new G4Region("BasementIn");
    regBase->AddRootLogicalVolume(basementInLog);
    regBase->AddRootLogicalVolume(bulkConcShieldLog);
    regBase->AddRootLogicalVolume(monoBaseLog);
    regBase->AddRootLogicalVolume(instrFloorLog);
    regBase->AddRootLogicalVolume(monoSteelLog);
    G4ProductionCuts* cutBase = new G4ProductionCuts();
    cutBase->SetProductionCut(10.*mm);
    regBase->SetProductionCuts(cutBase);


    G4Region* regBaseLayer = new G4Region("BasementLayer");
    regBaseLayer->AddRootLogicalVolume(basementLayerLog);
    G4ProductionCuts* cutBase2 = new G4ProductionCuts();
    cutBase2->SetProductionCut(1.*mm);
    regBaseLayer->SetProductionCuts(cutBase2);

    G4Region* regDet = new G4Region("Det_CsI");
    regDet->AddRootLogicalVolume(detCsILog);
    regDet->AddRootLogicalVolume(detPbLog);
    regDet->AddRootLogicalVolume(detCdLog);
    regDet->AddRootLogicalVolume(detWaterLog);
	////regDet->AddRootLogicalVolume(detPolyLog);
    G4ProductionCuts* cutDet = new G4ProductionCuts();
    cutDet->SetProductionCut(1.0*um);
    regDet->SetProductionCuts(cutDet);

		
    //force step in volume
    G4double maxStep = (detCsIRad < detCsIHalfHt )? 0.5*detCsIRad: 0.5*detCsIHalfHt;
    G4UserLimits* stepLimit = new G4UserLimits(maxStep);
    detCsILog->SetUserLimits(stepLimit);

    G4double maxStep2 =  0.5*5.5*m/65; //stepHt/2
    G4UserLimits* stepLimit2 = new G4UserLimits(maxStep2);
    basementInLog->SetUserLimits(stepLimit2);
    basementLayerLog->SetUserLimits(stepLimit2);



	G4cout << "World of size (m) = " << 2*halfX/m<< " "<< 2*halfY/m << " " 
								<< 2*halfZ/m << G4endl;
	G4cout << "Hg target of size (cm) " << 2*HgHalfX/cm << " " << 2*HgHalfY/cm 
					<< " " << 2*HgHalfHt/cm << G4endl;
	G4cout <<"Target Steel box of size (cm) " << 2*HgSteelhalfX/cm<< " " << 
					2*HgSteelhalfY/cm << " " << 2*HgSteelhalfZ/cm << G4endl;
	G4cout <<"inner Reflector Plug of Al rad:ht (cm) " << IRPRad/cm << " " 
					<< 2*IRPhalfHt/cm  << " from 0cm at center " << G4endl;
	G4cout <<"BePlug : top and Bottom of rad:ht (cm) " << BePlugRad/cm 
			<< " from 0 rad, and ht " << 2*BePlugHalfHt/cm << " from +/- z  "
			<<  IRPhalfHt+BePlugHalfHt << G4endl;
	G4cout <<"LiquidHydrogen moderators size (cm) " << 2*lH1halfX/cm << " " << 
			2*lH1halfY/cm  << " " << 2* lH1halfZ/cm << G4endl;
	G4cout <<" inner upper/lower Steel cylinders of rad:Ht (cm) " << 
		innerSteelRad2/cm-innerSteelRad1/cm << " " << 2*innerSteelHalfHt/cm << " from "
			<< innerSteelRad1/cm << " to " << innerSteelRad2/cm << " at +/- z (cm): " <<
		(IRPhalfHt+innerSteelGap+innerSteelHalfHt)/cm <<	G4endl;
	G4cout <<"OUTER Steel cylinders of rad:Ht (cm) " << 
		outerSteelR2/cm-outerSteelR1/cm << " " << 2*outerSteelHt/cm << " from "
		<< outerSteelR1/cm << " to " << outerSteelR2/cm << " at center" <<	G4endl;
	
	G4cout <<"OUTER Proton Box of CS (m) " <<  2*protonBoxOuterHalfXZ/m 
		<< " and length "<< 2.*protonBoxOuterHalfY/m << G4endl;
	G4cout << "MonolithSteelCyl of rad from "<< monoSteelRmin/m << " m to "
		<< monoSteelRmax/m << " m of ht " << 2.*monoSteelHalfHt/m << " m " << G4endl;
	G4cout << "Constructed Outer Concrete Building of rad from "
		<< bulkConcShieldRmin/m << " m to "	<< bulkConcShieldRmax/m 
		<< " of ht " << 2.*bulkConcShieldHalfHt/m << " m" <<G4endl;
	G4cout << "Constructed Basement of rad "<< (basementRad+tunnelWall)/m 
		<< " (m) of ht "  << 2.*basementHalfHt/m << " m of "<<  materialType<< G4endl;
	G4cout << "Defined Region: Basement" << G4endl;


    G4cout << "Constructed around Basement a ring Detector at z: "
    <<  -detToTargetFullZ/m << " m, radius: "  << detAxialDist/m 
    << " CsI:: rad= "<< detCsIRad/cm << " cm, ht= " << 2.*detCsIHalfHt/cm 
    << " cm:\nLeadShield:: rad= "<< detPbRad/cm << " cm, ht= " 
    << 2.*detPbHalfHt/cm << " cm.\nCd Shield:: rad= " << detCdRad/cm << " cm, ht= "
    << 2.*detCdHalfHt/cm << " cm.\nWATER:: rad= " << detWaterRad/cm << " cm, ht= "
    << 2.*detWaterHalfHt/cm << G4endl;

        

} 	//END OF DETECTOR construction

    //TODO: steel enclosure (4.5 × 4.5 × 6.5 m3) with a 1 m thick ceiling and 0.5 m thick sides


/*
Yuri:
Basement roof:
You have 18" of normal weight concrete for the utility tunnel roof. Above the 
tunnel roof there is 5 feet of compacted road base stone, then 18" of normal 
weight concrete for the Instrument floor slab. Once above the Instrument floor 
slab is the open area of the Target building all the way to the roof, which is 
steel framed with metal deck, foam insulation and built-up asphalt roofing.

Between target and the "place in the basement":

        1.Center of Target to edge of Monolith/liner = 17.12 feet (combined
        horiz and vertical). Material is steel.
        2. Edge of monolith/liner to underside of chopper shelf = 11.55 feet.
        Material is HD concrete.
        3. Underside of chopper shelf thru Instrument floor slab = 6.61 feet.
        Material is normal weight concrete.
        4. Underside of Instrument slab to outside of Utility Tunnel wall =
        43.34 feet. Material is compacted road base stone.
        5. Thru Utility Tunnel wall = 2.4 feet. Material is normal weight
        concrete.
        6. Inside edge of Utility Tunnel wall to assumed location of neutrino
        detector = 18.6 feet. Material is normal atmosphere.

Erik: defining y along the proton beam and z vertically, all from target
nose,  that upstream position in the tunnel will be at x=18.3, y=-15.2,
z=-6.1, =>los=24.6m 
NOTE:The 6.1m is 2m above floor. I took 1 foot above floor, so 6.1+2-0.3=7.8m
*/


/*static functions
G4Colour::White()
G4Colour::Gray()
G4Colour::Grey()
G4Colour::Black()
G4Colour::Brown()   
G4Colour::Red()
G4Colour::Green()
G4Colour::Blue()
G4Colour::Cyan()
G4Colour::Magenta()
G4Colour::Yellow()
*/

/*

    //NEUTRON TUBES
    G4double nTubeHalfDx11     = 1.0*m; //SET 
    G4double nTubeHalfDx12     = lH1halfX; //10.cm
    G4double nTubeHalfDy11     = 0.5*m; //SET 
    G4double nTubeHalfDy12     = lH1halfX;
    G4double nTubeHalfDz       = 5.*m; //SET
    G4double thickNtube        = 1.27*cm;
    G4Trd* nTubeOut1           = new G4Trd("nTubeOut1", nTubeHalfDx11, nTubeHalfDx12, 
                                 nTubeHalfDy11, nTubeHalfDy12, nTubeHalfDz);
    G4Trd* nTubeIn1            = new G4Trd("nTubeIn1", nTubeHalfDx11-thickNtube,  
                                 nTubeHalfDx12-thickNtube, nTubeHalfDy11-thickNtube, 
                                 nTubeHalfDy12-thickNtube, nTubeHalfDz);
    G4SubtractionSolid* nTube1  = new G4SubtractionSolid("nTube1", 
                                  nTubeOut1, nTubeIn1, rotCenter, *posCenter);
    G4LogicalVolume* nTube1Log  = new G4LogicalVolume(nTube1, Al, "NTube1Log");
    G4RotationMatrix* rotNTube1 = new G4RotationMatrix; 
    rotNTube1->rotateZ(90.*degree);
    rotNTube1->rotateX(-90.*degree);
    G4ThreeVector posNtube1(nTubeHalfDz+lH1halfY, lH1posYhalf, HgSteelhalfZ + lH1halfZ); 
    new G4PVPlacement( rotNTube1, posNtube1, nTube1Log, "NTube1",
                       fDetLogical, false, 0 , true);  

    G4RotationMatrix* rotNTube2 = new G4RotationMatrix; 
    rotNTube2->rotateX(90.*degree);
    G4ThreeVector posNtube2(lH3posXhalf, lH3posYhalf, HgSteelhalfZ + lH1halfZ);  
    new G4PVPlacement( rotNTube2, posNtube2, nTube1Log, "NTube2",
                       fDetLogical, false, 0 , true);  
    G4double nTubeHalfDx31     = 0.75*m; //SET 
    G4double nTubeHalfDx32     = lH1halfX; //10.cm
    G4double nTubeHalfDy31     = 1.5*m; //SET 
    G4double nTubeHalfDy32     = lH1halfX;
    G4Trd* nTubeOut3           = new G4Trd("nTubeOut3", nTubeHalfDx31, nTubeHalfDx32, 
                                 nTubeHalfDy31, nTubeHalfDy32, nTubeHalfDz);
    G4Trd* nTubeIn3            = new G4Trd("nTubeIn3", nTubeHalfDx31-thickNtube,  
                                 nTubeHalfDx32-thickNtube, nTubeHalfDy31-thickNtube, 
                                 nTubeHalfDy32-thickNtube, nTubeHalfDz);
    G4SubtractionSolid* nTube3  = new G4SubtractionSolid("nTube3", 
                                  nTubeOut3, nTubeIn3, rotCenter, *posCenter);
    G4LogicalVolume* nTube3Log  = new G4LogicalVolume(nTube3, Al, "NTube3Log");
    G4RotationMatrix* rotNTube3 = new G4RotationMatrix; 
    rotNTube3->rotateX(-90.*degree);
    G4ThreeVector posNtube3();
    new G4PVPlacement( rotNTube3, posNtube3, nTube3Log, "NTube3",
                       fDetLogical, false, 0 , true);  

    //subtract NTubes from IRP
    G4RotationMatrix* rotNTubeIRP1   = new G4RotationMatrix; 
    rotNTubeIRP1->rotateZ(90.*degree);
    rotNTubeIRP1->rotateX(-90.*degree);
    G4ThreeVector posNtubeIRP1(nTubeHalfDz+lH1halfY, lH1posYhalf, 
                            HgSteelhalfZ+lH1halfZ -(HgSteelhalfZ+BePlugHalfHt));
    G4SubtractionSolid* IRPsolidSub5 = new G4SubtractionSolid("IRPsolidSub5", 
                           IRPsolidSub4, nTubeOut1, rotNTubeIRP1, posNtubeIRP1);

   G4RotationMatrix* rotNTubeIRP2   = new G4RotationMatrix; 
    rotNTubeIRP2->rotateZ(90.*degree);
    G4ThreeVector posNtubeIRP2();
    G4SubtractionSolid* IRPsolidSub6 = new G4SubtractionSolid("IRPsolidSub6", 
                           IRPsolidSub5, nTubeOut1, rotNTubeIRP2, posNtubeIRP2);

    G4RotationMatrix* rotNTubeIRP3   = new G4RotationMatrix; 
    rotNTubeIRP3->rotateZ(90.*degree);
    G4ThreeVector posNtubeIRP3();
    G4SubtractionSolid* IRPsolidSub7 = new G4SubtractionSolid("IRPsolidSub7", 
                           IRPsolidSub6, nTubeOut3, rotNTubeIRP3, posNtubeIRP3);


*/


