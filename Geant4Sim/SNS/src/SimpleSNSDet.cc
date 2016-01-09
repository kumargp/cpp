// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "SimpleSNSDet.hh"
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
#include "G4AffineTransform.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
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
#include "SNSTargetOuterProtonBox.hh"
#include <sstream>
#include <vector>
#include <cmath>  
#include <cassert>
#include "G4RunManager.hh"

SimpleSNSDet::SimpleSNSDet(G4String detName )
:VBaseDetector(detName), fMSDet1Inner(0.), fMSDet2Inner(0.)
,fMSDet3Inner(0.), fMSDetCylThick(0.), fMSDetCylHht(0.)
,fIsMSDet2(true), fIsMSDet3(true)
{
    G4cout << "constructed SimpleSNSDet" <<G4endl;
}

SimpleSNSDet::~SimpleSNSDet()
{
    //delete fMessenger;
}

G4SubtractionSolid* SimpleSNSDet::SubMultiStepDet(G4SubtractionSolid* fromSolid, 
	G4String name, G4ThreeVector position, G4RotationMatrix* rot)

{
	//TODO get these cyliders from calling function which is defined elsewhere 
	assert(fMSDet1Inner > 0 || fMSDet2Inner > 0 || fMSDet3Inner > 0);
	assert(fMSDetCylThick > 0 && fMSDetCylHht >0);
	
	G4Tubs* mStepCyl1		= new G4Tubs("multiStepCylSolid1",fMSDet1Inner, 
				fMSDet1Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);

	G4SubtractionSolid* sub1	= new G4SubtractionSolid("sub1",
									 fromSolid, mStepCyl1, rot, position);
	G4SubtractionSolid* sub		= sub1;
	G4SubtractionSolid* sub2	= sub1;
	G4SubtractionSolid* sub3	= NULL;
	
	//Det2 and 3 are independent
	if(fIsMSDet2)
	{
		G4Tubs* mStepCyl2   = new G4Tubs("multiStepCylSolid2",fMSDet2Inner, 
				fMSDet2Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);
		sub2	= new G4SubtractionSolid("sub2", sub1, mStepCyl2, rot, position); 
		sub		= sub2;
	}
	
	if(fIsMSDet3)
	{
		G4Tubs* mStepCyl3   = new G4Tubs("multiStepCylSolid3",fMSDet3Inner, 
				fMSDet3Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi); 
		sub3    = new G4SubtractionSolid(name, sub2, mStepCyl3,  rot, position);
		sub		= sub3;
	}
	return sub;
}



