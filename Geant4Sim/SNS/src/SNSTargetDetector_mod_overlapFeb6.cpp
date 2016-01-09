// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


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
#include "SNSTargetConcreteRoom.hh"
#include "SNSTargetSteelCylinder.hh"
#include "SNSTargetOuterProtonBox.hh"
#include <sstream>
#include <vector>
#include <cmath>  
#include <cassert>
#include "G4RunManager.hh"

SNSTargetDetector::SNSTargetDetector(G4String detName )
:VBaseDetector(detName), fMSDet1Inner(0.), fMSDet2Inner(0.)
,fMSDet3Inner(0.), fMSDetCylThick(0.), fMSDetCylHht(0.)
,fIsMSDet2(true), fIsMSDet3(true)
{
    G4cout << "constructed SNSTargetDetector" <<G4endl;
}

SNSTargetDetector::~SNSTargetDetector()
{
    //delete fMessenger;
}

G4SubtractionSolid* SNSTargetDetector::SubMultiStepDet(G4SubtractionSolid* fromSolid, 
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


void SNSTargetDetector::CreateInnerNTube( G4double nTubeHalfDx1, 
		G4double nTubeHalfDx2, G4double nTubeHalfDy1, G4double nTubeHalfDy2,
		G4double nTubeHalfDz, G4double thickNTubeAlX, G4double thickNTubeAlY,
		G4RotationMatrix* rotNTube, G4ThreeVector posNTube, G4String tubeName,
		G4double outerSteelR2, G4double outerSteelHht, G4bool overlap)
{
	
	G4RotationMatrix* rotCenter = new G4RotationMatrix();
	G4ThreeVector* posCenter = new G4ThreeVector(0,0,0);
	G4Trd* nTubeOut     = new G4Trd("nTubeOut", nTubeHalfDx1, nTubeHalfDx2, 
									nTubeHalfDy1, nTubeHalfDy2, nTubeHalfDz);
	G4Trd* nTubeIn      = new G4Trd("nTubeIn", nTubeHalfDx1-thickNTubeAlX,  
						nTubeHalfDx2-thickNTubeAlX, nTubeHalfDy1-thickNTubeAlY, 
						nTubeHalfDy2-thickNTubeAlY, nTubeHalfDz);
	
	G4Tubs* outerSteelDummy = new G4Tubs ("outerSteelDummy", 0, 
										  outerSteelR2, outerSteelHht, 0, twopi);
	G4IntersectionSolid* nTubeXouterSteel1 = new G4IntersectionSolid(
		tubeName+"XouterSteel1", outerSteelDummy, nTubeOut, rotNTube, posNTube);
	G4IntersectionSolid* nTubeXouterSteel2 = new G4IntersectionSolid(
		tubeName+"XouterSteel2", outerSteelDummy, nTubeIn, rotNTube, posNTube);
	G4SubtractionSolid* nTubeSub = new G4SubtractionSolid(tubeName+"OutSub", 
				nTubeXouterSteel1, nTubeXouterSteel2, rotCenter, *posCenter);										  
	G4Material* Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"); 
	G4LogicalVolume* nTubeLog  = new G4LogicalVolume(nTubeSub, Al, tubeName + "Log");
	new G4PVPlacement( rotCenter, *posCenter, nTubeLog, tubeName,
					   fDetLogical, false, 0, overlap);  

}



std::vector <G4double> SNSTargetDetector::GetPosNOuter( G4int i, 
	G4double dT, G4double lH3posX, G4double lH3posY, G4double lH1halfX, 
	G4double nBeamOuterZhLen, G4double lH1posY, G4double rot, G4double radC)
{
	//y-y1 / x-x1  = tan(theta) => y=y1+(x-x1)*tan(a) . x^2+y^2=r^2; a.x^2+b.x+c=0
	//t=tan(theta); a= 1+t*t; b=2*t*(y1-x1*t);  
	//c= y1*y1+x1*x1*t*t-2*x1*y1*t-radC*radC;  x=-b+-sqrt(b*b-4*a*c)/2*a
	G4double dDelta = 0.00001;
	G4double posNTubeOuterX[] = {lH3posX +(lH1halfX+nBeamOuterZhLen)*std::cos(rot+dT),
		lH3posX - (lH1halfX+nBeamOuterZhLen)*std::cos(rot+dT), 
		(lH1halfX +nBeamOuterZhLen)*std::cos(dT),
		-lH3posX +(lH1halfX+nBeamOuterZhLen)*std::sin(90.*degree -rot+dT),
		-lH3posX -(lH1halfX+nBeamOuterZhLen)*std::sin(90.*degree -rot+dT),
		-(lH1halfX +nBeamOuterZhLen)*std::cos(dT)};

	G4double posNTubeOuterY[] = {lH3posY-(lH1halfX +nBeamOuterZhLen)*std::sin(rot+dT),
		lH3posY+(lH1halfX +nBeamOuterZhLen)*std::sin(rot+dT), 
		lH1posY- (lH1halfX +nBeamOuterZhLen)*std::sin(dT),
		lH3posY+ (lH1halfX +nBeamOuterZhLen)*std::cos(90.*degree -rot+dT),
		lH3posY- (lH1halfX +nBeamOuterZhLen)*std::sin(rot-dT),
		lH1posY+ (lH1halfX +nBeamOuterZhLen)*std::sin(dT)};

	G4int multx[] = {1,-1,1,1,1,1};
	G4int multy[] = {1,1,1,1,1,1};
	G4double tth = std::tan(rot); 
	G4double aa = 1. + tth * tth; 
	G4double bb = 2. * tth * (lH3posY - lH3posX * tth );  
	G4double cc = lH3posY*lH3posY+lH3posX*lH3posX*tth*tth - 2.*lH3posX*lH3posY*tth-radC*radC;  
	G4double xx = 0;
	if(std::abs(aa) > dDelta ) 
		xx = -bb +multx[i]*std::sqrt(bb*bb - 4.*aa*cc)/(2.*aa); //-bb-
	G4double yy = lH3posY + multy[i]*(xx - lH3posX) * tth;
	
	G4double fact = 0;
	if(std::abs(dT) > dDelta) fact =  dT/std::abs(dT)*15.*cm;	

	G4int ffx[] = {1,-1,0,-1,1,0};
	G4int ffy[] = {1,-1,1,1,-1,-1};
	G4int ff2x[] = {1,1,1,1,-1,1};	
	G4int ff2y[] = {1,1,0,-1,1,0};	
	std::vector <G4double> posxy(2);
	if( i==2) { xx = radC; }
	if( i==5) { xx = -radC; }
	posxy[0] = posNTubeOuterX[i] + ff2x[i]*xx - ffx[i]*fact*sin(rot) ;
	posxy[1] = posNTubeOuterY[i] - ff2y[i]*yy - ffy[i]*fact*cos(rot) ;
	
	return posxy;
}


void SNSTargetDetector::ConstructDet()
{
	
	G4bool fCheckOverlap 	= true;
	G4double nShieldXY 		= 1.*m;
	
G4bool multiStepRuns 	= false; //MSDETRUN

	G4bool braneBaseDet		= true; //BASEBRANEDET
	
	fIsMSDet2 				= true; //ISMSDET2
	fIsMSDet3 				= false;	//ISMSDET3
	fMSDet1Inner     		= 8.0*m;	//MSDET1INRAD
	fMSDet2Inner     		= 12.0*m;	//MSDET2INRAD
	fMSDet3Inner    		= 16.0*m;   // MSDET3INRAD
	
	G4bool basementCsIDet 	= false; //BASECSIDET
	G4bool testDet1     	= false; //no SNS : 
G4bool braneTest     	= false;  //no SNS : braneBaseDet is set below
	
	G4bool placeProtonBox 				= true; //PROTONBOX
	G4bool placeNTubeInner 				= true; //NTUBEINNER
	G4bool nOuterUnionShields			= true; //NSHIELD
	G4bool placeNTubeOuter				= true; //NTUBEOUTER
	G4bool basementHallRoof 			= true;	
	G4bool placeSurfaceHdcInnerShield	= true;
	G4bool placeSurfaceHdcLayer 		= true;
G4bool immersedDet   				= false;
G4bool hdcOuterMat 					= false; //HDCOUTERMAT  //vis problem	
	
    fMSDetCylThick                      = 1.*mm;
    G4double baseLayerDetHalf           = fMSDetCylThick;


	
	
	G4bool placeInner = true;  // false is for vis testing

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
	
    if(testDet1)
    {
        G4double r1     =   0.*cm;
		G4double r2     =   3.*m;
        G4double r3     =   5.*m;
		G4double r4     =   6.*m;	
        G4Material* stone = G4NistManager::Instance()->FindOrBuildMaterial("RoadStone");
		//G4Material* testConcrete = G4NistManager::Instance()->FindOrBuildMaterial("TestConcrete");
        G4Sphere* testSph1  = new G4Sphere ("testSph1", r1, r2, 0, twopi, 0, pi);
		G4LogicalVolume* testSph1Log = new G4LogicalVolume(testSph1,stone, "TestSph1Log");
		testSph1Log->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
		new G4PVPlacement( rotCenter, *posCenter, testSph1Log, "VolOne",
                       fDetLogical, false, 0 , fCheckOverlap);
        //r2=0.;

		G4Sphere* testSph2  = new G4Sphere ("testSph2", r2, r3, 0, twopi, 0, pi);
		G4LogicalVolume* testSph2Log = new G4LogicalVolume(testSph2,stone, "TestSph2Log");
		testSph2Log->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));
		new G4PVPlacement( rotCenter, *posCenter, testSph2Log, "VolTwo",
						   fDetLogical, false, 0 , fCheckOverlap);
		
		
		G4Sphere* testSph3  = new G4Sphere ("testSph3", r3, r4, 0, twopi, 0, pi);
		G4LogicalVolume* testSph3Log = new G4LogicalVolume(testSph3,stone, "TestSph3Log");
		testSph3Log->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
		new G4PVPlacement( rotCenter, *posCenter, testSph3Log, "VolThree",
						   fDetLogical, false, 0 , fCheckOverlap);
		
		/* G4double maxStep3 = 2.*cm;// 0.5*5.5*m/65 = 4.2cm; //stepHt/2
		G4UserLimits* stepLimit3 = new G4UserLimits(maxStep3);
		if(testSph1Log)testSph1Log->SetUserLimits(stepLimit3);
		if(testSph2Log)testSph2Log->SetUserLimits(stepLimit3);
		if(testSph3Log)testSph3Log->SetUserLimits(stepLimit3);		
		*/
		return;
    }


 
    //TARGET HG box   //red
    G4double HgHalfHt = 4.9*cm; //9.8/2
    G4double HgHalfX = 21.*cm; //42/2
    G4double HgHalfY = 20.3*cm; 
    G4Box* HgBox = new G4Box("HgBox", HgHalfX, HgHalfY, HgHalfHt); //HALF
    G4Material* Hg = G4NistManager::Instance()->FindOrBuildMaterial("G4_Hg");   
    G4LogicalVolume* HgTargetBoxLog = new G4LogicalVolume(HgBox, Hg, "HgTargetBoxLog");
    HgTargetBoxLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
	if(placeInner) new G4PVPlacement( rotCenter, *posCenter, HgTargetBoxLog, "HgTarget",
                       fDetLogical, false, 0 , fCheckOverlap);
    // beam profile footprint on target is 7x20 cm2, covering 90% of the beam, 
    //which means 2 sigma on each direction. SET it in macro

    /*if(braneTest)
    {
        return;
    } */

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
	if(placeInner) new G4PVPlacement( rotCenter, *posCenter, HgSteelBoxLog, "HgSteelBox",
                       fDetLogical, false, 0 , fCheckOverlap);



    //INNER REFLECTOR PLUG  to 49.5 cm rad, ht =  0 to 5.6+5 both ways
    G4double IRPRad 		=  49.5*cm;
	G4double innRadIRP      =  47.625*cm;
	G4double IRPhalfHt 		=  11.2*cm;//To be modified to Hht=52cm //7.6+ 2*1.8 = 11.2;
	G4Tubs* IRPFull  = new G4Tubs ("innerRefPlug", innRadIRP, IRPRad, IRPhalfHt, 0, twopi);
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
	if(placeInner) new G4PVPlacement( rotLH1, posLH1, liquidHLog1, "LiquidHydr1",
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
	if(placeInner) new G4PVPlacement( rotLH2, posLH2, liquidHLog2, "LiquidHydr2",
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
	if(placeInner) new G4PVPlacement( rotLH3, posLH3, liquidHLog3, "LiquidHydr3",
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
	if(placeInner) new G4PVPlacement( rotLH4, posWatermod, waterModLog, "WaterModerator",
                       fDetLogical, false, 0 , fCheckOverlap);   

    if(braneTest)
    {

		G4Tubs* outerSteelCyl 	= new G4Tubs ("outerSteelCyl", IRPRad, 95.*cm,  102.*cm, 0, twopi);
		G4Material* Steel 		= G4NistManager::Instance()->FindOrBuildMaterial("Steel");   

		G4LogicalVolume* outerSteelLog = new G4LogicalVolume(outerSteelCyl, Steel, "OuterSteelLog");
		outerSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Cyan()));	
		new G4PVPlacement( rotCenter, *posCenter, outerSteelLog, "OuterSteelCylinder",
						   fDetLogical, false, 0 , fCheckOverlap);  
        return;
    }


	G4SubtractionSolid* IRPsolidSub4 = new G4SubtractionSolid("IRPsolidSub4", 
										IRPsolidSub3, lHBox, rotLH4, posWatermod);
	G4ThreeVector posLH4_BeBot(-lH3posX, lH3posY, -lH1posZ-botBePlugZ);
    G4SubtractionSolid* BePlugBotSub2 = new G4SubtractionSolid("BePlugBotsub2", 
									BePlugBotSub1, lHBox, rotLH4, posLH4_BeBot);			

	//OUTER STEEL CYLINDER from 49 to 95cm. ht =219cm
	G4double outerSteelR1   		=  IRPRad;
	G4double outerSteelR2   		=  95.*cm; 
	G4double outerSteelHht   		=  109.5*cm; //219/2
	G4Tubs* outerSteelCyl 	= new G4Tubs ("outerSteelCyl", outerSteelR1, 
									outerSteelR2, outerSteelHht, 0, twopi);
	G4Material* Steel 		= G4NistManager::Instance()->FindOrBuildMaterial("Steel");   
	//Proton cut
	G4Box* cutPBox1 		= new G4Box("cutPBox", HgSteelhalfX, outerSteelR2, HgSteelhalfZ);
	G4ThreeVector posCutPBox1(0,-outerSteelR1-(outerSteelR2 -outerSteelR1)/2., 0);
	G4SubtractionSolid* outerSteelCylSub1 = new G4SubtractionSolid("outerSteelCylSub1",
										outerSteelCyl, 	cutPBox1, rotCenter, posCutPBox1);
	//placing later


//SETTINGS 
    G4double tunnelWall				= 18.*inch;
	// x=18.3, y=-15.2, z= -7.1m (-8.1 + 1.m). 19.6m    // old 22.3*m . 18.3*m for c5 
G4double detToTargetFullZ       	= 7.1*m;//8.1m-1m //old=7.8
G4double baseRadiusAll          	= 20.*m ;// 18.3*m for c5
G4double nBeamLineFullLength		= 30.*m;
    G4double detToWallGap       	= 1.5*m;
    G4double protonBoxOuterHalfXZ 	= 2.0*m;//3.52*m; 
    G4double instrFloorTop2Tgt  	= protonBoxOuterHalfXZ;//2.*m;
    G4double basementRad        	= baseRadiusAll - tunnelWall;//18.3 - 18"
    G4double instrFloorHalfHt   	= 9.*inch;//18"/2   
    G4double detAxialDist      		= baseRadiusAll+detToWallGap;
    G4double basementTop2Tgt   		= instrFloorTop2Tgt + 2.*instrFloorHalfHt;
    G4double basementHalfHt    		= (detToTargetFullZ - basementTop2Tgt+3.*m)/2.;  //8.3m/2 
    G4double basementPosZ     		= - basementHalfHt - basementTop2Tgt; 
G4double surfaceLayerWidth      	= 0.1*m;
    G4double bulkHdcRmax        	= 10.*m; 
    G4double hdcRaise           	= 1.5*m;
    G4double surfaceHDCInnnerMax	= bulkHdcRmax - surfaceLayerWidth;
    G4double bulkConcShieldHalfHt	= (instrFloorTop2Tgt+ hdcRaise)/2.;//1.5m above 

    G4Tubs* dummySolid				= new G4Tubs ("DummySolid",0, 1.*mm, 1.*mm, 0, twopi);


    //MultiRuns Split Geometry
	G4SubtractionSolid* multiStepCyl1Sub  	= NULL;
	G4SubtractionSolid* multiStepCyl2Sub  	= NULL;
	G4SubtractionSolid* multiStepCyl3Sub  	= NULL;
	G4double posMSDetZ						= 0.;
	
	if(multiStepRuns)
	{
		
		//fIsMSDet2 		= true;
		//fIsMSDet3 		= false;		
		//fMSDet1Inner     	= 7.*m;	//default
		//fMSDet2Inner     	= 9.8*m;	
		//fMSDet3Inner    	= 15.*m;
		
		if(fMSDet2Inner <= fMSDet1Inner) fIsMSDet2 = false;
		if(fMSDet3Inner <= fMSDet2Inner) fIsMSDet3 = false;
		fMSDetCylHht    			= bulkConcShieldHalfHt + basementHalfHt;
		posMSDetZ					= hdcRaise - fMSDetCylHht;

		G4Tubs* multiStepCyl1  		= new G4Tubs("multiStepCylSolid1",fMSDet1Inner, 
			fMSDet1Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);
		
		multiStepCyl1Sub 			= new G4SubtractionSolid("msNSub1",
								multiStepCyl1, dummySolid,rotCenter, *posCenter);
		
		if(fIsMSDet2)
		{
			G4Tubs* multiStepCyl2	= new G4Tubs("multiStepCylSolid2",fMSDet2Inner, 
				fMSDet2Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);
			multiStepCyl2Sub 		= new G4SubtractionSolid("msNSub2",
								multiStepCyl2, dummySolid,rotCenter, *posCenter);
		}
		if(fIsMSDet3)
		{
			G4Tubs* multiStepCyl3   = new G4Tubs("multiStepCylSolid3",fMSDet3Inner, 
				fMSDet3Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi); 
			multiStepCyl3Sub 		= new G4SubtractionSolid("msNSub3",
								multiStepCyl3, dummySolid,rotCenter, *posCenter);
		}
	}

	//INSTR. FLOOR
	G4double	instrFloorPosZ 		= -instrFloorTop2Tgt -instrFloorHalfHt;
	G4Tubs* instrFloorSolid     	= new G4Tubs ("instrFloorCyl", 0, 
												  baseRadiusAll, instrFloorHalfHt, 0, twopi);
	G4SubtractionSolid* instrFloorDumSub = new G4SubtractionSolid("instrFlCyl",
									instrFloorSolid, dummySolid,  rotCenter, *posCenter);
	G4SubtractionSolid* instrFloorSub = NULL;
		
	if(multiStepRuns)	
	{
		G4ThreeVector instrFlPos(0,0, -instrFloorPosZ + posMSDetZ );		
		instrFloorSub = SubMultiStepDet(instrFloorDumSub, "instrFloorCyl", instrFlPos, rotCenter);
	}	
	else	
		instrFloorSub = instrFloorDumSub;

	//MONOLITH STEEL cylinder
    G4double monoSteelRmax    = 5.*m;
    G4double monoSteelRmin    = outerSteelR2+1.*cm; //1.*m;
	G4double monoSteelHalfHt  = 6.25*m; //12.5/2 
	G4double protonBeamCutXZ  = 0.3*m; //42/2 cm is target in transverse 	
	G4double transMonoZ       = 1.75*m;//6.25+1.75=8m bove; 6.25-1.75=4.5mbelow
	//minumum 1m ht below target is needed for innerSteelD2Ocyl2
	G4double monoLidTopHalfHt = 0.5*m; // full ht=1m 
	G4double monoLidBotHalfHt = 6.*inch; // 1 foot/2 
	G4Tubs* monoSteelCyl      = new G4Tubs ("MonoSteelCyl",monoSteelRmin, 
										monoSteelRmax, monoSteelHalfHt, 0, twopi);
	G4SubtractionSolid* monoSteelCylDumSub = new G4SubtractionSolid("monoSteelCylDS",
										monoSteelCyl, dummySolid,  rotCenter, *posCenter);
	G4SubtractionSolid* monoSteelCyl1 = NULL;
	

	if(multiStepRuns)	
	{	
		G4ThreeVector  transMSteel(0.,0., -transMonoZ + posMSDetZ);		
		monoSteelCyl1 = SubMultiStepDet(monoSteelCylDumSub, "MonoSteelMSSub",  
										transMSteel, rotCenter);
	}	
	else	
		monoSteelCyl1 = monoSteelCylDumSub;
	
   
	

    //BULK SHIELD HDC outer concrete from monolith to 10m
	G4double posBulkShieldHDCZ		= bulkConcShieldHalfHt-instrFloorTop2Tgt;//protonBoxOuterHalfXZ;
    G4double bulkConcShieldRmin    	= monoSteelRmax;// 5.*m;
    G4Tubs* bulkHDCCylFull  	= new G4Tubs ("bulkHDCCyl",
        bulkConcShieldRmin, surfaceHDCInnnerMax, bulkConcShieldHalfHt,0, twopi);
	G4SubtractionSolid* bulkHDCCylDumSub = new G4SubtractionSolid(
		"bulkHDCCylDumSub", bulkHDCCylFull, dummySolid,  rotCenter, *posCenter);
    G4SubtractionSolid* bulkHDCCyl	= NULL;  //defined in multiStepRuns
    
    G4ThreeVector posBulkSHDC(0, 0, -posBulkShieldHDCZ + posMSDetZ);       
    if(multiStepRuns)
    {
		bulkHDCCyl	= SubMultiStepDet(bulkHDCCylDumSub, "bulkConcShieldMSSub",
						posBulkSHDC, rotCenter);
    }
    else
    {
		bulkHDCCyl   = bulkHDCCylDumSub;
    }   //placing later


    //HDC shield Layer
    G4Tubs* bulkShieldLayerCyl			= new G4Tubs ("BulkHdcLayerCylFull",
					surfaceHDCInnnerMax, bulkHdcRmax, bulkConcShieldHalfHt,0, twopi);
	G4SubtractionSolid* bulkShieldLayerDSub = new G4SubtractionSolid(
		"bulkShieldLayerMSSub", bulkShieldLayerCyl, dummySolid,  rotCenter, *posCenter);
    G4SubtractionSolid* bulkHdcLayerCyl = NULL;
    if(multiStepRuns)
    {
		bulkHdcLayerCyl     = SubMultiStepDet(bulkShieldLayerDSub, "bulkShieldLayerMSSub",
						posBulkSHDC, rotCenter);
    }
    else
    {
		bulkHdcLayerCyl		= bulkShieldLayerDSub;
    }


	//HDC out , extension
G4double detInPadThick		    = 7.*m; //for immersed detector extra light concrete  material or
	//18.3 + 1.08 = 19.4=> det hall= 18 to 20. + 5m (= nPar* drad). total 25m. 25-18.3=6.7m
	G4double hdcOutRadMax 		= baseRadiusAll + detInPadThick;// basementRad + tunnelWall; //18.3*m;
	//if(immersedDet && ! boxDet) hdcOutRadMax += thickNDet + padThick;
	G4double hdcOutHht 			= bulkConcShieldHalfHt;// + instrFloorHalfHt; // 1.75*m;//3.5/2
	G4double hdcOutLightPosZ	= posBulkShieldHDCZ;//hdcRaise -hdcOutHht;
	G4Tubs* hdcOutLight 		= new G4Tubs("hdcOutLight", bulkHdcRmax, hdcOutRadMax, hdcOutHht, 0, twopi);
	G4SubtractionSolid* hdcOutLightDSub = new G4SubtractionSolid(
			"hdcOutLightDSub", hdcOutLight, dummySolid,  rotCenter, *posCenter);
    G4SubtractionSolid* hdcOutLightShield       = NULL;

    if(multiStepRuns)
    {
		G4ThreeVector posOutHDCForSub( 0, 0, -hdcOutLightPosZ +posMSDetZ);
		hdcOutLightShield		= SubMultiStepDet(hdcOutLightDSub, "hdcOutLightDSub",
						posOutHDCForSub, rotCenter); 
    }
    else
    {
		hdcOutLightShield 		= hdcOutLightDSub;							   
	}

	//NEUTRON TUBES
    //Inner
    G4SubtractionSolid* outerSteelCylSub   		= NULL;
    G4SubtractionSolid* BePlugInnerNTubeTopSub  = NULL;
    G4SubtractionSolid* BePlugInnerNTubeBotSub  = NULL;

    G4String tubeNames[]	 = {"nInnerTube1", "nInnerTube2","nInnerTube3","nInnerTube4",
                                "nInnerTube5","nInnerTube6"};
    G4double nTubeHalfDx1[]	 = {21.59*cm, 31.38*cm, 21.59*cm,21.59*cm,21.59*cm,21.59*cm};
    G4double nTubeHalfDx2[]	 = {6.91*cm, 6.91*cm, 6.91*cm,6.91*cm,6.91*cm,6.91*cm};
    G4double nTubeHalfDy1[]	 = {7.235*cm, 7.235*cm, 8.035*cm,7.235*cm,7.235*cm,8.035*cm};
    G4double nTubeHalfDy2[]	 = {7.235*cm, 7.235*cm, 8.035*cm,7.235*cm,7.235*cm,8.035*cm};
	G4double nTubeInHZextra  = 8.*cm;
	G4double nTubeHalfDz 	 = outerSteelR2/2.+ nTubeInHZextra; //extra 		
    G4double thickNTubeAlX[] = {1.31*cm, 1.454*cm, 1.62*cm,1.454*cm,1.454*cm,1.62*cm};
    G4double thickNTubeAlY[] = {0.635*cm, 0.869*cm, 0.635*cm,0.779*cm,0.779*cm,0.635*cm};
    G4double rotZ[] = {lH1Rot - 90.*degree, lH1Rot - 90.*degree, -90.*degree,
                            90.*degree - lH1Rot, 90.*degree - lH1Rot, 90.*degree};
    //1=top -x,-y extends to -x+y; 2=top -x, -y extends to +x-y; 3; top to +x
    //4=bot +x-y extends to 
    G4double rotX[] = {90.*degree, -90.*degree, 90.*degree, -90.*degree,
                            90.*degree, 90.*degree};
    G4double posNTubeX[] = {lH3posX + (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot),
            lH3posX - (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot), lH1halfX +nTubeHalfDz,
            -lH3posX +(lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot),
            -lH3posX -(lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot),
            -lH1halfX -nTubeHalfDz};
    G4double posNTubeY[] = {lH3posY-(lH1halfX +nTubeHalfDz)*std::sin(lH1Rot),
            lH3posY+(lH1halfX +nTubeHalfDz)*std::sin(lH1Rot), lH1posY,
            lH3posY+(lH1halfX +nTubeHalfDz)*std::cos(90.*degree -lH1Rot),
            lH3posY-(lH1halfX +nTubeHalfDz)*std::cos(90.*degree -lH1Rot),lH1posY};


    G4SubtractionSolid* monoSteelCylNInnerSub = NULL;

     
    if(placeNTubeInner)
    {
        G4double posNTubeZ[6] = {0.,0.,0.,0.,0.,0.};
        G4SubtractionSolid* BePlugInnerNTubeTopSubIn = BePlugTopSub2;
        G4SubtractionSolid* BePlugInnerNTubeBotSubIn = BePlugBotSub2;
        G4SubtractionSolid* outerSteelCylSubIn       = outerSteelCylSub1;
        G4SubtractionSolid* monoSteelSubNInnerIn     = monoSteelCyl1;

        G4int i = 0;
        for (i=0; i<6; i++)
        {
            posNTubeZ[i]					= IRPhalfHt + nTubeHalfDy1[i]; //rotated y->z	
            if(i>2) posNTubeZ[i]			= - posNTubeZ[i];
            G4RotationMatrix* rotNTube 		= new G4RotationMatrix; 
            rotNTube->rotateZ(rotZ[i]);
            rotNTube->rotateX(rotX[i]);

            G4ThreeVector posNTube(posNTubeX[i], posNTubeY[i], posNTubeZ[i]);
            //CreateInnerNTube(	nTubeHalfDx1[i], nTubeHalfDx2[i], nTubeHalfDy1[i],
            //        nTubeHalfDy2[i], nTubeHalfDz, thickNTubeAlX[i], thickNTubeAlY[i],
            //        rotNTube, posNTube, tubeNames[i], outerSteelR2, outerSteelHht,
            //        fCheckOverlap );
    
            G4Trd* nInnerTubeOut     = new G4Trd("nInnerTubeOut", nTubeHalfDx1[i], 
                nTubeHalfDx2[i], nTubeHalfDy1[i], nTubeHalfDy2[i], nTubeHalfDz);
            G4Trd* nInnerTubeIn      = new G4Trd("nInnerTubeIn", nTubeHalfDx1[i]  
                -thickNTubeAlX[i], nTubeHalfDx2[i]-thickNTubeAlX[i],
                nTubeHalfDy1[i] -thickNTubeAlY[i], nTubeHalfDy2[i]-thickNTubeAlY[i], nTubeHalfDz+5.*cm);


            G4SubtractionSolid* nTubeOutSub = new G4SubtractionSolid(tubeNames[i]+"OutSub", 
                        nInnerTubeOut, nInnerTubeIn, rotCenter, *posCenter);
			
			//if(multiStepRuns). TODO if det is affecting inner N Tubes			
                                     
			G4LogicalVolume* nTubeLog  = new G4LogicalVolume(nTubeOutSub, Al, tubeNames[i]+"Log");
            new G4PVPlacement( rotNTube, posNTube, nTubeLog, tubeNames[i],
                       fDetLogical, false, 0, fCheckOverlap);  


            G4double bePlugZ    	= 0;
            if(i<3) bePlugZ     	= topBePlugZ;
            if(i>2) bePlugZ     	= botBePlugZ;
            
            
            G4ThreeVector posNTubeBePlug(posNTubeX[i], posNTubeY[i], posNTubeZ[i] - bePlugZ);
            if(i<3)
            {
                BePlugInnerNTubeTopSub = new G4SubtractionSolid("BePlugTopSub", 
                            BePlugInnerNTubeTopSubIn, nInnerTubeOut, rotNTube, posNTubeBePlug);
                BePlugInnerNTubeTopSubIn = BePlugInnerNTubeTopSub;
            }
            else
            {
                BePlugInnerNTubeBotSub = new G4SubtractionSolid("BePlugBotSub", 
                            BePlugInnerNTubeBotSubIn, nInnerTubeOut, rotNTube, posNTubeBePlug);
                BePlugInnerNTubeBotSubIn = BePlugInnerNTubeBotSub;
                
            }		
            outerSteelCylSub        = new G4SubtractionSolid("outerSteelCylSub", 
															 outerSteelCylSubIn, nInnerTubeOut, rotNTube, posNTube);
            outerSteelCylSubIn      = outerSteelCylSub;
  
			G4Trd* nInnerTubeMonoSub     = new G4Trd("nInnerTubeOut", nTubeHalfDx1[i], 
					nTubeHalfDx2[i], nTubeHalfDy1[i], nTubeHalfDy2[i], nTubeHalfDz+2.*nTubeInHZextra);
			
            G4ThreeVector posNTubeNinner(posNTubeX[i], posNTubeY[i], posNTubeZ[i]- transMonoZ);
            monoSteelCylNInnerSub   =  new G4SubtractionSolid("MonoSteelNInnerSub",  
					monoSteelSubNInnerIn, nInnerTubeMonoSub, rotNTube, posNTubeNinner);
            monoSteelSubNInnerIn    = monoSteelCylNInnerSub;
        }
    }
    G4cout << "Done Inner Neutron Tubes :)" << G4endl;
	
	//nOuter tubes from monolith
	//Monolith Neutron Beam lines .  10x12cm2.  3 beam line for each inner beam line
	//24 holes up to 10 by 12 cm2 with approximately 3 cm total in Al windows
	//and sometimes 30 cm of moving inconel intercepting the beam.
	// Active primary shutters in monolith; sometimes in beam, sometimes not 
	//	(with large hole in monolith moving around
	//Monolith is nominal 5 m steel and 1 m HD concrete
	//Beam lines are nominal 2 m (radius) concrete
    G4SubtractionSolid* monoSteelSub 			= NULL;
    G4SubtractionSolid* hdcNOuterSub 			= NULL;
    G4SubtractionSolid* hdcLayerNOuterSub 		= NULL;
    G4SubtractionSolid* nBeamShieldSub 			= NULL;
	G4SubtractionSolid* hdcOutLightShieldSub	= NULL;
	G4LogicalVolume* nBeamOuterLog    	        = NULL;
	G4LogicalVolume* nOuterShieldLog 		        = NULL;

	
	//used if NTube is sub from MS
	G4SubtractionSolid* multiStepCyl1NSub=NULL;
	G4SubtractionSolid* multiStepCyl2NSub=NULL;
	G4SubtractionSolid* multiStepCyl3NSub=NULL;
	
	

	//nOuterUnionShields	= true;
    if(placeNTubeOuter)
    {

		G4double monoExtraDigLHalf				= 3.*nTubeInHZextra;
		G4double nBeamOuterStart    			= monoSteelRmin+ monoExtraDigLHalf;//outerSteelR2;
			// extra is to shift the end of joint of in/out tubes into mono
        G4double dTheta             			= 8.5*degree;
G4double nBeamOuterZhLen 						= nBeamLineFullLength/2.;//15.*m;//7.*m; TODO: change length	
        std::vector <G4SubtractionSolid*> nTubeOuter(2);
        G4SubtractionSolid* monoSteelSubIn      = NULL;
        G4SubtractionSolid* hdcNOuterSubIn      = bulkHDCCyl;
        G4SubtractionSolid* hdcLayerNOuterSubIn = bulkHdcLayerCyl;
		G4SubtractionSolid* hdcOutLShieldSubIn 	= hdcOutLightShield;
	
		
        if(placeNTubeInner) monoSteelSubIn  	= monoSteelCylNInnerSub;
        else monoSteelSubIn                 	= monoSteelCyl1;
        G4double nBeamOuterX 	            	= 5.*cm;
        G4double nBeamOuterY 	            	= 6.*cm;
        G4IntersectionSolid* firstTube      	= NULL;
        G4UnionSolid* secTube               	= NULL;
        
        G4ThreeVector posNTubeOuter1;
        G4ThreeVector posNTubeOuter2;
        G4RotationMatrix* rotnNTube1 			= NULL;
        G4RotationMatrix* rotnNTube2 			= NULL;
        G4double radMaxDummyNOuter   			= 2.*nBeamOuterZhLen+2.*m; 
		
		//used if NTube is sub from MS
		G4SubtractionSolid* multiStepCyl1NSubIn = NULL;
		G4SubtractionSolid* multiStepCyl2NSubIn = NULL;
		G4SubtractionSolid* multiStepCyl3NSubIn = NULL;
		
		if(multiStepRuns)
		{
			multiStepCyl1NSubIn = multiStepCyl1Sub;
			if(fIsMSDet2)
			{
				multiStepCyl2NSubIn = multiStepCyl2Sub;
			}
			if(fIsMSDet3)
			{
				multiStepCyl3NSubIn = multiStepCyl3Sub;
			}
		}
		
		
		
	
		// nShieldXY = 2.*m; //set above

		G4cout << " ####### nShieldXY = " << nShieldXY/m << "  meter." <<  G4endl;
		if(nShieldXY <2.*m) G4cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n " << G4endl;


		for (G4int i=0; i<6; i++) //6 inner tube number
        {
												
            for (G4int n=0; n<3; n++) //3 outer tube number
            {
                    
                G4double posnTubeZ	= IRPhalfHt + nTubeHalfDy1[i]; //rotated y->z	
                if(i>2) posnTubeZ	= -posnTubeZ;
                
                G4double dT = 0.;
                if(n==1) dT = -dTheta;
                if(n==2) dT = dTheta;
                G4RotationMatrix* rotnNTube = new G4RotationMatrix; 
                rotnNTube->rotateZ(rotZ[i] + dT);
                rotnNTube->rotateX(rotX[i]);			
				
                std::ostringstream ss;
                ss << i+1 << n+1;
                G4String outerTubeName = "NTubeOuter"+ss.str(); 
                
                std::vector <G4double> posxy = GetPosNOuter(i, dT, lH3posX, lH3posY,
					lH1halfX, nBeamOuterZhLen, lH1posY, lH1Rot, nBeamOuterStart );
                
                assert(posxy.size()<2);     
                G4ThreeVector posNTubeOuter(posxy[0], posxy[1], posnTubeZ);
		
                //this tube              
                G4Box* nBeamOuterOut 		= new G4Box(outerTubeName+"Out", nBeamOuterX, 
						nBeamOuterY, nBeamOuterZhLen);
				G4Box* nBeamOuterOutMonoSub = new G4Box(outerTubeName+"Out", nBeamOuterX, 
									nBeamOuterY, nBeamOuterZhLen+2.*monoExtraDigLHalf);										
		        G4Box* nBeamOuterIn  		= new G4Box(outerTubeName+"In", nBeamOuterX
				    -thickNTubeAlX[i], 	nBeamOuterY-thickNTubeAlY[i], nBeamOuterZhLen+5.*cm);	
				
			    G4SubtractionSolid* nBeamOuterSub = new G4SubtractionSolid(outerTubeName+"Sub", 
				    nBeamOuterOut, nBeamOuterIn, rotCenter, *posCenter);
				G4SubtractionSolid*	nBeamOutMSSub = nBeamOuterSub; 
				
				
                if(multiStepRuns)
                {
					 // NTube is sub from MS
					G4ThreeVector posNOutMS(posNTubeOuter.getX(), posNTubeOuter.getY(),
											posNTubeOuter.getZ() - posMSDetZ);
					multiStepCyl1NSub=new G4SubtractionSolid("temp1",
						multiStepCyl1NSubIn, nBeamOuterSub, rotnNTube, posNOutMS);
					multiStepCyl1NSubIn = multiStepCyl1NSub;	
					if(fIsMSDet2)
					{
						multiStepCyl2NSub=new G4SubtractionSolid("temp2",
							multiStepCyl2NSubIn, nBeamOuterSub, rotnNTube, posNOutMS);
						multiStepCyl2NSubIn = multiStepCyl2NSub;
					}
					if(fIsMSDet3)
					{
						multiStepCyl3NSub=new G4SubtractionSolid("temp3",
							multiStepCyl3NSubIn, nBeamOuterSub, rotnNTube, posNOutMS);
						multiStepCyl3NSubIn = multiStepCyl3NSub;
					}
					/*
					 //Not working
					G4ThreeVector posNOuterMS(-posxy[0] ,-posxy[1], -posnTubeZ + posMSDetZ);		
					G4RotationMatrix* invRotnNTube = new G4RotationMatrix;
					invRotnNTube->rotateZ(-rotX[i]);
					invRotnNTube->rotateZ(-rotZ[i] - dT); //note reverse order					
					nBeamOutMSSub	= SubMultiStepDet(nBeamOuterSub,outerTubeName+"MSSub",
													  posNOuterMS, invRotnNTube);
					*/
				}


				nBeamOuterLog		= new G4LogicalVolume(nBeamOutMSSub, Al, outerTubeName+"Log");
 		        new G4PVPlacement( rotnNTube, posNTubeOuter, nBeamOuterLog, outerTubeName,
							fDetLogical, false, 0, fCheckOverlap);               


		 	    //subtract from monolith
				G4ThreeVector posMonoNTubeOuter( posxy[0], posxy[1], posnTubeZ - transMonoZ);
                monoSteelSub   		= new G4SubtractionSolid("MonoSteelSub", monoSteelSubIn, 
								nBeamOuterOutMonoSub, rotnNTube, posMonoNTubeOuter);
                monoSteelSubIn 		= monoSteelSub;
                
                //subtract from HDC
			    G4ThreeVector posHDCNTubeOuter(posxy[0], posxy[1], posnTubeZ-posBulkShieldHDCZ);
                hdcNOuterSub   		=  new G4SubtractionSolid("hdcNOuterSub", hdcNOuterSubIn, 
										nBeamOuterOut, rotnNTube, posHDCNTubeOuter);
                hdcNOuterSubIn 		= hdcNOuterSub;
                
                //subtract from layerHDC
                
                hdcLayerNOuterSub 	= new G4SubtractionSolid("hdcLayerNOuterSub", 
                        hdcLayerNOuterSubIn, nBeamOuterOut, rotnNTube, posHDCNTubeOuter);
                hdcLayerNOuterSubIn = hdcLayerNOuterSub;

				
                //shields outer to HDC
			    if(nOuterUnionShields && radMaxDummyNOuter > bulkHdcRmax)
                {
					
                    G4Tubs* nTubeOuterCylDummy	= new G4Tubs("nTubeOuterCylDummy",  
		                bulkHdcRmax, radMaxDummyNOuter, 5.*m, 0, twopi); //move out
		            std::ostringstream ss1;
                    ss1 << i+1 ;
                    G4String outerNTubeNameHdc	= "nTubeShieldSolid"+ss1.str(); 
            
			        //shields.  keep here just to get name each time
	                G4Box* nBeamShield 			= new G4Box(outerTubeName+"Shield",
                                nShieldXY,nShieldXY, nBeamOuterZhLen+3.*m);
	                G4IntersectionSolid* nBeamShieldXDummy 	= new G4IntersectionSolid(
		                        outerNTubeNameHdc+"Xs", nTubeOuterCylDummy, nBeamShield, 
		                                            rotnNTube, posNTubeOuter); 
					
							
					G4ThreeVector posNTubeHDCOut(posxy[0], posxy[1], posnTubeZ- hdcOutLightPosZ);		
					hdcOutLightShieldSub 	= new G4SubtractionSolid("hdcOutLightShieldSub",
								hdcOutLShieldSubIn, nBeamShield, rotnNTube, posNTubeHDCOut);
					hdcOutLShieldSubIn 	= hdcOutLightShieldSub;


                    if(n==0)
                    { 
                        firstTube           = nBeamShieldXDummy; 
                        rotnNTube1          = rotnNTube;
                        posNTubeOuter1      = posNTubeOuter;
                    }
                    if(n==1 && firstTube != NULL)
                    {
                        secTube 			=  new G4UnionSolid("volNOuter2", firstTube,
												nBeamShieldXDummy, rotCenter, *posCenter);
                        rotnNTube2       	= rotnNTube;
                        posNTubeOuter2   	= posNTubeOuter;
                    }
                    if(n==2 && secTube != NULL)
                    {
                        G4ThreeVector instrFloorNTubePos(0, 0, 
                                -instrFloorTop2Tgt-instrFloorHalfHt- posnTubeZ);

                        G4UnionSolid* allTubes = new G4UnionSolid("volNOuter3", 
                                secTube, nBeamShieldXDummy, rotCenter, *posCenter);

                        G4SubtractionSolid*  allTubesSub0 = new  G4SubtractionSolid(
                                    outerNTubeNameHdc+"allSub0", allTubes, 
                                instrFloorSub, rotCenter, instrFloorNTubePos);

                        G4SubtractionSolid*  allTubesSub1 = new  G4SubtractionSolid(
		                            outerNTubeNameHdc+"allSub1", allTubesSub0, 
				                        nBeamOuterOut, rotnNTube1, posNTubeOuter1); 
                        G4SubtractionSolid*  allTubesSub2 = new  G4SubtractionSolid(
		                            outerNTubeNameHdc+"allSub2", allTubesSub1,  
				                        nBeamOuterOut, rotnNTube2, posNTubeOuter2); 	
                        nBeamShieldSub = new  G4SubtractionSolid(
                                    outerNTubeNameHdc+"allSub", allTubesSub2,  
                                        nBeamOuterOut, rotnNTube, posNTubeOuter); 
									
                        //place shields to ntubes
                        std::ostringstream ss2;
                        ss2 << i+1 ;
                        G4String outerTubeNameHdc = "nTubeShield"+ss2.str(); 
						G4SubtractionSolid* nOutShieldSub = nBeamShieldSub;
						
                        if(multiStepRuns)
                        {
							G4ThreeVector posNOutMStep(0,0, posMSDetZ); //w.r.t center
							nOutShieldSub = SubMultiStepDet(nBeamShieldSub, 
									tubeNames[i]+"OutSub", posNOutMStep, rotCenter);
							
							/* //used if NTube is sub from MS
							 G4ThreeVector posNOutShieldMS(0,0, - posMSDetZ);													 
							 multiStepCyl1NSub=new G4SubtractionSolid("temp11",
							 multiStepCyl1NSubIn, nBeamShieldSub, rotCenter, posNOutShieldMS);
							 multiStepCyl1NSubIn = multiStepCyl1NSub;
							 if(fIsMSDet2)
							 {
								multiStepCyl2NSub=new G4SubtractionSolid("temp22",
								multiStepCyl2NSubIn, nBeamShieldSub, rotCenter, posNOutShieldMS);
								multiStepCyl2NSubIn = multiStepCyl2NSub;								
							}
							if(fIsMSDet3)
							{
								multiStepCyl3NSub=new G4SubtractionSolid("temp33",
								multiStepCyl3NSubIn, nBeamShieldSub, rotCenter, posNOutShieldMS);
								multiStepCyl3NSubIn = multiStepCyl3NSub;
							}
							*/
                        }	
                                                
                        nOuterShieldLog = new G4LogicalVolume(
							nOutShieldSub, concrete, outerTubeNameHdc+"Log");
                        

                        G4bool nShieldSkipVis = true;     // move it out of loop
						if(nShieldXY < 1.1*m )  nShieldSkipVis = false; //move it out of loop
						if(nShieldSkipVis) nOuterShieldLog->SetVisAttributes(
                               new G4VisAttributes (G4VisAttributes::Invisible));
						new G4PVPlacement(  rotCenter, *posCenter,//G4ThreeVector(0.,0.,-posBulkShieldHDCZ), 
							nOuterShieldLog, outerTubeNameHdc, fDetLogical, false, 0, fCheckOverlap);  

                    } //n==2
                }//if outer shield                      
            }//n loop          
        }//i loop
	}//if placeOuter
	G4cout << "Done Outer Neutron Shields" << G4endl;
	
	
	if(multiStepRuns)
	{	
		G4ThreeVector posMSDet(0,0, posMSDetZ);
		G4SubtractionSolid* multiStepDetCyl1= NULL;
		if(placeNTubeOuter)  //used if NTube is sub from MS
			multiStepDetCyl1				= multiStepCyl1NSub;
		else
			multiStepDetCyl1				= multiStepCyl1Sub;

		G4LogicalVolume* multiStepCyl1Log 	= new G4LogicalVolume(multiStepDetCyl1, 
										lightMaterial, "multiStepCyl1Log");
		multiStepCyl1Log->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));	
		new G4PVPlacement( rotCenter, posMSDet, multiStepCyl1Log, "MultiStepDet1",
										fDetLogical, false, 0 , fCheckOverlap); 		
		if(fIsMSDet2)
		{
			G4SubtractionSolid* multiStepDetCyl2= NULL;
			if(placeNTubeOuter) //used if NTube is sub from MS
				multiStepDetCyl2				= multiStepCyl2NSub;
			else
				multiStepDetCyl2				= multiStepCyl2Sub;
			
			G4LogicalVolume* multiStepCyl2Log = new G4LogicalVolume(multiStepDetCyl2, 
										lightMaterial, "multiStepCyl2Log");	
			multiStepCyl2Log->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));	
			new G4PVPlacement( rotCenter, posMSDet, multiStepCyl2Log, "MultiStepDet2",
										fDetLogical, false, 0 , fCheckOverlap); 			
		}
		if(fIsMSDet3)
		{	
			G4SubtractionSolid* multiStepDetCyl3= NULL;
			if(placeNTubeOuter) //used if NTube is sub from MS
				multiStepDetCyl3				= multiStepCyl3NSub;
			else
				multiStepDetCyl3				= multiStepCyl3Sub;
			
			G4LogicalVolume* multiStepCyl3Log = new G4LogicalVolume(multiStepDetCyl3, 
										lightMaterial, "multiStepCyl3Log");
			multiStepCyl3Log->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));	
			new G4PVPlacement( rotCenter, posMSDet, multiStepCyl3Log, "MultiStepDet3",
										fDetLogical, false, 0 , fCheckOverlap); 			
		}
		G4cout << "Done Multi Step Detector(s) " <<  G4endl;
		G4cout << "\t Det1:radius " << fMSDet1Inner/m << G4endl;
		if(fIsMSDet2) G4cout	<< "\t Det2:radius "<< fMSDet2Inner/m << G4endl; 
		if(fIsMSDet3) G4cout	<< "\t Det2:radius "<< fMSDet2Inner/m << G4endl; 
	}


	//IRP plug placement
	G4Material* BeD2O = G4NistManager::Instance()->FindOrBuildMaterial("BeD2O"); 
	G4LogicalVolume* irpLog = new G4LogicalVolume(IRPsolidSub4, Al, "InnerRefPlugLog");
	irpLog->SetVisAttributes(new G4VisAttributes (G4Colour::Green()));
	irpLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
	if(placeInner) new G4PVPlacement( rotCenter, *posCenter, irpLog, "InnerRefPlug",
					   fDetLogical, false, 0 , fCheckOverlap);  
	
	//Top & Bottom Be Plug placement 
    G4LogicalVolume* BePlugTopLog = NULL;
    G4LogicalVolume* BePlugBotLog = NULL;
    if(placeNTubeInner)
    {
        BePlugTopLog = new G4LogicalVolume(BePlugInnerNTubeTopSub, 
												BeD2O, "UpperBePlugLog");
        BePlugBotLog = new G4LogicalVolume(BePlugInnerNTubeBotSub, 
                                                BeD2O, "LowerBePlugLog");
    }
    else
    {
        BePlugTopLog = new G4LogicalVolume(BePlugTopSub2, BeD2O, "UpperBePlugLog");
        BePlugBotLog = new G4LogicalVolume(BePlugBotSub2, BeD2O, "LowerBePlugLog");
    }
	BePlugTopLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));
    BePlugBotLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));	
	if(placeInner) new G4PVPlacement( rotCenter, posUpperBePlug, BePlugTopLog, "UpperBePlug",
					   fDetLogical, false, 0 , fCheckOverlap); 	 
	if(placeInner) new G4PVPlacement( rotCenter, posLowerBePlug, BePlugBotLog, "BePlugBottom",
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
	if(placeInner) new G4PVPlacement( rotCenter, posInUpSteel, innerUpperSteelD2OLog, "InnerUpperSteelD2",
					   fDetLogical, false, 0 , fCheckOverlap);	
	
	//INNER STEEL cylinder BOTTOM  
	G4LogicalVolume* innerBotSteelD2OLog = new G4LogicalVolume(innerSteelD2Ocyl, 
												SteelD2O, "InnerBotSteelD2OLog");
	G4ThreeVector posInBotSteel(0, 0, -(IRPhalfHt+innerSteelGap+innerSteelHalfHt));
	innerBotSteelD2OLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
	if(placeInner) new G4PVPlacement( rotCenter, posInBotSteel, innerBotSteelD2OLog, "InnerBottomSteelD2O",
                       fDetLogical, false, 0 , fCheckOverlap);   				
		
		
 																
    //placing 	OuterSteel Cylinder			outerSteelCylSub1
    G4LogicalVolume* outerSteelLog = NULL;
    if(placeNTubeInner)
        outerSteelLog = new G4LogicalVolume(outerSteelCylSub, Steel, "OuterSteelLog");	
    else 
        outerSteelLog = new G4LogicalVolume(outerSteelCylSub1, Steel, "OuterSteelLog");
	outerSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Cyan()));	
	if(placeInner) new G4PVPlacement( rotCenter, *posCenter, outerSteelLog, "OuterSteelCylinder",
					   fDetLogical, false, 0 , fCheckOverlap);   


	
	//MONOLITH CONTINUED after sub outer NTubes
    G4Tubs* monoSteelLidTop   = new G4Tubs ("monoSteelLidTop",0, monoSteelRmin,
                                                    monoLidTopHalfHt, 0, twopi);
    G4Tubs* monoSteelLidBot   = new G4Tubs ("monoSteelLidBot",0, monoSteelRmin,
                                                   monoLidBotHalfHt, 0, twopi);
    G4ThreeVector transMonoLidTop(0, 0,  monoSteelHalfHt-monoLidTopHalfHt);
    G4ThreeVector transMonoLidBot(0, 0, -monoSteelHalfHt+monoLidBotHalfHt);
	G4SubtractionSolid* monoSteelSubUnion = NULL;
	
	
	if(placeNTubeOuter)// placeNTubeInner is taken care of in it	
	{
		monoSteelSubUnion  = monoSteelSub;
	}
	else if(placeNTubeInner)
	{
		monoSteelSubUnion	= monoSteelCylNInnerSub;
			
	}
	else //if( !(placeNTubeOuter || placeNTubeInner))
    {
		monoSteelSubUnion  = monoSteelCylDumSub;
	}

	G4UnionSolid* monoSteel2  = new G4UnionSolid("MonoSteel2",
				monoSteelSubUnion, monoSteelLidTop, rotCenter, transMonoLidTop);

    G4UnionSolid* monoSteel3  = new G4UnionSolid("MonoSteel3", monoSteel2, 
									monoSteelLidBot, rotCenter, transMonoLidBot);
	G4LogicalVolume* monoSteelLog = NULL;


	//beam entrance hole box1 
	if(placeProtonBox)
	{
		G4double cutPBoxHalfY1  = (monoSteelRmax - monoSteelRmin)/2. + 0.3*m;  //extra
		G4Box* monoSteelCutBox1   = new G4Box("MonoSteelCutBox", protonBeamCutXZ, 
											cutPBoxHalfY1, protonBeamCutXZ);
		G4ThreeVector transBoxMonoSteel1(0., -monoSteelRmin-
									(monoSteelRmax-monoSteelRmin)/2.,-transMonoZ);  
		G4SubtractionSolid* monoSteel4 = new G4SubtractionSolid("MonoSteel4", 
					monoSteel3, monoSteelCutBox1, rotCenter, transBoxMonoSteel1);
		monoSteelLog = new G4LogicalVolume(monoSteel4, steel, "MonoSteelLog");		
	}
	else
		monoSteelLog = new G4LogicalVolume(monoSteel3, steel, "MonoSteelLog");			
	
    G4ThreeVector  transMonoSteel(0.,0., transMonoZ);// shift Y +0.5m
    monoSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
    //monoSteelLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
   new G4PVPlacement( rotCenter,  transMonoSteel, monoSteelLog, "MonolithSteelCyl",
                                                fDetLogical, false, 0 , fCheckOverlap); 


	//PROTON BEAM BOX outside of concrete and steel
	//protonBoxOuterHalfXZ = 2.0*m;//3.52*m; //Set above
 	//if(surfaceDet) protonBoxOuterHalfY  = 8.*m;  
	G4double protonBoxOuterHalfY  		= 15.*m; //NOTE: this will be moved out another 21 m
    G4ThreeVector* posOuterProtonBox 	= NULL;
	if(placeProtonBox)
	{
		SNSTargetOuterProtonBox* outerProtonBox = new SNSTargetOuterProtonBox("OuterProtonBox"
            ,protonBoxOuterHalfXZ, protonBoxOuterHalfY, protonBoxOuterHalfXZ
            ,G4Colour::Cyan(), wire, multiStepRuns, fIsMSDet2, fIsMSDet3, fMSDet1Inner
            ,fMSDet2Inner, fMSDet3Inner, fMSDetCylThick, fMSDetCylHht, posMSDetZ, fCheckOverlap);
		posOuterProtonBox = new G4ThreeVector(0, -protonBoxOuterHalfY-monoSteelRmax, 0);
		outerProtonBox->Place(rotCenter, posOuterProtonBox, fDetLogical); 
	}
		
	//////////////////////////////////
    //################################
    //BUILDING, BASEMENT and DETECTORS
	//
	// BASEMENT road base stone(gravel), or granite loose
    //G4double tunnelWall        = 18.*inch; //SET ABOVE
	//G4double basementRad 		= 22.3*m - tunnelWall;//rad 23.8-1.5m -wall. distLOS=25m
    //x=18.3, y=-15.2 =>rad 23.8.;  z=-7.8 (8.1m-0.3 for det 1foot above ground)=>25m LOS
	//G4double detToTargetFullZ  = 7.5*m;//7.1*m //8.1m-1m //old=7.8 
    //G4double detToWallGap      = 1.5*m;
	//G4double detAxialDist      = baseRadiusAll+detToWallGap;
    //instrFloorTop2Tgt = protonBoxOuterHalfXZ;//2.*m; //Set above
    //G4double instrFloorHalfHt  = 9.*inch;//18"/2 //set above
    //G4double basementTop2Tgt   = instrFloorTop2Tgt + 2.*instrFloorHalfHt;
    //G4double basementHalfHt    = (detToTargetFullZ - basementTop2Tgt+3.*m)/2.;  //8.3m/2 
    //G4double basementPosZ     = -basementHalfHt - basementTop2Tgt; 


    
	G4double monoBase		   = 1.*m; //instrFloorHalfHt;

    //INSTR. FLOOR slab concrete 18"
	G4ThreeVector instrFloorPos(0,0, instrFloorPosZ );
    G4Tubs* floorMonoCut       = new G4Tubs ("floorMonoCut", 0,  
						monoSteelRmax+monoBase, instrFloorHalfHt, 0, twopi);
							
    G4SubtractionSolid* instrFloorCyl = new G4SubtractionSolid("basementCyl", 
                        instrFloorSub, floorMonoCut, rotCenter, *posCenter);
    
    G4LogicalVolume* instrFloorLog = new G4LogicalVolume(instrFloorCyl, 
                                        concrete, "InstrFloorLog"); 
    //instrFloorLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
    new G4PVPlacement( rotCenter, instrFloorPos, instrFloorLog, "InstrFloor", 
                                     fDetLogical, false, 0, fCheckOverlap);  

	//BASEMENT Inner
    G4double basementLayerWidth   = 2.*m;
	G4double basementRadInnerDisk = basementRad - basementLayerWidth;
    
	G4cout 	<<  "basementRadInnerDisk(m) " << basementRadInnerDisk/m 
			<< G4endl;
	
	G4Tubs*  basementInSolid    = new G4Tubs ("basementInSolid", 0,  
						basementRadInnerDisk, basementHalfHt, 0, twopi);
	G4double monoCutHalfHt	  = (monoSteelHalfHt-transMonoZ-basementTop2Tgt+monoBase)/2.;
	G4Tubs* basementMonoCut	  = new G4Tubs ("BasementMonoCut", 0, 
					monoSteelRmax+ monoBase, monoCutHalfHt, 0, twopi);

	G4ThreeVector posMonoCut(0, 0, basementHalfHt-monoCutHalfHt); 
	G4SubtractionSolid* basementInSolidDSub = new G4SubtractionSolid(
		"basementInSolidDSub", basementInSolid, dummySolid,  rotCenter, *posCenter);
	
	G4ThreeVector baseSubPos (0, 0, -basementPosZ + posMSDetZ);		
	G4SubtractionSolid* basementInMSSub = basementInSolidDSub;	
	if(multiStepRuns)
	{	
		basementInMSSub  = SubMultiStepDet(basementInSolidDSub, 
										 "bulkShieldLayerMSSub", baseSubPos, rotCenter);
	}	
	
	G4SubtractionSolid*	basementInCyl = new G4SubtractionSolid("basementCyl", 
						basementInMSSub, basementMonoCut, rotCenter, posMonoCut);

    //G4Material* vac = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");	 
	G4Material* stone = G4NistManager::Instance()->FindOrBuildMaterial("RoadStone");	
	G4String materialType 		 = 										"RoadStone";
	G4LogicalVolume* basementInLog = new G4LogicalVolume(basementInCyl, stone, "BasementInLog"); 
	
	G4ThreeVector basementPos (0, 0, basementPosZ);//cylinder
	basementInLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
	new G4PVPlacement( rotCenter, basementPos, basementInLog, "BasementIn", 
                                        fDetLogical, false, 0 , fCheckOverlap); 


    //BASEMENT Outer layer
    G4Tubs*  basementLayerSolid = new G4Tubs ("basementLayerSolid", 
							basementRadInnerDisk, basementRad, basementHalfHt, 0, twopi);
	G4SubtractionSolid* basementLayerSolidDSub = new G4SubtractionSolid(
		"basementLayerSolidDSub", basementLayerSolid, dummySolid,  rotCenter, *posCenter);
		
	G4SubtractionSolid* basementLayerMSSub = basementLayerSolidDSub;	
	if(multiStepRuns)
	{	
		basementLayerMSSub  = SubMultiStepDet(basementLayerSolidDSub, 
										   "basementLayerMSSub", baseSubPos, rotCenter);
	}	
	
	G4LogicalVolume* basementLayerLog =  new G4LogicalVolume(basementLayerMSSub, stone, "BasementLayerLog"); 
	basementLayerLog->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
    new G4PVPlacement( rotCenter, basementPos, basementLayerLog, "BasementLayer", 
                                                fDetLogical, false, 0 , fCheckOverlap);  

	
	//MONOLITH BASE
	G4double monoBaseCutHalf = monoCutHalfHt+instrFloorHalfHt;
	G4Tubs*  monoBaseCyl     = new G4Tubs ("monoBaseCyl", monoSteelRmax, 
							monoSteelRmax+monoBase,	 monoBaseCutHalf, 0, twopi);
	G4Tubs*  monoBaseBot     = new G4Tubs ("monoBaseBot", 0, 
								monoSteelRmax,	monoBase/2., 0, twopi);	
	G4ThreeVector monoBaseBotPos(0,0,-monoBaseCutHalf+monoBase/2.);
	G4UnionSolid* monoBaseSolid  = new G4UnionSolid("monoBaseSolid", monoBaseCyl,
									monoBaseBot, rotCenter, monoBaseBotPos);
	G4double monoBasePosZ		 =	-instrFloorTop2Tgt- monoBaseCutHalf;
	G4LogicalVolume* monoBaseLog = NULL;
	if(multiStepRuns)
	{
		G4ThreeVector monoBPos(0,0, -monoBasePosZ + posMSDetZ);
		G4SubtractionSolid* monoBaseSolidDSub = new G4SubtractionSolid(
			"basementLayerCylSub", monoBaseSolid, dummySolid,  rotCenter, *posCenter);
		G4SubtractionSolid* sub3		   	  = SubMultiStepDet(monoBaseSolidDSub, 
							"monoBaseSolidDSub", monoBPos, rotCenter);
		monoBaseLog = new G4LogicalVolume(sub3,concreteHDC, "MonoBaseLog"); 
	}	
	else
	{
		monoBaseLog = new G4LogicalVolume(monoBaseSolid,concreteHDC, "MonoBaseLog"); 
	}
	
	//G4LogicalVolume* monoBaseLog = new G4LogicalVolume(monoBaseSolid,concreteHDC, "MonoBaseLog"); 
	G4ThreeVector monoBasePos(0,0, monoBasePosZ);
	new G4PVPlacement( rotCenter, monoBasePos, monoBaseLog, "MonoBase", 
											fDetLogical, false, 0 , fCheckOverlap);  	
	
    //TUNNEL WALL
    G4Tubs*  tunnelWallCyl          = new G4Tubs ("tunnelWall", basementRad, 
                basementRad+tunnelWall-2.*baseLayerDetHalf, basementHalfHt, 0, twopi);
    G4LogicalVolume* tunnelWallLog  = new G4LogicalVolume(tunnelWallCyl, concrete, "TunnelWallLog"); 
    G4ThreeVector tunnelWallPos(0, 0, basementPosZ);

    new G4PVPlacement( rotCenter, tunnelWallPos, tunnelWallLog, "TunnelWall", 
                                                    fDetLogical, false, 0 , fCheckOverlap);  
    //TUNNEL WALL det layer
    G4Tubs*  tunWallDetCyl          = new G4Tubs ("tunWallDetCyl", basementRad+tunnelWall
                -2.*baseLayerDetHalf, basementRad+tunnelWall, basementHalfHt, 0, twopi);
    G4LogicalVolume* tunWallDetLog  = new G4LogicalVolume(tunWallDetCyl, concrete, "TunWallDetLog"); 
    new G4PVPlacement( rotCenter, tunnelWallPos, tunWallDetLog, "TunWallDet", 
                                                    fDetLogical, false, 0 ,fCheckOverlap);  



    //BASEMENT HALL ROOF
	G4Tubs*  basementRoofTop   			    = NULL;
	G4Tubs*  basementRoofStone 			    = NULL;
	G4Tubs*  basementRoofIn    			    = NULL;
    G4ThreeVector* posHallRoofTop   	    = NULL;
    G4ThreeVector* posHallRoofStone 	    = NULL;
    G4ThreeVector* posHallRoofIn    	    = NULL;
    G4ThreeVector* posHallRoofDet           = NULL;
    G4LogicalVolume* basementRoofInLog 	    = NULL;
	G4LogicalVolume* basementRoofTopLog	    = NULL;
	G4LogicalVolume* basementRoofStoneLog   = NULL;
	G4LogicalVolume*  baseRoofDetLog 		= NULL;
	
	G4double baseRoofTopHalf    = 9.*inch;//18/2 instrument floor slab
	G4double baseRoofStoneHalf  = 2.5*12.*inch; //5/2 feet
	G4double baseRoofInHalf     = 9.*inch;//18/2  
	G4double baseRoofGravel     = 4.*m+tunnelWall; // 

	//
	if(basementHallRoof)
	{
		posHallRoofTop 	= new G4ThreeVector(0,0, - instrFloorTop2Tgt -2.*instrFloorHalfHt -baseRoofTopHalf);
		posHallRoofStone = new G4ThreeVector(0,0, -instrFloorTop2Tgt -2.*instrFloorHalfHt
					-2.*baseRoofTopHalf - baseRoofStoneHalf); 
		posHallRoofIn 	= new  G4ThreeVector(0,0, -instrFloorTop2Tgt -2.*instrFloorHalfHt 
					-2.*baseRoofTopHalf - 2.*baseRoofStoneHalf-baseRoofInHalf);

		posHallRoofDet   = new  G4ThreeVector(0,0, -instrFloorTop2Tgt -2.*instrFloorHalfHt 
                    -2.*baseRoofTopHalf - 2.*baseRoofStoneHalf 
                    -2.*baseRoofInHalf - baseLayerDetHalf);

		basementRoofTop   = new G4Tubs ("basementRoof1", basementRad+tunnelWall, 
							basementRad+baseRoofGravel, baseRoofTopHalf, 0, twopi);
		basementRoofTopLog = new G4LogicalVolume( basementRoofTop,
											concrete, "basementRoofTopLog");
		new G4PVPlacement( rotCenter, *posHallRoofTop, basementRoofTopLog,
							"BasementRoofTop", fDetLogical, false, 0 , fCheckOverlap); 

		basementRoofStone  = new G4Tubs ("basementRoof2", basementRad+tunnelWall, 
							basementRad+baseRoofGravel, baseRoofStoneHalf, 0, twopi);

		basementRoofStoneLog = new G4LogicalVolume( basementRoofStone,
											stone, "basementRoofStoneLog");
		new G4PVPlacement( rotCenter, *posHallRoofStone, basementRoofStoneLog,
							"BasementRoofStone", fDetLogical, false, 0 , fCheckOverlap);  

		basementRoofIn     = new G4Tubs ("basementRoof3", basementRad+tunnelWall, 
							basementRad+baseRoofGravel, baseRoofInHalf, 0, twopi);
		basementRoofInLog = new G4LogicalVolume( basementRoofIn,
											concrete, "basementRoofInLog");
        //basementRoofInLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
		new G4PVPlacement( rotCenter, *posHallRoofIn, basementRoofInLog, 
							"BasementRoofIn", fDetLogical, false, 0 , fCheckOverlap);  

        //Roof Roof detector
        G4Tubs* baseRoofDet     = new G4Tubs ("baseRoofDet", basementRad+tunnelWall, 
                            basementRad+baseRoofGravel, baseLayerDetHalf, 0, twopi);
        baseRoofDetLog = new G4LogicalVolume( baseRoofDet,
                                            concrete, "baseRoofDetLog");
        new G4PVPlacement( rotCenter, *posHallRoofDet, baseRoofDetLog, 
                            "BaseRoofDet", fDetLogical, false, 0 ,fCheckOverlap);  

	}
	


    //BULK SHIELD HDC continued ... outer concrete from monolith to 10m
    G4LogicalVolume* bulkConcShieldLog = NULL;
	G4ThreeVector posBulkShieldHDC(0, 0, posBulkShieldHDCZ);
	if(placeSurfaceHdcInnerShield)
	{
		G4double cutBoxBulkShieldHalfY  = (surfaceHDCInnnerMax-
							bulkConcShieldRmin)/2. + 1.*m;  //extra 
		G4Box* bulkShieldPCutBox        = new G4Box("bulkHDCProtonCutBox1", 
			protonBoxOuterHalfXZ, cutBoxBulkShieldHalfY, protonBoxOuterHalfXZ);    
		G4ThreeVector  transBulkShieldCut(0., -bulkConcShieldRmin-(surfaceHDCInnnerMax
				-bulkConcShieldRmin)/2. + 0.6*m, -posBulkShieldHDCZ); //adjust extra Y

		G4SubtractionSolid* bulkHDCCylAllSub = NULL;

		//multiStepRuns already taken care of
		if(placeNTubeOuter )
		{
			if( placeProtonBox )
			{
				bulkHDCCylAllSub = new G4SubtractionSolid("bulkConcShieldSub", 
					hdcNOuterSub, bulkShieldPCutBox, rotCenter, transBulkShieldCut);
			}
			else
				bulkHDCCylAllSub  =  hdcNOuterSub;
		}
		else
		{
			if( placeProtonBox )
			{
				bulkHDCCylAllSub = new G4SubtractionSolid("bulkConcShieldSub", 
					bulkHDCCyl, bulkShieldPCutBox, rotCenter, transBulkShieldCut);
			}
			else
				bulkHDCCylAllSub  = bulkHDCCyl;
		}
		bulkConcShieldLog = new G4LogicalVolume(bulkHDCCylAllSub,
											concreteHDC, "BulkNBeamConcShieldLog");
		bulkConcShieldLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));
		//bulkConcShieldLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
		new G4PVPlacement( rotCenter, posBulkShieldHDC, bulkConcShieldLog, "SurfaceHdcInnerShield",
							fDetLogical, false, 0, fCheckOverlap); 
	}//placeSurfaceHDCInnerShield
	

	//LAYER  HDC SURFACE . neutronShielding to get n flux crossing	
	G4LogicalVolume* surfaceHdcLayerLog  = NULL;
	if(placeSurfaceHdcLayer)
	{
		G4double cutBoxHDCLayerHalfY  = (bulkHdcRmax - surfaceHDCInnnerMax)/2. + 1.*m;  //extra 
		G4Box* bulkHDCLayerPCutBox    = new G4Box("bulkHDCLayerPCutBox", 
				protonBoxOuterHalfXZ, cutBoxHDCLayerHalfY, protonBoxOuterHalfXZ);
		G4ThreeVector  transBulkLayerShieldCut(0., -surfaceHDCInnnerMax- (bulkHdcRmax
				- surfaceHDCInnnerMax)/2., -posBulkShieldHDCZ); //adjust extra Y
	
		
		G4SubtractionSolid* surfaceHdcLayerAnySub = NULL;
		//multiStepRuns already taken care of
		if(placeNTubeOuter )
		{
			if(placeProtonBox)
			{
				surfaceHdcLayerAnySub  = new G4SubtractionSolid("bulkShieldLayerSub", 
					hdcLayerNOuterSub, bulkHDCLayerPCutBox, rotCenter, transBulkLayerShieldCut);
			}
			else
				surfaceHdcLayerAnySub =  hdcNOuterSub;
		}
		else
		{
			if(placeProtonBox)
			{
				surfaceHdcLayerAnySub = new G4SubtractionSolid("bulkShieldLayerSub", 
					bulkHdcLayerCyl, bulkHDCLayerPCutBox, rotCenter, transBulkLayerShieldCut);
			}
			else
				surfaceHdcLayerAnySub  = bulkShieldLayerDSub;

		}
		surfaceHdcLayerLog  = new G4LogicalVolume( surfaceHdcLayerAnySub,
																	concreteHDC, "SurfaceHdcLayerLog"); 
		surfaceHdcLayerLog->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));
		//surfaceHdcLayerLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
		new G4PVPlacement( rotCenter, posBulkShieldHDC, surfaceHdcLayerLog, "SurfaceHdcLayer",
						fDetLogical, false, 0, fCheckOverlap); 	
	}
	

	G4double angleStart 	      = 0;
	G4double angleSpan			  = twopi;
	if(placeProtonBox) 
	{
		angleStart 				  = -84*degree; //-83*degree, 345*degree);
		angleSpan				  = 348*degree;
	}

		
	
	if(braneBaseDet && basementCsIDet) 
	{
		G4cout << "Detectors: Brane and CsI set. Aborting " << G4endl;
		std::abort();
	}
	G4bool boxDet	   			= false;
	G4bool surfaceDet 			= false;
	G4double maxStepFactor		= 0.5; // x maxStep	
	G4double detCenterDist		= detAxialDist; // 	if(! immersedDet)
	G4double detWallDist		= 0.25*m;
	//Cuts
	G4Region* regDet = new G4Region("Det_CsI");	
	
	////
	//DETECTOR, annulus
    G4Tubs*  detWaterCylFull 		= NULL;
    G4ThreeVector* posDetector 		= NULL;
	
	if(basementCsIDet && ! boxDet)
	{	
				
		G4double detCsIRad          = 5.5*cm;
		G4double detPbRad           = detCsIRad +18.*cm;
		G4double detCdRad           = detPbRad + 0.5*mm;
		G4double detWaterRad        = detCdRad + 12.*2.54*cm;
		G4double detCsIHalfHt       = 16.5*cm;//33/2 
		G4double detPbHalfHt        = detCsIHalfHt + 18.*cm;
		G4double detCdHalfHt        = detPbHalfHt + 0.5*mm; 
		G4double detWaterHalfHt	    = detCdHalfHt + 12.*2.54*cm; 
	

		if(immersedDet)
		{
			detCenterDist	= basementRad + tunnelWall + detWallDist+ detWaterRad;
		}

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

		
		G4cout << "\n\nConstructed around Basement a **BASEMENT DETECTOR** at z: "
		<<  -detToTargetFullZ/m << " m, radius: "  << detCenterDist/m 
		<< " CsI:: rad= "<< detCsIRad/cm << " cm, ht= " << 2.*detCsIHalfHt/cm 
		<< " cm:\nLeadShield:: rad= "<< detPbRad/cm << " cm, ht= " 
		<< 2.*detPbHalfHt/cm << " cm.\nCd Shield:: rad= " << detCdRad/cm << " cm, ht= "
		<< 2.*detCdHalfHt/cm << " cm.\nWATER:: rad= " << detWaterRad/cm << " cm, ht= "
		<< 2.*detWaterHalfHt/cm << G4endl;
		
	}

	G4Tubs* braneDetScintCyl	= NULL;
	
	if(braneBaseDet)
	{
		G4double detScintRad	= 2.5*2.54*cm; //5inch/2
		G4double detScintHht	= 2.5*2.54*cm; //5inch/2	
		
		if(immersedDet)
		{
			detCenterDist		= basementRad + tunnelWall + detWallDist+ detScintRad;
		}
		
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

	
	//DetOuterPad to contain detector	
	G4LogicalVolume* outerPadLog 		= NULL;
	if(immersedDet && ( basementCsIDet || braneBaseDet))
	{
		G4double basementRoofHht = baseRoofTopHalf + baseRoofStoneHalf 
                                    + baseRoofInHalf + baseLayerDetHalf; 
									//5/2ft + 18/2 inch. excl instr. floor
		G4double nDetIn			 = basementRad + tunnelWall; //baseRadiusAll
		G4double posNPadZ 		 = basementPosZ - basementRoofHht - instrFloorHalfHt;
		
		G4SubtractionSolid* outerPadSub = NULL;
		G4ThreeVector posDetImmerseSub(0, 0, -posNPadZ -detToTargetFullZ );
		G4Tubs* outerPad = new G4Tubs("outerPadLayer", nDetIn, nDetIn + detInPadThick, 
						basementHalfHt + instrFloorHalfHt - basementRoofHht, 0, twopi);		
		if( basementCsIDet )
		{
			if(!detWaterCylFull) { G4cout << " detWaterCylFull Null " << G4endl; std::abort();}
			outerPadSub = new G4SubtractionSolid("OuterPadSub",
						outerPad, detWaterCylFull, rotCenter, posDetImmerseSub);
		}
		else if(braneBaseDet)
		{
			if(!braneDetScintCyl) { G4cout << " braneDetScintCyl Null " << G4endl; std::abort();}
			outerPadSub = new G4SubtractionSolid("OuterPadSub",
						outerPad, braneDetScintCyl, rotCenter, posDetImmerseSub);		
		}
		outerPadLog = new G4LogicalVolume(outerPadSub, lightMaterial, "outerPadLayerLog");
		outerPadLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta())); 
		G4ThreeVector posNPad(0, 0, posNPadZ); //-8.0906
		new G4PVPlacement( rotCenter, posNPad, outerPadLog, "DetOuterPad",
							fDetLogical, false, 0 , fCheckOverlap);	

	}

	
	//Production Cut
	G4ProductionCuts* cutDet = new G4ProductionCuts();
	cutDet->SetProductionCut(1.0*um);
	regDet->SetProductionCuts(cutDet);
	

	
	G4LogicalVolume* hdcOuterLightLog = NULL;
	if(hdcOuterMat)
	{		
		G4ThreeVector posOutHDC( 0, 0, hdcOutLightPosZ );
		
		G4Box* protBox = new G4Box("ProtBoxhdcOut" ,protonBoxOuterHalfXZ, 
								protonBoxOuterHalfY, protonBoxOuterHalfXZ);
	
		G4ThreeVector  transHdcOutPBox(0, -protonBoxOuterHalfY-monoSteelRmax, hdcOutLightPosZ); 
		G4ThreeVector hdcOutInsPos (0,0, -instrFloorTop2Tgt-instrFloorHalfHt + hdcOutLightPosZ );
		
		//multiStepRuns already taken care of
		if(placeNTubeOuter)
		{
			G4SubtractionSolid* hdcOutInstFS   =  new G4SubtractionSolid("hdcOutInstSub",
						hdcOutLightShieldSub, instrFloorCyl, rotCenter, hdcOutInsPos);
			
			if( placeProtonBox )
			{
				G4SubtractionSolid* hdcOutLSPS = new G4SubtractionSolid("hdcOutLightSubPbox", 
									hdcOutInstFS, protBox, rotCenter, transHdcOutPBox);
				hdcOuterLightLog = new G4LogicalVolume(hdcOutLSPS, lightMaterial, "hdcOutLightLog");
			}
			else
				hdcOuterLightLog = new G4LogicalVolume(hdcOutInstFS, lightMaterial, "hdcOutLightLog");
		}
		else
		{
			G4SubtractionSolid* hdcOutInstFS2   =  new G4SubtractionSolid("hdcOutInstSub",
									hdcOutLightShield, instrFloorCyl, rotCenter, hdcOutInsPos);
			if( placeProtonBox )
			{
				G4SubtractionSolid* hdcOutLSS2 = new G4SubtractionSolid("hdcOutLightSub2", 
									hdcOutInstFS2, protBox, rotCenter, transHdcOutPBox);
				hdcOuterLightLog = new G4LogicalVolume(hdcOutLSS2, lightMaterial, "hdcOutLightLog");				
			}
			else
				hdcOuterLightLog = new G4LogicalVolume(hdcOutInstFS2, lightMaterial, "hdcOutLightLog");
		}
		hdcOuterLightLog->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow())); 
		new G4PVPlacement( rotCenter, posOutHDC, hdcOuterLightLog, "OuterHDClight", 
						   fDetLogical, false, 0 , fCheckOverlap);
	}
	
	
	
	G4Tubs* surfaceDetSolid 		= NULL;
	G4ThreeVector* posSurfDet 		= NULL;
	G4LogicalVolume* surfaceDetLog 	= NULL;
	if(surfaceDet)
	{
		detAxialDist = 25.*m;//10.25*m ;//25.*m; //x=19.2, y -16 =>detAxialDist = 25m;
		
		G4double surfaceDetHalfThick = 5.*cm; //10cm/2
		G4double surfaceDetInner     = detAxialDist - surfaceDetHalfThick;
		G4double surfaceDetOuter     = detAxialDist + surfaceDetHalfThick; 
		G4double surfaceDetHalfHt    = 1.75*m; //3m/2 
		surfaceDetHalfHt    = 0.75*m; //1.25*m;
		G4double sufDetPosZ = -1.25*m;
		
		//sufDetPosZ = -0.75*m;
		
		posSurfDet = new G4ThreeVector(0, 0, sufDetPosZ);
		surfaceDetSolid      = new G4Tubs ("surfaceDetSolid", surfaceDetInner, 
										   surfaceDetOuter, surfaceDetHalfHt, angleStart, angleSpan );
		G4Material* poly1  = G4NistManager::Instance()->FindOrBuildMaterial("HDPE");
		surfaceDetLog = new G4LogicalVolume(surfaceDetSolid, poly1, "surfaceDetLog");
		surfaceDetLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red())); 
		new G4PVPlacement( rotCenter, *posSurfDet, surfaceDetLog, "SurfaceDetector", 
						   fDetLogical, false, 0 , fCheckOverlap);
		G4cout << "\n\nConstructed ***SURFACE DETECTOR **** on the Target Hall at " 
		<< detAxialDist/m << " m, having thickness: " << 2*surfaceDetHalfThick/cm
		<< " cm,ht= " << 2*surfaceDetHalfHt/m << " m" << G4endl;
	}			

    //cuts. NOTE: don't define region without setting default cut/or in macro
    //parallel world has cut of the world by default. Don't set it, otherwise it will
     // be taken for the default.
    //G4RunManagerKernel::GetRunManagerKernel()->SetVerboseLevel(2);
    G4Region* regBase = new G4Region("BasementIn");
	if(basementInLog)regBase->AddRootLogicalVolume(basementInLog);
	if(bulkConcShieldLog)regBase->AddRootLogicalVolume(bulkConcShieldLog);
	if(monoBaseLog)regBase->AddRootLogicalVolume(monoBaseLog);
	if(instrFloorLog)regBase->AddRootLogicalVolume(instrFloorLog);
	if(monoSteelLog)regBase->AddRootLogicalVolume(monoSteelLog);
    if(innerUpperSteelD2OLog)regBase->AddRootLogicalVolume(innerUpperSteelD2OLog);
    if(innerBotSteelD2OLog)regBase->AddRootLogicalVolume(innerBotSteelD2OLog);
    if(outerSteelLog)regBase->AddRootLogicalVolume(outerSteelLog);
    if(monoSteelLog)regBase->AddRootLogicalVolume(monoSteelLog);
    if(basementRoofInLog)regBase->AddRootLogicalVolume(basementRoofInLog);
    if(surfaceHdcLayerLog)regBase->AddRootLogicalVolume(surfaceHdcLayerLog);

    G4ProductionCuts* cutBase = new G4ProductionCuts();
    cutBase->SetProductionCut(10.*mm);
    regBase->SetProductionCuts(cutBase);


    G4Region* regBaseLayer = new G4Region("BasementLayer");
	if(basementLayerLog)regBaseLayer->AddRootLogicalVolume(basementLayerLog);
	if(tunnelWallLog)regBaseLayer->AddRootLogicalVolume(tunnelWallLog);
    if(tunWallDetLog)regBaseLayer->AddRootLogicalVolume(tunWallDetLog);
	if(baseRoofDetLog)regBaseLayer->AddRootLogicalVolume(baseRoofDetLog);
    G4ProductionCuts* cutBase2 = new G4ProductionCuts();
    cutBase2->SetProductionCut(1.*mm);
    regBaseLayer->SetProductionCuts(cutBase2);

    G4bool chargedParticle = false; //not needed for neutrons as biaing particles
	if(chargedParticle)
    { //select from the following
        G4double maxStep2 = 2.*cm;// 0.5*5.5*m/65 = 4.2cm; //stepHt/2
        G4UserLimits* stepLimit2 = new G4UserLimits(maxStep2);
        if(nBeamOuterLog)nBeamOuterLog->SetUserLimits(stepLimit2);
        if(nOuterShieldLog)nOuterShieldLog->SetUserLimits(stepLimit2);
        if(monoSteelLog)monoSteelLog->SetUserLimits(stepLimit2);	
        if(instrFloorLog)instrFloorLog->SetUserLimits(stepLimit2);
        if(basementInLog)basementInLog->SetUserLimits(stepLimit2);
        if(basementLayerLog)basementLayerLog->SetUserLimits(stepLimit2);	
        if(monoBaseLog)monoBaseLog->SetUserLimits(stepLimit2);
        if(tunnelWallLog)tunnelWallLog->SetUserLimits(stepLimit2);
        if(tunWallDetLog)tunWallDetLog->SetUserLimits(stepLimit2);
        if(basementRoofInLog)basementRoofInLog->SetUserLimits(stepLimit2);
        if(basementRoofTopLog)basementRoofTopLog->SetUserLimits(stepLimit2);
        if(basementRoofStoneLog)basementRoofStoneLog->SetUserLimits(stepLimit2);
        if(bulkConcShieldLog)bulkConcShieldLog->SetUserLimits(stepLimit2);
        if(surfaceHdcLayerLog)surfaceHdcLayerLog->SetUserLimits(stepLimit2);	
        if(outerPadLog)outerPadLog->SetUserLimits(stepLimit2);
        if(hdcOuterLightLog)hdcOuterLightLog->SetUserLimits(stepLimit2);
        if(surfaceDetLog)surfaceDetLog->SetUserLimits(stepLimit2);
	}
	G4cout << "\nWorld of size (m) = " << 2*halfX/m<< " "<< 2*halfY/m << " " 
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
		outerSteelR2/cm-outerSteelR1/cm << " " << 2*outerSteelHht/cm << " from "
		<< outerSteelR1/cm << " to " << outerSteelR2/cm << " at center" <<	G4endl;
		
	if(placeProtonBox)
	{
		G4cout <<"OUTER Proton Box of CS (m) " <<  2*protonBoxOuterHalfXZ/m 
				<< " and length "<< 2.*protonBoxOuterHalfY/m << G4endl;
	}
	G4cout << "MonolithSteelCyl of rad from "<< monoSteelRmin/m << " m to "
		<< monoSteelRmax/m << " m of ht " << 2.*monoSteelHalfHt/m << " m " << G4endl;
	G4cout << "Constructed Outer Concrete Building of rad from "
		<< bulkConcShieldRmin/m << " m to "	<< bulkHdcRmax/m 
		<< " of ht " << 2.*bulkConcShieldHalfHt/m << " m" <<G4endl;
	G4cout << "Constructed Basement of rad "<< (basementRad+tunnelWall)/m 
		<< " (m) of ht "  << 2.*basementHalfHt/m << " m of "<<  materialType<< G4endl;
	G4cout << "Defined Region: Basement" << G4endl;
       

} 	//END OF DETECTOR construction
//define G4StepLimiter process class in physics list, to each particle the step to be limitted 
//otherwise, G4UserLimit is not effective.
//to make more smooth visulisation:
//G4TransportationManager* tm = G4TransportationManager::GetTransportationManager();
//tm->GetPropagatorInField()->SetLargestAcceptableStep(1.*mm);

