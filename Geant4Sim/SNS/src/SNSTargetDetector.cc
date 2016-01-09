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
#include "SNSTargetOuterProtonBox.hh"
#include <sstream>
#include <vector>
#include <cmath>  
#include <cassert>
#include "G4RunManager.hh"

SNSTargetDetector::SNSTargetDetector(G4String detName )
:VBaseDetector(detName), fMSDet1Inner(0.), fMSDet2Inner(0.)
,fMSDetCylThick(0.), fMSDetCylHht(0.),fIsMSDet2(true)
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
	assert(fMSDet1Inner > 0 || fMSDet2Inner > 0);
	assert(fMSDetCylThick > 0 && fMSDetCylHht >0);
	
	G4Tubs* mStepCyl1		= new G4Tubs("multiStepCylSolid1",fMSDet1Inner, 
				fMSDet1Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);

	G4SubtractionSolid* sub1	= new G4SubtractionSolid("sub1",
									 fromSolid, mStepCyl1, rot, position);
	G4SubtractionSolid* sub		= sub1;
	G4SubtractionSolid* sub2	= sub1;
	
	//Det2 and 3 are independent
	if(fIsMSDet2)
	{
		G4Tubs* mStepCyl2   = new G4Tubs("multiStepCylSolid2",fMSDet2Inner, 
				fMSDet2Inner+ fMSDetCylThick, fMSDetCylHht, 0, twopi);
		sub2	= new G4SubtractionSolid(name, sub1, mStepCyl2, rot, position); 
		sub		= sub2;
	}
	
	return sub;
}


void SNSTargetDetector::CreateInnerNTube( G4double nTubeHalfDx1, 
		G4double nTubeHalfDx2, G4double nTubeHalfDy1, G4double nTubeHalfDy2,
		G4double nTubeHalfDz, G4double thickNTubeAlX, G4double thickNTubeAlY,
		G4RotationMatrix* rotNTube, G4ThreeVector posNTube, G4String tubeName,
		G4double outerSteelR2, G4double outerSteelHt, const G4bool overlap)
{
	
	G4RotationMatrix* rotCenter = new G4RotationMatrix();
	G4ThreeVector* posCenter = new G4ThreeVector(0,0,0);
	G4Trd* nTubeOut     = new G4Trd("nTubeOut", nTubeHalfDx1, nTubeHalfDx2, 
									nTubeHalfDy1, nTubeHalfDy2, nTubeHalfDz);
	G4Trd* nTubeIn      = new G4Trd("nTubeIn", nTubeHalfDx1-thickNTubeAlX,  
						nTubeHalfDx2-thickNTubeAlX, nTubeHalfDy1-thickNTubeAlY, 
						nTubeHalfDy2-thickNTubeAlY, nTubeHalfDz);
	
	G4Tubs* outerSteelDummy = new G4Tubs ("outerSteelDummy", 0, 
										  outerSteelR2, outerSteelHt, 0, twopi);
	G4IntersectionSolid* nTubeXouterSteel1 = new G4IntersectionSolid(
		tubeName+"XouterSteel1", outerSteelDummy, nTubeOut, rotNTube, posNTube);
	G4IntersectionSolid* nTubeXouterSteel2 = new G4IntersectionSolid(
		tubeName+"XouterSteel2", outerSteelDummy, nTubeIn, rotNTube, posNTube);
	G4SubtractionSolid* nTubeSub = new G4SubtractionSolid(tubeName+"OutSub", 
				nTubeXouterSteel1, nTubeXouterSteel2, rotCenter, *posCenter);										  
	G4Material* alum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"); 
	G4LogicalVolume* nTubeLog  = new G4LogicalVolume(nTubeSub, alum, tubeName + "Log");
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
	const G4double posNTubeOuterX[] = {lH3posX +(lH1halfX+nBeamOuterZhLen)*std::cos(rot+dT),
		lH3posX - (lH1halfX+nBeamOuterZhLen)*std::cos(rot+dT), 
		(lH1halfX +nBeamOuterZhLen)*std::cos(dT),
		-lH3posX +(lH1halfX+nBeamOuterZhLen)*std::sin(90.*degree -rot+dT),
		-lH3posX -(lH1halfX+nBeamOuterZhLen)*std::sin(90.*degree -rot+dT),
		-(lH1halfX +nBeamOuterZhLen)*std::cos(dT)};

	const G4double posNTubeOuterY[] = {lH3posY-(lH1halfX +nBeamOuterZhLen)*std::sin(rot+dT),
		lH3posY+(lH1halfX +nBeamOuterZhLen)*std::sin(rot+dT), 
		lH1posY- (lH1halfX +nBeamOuterZhLen)*std::sin(dT),
		lH3posY+ (lH1halfX +nBeamOuterZhLen)*std::cos(90.*degree -rot+dT),
		lH3posY- (lH1halfX +nBeamOuterZhLen)*std::sin(rot-dT),
		lH1posY+ (lH1halfX +nBeamOuterZhLen)*std::sin(dT)};

	const G4int multx[] = {1,-1,1,1,1,1};
	const G4int multy[] = {1,1,1,1,1,1};
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

	const G4int ffx[] = {1,-1,0,-1,1,0};
	const G4int ffy[] = {1,-1,1,1,-1,-1};
	const G4int ff2x[] = {1,1,1,1,-1,1};	
	const G4int ff2y[] = {1,1,0,-1,1,0};	
	std::vector <G4double> posxy(2);
	if( i==2) { xx = radC; }
	if( i==5) { xx = -radC; }
	posxy[0] = posNTubeOuterX[i] + ff2x[i]*xx - ffx[i]*fact*sin(rot) ;
	posxy[1] = posNTubeOuterY[i] - ff2y[i]*yy - ffy[i]*fact*cos(rot) ;
	
	return posxy;
}