void SimpleSNSDet::ConstructDet()
{
	
	G4bool fCheckOverlap 	= true;
	
	G4bool multiStepRuns 	= true; //MSDETRUN
	G4bool braneBaseDet		= true; //BASEBRANEDET
	G4bool basementCsIDet 	= false; //BASECSIDET
	
	fIsMSDet2 				= true; //ISMSDET2
	fIsMSDet3 				= false;	//ISMSDET3
	fMSDet1Inner     		= 8.0*m;	//MSDET1INRAD
	fMSDet2Inner     		= 12.0*m;	//MSDET2INRAD
	fMSDet3Inner    		= 16.0*m;   // MSDET3INRAD
	
    fMSDetCylThick                      = 1.*mm;


    // expt volume
    G4NistManager* nist = G4NistManager::Instance();
    //G4Material* airMat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* expMat = nist->FindOrBuildMaterial("G4_Galactic");

    G4double halfX = 35.*m;
    G4double halfY = 35.*m;//7.*m;
    G4double halfZ = 35.*m;//60.*m;
    G4Box* snsTargetBox = new G4Box("SNSexptBox", halfX, halfY, halfZ);
    G4String detLogName = "SNSTargetLogical";
    fDetLogical = new G4LogicalVolume(snsTargetBox, expMat, detLogName );

    enum draw { wire=0, solid=1, invisible=2 };
	G4ThreeVector* posCenter = new G4ThreeVector(0, 0, 0);  
	G4RotationMatrix* rotCenter = new G4RotationMatrix;	
	G4Material* steel = G4NistManager::Instance()->FindOrBuildMaterial("Steel");   
	G4Material* concrete = G4NistManager::Instance()->FindOrBuildMaterial("Concrete"); 
	G4Material* concreteHDC = G4NistManager::Instance()->FindOrBuildMaterial("HDConcrete");
	G4Material* water = nist->FindOrBuildMaterial("G4_WATER"); 
	G4Material* SteelD2O = G4NistManager::Instance()->FindOrBuildMaterial("SteelD2O"); 
	G4Material* Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"); 
	G4Material* lightMaterial = G4NistManager::Instance()->FindOrBuildMaterial("LightConcrete");
	
	
    //TARGET HG box   //red
    G4double HgHalfHt = 4.9*cm; //9.8/2
    G4double HgHalfX = 21.*cm; //42/2
    G4double HgHalfY = 20.3*cm; 
    G4Box* HgBox = new G4Box("HgBox", HgHalfX, HgHalfY, HgHalfHt); //HALF
    G4Material* Hg = G4NistManager::Instance()->FindOrBuildMaterial("G4_Hg");   
    G4LogicalVolume* HgTargetBoxLog = new G4LogicalVolume(HgBox, Hg, "HgTargetBoxLog");
    HgTargetBoxLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
    new G4PVPlacement( rotCenter, *posCenter, HgTargetBoxLog, "HgTarget",
                       fDetLogical, false, 0 , fCheckOverlap);
    // beam profile footprint on target is 7x20 cm2, covering 90% of the beam, 
    //which means 2 sigma on each direction. SET it in macro


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
                       fDetLogical, false, 0 , fCheckOverlap);



    //INNER REFLECTOR PLUG  0 to 49.5 cm rad, ht =  0 to 5.6+5 both ways
    G4double IRPRad 		=  49.5*cm;
	//G4double  innRadIRP    =  47.625*cm;
	G4double IRPhalfHt 		=  11.2*cm;//10.6*cm; //7.6+ 2*1.8 = 11.2;
    G4Tubs* IRPFull  = new G4Tubs ("innerRefPlug", 0, IRPRad, IRPhalfHt, 0, twopi);
	G4Box* IRPcutBox = new G4Box("IRPcutBox",HgSteelhalfX, IRPRad,HgSteelhalfZ);
    G4SubtractionSolid* IRPsolidSub = new G4SubtractionSolid("IRPsolidSub", 
                                IRPFull, IRPcutBox, rotCenter, *posCenter);
    //placing later

    //BE PLUG
    //rad=0. to 33 . Ht=  from IRPhalfHt to IRPhalfHt+35. cm
    G4double BePlugRad  = 33.*cm;
	G4double BePlugHalfHt = 17.5*cm;//35/2  from HgSteelhalfZ up/down
    G4Tubs* BePlugCyl = new G4Tubs ("BePlugCyl", 0.*cm, BePlugRad, BePlugHalfHt, 0, twopi);
	G4double topBePlugZ		= IRPhalfHt + BePlugHalfHt;
	G4ThreeVector posUpperBePlug(0, 0, topBePlugZ);
	G4double botBePlugZ		=	-topBePlugZ;
	G4ThreeVector posLowerBePlug(0, 0, botBePlugZ); 
    //placing later

	
    //MODERATORS
    //ht 5.6 to 5.6+25cm?.    rad upto corner of box 21 half size =29.8cm
    //coupled H Top //red
	G4double lH1halfX = 5.*cm; //NOTE x,y interchanged to avoid rotation
	G4double lH1halfY = 10.*cm;
	G4double lH1halfZ = 12.5*cm;//25/2
	G4double lH1posY = 10.*cm;
	G4Box* lHBox = new G4Box("liquidHBox1",lH1halfX, lH1halfY, lH1halfZ);
    //G4Tubs* liquidHCyl1 = new G4Tubs ("LiquidHCyl1", 0, 5.*cm, 15.*cm, 0, twopi);
	G4double lH1posZ = HgSteelhalfZ + lH1halfZ;
	G4ThreeVector posLH1(0, lH1posY, lH1posZ);  //z=5.6 to 30.6
    G4RotationMatrix* rotLH1 = new G4RotationMatrix; 
    G4Material* matLHydr = G4NistManager::Instance()->FindOrBuildMaterial("LiquidHydrogen");  
    G4LogicalVolume* liquidHLog1 = new G4LogicalVolume(lHBox, matLHydr, "LiquidH1Log");
    liquidHLog1->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotLH1, posLH1, liquidHLog1, "LiquidHydr1",
                       fDetLogical, false, 0 , fCheckOverlap);   

	G4SubtractionSolid* IRPsolidSub1 = new G4SubtractionSolid("IRPsolidSub1", 
										IRPsolidSub, lHBox, rotLH1, posLH1);
	G4ThreeVector posLH1_BeTop(0, lH1posY, lH1posZ -topBePlugZ);//(HgSteelhalfZ+BePlugHalfHt));
    G4SubtractionSolid* BePlugTopSub1 = new G4SubtractionSolid("BePlugTopSub1", 
										BePlugCyl, lHBox, rotLH1, posLH1_BeTop);

								
    //coupled H Bottom
	G4ThreeVector posLH2(0, lH1posY, -lH1posZ);      
    G4RotationMatrix* rotLH2 = new G4RotationMatrix; 
    G4LogicalVolume* liquidHLog2 = new G4LogicalVolume(lHBox, matLHydr, "LiquidH2Log");
    liquidHLog2->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotLH2, posLH2, liquidHLog2, "LiquidHydr2",
                       fDetLogical, false, 0 , fCheckOverlap);  
 

	G4SubtractionSolid* IRPsolidSub2 = new G4SubtractionSolid("IRPsolidSub2", 
											IRPsolidSub1, lHBox, rotLH2, posLH2);
	G4ThreeVector posLH1_BeBot(0, lH1posY, -lH1posZ- botBePlugZ);
    G4SubtractionSolid* BePlugBotSub1 = new G4SubtractionSolid("BePlugBotSub1", 
										BePlugCyl, lHBox, rotLH2, posLH1_BeBot);

	
    // liquid hydrogen decoupled moderator 3 Top
	G4double lH3posX = -10.*cm;
	G4double lH3posY = -10.*cm;	
	G4ThreeVector posLH3(lH3posX, lH3posY, lH1posZ);      
    G4RotationMatrix* rotLH3 = new G4RotationMatrix; 
    G4double lH1Rot = 48.75*degree;
    rotLH3->rotateZ(lH1Rot);
	G4LogicalVolume* liquidHLog3 = new G4LogicalVolume(lHBox, matLHydr, "LiquidH3Log");
    liquidHLog3->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotLH3, posLH3, liquidHLog3, "LiquidHydr3",
                       fDetLogical, false, 0 , fCheckOverlap);  
   
	G4SubtractionSolid* IRPsolidSub3 = new G4SubtractionSolid("IRPsolidSub3", 
											IRPsolidSub2, lHBox, rotLH3, posLH3);

	G4ThreeVector posLH3_BeTop(lH3posX, lH3posY, lH1posZ -topBePlugZ);
    G4SubtractionSolid* BePlugTopSub2 = new G4SubtractionSolid("BePlugTopSub2", 
										BePlugTopSub1, lHBox, rotLH3, posLH3_BeTop);

    //water moderator  decoupled   Bottom  
	G4ThreeVector posWatermod(-lH3posX, lH3posY, -lH1posZ);  
    G4RotationMatrix* rotLH4     = new G4RotationMatrix; 
    rotLH4->rotateZ(-lH1Rot);
    G4LogicalVolume* waterModLog = new G4LogicalVolume(lHBox, water, "WaterModeratorLog");
    waterModLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
    new G4PVPlacement( rotLH4, posWatermod, waterModLog, "WaterModerator",
                       fDetLogical, false, 0 , fCheckOverlap);   



	G4SubtractionSolid* IRPsolidSub4 = new G4SubtractionSolid("IRPsolidSub4", 
										IRPsolidSub3, lHBox, rotLH4, posWatermod);
	G4ThreeVector posLH4_BeBot(-lH3posX, lH3posY, -lH1posZ-botBePlugZ);
    G4SubtractionSolid* BePlugBotSub2 = new G4SubtractionSolid("BePlugBotsub2", 
									BePlugBotSub1, lHBox, rotLH4, posLH4_BeBot);			

	//OUTER STEEL CYLINDER from 49 to 95cm. ht =219cm
	G4double outerSteelR1   		=  IRPRad;
	G4double outerSteelR2   		=  95.*cm; 
	G4double outerSteelHt   		=  109.5*cm; //219/2
	G4Tubs* outerSteelCyl 	= new G4Tubs ("outerSteelCyl", outerSteelR1, 
									outerSteelR2, outerSteelHt, 0, twopi);
	G4Material* Steel 		= G4NistManager::Instance()->FindOrBuildMaterial("Steel");   
	//Proton cut
	G4Box* cutPBox1 		= new G4Box("cutPBox", HgSteelhalfX, outerSteelR2, HgSteelhalfZ);
	G4ThreeVector posCutPBox1(0,-outerSteelR1-(outerSteelR2 -outerSteelR1)/2., 0);
	G4SubtractionSolid* outerSteelCylSub1 = new G4SubtractionSolid("outerSteelCylSub1",
										outerSteelCyl, 	cutPBox1, rotCenter, posCutPBox1);
	G4LogicalVolume* outerSteelLog = new G4LogicalVolume(outerSteelCylSub1, Steel, "OuterSteelLog");
	outerSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));	
	new G4PVPlacement( rotCenter, *posCenter, outerSteelLog, "OuterSteelCylinder",
					   fDetLogical, false, 0 , fCheckOverlap);   

	
	//IRP plug placement
	G4Material* BeD2O = G4NistManager::Instance()->FindOrBuildMaterial("BeD2O"); 
	G4LogicalVolume* IRPLog = new G4LogicalVolume(IRPsolidSub4, Al, "InnerRefPlugLog");
	IRPLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
	new G4PVPlacement( rotCenter, *posCenter, IRPLog, "InnerRefPlug",
					   fDetLogical, false, 0 , fCheckOverlap);  
	
	//Top & Bottom Be Plug placement 
	G4LogicalVolume* BePlugTopLog = new G4LogicalVolume(BePlugTopSub2, BeD2O, "UpperBePlugLog");
	G4LogicalVolume* BePlugBotLog = new G4LogicalVolume(BePlugBotSub2, BeD2O, "LowerBePlugLog");
	
	BePlugTopLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));
	BePlugBotLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));	
	new G4PVPlacement( rotCenter, posUpperBePlug, BePlugTopLog, "UpperBePlug",
					   fDetLogical, false, 0 , fCheckOverlap); 	 
	new G4PVPlacement( rotCenter, posLowerBePlug, BePlugBotLog, "BePlugBottom",
					   fDetLogical, false, 0 , fCheckOverlap);   
	
	
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
					   fDetLogical, false, 0 , fCheckOverlap);	
	
	//INNER STEEL cylinder BOTTOM  
	G4LogicalVolume* innerBotSteelD2OLog = new G4LogicalVolume(innerSteelD2Ocyl, 
												SteelD2O, "InnerBotSteelD2OLog");
	G4ThreeVector posInBotSteel(0, 0, -(IRPhalfHt+innerSteelGap+innerSteelHalfHt));
	innerBotSteelD2OLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
	new G4PVPlacement( rotCenter, posInBotSteel, innerBotSteelD2OLog, "InnerBottomSteelD2O",
					   fDetLogical, false, 0 , fCheckOverlap);   				
	
	
	
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	
	//SETTINGS 
	G4double tunnelWallThick		= 2.*m;
	G4double tunnelDetThick			= 1*cm;
	// x=18.3, y=-15.2, z= -7.1m (-8.1 + 1.m). 19.6m    // old 22.3*m . 18.3*m for c5 
	G4double detToTargetFullZ       = 7.1*m;//8.1m-1m //old=7.8
	G4double baseRadiustTot         = 20.*m ;// 18.3*m for c5
	G4double detToWallGap       	= 0.75*m;
	G4double baseInRad        		= baseRadiustTot - tunnelWallThick - tunnelDetThick;//18.3 - 18"
	G4double detAxialDist      		= baseRadiustTot + detToWallGap;
	G4double basementTop2Tgt   		= 2.0*m;//3.52*m; 
	G4double basementHalfHt    		= (detToTargetFullZ - basementTop2Tgt+3.*m)/2.;  //8.3m/2 
	G4double basementPosZ     		= - basementHalfHt - basementTop2Tgt; 
	G4double surfaceLayerHalfWid    = 1.*cm;
	G4double bulkHdcTotR        	= 20.*m; 
	G4double hdcTgtUp           	= 1.5*m;
	G4double radOutHDC				= bulkHdcTotR - surfaceLayerHalfWid;
	G4double halfHtHDC				= (basementTop2Tgt+ hdcTgtUp)/2.;//1.5m above 
	
	
	
	//MONOLITH STEEL cylinder
    G4double monoSteelRmax    		= 5.*m;
    G4double monoSteelRmin    		= outerSteelR2+1.*cm; //1.*m;
	G4double monoSteelHalfHt  		= 6.25*m; //12.5/2 
	G4double posMonoZ       		= 1.75*m;//6.25+1.75=8m bove; 6.25-1.75=4.5mbelow
	//minumum 1m ht below target is needed for innerSteelD2Ocyl2
	G4double monoLidTopHalfHt 		= 1.*m; 
	
	G4Tubs* monoSteelCyl      		= new G4Tubs ("MonoSteelCyl",monoSteelRmin, 
										monoSteelRmax, monoSteelHalfHt, 0, twopi);

	G4Tubs* monoSteelLidTop   		= new G4Tubs ("monoSteelLidTop",0, monoSteelRmin,
											monoLidTopHalfHt, 0, twopi);
	G4ThreeVector transMonoLidTop(0, 0,  monoSteelHalfHt-monoLidTopHalfHt);
	
	G4UnionSolid* monoSteel2  		= new G4UnionSolid("MonoSteel2",
						monoSteelCyl, monoSteelLidTop, rotCenter, transMonoLidTop);
	
	G4LogicalVolume* monoSteelLog 	= new G4LogicalVolume(monoSteel2, steel, "MonoSteelLog");			
	
	G4ThreeVector  posMonoSteel(0.,0., posMonoZ);// shift Y +0.5m
	monoSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
	new G4PVPlacement( rotCenter,  posMonoSteel, monoSteelLog, "MonolithSteelCyl",
					   fDetLogical, false, 0 , fCheckOverlap); 
	
	
	

	G4Tubs* dummySolid				= new G4Tubs ("DummySolid",0, 1.*mm, 1.*mm, 0, twopi);
	
	
	//MultiRuns Split Geometry
	G4double posMSDetZ						= 0.;
	
	if(multiStepRuns)
	{	
		if(fMSDet2Inner <= fMSDet1Inner) fIsMSDet2 = false;
		if(fMSDet3Inner <= fMSDet2Inner) fIsMSDet3 = false;
		fMSDetCylHht    			= halfHtHDC + basementHalfHt;
		posMSDetZ					= hdcTgtUp - fMSDetCylHht;
		G4ThreeVector posMSDet(0, 0, posMSDetZ);
		G4Tubs* multiStepCyl1  	= new G4Tubs("multiStepCylSolid1",fMSDet1Inner, 
								 fMSDet1Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);

		G4LogicalVolume* multiStepCyl1Log 	= new G4LogicalVolume(multiStepCyl1, 
																  lightMaterial, "multiStepCyl1Log");
		multiStepCyl1Log->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));	
		new G4PVPlacement( rotCenter, posMSDet, multiStepCyl1Log, "MultiStepDet1",
						   fDetLogical, false, 0 , fCheckOverlap); 		
		if(fIsMSDet2)
		{
			
			G4Tubs* multiStepCyl2	= new G4Tubs("multiStepCylSolid2",fMSDet2Inner, 
									 fMSDet2Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);
			G4LogicalVolume* multiStepCyl2Log = new G4LogicalVolume(multiStepCyl2, 
																	lightMaterial, "multiStepCyl2Log");	
			multiStepCyl2Log->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));	
			new G4PVPlacement( rotCenter, posMSDet, multiStepCyl2Log, "MultiStepDet2",
							   fDetLogical, false, 0 , fCheckOverlap); 	
			
		}
		if(fIsMSDet3)
		{
			G4Tubs* multiStepCyl3   = new G4Tubs("multiStepCylSolid3",fMSDet3Inner, 
								fMSDet3Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi); 
			G4LogicalVolume* multiStepCyl3Log = new G4LogicalVolume(multiStepCyl3, 
												lightMaterial, "multiStepCyl3Log");
			multiStepCyl3Log->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));	
			new G4PVPlacement( rotCenter, posMSDet, multiStepCyl3Log, "MultiStepDet3",
							   fDetLogical, false, 0 , fCheckOverlap); 	
			
		}
	}
	
	G4cout << "Done Multi Step Detector(s) " <<  G4endl;
	G4cout << "\t Det1:radius " << fMSDet1Inner/m << G4endl;
	if(fIsMSDet2) G4cout	<< "\t Det2:radius "<< fMSDet2Inner/m << G4endl; 
	if(fIsMSDet3) G4cout	<< "\t Det2:radius "<< fMSDet2Inner/m << G4endl; 



    //BULK SHIELD HDC outer concrete from monolith to 10m
	G4double posHDCZ		= halfHtHDC - basementTop2Tgt;
    G4double rMinHDC    	= monoSteelRmax;// 5.*m;
	G4Tubs* bulkHDCCylFull  = new G4Tubs ("bulkHDCCylFull", rMinHDC, 
									radOutHDC, halfHtHDC,0, twopi);
	G4SubtractionSolid* bulkHDCCylDumSub = new G4SubtractionSolid(
		"bulkHDCCylDumSub", bulkHDCCylFull, dummySolid,  rotCenter, *posCenter);
    G4SubtractionSolid* bulkHDCCyl	= NULL;  //defined in multiStepRuns
    
    G4ThreeVector posMSsubHDC(0, 0, -posHDCZ + posMSDetZ);       
    if(multiStepRuns)
    {
		bulkHDCCyl	= SubMultiStepDet(bulkHDCCylDumSub, "bulkCHDCMSSub",
									  posMSsubHDC, rotCenter);
    }
    else
    {
		bulkHDCCyl   = bulkHDCCylDumSub;
    }  

	G4ThreeVector posBulkShieldHDC(0, 0, posHDCZ);
	
	G4LogicalVolume* bulkHDCInLog = new G4LogicalVolume(bulkHDCCyl,
												concreteHDC, "BulkHDCInLog");
	bulkHDCInLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));
	new G4PVPlacement( rotCenter, posBulkShieldHDC, bulkHDCInLog, "BulkHDCInShield",
					   fDetLogical, false, 0, fCheckOverlap); 
	
	//HDC shield Layer
	G4Tubs* hdcLayerCyl			= new G4Tubs ("HdcLayerCylFull",
										radOutHDC, bulkHdcTotR, halfHtHDC,0, twopi);

	G4LogicalVolume* hdcLayerLog  = new G4LogicalVolume( hdcLayerCyl,
											   concreteHDC, "HdcLayerLog"); 
	hdcLayerLog->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));
	new G4PVPlacement( rotCenter, posBulkShieldHDC, hdcLayerLog, "HdcLayer",
					   fDetLogical, false, 0, fCheckOverlap); 	
	

	//BASEMENT Inner
	G4Tubs*  basementInSolid    = new G4Tubs ("basementInSolid", 0,  
											  baseInRad, basementHalfHt, 0, twopi);
	G4ThreeVector posBaseMonoSub(0, 0, -basementPosZ +posMonoZ); 
	G4SubtractionSolid* basementInSub = new G4SubtractionSolid(
		 "basementInSub", basementInSolid, monoSteel2,  rotCenter, posBaseMonoSub);
	G4SubtractionSolid* basementInSolidDSub = new G4SubtractionSolid(
		"basementInSolidDSub", basementInSub, dummySolid,  rotCenter, *posCenter);
	
	G4ThreeVector baseSubPos (0, 0, -basementPosZ + posMSDetZ);		
	G4SubtractionSolid* basementInMSSub = basementInSolidDSub;	
	if(multiStepRuns)
	{	
		basementInMSSub  = SubMultiStepDet(basementInSolidDSub, 
										 "bulkShieldLayerMSSub", baseSubPos, rotCenter);
	}	
	 
	G4Material* stone = G4NistManager::Instance()->FindOrBuildMaterial("RoadStone");	
	G4String materialType 		 = 										"RoadStone";
	G4LogicalVolume* basementInLog = new G4LogicalVolume(basementInMSSub, stone, "BasementInLog"); 
	
	G4ThreeVector basementPos (0, 0, basementPosZ);//cylinder
	basementInLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
	new G4PVPlacement( rotCenter, basementPos, basementInLog, "BasementIn", 
                                        fDetLogical, false, 0 , fCheckOverlap); 
	
	//TUNNEL WALL 
	G4Tubs*  tunWallCyl          = new G4Tubs ("tunWallDetCyl", baseInRad, 
							baseInRad + tunnelWallThick, basementHalfHt, 0, twopi);
	G4LogicalVolume* tunWallLog  = new G4LogicalVolume(tunWallCyl, concrete, "TunWallLog"); 
	new G4PVPlacement( rotCenter, basementPos, tunWallLog, "TunWall", 
					   fDetLogical, false, 0 ,fCheckOverlap);  

    //Det Tunnel Wall
	G4Tubs*  tunWallDetCyl          = new G4Tubs ("tunWallDetCyl", baseInRad+tunnelWallThick, 
						baseInRad + tunnelWallThick+tunnelDetThick, basementHalfHt, 0, twopi);
    G4LogicalVolume* tunWallDetLog  = new G4LogicalVolume(tunWallDetCyl, concrete, "TunWallDetLog"); 
	new G4PVPlacement( rotCenter, basementPos, tunWallDetLog, "TunWallDet", 
										fDetLogical, false, 0 ,fCheckOverlap);  


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	if(braneBaseDet && basementCsIDet) 
	{
		G4cout << "Detectors: Brane and CsI set. Aborting " << G4endl;
		std::abort();
	}
	G4double maxStepFactor		= 0.5; // x maxStep	
	G4double detCenterDist		= detAxialDist; // 	if(! immersedDet)
	//Cuts
	G4Region* regDet = new G4Region("Det_CsI");	
	
	////
	//DETECTOR, annulus
    G4Tubs*  detWaterCylFull 		= NULL;
    G4ThreeVector* posDetector 		= NULL;
	
	if(basementCsIDet)
	{	
		G4double detCsIRad          = 5.5*cm;
		G4double detPbRad           = detCsIRad +18.*cm;
		G4double detCdRad           = detPbRad + 0.5*mm;
		G4double detWaterRad        = detCdRad + 12.*2.54*cm;
		G4double detCsIHalfHt       = 16.5*cm;//33/2 
		G4double detPbHalfHt        = detCsIHalfHt + 18.*cm;
		G4double detCdHalfHt        = detPbHalfHt + 0.5*mm; 
		G4double detWaterHalfHt	    = detCdHalfHt + 12.*2.54*cm; 

		G4double detCsIRadInner     = detCenterDist - detCsIRad;
		G4double detCsIRadOuter     = detCenterDist + detCsIRad; 
		
		//defined above detToTargetFullZ = 7.5*m; //8.1 - 60cm
		posDetector   = new G4ThreeVector(0,0, -detToTargetFullZ ); //NOTE
		
		
		G4Tubs* detCsICyl = new G4Tubs ("DetCsICyl", detCsIRadInner, 
										detCsIRadOuter, detCsIHalfHt,0., twopi);
		G4Material* csI     = G4NistManager::Instance()->FindOrBuildMaterial("CsI"); //d=4.5g/cc
		G4Material* lead    = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
		G4Material* cadmium = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cd");
		//G4Material* poly  = G4NistManager::Instance()->FindOrBuildMaterial("HDPE");
		G4LogicalVolume* detCsILog = new G4LogicalVolume(detCsICyl, csI, "DetCsILog");
		detCsILog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta())); 
		new G4PVPlacement( rotCenter, *posDetector, detCsILog, "SNSDetectorCsI", 
						   fDetLogical, false, 0 , fCheckOverlap);
		
		G4double detPbRadInner      = detCenterDist - detPbRad;
		G4double detPbRadOuter      = detCenterDist + detPbRad; 
		G4Tubs* detPbCylFull        = new G4Tubs ("DetPbCylFull", detPbRadInner, 
												  detPbRadOuter, detPbHalfHt,0., twopi);
		G4SubtractionSolid* detPbcyl = new G4SubtractionSolid("detPbcyl", 
															  detPbCylFull, detCsICyl, rotCenter, *posCenter);
		G4LogicalVolume* detPbLog = new G4LogicalVolume(detPbcyl, lead, "DetPbLog");
		detPbLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));    
		new G4PVPlacement( rotCenter, *posDetector, detPbLog, "DetLeadShield", 
						   fDetLogical, false, 0 , fCheckOverlap); 

		G4double detCdInner         = detCenterDist - detCdRad;
		G4double detCdOuter         = detCenterDist + detCdRad; 
		G4Tubs* detCdCylFull        = new G4Tubs ("DetCdCylFull", detCdInner, 
												  detCdOuter, detCdHalfHt, 0., twopi);
		G4SubtractionSolid* detCdCyl= new G4SubtractionSolid("detCdCyl", 
															 detCdCylFull, detPbCylFull, rotCenter, *posCenter);
		G4LogicalVolume* detCdLog 	= new G4LogicalVolume(detCdCyl, cadmium, "DetCdLog");
		detCdLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey())); 
		new G4PVPlacement( rotCenter, *posDetector, detCdLog, "DetCdShield", 
						   fDetLogical, false, 0 , fCheckOverlap);
		
		G4double detWaterInner      = detCenterDist - detWaterRad;
		G4double detWaterOuter      = detCenterDist + detWaterRad; 
		detWaterCylFull    			= new G4Tubs ("DetPbCylFull", detWaterInner, 
										 detWaterOuter, detWaterHalfHt, 0., twopi);
		G4SubtractionSolid* detWaterCyl = new G4SubtractionSolid("detWaterCyl", 
								detWaterCylFull, detCdCylFull, rotCenter, *posCenter);
		
		G4LogicalVolume* detWaterLog = new G4LogicalVolume(detWaterCyl, water, "DetWaterLog");
		detWaterLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red())); 
		new G4PVPlacement( rotCenter, *posDetector, detWaterLog, "DetWaterShield", 
						   fDetLogical, false, 0 , fCheckOverlap);
		
	
		//Cut
		if(regDet)
		{
			if(detCsILog)	regDet->AddRootLogicalVolume(detCsILog);
			if(detPbLog)	regDet->AddRootLogicalVolume(detPbLog);
			if(detCdLog)	regDet->AddRootLogicalVolume(detCdLog);
			if(detWaterLog)	regDet->AddRootLogicalVolume(detWaterLog);
			////(detPolyLog)regDet->AddRootLogicalVolume(detPolyLog);
		}
		
		//ForceStep size		
		G4double maxStep		= detCsIRad;
		if(detCsIHalfHt < detCsIRad) 
			maxStep = detCsIHalfHt; 	
		G4UserLimits* stepLimit = new G4UserLimits(maxStep*maxStepFactor);
		if(detCsILog)
			detCsILog->SetUserLimits(stepLimit);
	}

	G4Tubs* braneDetScintCyl	= NULL;
	
	if(braneBaseDet)
	{
		G4double detScintRad	= 2.5*2.54*cm; //5inch/2
		G4double detScintHht	= 2.5*2.54*cm; //5inch/2	

		G4double detScintRadInner   = detCenterDist - detScintRad;
		G4double detScintRadOuter   = detCenterDist + detScintRad; 
		
		posDetector   			= new G4ThreeVector(0,0, -detToTargetFullZ ); //NOTE
		braneDetScintCyl		= new G4Tubs ("braneDetScintCyl", detScintRadInner, 
											detScintRadOuter, detScintHht,0., twopi);
		G4Material* matc7h8 	= G4NistManager::Instance()->FindOrBuildMaterial("MatC7H8");
		
		G4LogicalVolume* detScintLog = new G4LogicalVolume(braneDetScintCyl, 
													matc7h8, "BraneDetScintLog");
		detScintLog->SetVisAttributes(new G4VisAttributes (G4Colour::Green())); 
		new G4PVPlacement( rotCenter, *posDetector, detScintLog, "BraneDetScintC7H8", 
							fDetLogical, false, 0 , fCheckOverlap);
		
		G4cout  << "Placed BraneDaseDet of thickness " <<  2.*detScintRad/cm 
				<< " around around " << detCenterDist/m << G4endl;
		
		//add to region, for production cut
		if(regDet && detScintLog)regDet->AddRootLogicalVolume(detScintLog);
		
		//ForceStep size for charged particles only		
		G4double maxStep				= detScintRad;
		if(detScintHht < detScintRad) 
						maxStep 		= detScintHht; 	
		G4UserLimits* stepLimit 		= new G4UserLimits(maxStep*maxStepFactor);
		if(detScintLog)
			detScintLog->SetUserLimits(stepLimit);
		
	}//braneDet


	//Production Cut
	G4ProductionCuts* cutDet = new G4ProductionCuts();
	cutDet->SetProductionCut(1.0*um);
	regDet->SetProductionCuts(cutDet);
				

    //cuts. NOTE: don't define region without setting default cut/or in macro
    //parallel world has cut of the world by default. Don't set it, otherwise it will
     // be taken for the default.
    //G4RunManagerKernel::GetRunManagerKernel()->SetVerboseLevel(2);
    G4Region* regBase = new G4Region("BasementIn");
	if(basementInLog)regBase->AddRootLogicalVolume(basementInLog);
	if(bulkHDCInLog)regBase->AddRootLogicalVolume(bulkHDCInLog);
	if(monoSteelLog)regBase->AddRootLogicalVolume(monoSteelLog);
    if(innerUpperSteelD2OLog)regBase->AddRootLogicalVolume(innerUpperSteelD2OLog);
    if(innerBotSteelD2OLog)regBase->AddRootLogicalVolume(innerBotSteelD2OLog);
    if(outerSteelLog)regBase->AddRootLogicalVolume(outerSteelLog);
    if(monoSteelLog)regBase->AddRootLogicalVolume(monoSteelLog);


    G4ProductionCuts* cutBase = new G4ProductionCuts();
    cutBase->SetProductionCut(10.*mm);
    regBase->SetProductionCuts(cutBase);


    G4Region* regBaseLayer = new G4Region("BasementLayer");
    if(tunWallLog)regBaseLayer->AddRootLogicalVolume(tunWallLog);
    G4ProductionCuts* cutBase2 = new G4ProductionCuts();
    cutBase2->SetProductionCut(1.*mm);
    regBaseLayer->SetProductionCuts(cutBase2);

} 	//END OF DETECTOR construction