/*static functions
 G 4*Colour::White()
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
 // Testing sub Multi step det from nTubes outer
 
 std::vector <G4double> posxy = GetPosNOuter(0, 0, lH3posX, lH3posY,
 lH1halfX, 8.*m, lH1posY, lH1Rot, nBeamOuterStart );
 
 G4RotationMatrix* rotFor = new G4RotationMatrix; 
 rotFor->rotateZ(rotZ[0]);			
 rotFor->rotateX(rotX[0]);		
 G4RotationMatrix* rotInv = new G4RotationMatrix; 
 rotInv->rotateX(-rotX[0]);		
 rotInv->rotateZ(-rotZ[0]);//note order
 G4RotationMatrix* rotNull = new G4RotationMatrix; 
 
 G4double posnTubeZ	= IRPhalfHt + nTubeHalfDy1[0]; //rotated y->z	
 G4ThreeVector posFor(posxy[0], posxy[1], posnTubeZ);
 G4Tubs* testMS  = new G4Tubs("Test",4.*m, 5.*m, 5.*m, 0, twopi);
 //G4ThreeVector posInv(-posxy[0], -posnTubeZ, -posxy[1]);
 G4Box * testT = new G4Box("testTube", 1.*m, 2.*m, 10.*m);
 
 
 G4SubtractionSolid* testMSsub = new G4SubtractionSolid("sub", testMS, testT, rotFor, posFor);
 G4LogicalVolume* testL = new G4LogicalVolume(testMS, Al, "testMS1Log");
 new G4PVPlacement( rotInv, posInv, testL, "TestMS", fDetLogical, false, 0, fCheckOverlap); 
 
 G4cout << posInv.getX() << " " << posInv.getY() << " " << posInv.getZ() << G4endl;
 G4SubtractionSolid* sub = new G4SubtractionSolid("sub", testT, testMS, rotInv, posInv);
 G4LogicalVolume* testTL		= new G4LogicalVolume(testT, Al, "TestTubeLog");
 new G4PVPlacement( rotNull, *posCenter, testTL, "TestTube", fDetLogical, false, 0, fCheckOverlap);  
 
 return;
 //Testing END
 */
 
 