void SNSTargetDetector::ConstructDet()
{

	G4bool fCheckOverlap 			= false;
	const G4double nShieldXY 		= 1.5*m;

	const G4bool braneBaseDet		= true; //BASEBRANEDET
	const G4bool basementCsIDet 	= false; //BASECSIDET


	G4bool testDet1     			= false; //NOTE no SNS :
	G4bool boxBasement				= false; //rectangular basement
	
	const G4bool multiStepRuns 		= false; //MSDETRUN
	fIsMSDet2 						= true; //ISMSDET2
	fMSDet1Inner     				= 8.0*m;	//MSDET1INRAD
	fMSDet2Inner     				= 10.0*m;	//MSDET2INRAD
	
	const G4bool testPbox 			= false; 
	const G4bool braneTest     		= false;  //no SNS : braneBaseDet is set below

	G4bool fStairCase				= true;  
	G4bool fPlaceMagnet				= true;
	G4bool baseExtn					= true;
	const G4bool placeProtonBox		= true; //PROTONBOX
	const G4bool placeNTubeInner 	= true; //NTUBEINNER
	const G4bool nOuterUnionShields	= true; //NSHIELD
	const G4bool placeNTubeOuter	= true; //NTUBEOUTER
	const G4bool basementHallRoof 	= true;	
	const G4bool placeSurfHdcInShield	= true;
	const G4bool placeSurfaceHdcLayer	= true;

	
    fMSDetCylThick                  = 1.*mm;
	const G4double tunWallDetThick  = 1.*mm;
	const G4double tunWallDetHht 	= 0.5*m;
	const G4double baseExnFillWidth	= 5.*m;
	
	const G4double stairPosX		= 9.*m;
	const G4double stairPosY 		= -21.*m;
	const G4double stairPosZ 		= -5.*m;
	const G4double stairHfX			= 2.5*m;
	const G4double stairHfY			= 1.5*m;
	const G4double stairHfZ			= 4.*m;
	
	if(boxBasement) baseExtn 		= false;
	   
	G4RotationMatrix* rotStairCase 	= new G4RotationMatrix;
	G4ThreeVector posStairCase(stairPosX, stairPosY, stairPosZ);
	G4Box* stairCaseSolid = new G4Box("StairCase", stairHfX, stairHfY, stairHfZ);
	
    // expt volume
    G4NistManager* nist = G4NistManager::Instance();
    //G4Material* airMat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* expMat = nist->FindOrBuildMaterial("G4_Galactic");
	
	const G4bool placeInner = true;  // false is for vis testing
	
    const G4double halfX 	= 35.*m;
    const G4double halfY 	= 35.*m;//7.*m;
    const G4double halfZ 	= 35.*m;//60.*m;
    G4Box* snsTargetBox 	= new G4Box("SNSexptBox", halfX, halfY, halfZ);
    G4String detLogName 	= "SNSTargetLogical";
    fDetLogical = new G4LogicalVolume(snsTargetBox, expMat, detLogName );

    enum draw { wire=0, solid=1, invisible=2 };
	G4ThreeVector* posCenter	= new G4ThreeVector(0, 0, 0);  
	G4RotationMatrix* rotCenter	= new G4RotationMatrix;	
	
	G4Material* steel 		= G4NistManager::Instance()->FindOrBuildMaterial("Steel");   
	G4Material* concrete 	= G4NistManager::Instance()->FindOrBuildMaterial("Concrete"); 
	G4Material* concreteHDC = G4NistManager::Instance()->FindOrBuildMaterial("HDConcrete");
	G4Material* water 		= nist->FindOrBuildMaterial("G4_WATER"); 
	G4Material* steelD20 	= G4NistManager::Instance()->FindOrBuildMaterial("SteelD2O"); 
	G4Material* alum 		= G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"); 
	G4Material* lightMat 	= G4NistManager::Instance()->FindOrBuildMaterial("LightConcrete");
	G4Material* stone 		= G4NistManager::Instance()->FindOrBuildMaterial("RoadStone");
	G4Material* matc7h8 	= G4NistManager::Instance()->FindOrBuildMaterial("MatC7H8");
	G4Material* hdpe        = G4NistManager::Instance()->FindOrBuildMaterial("HDPE");
	G4Material* acrylic     = G4NistManager::Instance()->FindOrBuildMaterial("Acrylic");	

	if(testPbox)
	{
		SNSTargetOuterProtonBox* pbox = new SNSTargetOuterProtonBox("testPBox"
		,2.0*m, 15.*m, 2.0*m, false, true, 4.*m, 8.0*m, 5.*cm ,10.*m, 0,
						true, 40.*cm, 60*cm, true);
		G4ThreeVector* psPbox = new G4ThreeVector(0, -15.*m-5.*m, 0);
		pbox->Place(rotCenter, psPbox, fDetLogical); 
		return;
	}
		
	
    if(testDet1)
    {
		G4bool fluxRun = true;
		if(fluxRun)
		{
			const G4double detRad 		= 2.5*2.54*cm;
			const G4double detHfZ 		= 2.5*2.54*cm; 
			const G4double detAcrThick  = 2.* 2.54*cm;
			const G4double detHdpeThick = 1.5* 2.54*cm;
			const G4double detWaterThick= 3.5* 2.54*cm;
			
			G4Tubs* fluxDet  		= new G4Tubs ("fluxDet", 0, detRad, detHfZ, 0, twopi);

			
			G4LogicalVolume* fluxDetLog = new G4LogicalVolume(fluxDet, matc7h8, "FluxDetLog");
			fluxDetLog->SetVisAttributes(new G4VisAttributes (G4Colour::Cyan()));
			new G4PVPlacement( rotCenter, *posCenter, fluxDetLog, "FluxRunDet",
										fDetLogical, false, 0 , fCheckOverlap);

			G4cout  << "Placed test FluxRunDet of rad " <<  detRad/cm << G4endl;
			
	
			G4Tubs*   detAcrCyl           = new G4Tubs ("FluxRunAcr", detRad, 
							detRad+detAcrThick,	detHfZ+ detAcrThick, 0, twopi);

			G4LogicalVolume* detAcrlicLog = new G4LogicalVolume(detAcrCyl,
													acrylic, "FluxRunAcrlicLog");
			
			detAcrlicLog->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));    
			new G4PVPlacement( rotCenter, *posCenter, detAcrlicLog, "FluxRunAcrlic", 
							   fDetLogical, false, 0 , fCheckOverlap); 
			G4cout  << "Placed Acrylic of rad " <<  detRad/cm << " to " <<
												(detRad+detAcrThick)/cm << G4endl;
			
			
			G4Tubs*   detHdpe           = new G4Tubs ("FluxRunHdpe", detRad+detAcrThick, 
			detRad+detAcrThick + detHdpeThick, detHfZ+ detAcrThick + detHdpeThick, 0, twopi);

			G4LogicalVolume* detdetHdpeLog = new G4LogicalVolume(detHdpe,
																hdpe, "FluxRunHdpeLog");
			
			detdetHdpeLog->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));    
			new G4PVPlacement( rotCenter, *posCenter, detdetHdpeLog, "FluxRunHdpe", 
							   fDetLogical, false, 0 , fCheckOverlap); 
			G4cout  << "Placed Hdpe of rad " <<  (detRad+detAcrThick)/cm
					<<  " to " << (detRad+detAcrThick + detHdpeThick)/cm 
					<< G4endl;												
			
			
			G4Tubs*   detWater          = new G4Tubs ("FluxRunHdpe", 
				detRad+detAcrThick+ detHdpeThick, detRad+detAcrThick +detHdpeThick 
				+detWaterThick, detHfZ+ detAcrThick + detHdpeThick+detWaterThick, 0, twopi);

			G4LogicalVolume* detWaterLog = new G4LogicalVolume(detWater,
													water, "FluxRunWaterLog");
			
			detWaterLog->SetVisAttributes(new G4VisAttributes (G4Colour::White()));    
			new G4PVPlacement( rotCenter, *posCenter, detWaterLog, "DetWaterShield", 
							   fDetLogical, false, 0 , fCheckOverlap); 
			G4cout  << "Placed Water of rad " << (detRad+detAcrThick+ detHdpeThick )/cm
					<< " to " << (detRad+detAcrThick +detHdpeThick +detWaterThick )/cm
					<< G4endl;
			
			return;
		} //end of FluxRunDet
		
		
		//if fluxrun false
        const G4double r1     =   0.*cm;
		const G4double r2     =   5.0*m;
        const G4double r3     =   15.0*m;
		const G4double r4     =   20.0*m;	

		G4Material* testConcrete = G4NistManager::Instance()->FindOrBuildMaterial("TestConcrete");
        G4Sphere* testSph1  = new G4Sphere ("testSph1", r1, r2, 0, twopi, 0, pi);
		G4LogicalVolume* testSph1Log = new G4LogicalVolume(testSph1,testConcrete, "TestSph1Log");
		testSph1Log->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
		new G4PVPlacement( rotCenter, *posCenter, testSph1Log, "VolOne",
                       fDetLogical, false, 0 , fCheckOverlap);
        //r2=0.;

		G4Sphere* testSph2  = new G4Sphere ("testSph2", r2, r3, 0, twopi, 0, pi);
		G4LogicalVolume* testSph2Log = new G4LogicalVolume(testSph2,matc7h8, "TestSph2Log");
		testSph2Log->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));
		new G4PVPlacement( rotCenter, *posCenter, testSph2Log, "VolTwo",
						   fDetLogical, false, 0 , fCheckOverlap);
		
		
		G4Sphere* testSph3  = new G4Sphere ("testSph3", r3, r4, 0, twopi, 0, pi);
		G4LogicalVolume* testSph3Log = new G4LogicalVolume(testSph3,stone, "TestSph3Log");
		testSph3Log->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
		//new G4PVPlacement( rotCenter, *posCenter, testSph3Log, "VolThree",
		//				   fDetLogical, false, 0 , fCheckOverlap);
		
		G4Region* regTest 			= new G4Region("RegionTest");
		if(testSph1Log)regTest->AddRootLogicalVolume(testSph1Log);
		G4ProductionCuts* cutTest 	= new G4ProductionCuts();
		cutTest->SetProductionCut(10*mm);
		regTest->SetProductionCuts(cutTest);
		
		G4Region* regTest2 			= new G4Region("RegionTest2");
		if(testSph2Log)regTest2->AddRootLogicalVolume(testSph2Log);
		G4ProductionCuts* cutTest2 	= new G4ProductionCuts();
		cutTest2->SetProductionCut(0.01*mm);
		regTest2->SetProductionCuts(cutTest2);
		
		return;
    } //end of testDet1

 
    //TARGET HG box   //red
    const G4double tgtHalfHt = 5.2*cm; //4.9  //Jul 7, 2015
    const G4double tgtHalfX = 19.95*cm; //21
    const G4double tgtHalfY = 25.0*cm; //20.3
    G4Box* targetBox 		= new G4Box("HgBox", tgtHalfX, tgtHalfY, tgtHalfHt); //HALF
    G4Material* matHg 		= G4NistManager::Instance()->FindOrBuildMaterial("G4_Hg");   
    G4LogicalVolume* targetBoxLog = new G4LogicalVolume(targetBox, matHg, "TargetBoxLog");
    targetBoxLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
	if(placeInner) new G4PVPlacement( rotCenter, *posCenter, targetBoxLog, "HgTarget",
                       fDetLogical, false, 0, fCheckOverlap);
    // beam profile footprint on target is 7x20 cm2, covering 90% of the beam, 
    //which means 2 sigma on each direction. SET it in macro

    /*if(braneTest) 
	 * 		return;
    */

    //HG STEELBOX steel plate surrounding matHg target.  thick =0.7cm
    const G4double tgtSteelThick = 0.7*cm; //all around
    const G4double tgtSteelHfX = tgtHalfX+tgtSteelThick;
	const G4double tgtSteelHfY = tgtHalfY+tgtSteelThick;
	const G4double tgtSteelHfZ = tgtHalfHt+tgtSteelThick; //5.6
	G4Box* steelHgBoxfull = new G4Box("SteelHgBoxFull", tgtSteelHfX,tgtSteelHfY, tgtSteelHfZ);
    G4SubtractionSolid* steelHgBox = new G4SubtractionSolid("SteelHgBox", 
                                   steelHgBoxfull, targetBox, rotCenter, *posCenter);
    G4LogicalVolume* HgSteelBoxLog = new G4LogicalVolume(steelHgBox, steel, "HgSteelBoxLog");
    HgSteelBoxLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
	if(placeInner) new G4PVPlacement( rotCenter, *posCenter, HgSteelBoxLog, "HgSteelBox",
                       fDetLogical, false, 0 , fCheckOverlap);


    //INNER REFLECTOR PLUG  0 to 49.5 cm rad, ht =  0 to 5.6+5 both ways
    const G4double innRPRad 		=  49.5*cm;
	//G4double  innRadIRP    	=  47.625*cm;
	const G4double innRPHHt 	=  11.2*cm;//10.6*cm; //7.6+ 2*1.8 = 11.2;
    G4Tubs* innRPFullCyl  = new G4Tubs ("innerRefPlug", 0, innRPRad, innRPHHt, 0, twopi);
	G4Box* IRPcutBox = new G4Box("IRPcutBox",tgtSteelHfX, innRPRad,tgtSteelHfZ);
    G4SubtractionSolid* IRPsolidSub = new G4SubtractionSolid("IRPsolidSub", 
                                innRPFullCyl, IRPcutBox, rotCenter, *posCenter);
    //placing later

    //BE PLUG
    //rad=0. to 33 . Ht=  from innRPHHt to innRPHHt+35. cm
    const G4double BePlugRad  	= 33.*cm;
	const G4double BePlugHalfHt = 17.5*cm;//35/2  from tgtSteelHfZ up/down
    G4Tubs* BePlugCyl = new G4Tubs ("BePlugCyl", 0.*cm, BePlugRad, BePlugHalfHt, 0, twopi);
	const G4double topBePlugZ		= innRPHHt + BePlugHalfHt;
	G4ThreeVector posUpperBePlug(0, 0, topBePlugZ);
	const G4double botBePlugZ		=	-topBePlugZ;
	G4ThreeVector posLowerBePlug(0, 0, botBePlugZ); 
    //placing later

	
    //MODERATORS
    //ht 5.6 to 5.6+25cm?.    rad upto corner of box 21 half size =29.8cm
    //coupled H Top //red
	const G4double lH1halfX = 5.*cm; //NOTE x,y interchanged to avoid rotation
	const G4double lH1halfY = 10.*cm;
	const G4double lH1halfZ = 12.5*cm;//25/2
	const G4double lH1posY = 10.*cm;
	G4Box* lHBox = new G4Box("liquidHBox1",lH1halfX, lH1halfY, lH1halfZ);
    //G4Tubs* liquidHCyl1 = new G4Tubs ("LiquidHCyl1", 0, 5.*cm, 15.*cm, 0, twopi);
	const G4double lH1posZ = tgtSteelHfZ + lH1halfZ;
	G4ThreeVector posLH1(0, lH1posY, lH1posZ);  //z=5.6 to 30.6
    G4RotationMatrix* rotLH1 = new G4RotationMatrix; 
    G4Material* matLHydr = G4NistManager::Instance()->FindOrBuildMaterial("LiquidHydrogen");  
    G4LogicalVolume* liquidHLog1 = new G4LogicalVolume(lHBox, matLHydr, "LiquidH1Log");
    liquidHLog1->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
	if(placeInner) new G4PVPlacement( rotLH1, posLH1, liquidHLog1, "LiquidHydr1",
                       fDetLogical, false, 0 , fCheckOverlap);   

	G4SubtractionSolid* IRPsolidSub1 = new G4SubtractionSolid("IRPsolidSub1", 
										IRPsolidSub, lHBox, rotLH1, posLH1);
	G4ThreeVector posLH1_BeTop(0, lH1posY, lH1posZ -topBePlugZ);//(tgtSteelHfZ+BePlugHalfHt));
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
	const G4double lH3posX = -10.*cm;
	const G4double lH3posY = -10.*cm;	
	G4ThreeVector posLH3(lH3posX, lH3posY, lH1posZ);      
    G4RotationMatrix* rotLH3 = new G4RotationMatrix; 
    const G4double lH1Rot = 48.75*degree;
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
        return;
    }

   

	G4SubtractionSolid* IRPsolidSub4 = new G4SubtractionSolid("IRPsolidSub4", 
										IRPsolidSub3, lHBox, rotLH4, posWatermod);
	G4ThreeVector posLH4_BeBot(-lH3posX, lH3posY, -lH1posZ-botBePlugZ);
    G4SubtractionSolid* BePlugBotSub2 = new G4SubtractionSolid("BePlugBotsub2", 
									BePlugBotSub1, lHBox, rotLH4, posLH4_BeBot);			

	
	//OUTER STEEL CYLINDER from 49 to 95cm. ht =219cm
	const G4double outerSteelR1   		=  innRPRad;
	const G4double outerSteelR2   		=  95.*cm; 
	const G4double outerSteelHt   		=  109.5*cm; //219/2
	G4Tubs* outerSteelCyl 	= new G4Tubs ("outerSteelCyl", outerSteelR1, 
									outerSteelR2, outerSteelHt, 0, twopi);
	//Proton cut
	G4Box* cutPBox1 		= new G4Box("cutPBox", tgtSteelHfX, outerSteelR2, tgtSteelHfZ);
	G4ThreeVector posCutPBox1(0,-outerSteelR1-(outerSteelR2 -outerSteelR1)/2., 0);
	G4SubtractionSolid* outerSteelCylSub1 = new G4SubtractionSolid("outerSteelCylSub1",
										outerSteelCyl, 	cutPBox1, rotCenter, posCutPBox1);
	//placing later