//G4Trd* nBeamShieldTrd = new G4Trd("nBeamShieldTrd", nTubeHalfDx1, nTubeHalfDx2, 
//                                nTubeHalfDy1, nTubeHalfDy2, nTubeHalfDz);      
/*
     G4AffineTransform* affine1 = new G4AffineTransform(rotnNTube, posNTubeOuter);
 G4AffineTransform* affine2 = new G4AffineTransform(rotnNTube1->inverse(),
 G4ThreeVector(posNTubeOuter1.x()-posNTubeOuter.x(), posNTubeOuter1.y()-posNTubeOuter.y(),
 posNTubeOuter1.z()-posNTubeOuter.z()));   
 
 *affine1 *= *affine2;
 
 G4RotationMatrix* netRot = new G4RotationMatrix(affine1->NetRotation());
 G4ThreeVector* netPos = new G4ThreeVector(affine1->NetTranslation());
 secTube =  new G4UnionSolid("volNOuter2",nBeamShield,nBeamShield,netRot,*netPos);
 
 G4LogicalVolume* testLog = new G4LogicalVolume(secTube, concrete, "testLog");
 new G4PVPlacement(  rotnNTube1, posNTubeOuter1,testLog, "test", fDetLogical, false, 0, true);  
 
 G4Trd* singleBox = new G4Trd("singlebox", 3*m, 4*m, 3*m, 4*m, 5*m);
 */