//SETTINGS 
	// x=18.3, y=-15.2, z= -7.1m (-8.1 + 1.m). 19.6m    // old 22.3*m . 18.3*m for c5 
const G4double detToTargetFullZ       	= 7.6*m;//8.1m-1m //old=7.8
const G4double detToWallGap       		= 0.5*m;	
const G4double detDiagonalDist      	= 20.*m + detToWallGap;
const G4double tunnelWidth				= 2.*m;

	const G4double baseExtraThick		= 7.*m; 
	const G4double baseRadiusAll		=  std::sqrt(detDiagonalDist*detDiagonalDist-detToTargetFullZ*detToTargetFullZ) 
										- detToWallGap;  //18.2*m ;// ; 18.3*m for c5
	G4double detCenterDist				= baseRadiusAll+ detToWallGap;

const G4double nBeamLineFullLength		= 20.0*m;
const G4double nShieldEnd 				= 1.0*m;
const G4double magnetPosInMonoSteel		= 2.5*m;
	const G4double magnetCutHalfL		= 45.*cm;
	const G4double magnetCutHZ			= 60.*cm;

	const G4double tunnelWall			= 18.*inch;
	const G4double protonBoxOuterHalfXZ = 2.0*m + magnetCutHZ;//3.52*m; // +beamHole
    const G4double instrFloorTop2Tgt  	= protonBoxOuterHalfXZ;//2.*m;
    const G4double basementRad        	= baseRadiusAll - tunnelWall;//18.3 - 18"
    const G4double instrFloorHalfHt   	= 9.*inch;//18"/2   
    const G4double baseRoofGravelRadLen = 4.*m+tunnelWall; // 
    const G4double basementTop2Tgt   	= instrFloorTop2Tgt + 2.*instrFloorHalfHt;
	const G4double basementHalfHt    	= (detToTargetFullZ - basementTop2Tgt+3.*m)/2.;  //8.3m/2 
	const G4double basementPosZ     	= - basementHalfHt - basementTop2Tgt; 
	const G4double surfaceLayerWidth    = 0.1*m;
	const G4double bulkHdcRmax        	= 10.*m; 
	const G4double hdcRaise           	= 1.5*m;
	const G4double surfaceHDCInnnerMax	= bulkHdcRmax - surfaceLayerWidth;
	const G4double bulkConcShieldHalfHt	= (instrFloorTop2Tgt+ hdcRaise)/2.;//1.5m above 

	const G4double boxBaseHalfY 		= baseRadiusAll + baseExtraThick;
	
    G4Tubs* dummySolid					= new G4Tubs ("DummySolid",0, 1.*mm, 1.*mm, 0, twopi);


    //MultiRuns Split Geometry
	G4SubtractionSolid* multiStepCyl1Sub  	= NULL;
	G4SubtractionSolid* multiStepCyl2Sub  	= NULL;
	G4double posMSDetZ						= 0.;
	
	if(multiStepRuns)
	{
		
		if(fMSDet2Inner <= fMSDet1Inner) fIsMSDet2 = false;
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
	}

	//INSTR. FLOOR
	const G4double	instrFloorPosZ 		= -instrFloorTop2Tgt -instrFloorHalfHt;
	G4SubtractionSolid* instrFloorDumSub = NULL;

	if( !boxBasement) 
	{
		G4Tubs* instrFloorSolid     	= new G4Tubs ("instrFloorCyl", 0, 
				basementRad + baseRoofGravelRadLen + 5.*m, instrFloorHalfHt, 0, twopi);
		instrFloorDumSub = new G4SubtractionSolid("instrFlSolid",
									instrFloorSolid, dummySolid,  rotCenter, *posCenter);
	}
	else //boxBasement
	{

		G4Box* instrFloorSolid	= new G4Box("instrFloorBox", baseRadiusAll + baseRoofGravelRadLen, 
											boxBaseHalfY, instrFloorHalfHt);
		instrFloorDumSub 		= new G4SubtractionSolid("instrFlSolid",
								instrFloorSolid, dummySolid,  rotCenter, *posCenter);
	}

	G4SubtractionSolid* instrFloorSub = NULL;
		
	if(multiStepRuns)	
	{
		G4ThreeVector instrFlPos(0,0, -instrFloorPosZ + posMSDetZ );		
		instrFloorSub = SubMultiStepDet(instrFloorDumSub, "instrFloorCyl", instrFlPos, rotCenter);
	}	
	else
		instrFloorSub = instrFloorDumSub;

	//MONOLITH STEEL cylinder
	const G4double monoSteelRmax    = 5.*m;
	const G4double monoSteelRmin    = outerSteelR2+1.*cm; //1.*m;
	const G4double monoSteelHalfHt  = 6.25*m; //12.5/2 
	const G4double protonBeamCutHXZ  = 21.0*cm; //42/2 cm is target in transverse 	
	const G4double transMonoZ       = 1.75*m;//6.25+1.75=8m bove; 6.25-1.75=4.5mbelow
	//minumum 1m ht below target is needed for innerSteelD2Ocyl2
	const G4double monoLidTopHalfHt = 2.0*m; // full ht=1m 
	const G4double monoLidBotHalfHt = 37.5*cm; // 1 foot/2 
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
	const G4double posBulkShieldHDCZ		= bulkConcShieldHalfHt-instrFloorTop2Tgt;//protonBoxOuterHalfXZ;
    const G4double bulkConcShieldRmin    	= monoSteelRmax;// 5.*m;
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
	//18.3 + 1.08 = 19.4=> det hall= 18 to 20. + 5m (= nPar* drad). total 25m. 25-18.3=6.7m
	const G4double hdcOutRadMax 		= baseRadiusAll + baseExtraThick;// basementRad + tunnelWall; //18.3*m;
	//if(immersedDet && ! boxDet) hdcOutRadMax += thickNDet + padThick;
	const G4double hdcOutHht 			= bulkConcShieldHalfHt;// + instrFloorHalfHt; // 1.75*m;//3.5/2
	const G4double hdcOutLightPosZ	= posBulkShieldHDCZ;//hdcRaise -hdcOutHht;
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
    const G4double nTubeHalfDx1[]	 = {21.59*cm, 31.38*cm, 21.59*cm,21.59*cm,21.59*cm,21.59*cm};
    const G4double nTubeHalfDx2[]	 = {6.91*cm, 6.91*cm, 6.91*cm,6.91*cm,6.91*cm,6.91*cm};
    const G4double nTubeHalfDy1[]	 = {7.235*cm, 7.235*cm, 8.035*cm,7.235*cm,7.235*cm,8.035*cm};
    const G4double nTubeHalfDy2[]	 = {7.235*cm, 7.235*cm, 8.035*cm,7.235*cm,7.235*cm,8.035*cm};
	const G4double nTubeInHZextra  = 8.*cm;
	const G4double nTubeHalfDz 	 = outerSteelR2/2.+ nTubeInHZextra; //extra 		
    const G4double thickNTubeAlX[] = {1.31*cm, 1.454*cm, 1.62*cm,1.454*cm,1.454*cm,1.62*cm};
    const G4double thickNTubeAlY[] = {0.635*cm, 0.869*cm, 0.635*cm,0.779*cm,0.779*cm,0.635*cm};
    const G4double rotZ[] = {lH1Rot - 90.*degree, lH1Rot - 90.*degree, -90.*degree,
                            90.*degree - lH1Rot, 90.*degree - lH1Rot, 90.*degree};
    //1=top -x,-y extends to -x+y; 2=top -x, -y extends to +x-y; 3; top to +x
    //4=bot +x-y extends to 
    const G4double rotX[] = {90.*degree, -90.*degree, 90.*degree, -90.*degree,
                            90.*degree, 90.*degree};
    const G4double posNTubeX[] = {lH3posX + (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot),
            lH3posX - (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot), lH1halfX +nTubeHalfDz,
            -lH3posX +(lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot),
            -lH3posX -(lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot),
            -lH1halfX -nTubeHalfDz};
    const G4double posNTubeY[] = {lH3posY-(lH1halfX +nTubeHalfDz)*std::sin(lH1Rot),
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
            posNTubeZ[i]					= innRPHHt + nTubeHalfDy1[i]; //rotated y->z	
            if(i>2) posNTubeZ[i]			= - posNTubeZ[i];
            G4RotationMatrix* rotNTube 		= new G4RotationMatrix; 
            rotNTube->rotateZ(rotZ[i]);
            rotNTube->rotateX(rotX[i]);

            G4ThreeVector posNTube(posNTubeX[i], posNTubeY[i], posNTubeZ[i]);
            //CreateInnerNTube(	nTubeHalfDx1[i], nTubeHalfDx2[i], nTubeHalfDy1[i],
            //        nTubeHalfDy2[i], nTubeHalfDz, thickNTubeAlX[i], thickNTubeAlY[i],
            //        rotNTube, posNTube, tubeNames[i], outerSteelR2, outerSteelHt,
            //        fCheckOverlap );
    
            G4Trd* nInnerTubeOut     = new G4Trd("nInnerTubeOut", nTubeHalfDx1[i], 
                nTubeHalfDx2[i], nTubeHalfDy1[i], nTubeHalfDy2[i], nTubeHalfDz);
            G4Trd* nInnerTubeIn      = new G4Trd("nInnerTubeIn", nTubeHalfDx1[i]  
                -thickNTubeAlX[i], nTubeHalfDx2[i]-thickNTubeAlX[i],
                nTubeHalfDy1[i] -thickNTubeAlY[i], nTubeHalfDy2[i]-thickNTubeAlY[i], nTubeHalfDz+5.*cm);


            G4SubtractionSolid* nTubeOutSub = new G4SubtractionSolid(tubeNames[i]+"OutSub", 
                        nInnerTubeOut, nInnerTubeIn, rotCenter, *posCenter);
			
			//if(multiStepRuns). TODO if det is affecting inner N Tubes			
                                     
			G4LogicalVolume* nTubeLog  = new G4LogicalVolume(nTubeOutSub, alum, tubeNames[i]+"Log");
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
  
            G4ThreeVector posNTubeNinner(posNTubeX[i], posNTubeY[i], posNTubeZ[i]- transMonoZ);
            monoSteelCylNInnerSub   =  new G4SubtractionSolid("MonoSteelNInnerSub",  
				monoSteelSubNInnerIn, nInnerTubeOut, rotNTube, posNTubeNinner);
            monoSteelSubNInnerIn    = monoSteelCylNInnerSub;
        }
    }
    G4cout << "Done Inner Neutron Tubes :)" << G4endl;
	
	//nOuter tubes from monolith
	//Monolith Neutron Beam lines .  10x12cm2.  3 beam line for each inner beam line
	//24 holes up to 10 by 12 cm2 with approximately 3 cm total in alum windows
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
	

	//nOuterUnionShields	= true;
    if(placeNTubeOuter)
    {

		const G4double monoExtraDigLHalf				= 3.*nTubeInHZextra;
		const G4double nBeamOuterStart    			= monoSteelRmin + monoExtraDigLHalf;//outerSteelR2;
			// extra is to shift the end of joint of in/out tubes into mono
        const G4double dTheta             			= 8.5*degree;
		const G4double nBeamOuterZhLen 				= nBeamLineFullLength/2.;//15.*m;//7.*m;
        std::vector <G4SubtractionSolid*> nTubeOuter(2);
        G4SubtractionSolid* monoSteelSubIn      = NULL;
        G4SubtractionSolid* hdcNOuterSubIn      = bulkHDCCyl;
        G4SubtractionSolid* hdcLayerNOuterSubIn = bulkHdcLayerCyl;
		G4SubtractionSolid* hdcOutLShieldSubIn 	= hdcOutLightShield;
	
		
        if(placeNTubeInner) monoSteelSubIn  	= monoSteelCylNInnerSub;
        else monoSteelSubIn                 	= monoSteelCyl1;
        const G4double nBeamOuterX 	            = 5.*cm;
        const G4double nBeamOuterY 	            = 6.*cm;
        G4IntersectionSolid* firstTube      	= NULL;
        G4UnionSolid* secTube               	= NULL;
        
        G4ThreeVector posNTubeOuter1;
        G4ThreeVector posNTubeOuter2;
        G4RotationMatrix* rotnNTube1 			= NULL;
        G4RotationMatrix* rotnNTube2 			= NULL;
        const G4double radMaxDummyNOuter   		= 2.*nBeamOuterZhLen+2.*m; 
		
		//used if NTube is sub from MS
		G4SubtractionSolid* multiStepCyl1NSubIn = NULL;
		G4SubtractionSolid* multiStepCyl2NSubIn = NULL;
		
		if(multiStepRuns)
		{
			multiStepCyl1NSubIn = multiStepCyl1Sub;
			if(fIsMSDet2)
			{
				multiStepCyl2NSubIn = multiStepCyl2Sub;
			}
		}
		

		G4cout << " ####### nShieldXY = " << nShieldXY/m << "  meter." <<  G4endl <<  G4endl;

		for (G4int i=0; i<6; i++) //6 inner tube number
        {
												
            for (G4int n=0; n<3; n++) //3 outer tube number
            {
                    
                G4double posnTubeZ	= innRPHHt + nTubeHalfDy1[i]; //rotated y->z	
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
									nBeamOuterY, nBeamOuterZhLen+monoExtraDigLHalf);										
		        G4Box* nBeamOuterIn  		= new G4Box(outerTubeName+"In", nBeamOuterX
				    -thickNTubeAlX[i], 	nBeamOuterY-thickNTubeAlY[i], nBeamOuterZhLen+5.*cm);	
				
			    G4SubtractionSolid* nBeamOuterSub = new G4SubtractionSolid(outerTubeName+"Sub", 
				    nBeamOuterOut, nBeamOuterIn, rotCenter, *posCenter);
				//G4SubtractionSolid*	nBeamOutMSSub = nBeamOuterSub; 
				
				
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



				nBeamOuterLog		= new G4LogicalVolume(nBeamOuterSub, alum, outerTubeName+"Log");
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
									nShieldXY,nShieldXY, nBeamOuterZhLen+ nShieldEnd);
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
						G4SubtractionSolid* nOutShieldSub = NULL;
						
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
						else
							nOutShieldSub = nBeamShieldSub;
                                                
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
										lightMat, "multiStepCyl1Log");
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
										lightMat, "multiStepCyl2Log");	
			multiStepCyl2Log->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));	
			new G4PVPlacement( rotCenter, posMSDet, multiStepCyl2Log, "MultiStepDet2",
										fDetLogical, false, 0 , fCheckOverlap); 			
		}
		G4cout << "Done Multi Step Detector(s) " <<  G4endl;
		G4cout << "\t Det1:radius " << fMSDet1Inner/m << G4endl;
		if(fIsMSDet2) G4cout	<< "\t Det2:radius "<< fMSDet2Inner/m << G4endl; 
	}


	//IRP plug placement
	G4Material* BeD2O = G4NistManager::Instance()->FindOrBuildMaterial("BeD2O"); 
	G4LogicalVolume* IRPLog = new G4LogicalVolume(IRPsolidSub4, alum, "InnerRefPlugLog");
	IRPLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta()));
	if(placeInner) new G4PVPlacement( rotCenter, *posCenter, IRPLog, "InnerRefPlug",
					   fDetLogical, false, 0 , fCheckOverlap);  
	
	//Top & Bottom Be Plug placement 
    G4LogicalVolume* bePlugTopLog = NULL;
    G4LogicalVolume* bePlugBotLog = NULL;
    if(placeNTubeInner)
    {
        bePlugTopLog = new G4LogicalVolume(BePlugInnerNTubeTopSub, 
												BeD2O, "UpperBePlugLog");
        bePlugBotLog = new G4LogicalVolume(BePlugInnerNTubeBotSub, 
                                                BeD2O, "LowerBePlugLog");
    }
    else
    {
        bePlugTopLog = new G4LogicalVolume(BePlugTopSub2, BeD2O, "UpperBePlugLog");
        bePlugBotLog = new G4LogicalVolume(BePlugBotSub2, BeD2O, "LowerBePlugLog");
    }
	bePlugTopLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));
    bePlugBotLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));	
	if(placeInner) new G4PVPlacement( rotCenter, posUpperBePlug, bePlugTopLog, "UpperBePlug",
					   fDetLogical, false, 0 , fCheckOverlap); 	 
	if(placeInner) new G4PVPlacement( rotCenter, posLowerBePlug, bePlugBotLog, "BePlugBottom",
					   fDetLogical, false, 0 , fCheckOverlap);   

	
	//INNER STEEL upper cylinder BePlugRad=33 to 49 cm rad, ht =  from n plug gap upto Beplug  
	const G4double innerSteelRad1 = BePlugRad;
	const G4double innerSteelRad2 = innRPRad;
	const G4double innerSteelGap  =  BePlugHalfHt;
	const G4double innerSteelHalfHt   = (2.*BePlugHalfHt -innerSteelGap)/2.; //subtract gap for n plugs
	G4Tubs* innerSteelD2Ocyl = new G4Tubs ("innerSteelD2Ocyl", innerSteelRad1, 
									innerSteelRad2, innerSteelHalfHt, 0, twopi);
	G4LogicalVolume* innerUpperSteelD2OLog = new G4LogicalVolume(innerSteelD2Ocyl, 
												steelD20, "InnerUpperSteelD2OLog");
	G4ThreeVector posInUpSteel(0, 0, innRPHHt+innerSteelGap+innerSteelHalfHt);
	innerUpperSteelD2OLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
	if(placeInner) new G4PVPlacement( rotCenter, posInUpSteel, innerUpperSteelD2OLog, "InnerUpperSteelD2",
					   fDetLogical, false, 0 , fCheckOverlap);	
	
	//INNER STEEL cylinder BOTTOM  
	G4LogicalVolume* innerBotSteelD2OLog = new G4LogicalVolume(innerSteelD2Ocyl, 
												steelD20, "InnerBotSteelD2OLog");
	G4ThreeVector posInBotSteel(0, 0, -(innRPHHt+innerSteelGap+innerSteelHalfHt));
	innerBotSteelD2OLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey()));
	if(placeInner) new G4PVPlacement( rotCenter, posInBotSteel, innerBotSteelD2OLog, "InnerBottomSteelD2O",
                       fDetLogical, false, 0 , fCheckOverlap);   				
		
		
 																
    //placing 	OuterSteel Cylinder			outerSteelCylSub1
    G4LogicalVolume* outerSteelLog = NULL;
    if(placeNTubeInner)
        outerSteelLog = new G4LogicalVolume(outerSteelCylSub, steel, "OuterSteelLog");	
    else 
        outerSteelLog = new G4LogicalVolume(outerSteelCylSub1, steel, "OuterSteelLog");
	outerSteelLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));	
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
		const G4double cutPBoxHalfY1  = (monoSteelRmax - monoSteelRmin)/2. + 0.3*m;  //extra
		G4Box* monoSteelCutBox1   = new G4Box("MonoSteelCutBox", protonBeamCutHXZ, 
											cutPBoxHalfY1, protonBeamCutHXZ/2.);
		G4ThreeVector transBoxMonoSteel1(0., -monoSteelRmin-
									(monoSteelRmax-monoSteelRmin)/2.,-transMonoZ);  
		G4SubtractionSolid* monoSteel4 = new G4SubtractionSolid("MonoSteel4", 
					monoSteel3, monoSteelCutBox1, rotCenter, transBoxMonoSteel1);
		monoSteelLog = new G4LogicalVolume(monoSteel4, steel, "MonoSteelLog");		
		
		if(fPlaceMagnet)
		{
			G4Box* monoSteelMagCut		  = new G4Box("monoSteelMagCut", magnetCutHZ, 
													  magnetCutHalfL, magnetCutHZ);
			G4ThreeVector transMagCut(0., -magnetPosInMonoSteel,-transMonoZ);  
			G4SubtractionSolid* monoSteel5 = new G4SubtractionSolid("MonoSteel5", 
						monoSteel4, monoSteelMagCut, rotCenter, transMagCut);
			monoSteelLog = new G4LogicalVolume(monoSteel5, steel, "MonoSteelLog");		
			
		}
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
	const G4double protonBoxOuterHalfY  		= 15.*m; //NOTE: this will be moved out another 21 m
    G4ThreeVector* posOuterProtonBox 	= NULL;
	if(placeProtonBox)
	{
		SNSTargetOuterProtonBox* outerProtonBox = new SNSTargetOuterProtonBox("OuterProtonBox",
            protonBoxOuterHalfXZ, protonBoxOuterHalfY, protonBoxOuterHalfXZ,
            multiStepRuns, fIsMSDet2, fMSDet1Inner, fMSDet2Inner,
			fMSDetCylThick, fMSDetCylHht, posMSDetZ, fPlaceMagnet,
			 magnetCutHalfL, magnetCutHZ, fCheckOverlap);
		
		posOuterProtonBox = new G4ThreeVector(0, -protonBoxOuterHalfY-monoSteelRmax, 0);
		outerProtonBox->Place(rotCenter, posOuterProtonBox, fDetLogical); 
	}

	
	//////////////////////////////////
    //################################
    //BUILDING, BASEMENT and DETECTORS
	//
	// BASEMENT road base stone(gravel), or granite loose
    //const G4double tunnelWall        = 18.*inch; //SET ABOVE
	//const G4double basementRad 		= 22.3*m - tunnelWall;//rad 23.8-1.5m -wall. distLOS=25m
    //x=18.3, y=-15.2 =>rad 23.8.;  z=-7.8 (8.1m-0.3 for det 1foot above ground)=>25m LOS
	//const G4double detToTargetFullZ  = 7.5*m;//7.1*m //8.1m-1m //old=7.8 
    //const G4double detToWallGap      = 1.5*m;
	//const G4double detDiagonalDist      = baseRadiusAll+detToWallGap;
    //instrFloorTop2Tgt = protonBoxOuterHalfXZ;//2.*m; //Set above
    //const G4double instrFloorHalfHt  = 9.*inch;//18"/2 //set above
    //const G4double basementTop2Tgt   = instrFloorTop2Tgt + 2.*instrFloorHalfHt;
    //const G4double basementHalfHt    = (detToTargetFullZ - basementTop2Tgt+3.*m)/2.;  //8.3m/2 
    //const G4double basementPosZ     = -basementHalfHt - basementTop2Tgt; 


    
	const G4double monoBase		   = 75.0*cm; //instrFloorHalfHt;

    //INSTR. FLOOR slab concrete 18"
	G4ThreeVector instrFloorPos(0,0, instrFloorPosZ );
    G4Tubs* floorMonoCut       = new G4Tubs ("floorMonoCut", 0,  
						monoSteelRmax+monoBase, instrFloorHalfHt, 0, twopi);
							
    G4SubtractionSolid* instrFloorCyl = new G4SubtractionSolid("basementCyl", 
                        instrFloorSub, floorMonoCut, rotCenter, *posCenter);
    
	
	G4SubtractionSolid*	instrFloorStairSub = NULL;
	if(fStairCase)
	{
		G4ThreeVector posStairCaseIns(stairPosX, stairPosY, 0);
		instrFloorStairSub = new G4SubtractionSolid("instrFloorStairSub",
					instrFloorCyl, stairCaseSolid, rotStairCase, posStairCaseIns);
	}
	else
		instrFloorStairSub = instrFloorCyl;
	
	
	G4LogicalVolume* instrFloorLog = new G4LogicalVolume(instrFloorStairSub, 
                                        concrete, "InstrFloorLog"); 
    //instrFloorLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
    new G4PVPlacement( rotCenter, instrFloorPos, instrFloorLog, "InstrFloor", 
                                     fDetLogical, false, 0, fCheckOverlap);  

	//BASEMENT Inner
	G4LogicalVolume* basementInLog = NULL;
	
    const G4double basementLayerWidth   = 4.*m;
	const G4double basementRadInnerDisk = basementRad - basementLayerWidth;
	const G4double monoCutHalfHt	  = (monoSteelHalfHt-transMonoZ-basementTop2Tgt+monoBase)/2.;

	G4ThreeVector baseSubPos (0, 0, -basementPosZ + posMSDetZ);		
	G4ThreeVector basementPos (0, 0, basementPosZ);//cylinder	


	G4SubtractionSolid* basementInSolidDSub = NULL;
	
	if( ! boxBasement)
	{
		G4cout 	<<  "basementRadInnerDisk(m) " << basementRadInnerDisk/m 
				<< G4endl;
		
		G4Tubs*  basementInSolid    = new G4Tubs ("basementInSolid", 0,  
							basementRadInnerDisk, basementHalfHt, 0, twopi);
		basementInSolidDSub = new G4SubtractionSolid(
			"basementInSolidDSub", basementInSolid, dummySolid,  rotCenter, *posCenter);
		
	}
	else
	{ 
		G4Box* basementInSolid = new G4Box("basementInSolid", baseRadiusAll- tunnelWall, 
										boxBaseHalfY, basementHalfHt);
		basementInSolidDSub = new G4SubtractionSolid(
			"basementInSolidDSub", basementInSolid, dummySolid,  rotCenter, *posCenter);
	}
	G4Tubs* basementMonoCut	  = new G4Tubs ("BasementMonoCut", 0, 
						monoSteelRmax+ monoBase, monoCutHalfHt, 0, twopi);
		
	G4ThreeVector posMonoCut(0, 0, basementHalfHt-monoCutHalfHt); 
	
	G4SubtractionSolid* basementInMSSub = NULL;	
	if(multiStepRuns)
	{	
		basementInMSSub  = SubMultiStepDet(basementInSolidDSub, 
								"bulkShieldLayerMSSub", baseSubPos, rotCenter);
	}	
	else
		basementInMSSub = basementInSolidDSub;
		
	G4SubtractionSolid*	basementInStairSub = NULL;
	if(fStairCase)
	{
		G4ThreeVector posStairCaseBase(stairPosX, stairPosY, 0);
		basementInStairSub = new G4SubtractionSolid("basementInStairSub",
					basementInMSSub, stairCaseSolid, rotStairCase, posStairCaseBase);
	}
	else
		basementInStairSub = basementInMSSub;
	
	G4SubtractionSolid*	basementInCyl = new G4SubtractionSolid("basementCyl", 
					basementInStairSub, basementMonoCut, rotCenter, posMonoCut);

	//G4Material* vac = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");	 

	G4String materialType  = "RoadStone";
	basementInLog = new G4LogicalVolume(basementInCyl, stone, "BasementInLog"); 
	
	basementInLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));
	new G4PVPlacement( rotCenter, basementPos, basementInLog, "BasementIn", 
										fDetLogical, false, 0 , fCheckOverlap); 

	

    //BASEMENT Outer layer
	G4LogicalVolume* basementLayerLog =  NULL;
	if(!boxBasement && basementRadInnerDisk < basementRad)
	{
		G4Tubs*  basementLayerSolid = new G4Tubs ("basementLayerSolid", 
								basementRadInnerDisk, basementRad, basementHalfHt, 0, twopi);
		G4SubtractionSolid* basementLayerSolidDSub = new G4SubtractionSolid(
			"basementLayerSolidDSub", basementLayerSolid, dummySolid,  rotCenter, *posCenter);
			
		G4SubtractionSolid* basementLayerMSSub = NULL;	
		if(multiStepRuns)
		{	
			basementLayerMSSub  = SubMultiStepDet(basementLayerSolidDSub, 
											"basementLayerMSSub", baseSubPos, rotCenter);
		}	
		else
			basementLayerMSSub = basementLayerSolidDSub;	
			
		
		G4SubtractionSolid*	basementLayerStairSub = NULL;
		if(fStairCase)
		{
			G4ThreeVector posStairCaseLayer(stairPosX, stairPosY, 0);
			basementLayerStairSub = new G4SubtractionSolid("basementLayerStairSub",
					basementLayerMSSub, stairCaseSolid, rotStairCase, posStairCaseLayer);
		}
		else
			basementLayerStairSub = basementLayerMSSub;
		
		G4Material* stoneLayer =  G4NistManager::Instance()->FindOrBuildMaterial("RoadStoneLayer");	
		basementLayerLog =  new G4LogicalVolume(basementLayerStairSub, stoneLayer, "BasementLayerLog"); 
		basementLayerLog->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));
		new G4PVPlacement( rotCenter, basementPos, basementLayerLog, "BasementLayer", 
													fDetLogical, false, 0 , fCheckOverlap);  
	}
	
	//MONOLITH BASE
	const G4double monoBaseCutHalf = monoCutHalfHt+instrFloorHalfHt;
	G4Tubs*  monoBaseCyl     = new G4Tubs ("monoBaseCyl", monoSteelRmax, 
							monoSteelRmax+monoBase,	 monoBaseCutHalf, 0, twopi);
	G4Tubs*  monoBaseBot     = new G4Tubs ("monoBaseBot", 0, 
								monoSteelRmax,	monoBase/2., 0, twopi);	
	G4ThreeVector monoBaseBotPos(0,0,-monoBaseCutHalf+monoBase/2.);
	G4UnionSolid* monoBaseSolid  = new G4UnionSolid("monoBaseSolid", monoBaseCyl,
									monoBaseBot, rotCenter, monoBaseBotPos);
	const G4double monoBasePosZ		 =	-instrFloorTop2Tgt- monoBaseCutHalf;
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
	G4LogicalVolume* tunnelWallLog  = NULL;
	G4ThreeVector* tunnelWallPos = NULL;
	if( ! boxBasement)
	{
		G4Tubs*  tunnelWallCyl		= new G4Tubs ("tunnelWall", basementRad, 
					basementRad+tunnelWall, basementHalfHt, 0, twopi);
		tunnelWallLog  = new G4LogicalVolume(tunnelWallCyl, concrete, "TunnelWallLog"); 
		tunnelWallPos  = new G4ThreeVector (0, 0, basementPosZ);		
	}
	else
	{
		G4Box*  tunnelWallBox		= new G4Box ("tunnelWallBox", tunnelWall/2., 
											boxBaseHalfY, basementHalfHt);
		tunnelWallLog  = new G4LogicalVolume(tunnelWallBox, concrete, "TunnelWallLog"); 
		tunnelWallPos  = new G4ThreeVector (baseRadiusAll - tunnelWall/2., 0, basementPosZ);
	}
    new G4PVPlacement( rotCenter, *tunnelWallPos, tunnelWallLog, "TunnelWall", 
                                                    fDetLogical, false, 0 , fCheckOverlap);  
	

	//TUNNEL WALL det layer
	G4LogicalVolume* tunWallDetLog  = NULL;
	if( ! boxBasement)
	{	
		//same as basement LScint det
		const double angle1 	= 330.* degree;
		const double angle2 	= 120.*degree; // + start, not absolute poistion

		G4Tubs*  tunWallDetCyl  = new G4Tubs ("tunWallDetCyl", basementRad+tunnelWall,
			basementRad+tunnelWall+ tunWallDetThick, tunWallDetHht, angle1, angle2);		
		
		tunWallDetLog  = new G4LogicalVolume(tunWallDetCyl, concrete, "TunWallDetLog"); 
		tunWallDetLog->SetVisAttributes(new G4VisAttributes (G4Colour::Blue()));
		G4ThreeVector tunnelWallDetPos(0, 0, -detToTargetFullZ);
		new G4PVPlacement( rotCenter, tunnelWallDetPos, tunWallDetLog, "TunWallDet", 
										fDetLogical, false, 0 ,fCheckOverlap);  
	}
	
	
		
    //BASEMENT HALL ROOF
	G4Tubs*  basementRoofTop   			    = NULL;
	G4Tubs*  basementRoofStone 			    = NULL;
	G4Tubs*  basementRoofIn    			    = NULL;
    G4ThreeVector* posHallRoofTop   	    = NULL;
    G4ThreeVector* posHallRoofStone 	    = NULL;
    G4ThreeVector* posHallRoofIn    	    = NULL;
    G4LogicalVolume* basementRoofInLog 	    = NULL;
	G4LogicalVolume* basementRoofTopLog	    = NULL;
	G4LogicalVolume* basementRoofStoneLog   = NULL;

	G4Box*  baseBoxRoofTop   				= NULL;
	G4Box*  baseBoxRoofStone 				= NULL;
	G4Box*  baseBoxRoofIn    				= NULL;
	G4LogicalVolume* baseBoxRoofInLog 	    = NULL;
	G4LogicalVolume* baseBoxRoofTopLog	    = NULL;
	G4LogicalVolume* baseBoxRoofStoneLog    = NULL;
	
	
	const G4double baseRoofTopHalf    = 9.*inch;//18/2 instrument floor slab
	const G4double baseRoofStoneHalf  = 2.5*12.*inch; //5/2 feet
	const G4double baseRoofInHalf     = 9.*inch;//18/2  
	//const G4double baseRoofGravelRadLen     = 4.*m+tunnelWall; // 

	//
	if(basementHallRoof && ! boxBasement)
	{
		posHallRoofTop 	= new G4ThreeVector(0,0, - instrFloorTop2Tgt -2.*instrFloorHalfHt-baseRoofTopHalf);
		posHallRoofStone = new G4ThreeVector(0,0, -instrFloorTop2Tgt-2.*instrFloorHalfHt
					-2.*baseRoofTopHalf - baseRoofStoneHalf); 
		posHallRoofIn 	= new  G4ThreeVector(0,0, -instrFloorTop2Tgt -2.*instrFloorHalfHt
					-2.*baseRoofTopHalf - 2.*baseRoofStoneHalf-baseRoofInHalf);

// TODO : include stairwell
		
		basementRoofTop   = new G4Tubs ("basementRoof1", basementRad+tunnelWall, 
							basementRad+baseRoofGravelRadLen, baseRoofTopHalf, 0, twopi);
		basementRoofTopLog = new G4LogicalVolume( basementRoofTop,
											concrete, "basementRoofTopLog");
		new G4PVPlacement( rotCenter, *posHallRoofTop, basementRoofTopLog,
							"BasementRoofTop", fDetLogical, false, 0 , fCheckOverlap); 

		basementRoofStone  = new G4Tubs ("basementRoof2", basementRad+tunnelWall, 
							basementRad+baseRoofGravelRadLen, baseRoofStoneHalf, 0, twopi);

		basementRoofStoneLog = new G4LogicalVolume( basementRoofStone,
											stone, "basementRoofStoneLog");
		new G4PVPlacement( rotCenter, *posHallRoofStone, basementRoofStoneLog,
							"BasementRoofStone", fDetLogical, false, 0 , fCheckOverlap);  

		basementRoofIn     = new G4Tubs ("basementRoof3", basementRad+tunnelWall, 
							basementRad+baseRoofGravelRadLen, baseRoofInHalf, 0, twopi);
		basementRoofInLog = new G4LogicalVolume( basementRoofIn,
											concrete, "basementRoofInLog");
        //basementRoofInLog->SetVisAttributes(new G4VisAttributes (G4VisAttributes::Invisible));
		new G4PVPlacement( rotCenter, *posHallRoofIn, basementRoofInLog, 
							"BasementRoofIn", fDetLogical, false, 0 , fCheckOverlap);  

	}
	else if (basementHallRoof && boxBasement)
	{
		posHallRoofTop 	= new G4ThreeVector(baseRadiusAll+ baseRoofGravelRadLen/2. ,
							0, - instrFloorTop2Tgt -2.*instrFloorHalfHt-baseRoofTopHalf);
		posHallRoofStone = new G4ThreeVector(baseRadiusAll+ baseRoofGravelRadLen/2. ,
							0, -instrFloorTop2Tgt-2.*instrFloorHalfHt
								-2.*baseRoofTopHalf - baseRoofStoneHalf); 
		posHallRoofIn 	= new  G4ThreeVector(baseRadiusAll+ baseRoofGravelRadLen/2. , 
							0, -instrFloorTop2Tgt -2.*instrFloorHalfHt-2.*baseRoofTopHalf
							- 2.*baseRoofStoneHalf-baseRoofInHalf);
		
		baseBoxRoofTop   = new G4Box("baseBoxRoofTop", baseRoofGravelRadLen/2., 
									   boxBaseHalfY, baseRoofTopHalf);
		baseBoxRoofTopLog = new G4LogicalVolume( baseBoxRoofTop,
												 concrete, "baseBoxRoofTopLog");
		new G4PVPlacement( rotCenter, *posHallRoofTop, baseBoxRoofTopLog,
						   "BaseBoxRoofTop", fDetLogical, false, 0 , fCheckOverlap); 
// TODO : include stairwell		
		
		baseBoxRoofStone  	= new G4Box("baseBoxRoofStone", baseRoofGravelRadLen/2.,  
										boxBaseHalfY, baseRoofStoneHalf);		
		baseBoxRoofStoneLog = new G4LogicalVolume( baseBoxRoofStone,
												   stone, "baseBoxRoofStoneLog");
		new G4PVPlacement( rotCenter, *posHallRoofStone, baseBoxRoofStoneLog,
						   "BaseBoxRoofStone", fDetLogical, false, 0 , fCheckOverlap);  

		
		baseBoxRoofIn     = new G4Box ("baseBoxRoofIn", baseRoofGravelRadLen/2.,  
									   boxBaseHalfY, baseRoofInHalf);
		baseBoxRoofInLog = new G4LogicalVolume( baseBoxRoofIn,
												concrete, "baseBoxRoofInLog");
		new G4PVPlacement( rotCenter, *posHallRoofIn, baseBoxRoofInLog, 
						   "BaseBoxRoofIn", fDetLogical, false, 0 , fCheckOverlap);  
	}

	
    //BULK SHIELD HDC continued ... outer concrete from monolith to 10m
    G4LogicalVolume* bulkConcShieldLog = NULL;
	G4ThreeVector posBulkShieldHDC(0, 0, posBulkShieldHDCZ);
	if(placeSurfHdcInShield)
	{
		const G4double cutBoxBulkShieldHalfY  = (surfaceHDCInnnerMax-
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
		const G4double cutBoxHDCLayerHalfY  = (bulkHdcRmax - surfaceHDCInnnerMax)/2. + 1.*m;  //extra 
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

	const G4bool surfaceDet 			= false;
	const G4double maxStepFactor		= 0.5; // x maxStep	

	//Cuts
	G4Region* regDet = new G4Region("Det_CsI");	
	
	////
	//DETECTOR, annulus
    G4Tubs*  detWaterCylFull 		= NULL;
    G4ThreeVector* posDetector 		= NULL;
	
	if(basementCsIDet )
	{					
		const G4double detCsIRad          = 5.5*cm;
		const G4double detPbThick		  = 18.*cm;
		const G4double detCdThick		  = 0.5*mm;	
		const G4double detWaterThick	  = 12.*2.54*cm;			
		const G4double detPbRad           = detCsIRad+ detPbThick;
		const G4double detCdRad           = detPbRad + detCdThick;
		const G4double detWaterRad        = detCdRad + detWaterThick;
		const G4double detCsIHalfHt       = 16.5*cm;//33/2 
		const G4double detPbHalfHt        = detCsIHalfHt +detPbThick;
		const G4double detCdHalfHt        = detPbHalfHt + detCdThick; 
		const G4double detWaterHalfHt	  = detCdHalfHt + detWaterThick; 

		const G4double detCsIRadInner     = detCenterDist - detCsIRad;
		const G4double detCsIRadOuter     = detCenterDist + detCsIRad; 
		
		//defined above detToTargetFullZ = 7.5*m; //8.1 - 60cm

		G4Material* csI     = G4NistManager::Instance()->FindOrBuildMaterial("CsI"); //d=4.5g/cc
		G4Material* lead    = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
		G4Material* cadmium = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cd");

		G4LogicalVolume* detCsILog 	= NULL;
		G4Box* detCsIBox 			= NULL;
		G4Tubs* detCsICyl 			= NULL;
		if(!boxBasement)
		{
			posDetector   = new G4ThreeVector(0,0, -detToTargetFullZ ); //NOTE		
			
			detCsICyl = new G4Tubs ("DetCsICyl", detCsIRadInner, 
											detCsIRadOuter, detCsIHalfHt,0., twopi);
			detCsILog = new G4LogicalVolume(detCsICyl, csI, "DetCsILog");
		}
		else //if(boxBasement)
		{
			posDetector   = new G4ThreeVector( detCenterDist, 0, -detToTargetFullZ ); //NOTE		
			
			detCsIBox = new G4Box ("DetCsIBox", detCsIRad, boxBaseHalfY -  
							detWaterThick - detCdThick - detPbThick, detCsIHalfHt);
			detCsILog = new G4LogicalVolume(detCsIBox, csI, "DetCsILog");			
		}
		
		detCsILog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta())); 
		new G4PVPlacement( rotCenter, *posDetector, detCsILog, "SNSDetectorCsI", 
						   fDetLogical, false, 0 , fCheckOverlap);
		
		
		const G4double detPbRadInner    = detCenterDist - detPbRad;
		const G4double detPbRadOuter    = detCenterDist + detPbRad; 
		
		G4LogicalVolume* detPbLog 		= NULL;
		G4Box* detPbBoxFull 			= NULL;
		G4Tubs* detPbCylFull          	= NULL;
		if(!boxBasement)
		{
			detPbCylFull        = new G4Tubs ("DetPbCylFull", detPbRadInner, 
										detPbRadOuter, detPbHalfHt,0., twopi);
			G4SubtractionSolid* detPbcyl= new G4SubtractionSolid("detPbcyl", 
								detPbCylFull, detCsICyl, rotCenter, *posCenter);
			detPbLog = new G4LogicalVolume(detPbcyl, lead, "DetPbLog");
		}
		else //if(boxBasement)
		{
			detPbBoxFull 				= new G4Box ("DetPbBoxFull", detPbRad, 
							boxBaseHalfY - detWaterThick - detCdThick, detPbHalfHt);
			G4SubtractionSolid* detPbBox= new G4SubtractionSolid("detPbBox", 
									detPbBoxFull, detCsIBox, rotCenter, *posCenter);
			detPbLog 					= new G4LogicalVolume(detPbBox, lead, "DetPbLog");		
		}		
		detPbLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));    
		new G4PVPlacement( rotCenter, *posDetector, detPbLog, "DetLeadShield", 
						   fDetLogical, false, 0 , fCheckOverlap); 

		
		const G4double detCdInner         = detCenterDist - detCdRad;
		const G4double detCdOuter         = detCenterDist + detCdRad; 
		
		G4LogicalVolume* detCdLog 	= NULL;
		G4Tubs* detCdCylFull    	= NULL;
		G4Box* detCdBoxFull 		= NULL;
		if(!boxBasement)
		{
			detCdCylFull    	= new G4Tubs ("DetCdCylFull", detCdInner, 
											detCdOuter, detCdHalfHt, 0., twopi);
			G4SubtractionSolid* detCdCyl= new G4SubtractionSolid("detCdCyl", 
									  detCdCylFull, detPbCylFull, rotCenter, *posCenter);	
			detCdLog 			= new G4LogicalVolume(detCdCyl, cadmium, "DetCdLog");		
		}
		else //if(boxBasement)
		{
			detCdBoxFull 		= new G4Box ("DetCdBoxFull", detCdRad, 
							boxBaseHalfY - detWaterThick, detCdHalfHt);
			G4SubtractionSolid* detCdBox= new G4SubtractionSolid("detCdBox", 
								detCdBoxFull, detPbBoxFull, rotCenter, *posCenter);				
			detCdLog 					= new G4LogicalVolume(detCdBox, cadmium, "DetCdLog");	
		}
		detCdLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey())); 
		new G4PVPlacement( rotCenter, *posDetector, detCdLog, "DetCdShield", 
						   fDetLogical, false, 0 , fCheckOverlap);
		
		
		const G4double detWaterInner    = detCenterDist - detWaterRad;
		const G4double detWaterOuter    = detCenterDist + detWaterRad; 
		G4LogicalVolume* detWaterLog 	= NULL;
		if(!boxBasement)
		{
			detWaterCylFull    			= new G4Tubs ("detWaterBoxFull", detWaterInner, 
											detWaterOuter, detWaterHalfHt, 0., twopi);
			G4SubtractionSolid* detWaterCyl = new G4SubtractionSolid("detWaterCyl", 
									detWaterCylFull, detCdCylFull, rotCenter, *posCenter);		
			detWaterLog = new G4LogicalVolume(detWaterCyl, water, "DetWaterLog");
		}
		else //if(boxBasement)
		{
			G4Box* detWaterBoxFull   	= new G4Box ("detWaterBoxFull", detWaterRad, 
									boxBaseHalfY, detWaterHalfHt);
			G4SubtractionSolid* detWaterCyl = new G4SubtractionSolid("detWaterCyl", 
									detWaterBoxFull, detCdBoxFull, rotCenter, *posCenter);		
			detWaterLog			= new G4LogicalVolume(detWaterCyl, water, "DetWaterLog");		
		}
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
		G4double maxStep		= detCsIRad/2.;
		if(detCsIHalfHt < detCsIRad) 
			maxStep = detCsIHalfHt/3.; 	
		G4UserLimits* stepLimit = new G4UserLimits(maxStep*maxStepFactor);
		if(detCsILog)
			detCsILog->SetUserLimits(stepLimit);
		//delete stepLimit; //???
		if (stepLimit && maxStep>0.) stepLimit->SetMaxAllowedStep(maxStep);
		
		
		G4cout << "\n\nConstructed around Basement a **BASEMENT DETECTOR** at z: "
		<<  -detToTargetFullZ/m << " m, radius: "  << detCenterDist/m 
		<< " CsI:: rad= "<< detCsIRad/cm << " cm, ht= " << 2.*detCsIHalfHt/cm 
		<< " cm:\nLeadShield:: rad= "<< detPbRad/cm << " cm, ht= " 
		<< 2.*detPbHalfHt/cm << " cm.\nCd Shield:: rad= " << detCdRad/cm << " cm, ht= "
		<< 2.*detCdHalfHt/cm << " cm.\nWATER:: rad= " << detWaterRad/cm << " cm, ht= "
		<< 2.*detWaterHalfHt/cm << G4endl;
		
	}

	G4LogicalVolume* detScintLog    = NULL;
	if( braneBaseDet )
	{
		
		const double angle1 	= 330.* degree;
		const double angle2 	= 240.*degree; // + start, not absolute position
		const double subAngle1 	= 30.* degree;
		const double subAngle2 	= 120.*degree;	// + start, not absolute position	
		
		const G4double detScintRad	= 2.5*2.54*cm; //5inch/2
		const G4double detScintHht	= 2.5*2.54*cm; //5inch/2	
		//2" acrylic, 1.5" HDPE, 3.5" water

        const G4double detAcrThick      = 2.* 2.54*cm;
        const G4double detHdpeThick      = 1.5* 2.54*cm;
        const G4double detWaterThick      = 3.5* 2.54*cm;
		
		const G4double detScintRadInner   = detCenterDist - detScintRad;
		const G4double detScintRadOuter   = detCenterDist + detScintRad; 

        G4Tubs* cutDetector1    = new G4Tubs ("cutDetector1",
				0, detCenterDist + 2.*detWaterThick, 1.*m, subAngle1, subAngle2);


		posDetector   		= new G4ThreeVector(0,0, -detToTargetFullZ ); //NOTE		
		G4Tubs* braneDetScintCyl			= new G4Tubs ("braneDetScintCyl", 
				  detScintRadInner, detScintRadOuter, detScintHht,angle1, angle2);
        G4SubtractionSolid* braneDetScint 	= new G4SubtractionSolid("braneDetScint",
                            braneDetScintCyl, cutDetector1, rotCenter, *posCenter);

		detScintLog 		= new G4LogicalVolume(braneDetScint, matc7h8, "detScintLog");

		
		detScintLog->SetVisAttributes(new G4VisAttributes (G4Colour::Green())); 
		new G4PVPlacement( rotCenter, *posDetector, detScintLog, "BraneDetScintC7H8", 
							fDetLogical, false, 0 , fCheckOverlap);
		
		G4cout  << "Placed BraneDaseDet of thickness " <<  2.*detScintRad/cm 
				<< " around  " << detCenterDist/m << G4endl;
		

        const G4double detAcrRadIn      = detScintRadInner - detAcrThick;
        const G4double detAcrRadOut     = detScintRadOuter + detAcrThick; 
        const G4double detAcrHHt        = detScintHht + detAcrThick;

        G4Tubs*   detAcrFull          = new G4Tubs ("DetAcrFull", detAcrRadIn, 
                                        detAcrRadOut, detAcrHHt, angle1, angle2);
		
        G4SubtractionSolid* detAcrCylSub = new G4SubtractionSolid("detAcrlic", 
							detAcrFull, braneDetScintCyl, rotCenter, *posCenter);
		
		G4SubtractionSolid* detAcrCyl = new G4SubtractionSolid("detAcrCylSub",
								detAcrCylSub, cutDetector1, rotCenter, *posCenter);		
		
		G4LogicalVolume* detAcrlicLog = new G4LogicalVolume(detAcrCyl,
                                                acrylic, "DetAcrlicLog");

        detAcrlicLog->SetVisAttributes(new G4VisAttributes (G4Colour::Yellow()));    
        new G4PVPlacement( rotCenter, *posDetector, detAcrlicLog, "DetLSAcrlic", 
                           fDetLogical, false, 0 , fCheckOverlap); 
        G4cout  << "Placed Acrylic of thickness " <<  detAcrThick/cm << G4endl;


        const G4double detHdpeRadIn      = detAcrRadIn - detHdpeThick;
        const G4double detHdpeRadOut     = detAcrRadOut + detHdpeThick; 
        const G4double detHdpeHHt        = detAcrHHt + detHdpeThick;

        G4Tubs*   detHdpeFull          = new G4Tubs ("DetHdpeFull", detHdpeRadIn, 
                                        detHdpeRadOut, detHdpeHHt, angle1, angle2);
        G4SubtractionSolid* detHdpeCyl = new G4SubtractionSolid("detHdpe", 
                                detHdpeFull, detAcrFull, rotCenter, *posCenter);

        G4SubtractionSolid* detHdpeCylSub = new G4SubtractionSolid("detHdpeCylSub",
								detHdpeCyl, cutDetector1, rotCenter, *posCenter);

        G4LogicalVolume* detHdpeLog = new G4LogicalVolume(detHdpeCylSub, hdpe, "DetHdpeLog");

        detHdpeLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red()));    
        new G4PVPlacement( rotCenter, *posDetector, detHdpeLog, "DetLSHDPE", 
                           fDetLogical, false, 0 , fCheckOverlap); 
        G4cout  << "Placed HDPE of thickness " <<  detHdpeThick/cm << G4endl;

	
        const G4double detWaterRadIn      = detHdpeRadIn -detWaterThick;
        const G4double detWaterRadOut     = detHdpeRadOut + detWaterThick; 
        const G4double detWaterHHt        = detHdpeHHt + detWaterThick;

        G4Tubs*   detWaterFull          = new G4Tubs ("DetWaterFull", detWaterRadIn, 
                                        detWaterRadOut, detWaterHHt, angle1, angle2);
        G4SubtractionSolid* detWaterCyl = new G4SubtractionSolid("detWater", 
                                detWaterFull, detAcrFull, rotCenter, *posCenter);
        G4SubtractionSolid* detWaterCylSub = new G4SubtractionSolid("detWaterSub",
                        detWaterCyl, cutDetector1, rotCenter, *posCenter);

        G4LogicalVolume* detWaterLog = new G4LogicalVolume(detWaterCylSub, water, "DetWaterLog");

        detWaterLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));    
		new G4PVPlacement( rotCenter, *posDetector, detWaterLog, "DetWaterShield", 
										fDetLogical, false, 0 , fCheckOverlap); 
        G4cout  << "Placed Water of thickness " <<  detWaterThick/cm << G4endl;


		//add to region, for production cut
		if(regDet && detScintLog)regDet->AddRootLogicalVolume(detScintLog);
		
		//ForceStep size for charged particles only		
		G4double maxStep				= detScintRad/2.;
		if(detScintHht < detScintRad) 
						maxStep 		= detScintHht/3.; 	
		G4UserLimits* stepLimit 		= new G4UserLimits(maxStep*maxStepFactor);
		if(detScintLog)
			detScintLog->SetUserLimits(stepLimit);
		if (stepLimit && maxStep>0.) stepLimit->SetMaxAllowedStep(maxStep);
		//https://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s07.html
		
	}//braneDet


	//DetOuterPad to contain detector	
	G4LogicalVolume* baseExtraLog 		= NULL;
	if(  baseExtn && ! boxBasement )
	{		
		const G4double basementRoofHht 	= baseRoofTopHalf + baseRoofStoneHalf 
										+ baseRoofInHalf + tunWallDetThick/2.; 
									//5/2ft + 18/2 inch. excl instr. floor
		const G4double nDetIn			= basementRad + tunnelWall + tunnelWidth;
		const G4double posNPadZ 		= basementPosZ - basementRoofHht - instrFloorHalfHt;
		
	
		G4Tubs* baseExtra = new G4Tubs("outerPadLayer", nDetIn, nDetIn + baseExnFillWidth, 
									   basementHalfHt + instrFloorHalfHt - basementRoofHht, 0, twopi);	
		const G4double baseExnFillHht	= baseRoofTopHalf + baseRoofStoneHalf + baseRoofInHalf;			
		G4ThreeVector posbaseExnFill(0,0, - instrFloorTop2Tgt -2.*instrFloorHalfHt-baseExnFillHht);
		
		
		G4Tubs* baseExnFill		= new G4Tubs("baseExnFill",basementRad+baseRoofGravelRadLen,
							basementRad+baseRoofGravelRadLen + baseExnFillWidth, baseExnFillHht, 0, twopi);
		G4SubtractionSolid* outerPadSub = new G4SubtractionSolid("OuterPadSub",
									baseExtra, baseExnFill, rotCenter, posbaseExnFill);
		
		
		G4SubtractionSolid*	baseExnStairSub = NULL;
		if(fStairCase)
		{
			baseExnStairSub = new G4SubtractionSolid("baseExnStairSub",
						outerPadSub, stairCaseSolid, rotStairCase, posStairCase);
		}
		else 
			baseExnStairSub = outerPadSub;
		
		baseExtraLog = new G4LogicalVolume(baseExnStairSub, stone, "baseExtnLayerLog");
		baseExtraLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta())); 
		G4ThreeVector posNPad(0, 0, posNPadZ); //-8.0906
		new G4PVPlacement( rotCenter, posNPad, baseExtraLog, "BaseExtn",
							fDetLogical, false, 0 , fCheckOverlap);	

	}

	
	//Production Cut
	G4ProductionCuts* cutDet = new G4ProductionCuts();
	cutDet->SetProductionCut(1.*um);
	regDet->SetProductionCuts(cutDet);
	
	
	G4Tubs* surfaceDetSolid 		= NULL;
	G4ThreeVector* posSurfDet 		= NULL;
	G4LogicalVolume* surfaceDetLog 	= NULL;
	if(surfaceDet)
	{
		const G4double detAxialDistSdet = 10.*m;//10.25*m ;//25.*m; //x=19.2, y -16 =>detDiagonalDist = 25m;
		
		const G4double surfaceDetHalfThick = 5.*cm; //10cm/2
		const G4double surfaceDetInner     = detAxialDistSdet - surfaceDetHalfThick;
		const G4double surfaceDetOuter     = detAxialDistSdet + surfaceDetHalfThick; 
		const G4double surfaceDetHalfHt    = 0.75*m; //3m/2 
		const G4double sufDetPosZ = -1.25*m;
		
		//sufDetPosZ = -0.75*m;
		
		posSurfDet = new G4ThreeVector(0, 0, sufDetPosZ);
		surfaceDetSolid      = new G4Tubs ("surfaceDetSolid", surfaceDetInner, 
						surfaceDetOuter, surfaceDetHalfHt, angleStart, angleSpan );
		
		surfaceDetLog = new G4LogicalVolume(surfaceDetSolid, hdpe, "surfaceDetLog");
		surfaceDetLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red())); 
		new G4PVPlacement( rotCenter, *posSurfDet, surfaceDetLog, "SurfaceDetector", 
						   fDetLogical, false, 0 , fCheckOverlap);
		G4cout << "\n\nConstructed ***SURFACE DETECTOR **** on the Target Hall at " 
		<< detAxialDistSdet/m << " m, having thickness: " << 2*surfaceDetHalfThick/cm
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
    if(surfaceHdcLayerLog)regBase->AddRootLogicalVolume(surfaceHdcLayerLog);

    G4ProductionCuts* cutBase = new G4ProductionCuts();
    cutBase->SetProductionCut(10.*mm);
    regBase->SetProductionCuts(cutBase);

	//NOTE region (default=5mm) ProtBox is defined in SNSTargetOuterProtonBox.cc
	
	
    G4Region* regBaseLayer = new G4Region("BasementLayer");
	if(basementLayerLog)regBaseLayer->AddRootLogicalVolume(basementLayerLog);
	if(tunnelWallLog)regBaseLayer->AddRootLogicalVolume(tunnelWallLog);
    if(tunWallDetLog)regBaseLayer->AddRootLogicalVolume(tunWallDetLog);
    G4ProductionCuts* cutBase2 = new G4ProductionCuts();
    cutBase2->SetProductionCut(1.*mm);
    regBaseLayer->SetProductionCuts(cutBase2);


	G4cout << "\nWorld of size (m) = " << 2*halfX/m<< " "<< 2*halfY/m << " " 
								<< 2*halfZ/m << G4endl;
	G4cout << "Hg target of size (cm) " << 2*tgtHalfX/cm << " " << 2*tgtHalfY/cm 
					<< " " << 2*tgtHalfHt/cm << G4endl;
	G4cout <<"Target Steel box of size (cm) " << 2*tgtSteelHfX/cm<< " " << 
					2*tgtSteelHfY/cm << " " << 2*tgtSteelHfZ/cm << G4endl;
	G4cout <<"inner Reflector Plug of Al rad:ht (cm) " << innRPRad/cm << " " 
					<< 2*innRPHHt/cm  << " from 0cm at center " << G4endl;
	G4cout <<"BePlug : top and Bottom of rad:ht (cm) " << BePlugRad/cm 
			<< " from 0 rad, and ht " << 2*BePlugHalfHt/cm << " from +/- z  "
			<<  innRPHHt+BePlugHalfHt << G4endl;
	G4cout <<"LiquidHydrogen moderators size (cm) " << 2*lH1halfX/cm << " " << 
			2*lH1halfY/cm  << " " << 2* lH1halfZ/cm << G4endl;
	G4cout <<" inner upper/lower Steel cylinders of rad:Ht (cm) " << 
		innerSteelRad2/cm-innerSteelRad1/cm << " " << 2*innerSteelHalfHt/cm << " from "
			<< innerSteelRad1/cm << " to " << innerSteelRad2/cm << " at +/- z (cm): " <<
		(innRPHHt+innerSteelGap+innerSteelHalfHt)/cm <<	G4endl;
	G4cout <<"OUTER Steel cylinders of rad:Ht (cm) " << 
		outerSteelR2/cm-outerSteelR1/cm << " " << 2*outerSteelHt/cm << " from "
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
 
 const G4double posnTubeZ	= innRPHHt + nTubeHalfDy1[0]; //rotated y->z	
 G4ThreeVector posFor(posxy[0], posxy[1], posnTubeZ);
 G4Tubs* testMS  = new G4Tubs("Test",4.*m, 5.*m, 5.*m, 0, twopi);
 //G4ThreeVector posInv(-posxy[0], -posnTubeZ, -posxy[1]);
 G4Box * testT = new G4Box("testTube", 1.*m, 2.*m, 10.*m);
 
 
 G4SubtractionSolid* testMSsub = new G4SubtractionSolid("sub", testMS, testT, rotFor, posFor);
 G4LogicalVolume* testL = new G4LogicalVolume(testMS, alum, "testMS1Log");
 new G4PVPlacement( rotInv, posInv, testL, "TestMS", fDetLogical, false, 0, fCheckOverlap); 
 
 G4cout << posInv.getX() << " " << posInv.getY() << " " << posInv.getZ() << G4endl;
 G4SubtractionSolid* sub = new G4SubtractionSolid("sub", testT, testMS, rotInv, posInv);
 G4LogicalVolume* testTL		= new G4LogicalVolume(testT, alum, "TestTubeLog");
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