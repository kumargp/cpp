// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

#include <vector>
#include "VBaseDetPartAssembly.hh"
#include "SNSTargetOuterProtonBox.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4Trap.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ProductionCuts.hh"
#include "G4PhysicalConstants.hh"  //pi, universe_mean_density
#include "G4SystemOfUnits.hh"


SNSTargetOuterProtonBox::SNSTargetOuterProtonBox(G4String physName, 
G4double x1, G4double y1, G4double z1, G4bool msDet, G4bool mStepDet2, 
G4double ms1Inner, G4double ms2Inner, G4double msThick, G4double msHht,
G4double posMSDetZ, G4bool magnet, G4double magnetCutHalfL,
G4double magnetCutHZ, G4bool overlap)
: VBaseDetPartAssembly(physName), fX(x1), fY(y1), fZ(z1), 
 fMSDet(msDet), fMStepDet2(mStepDet2),
 fMS1Inner(ms1Inner), fMS2Inner(ms2Inner),fMSthick(msThick), fMSHht(msHht), 
 fPosMSDetZ(posMSDetZ), fMagnet(magnet),  fMagnetCutHalfL(magnetCutHalfL),
 fMagnetCutHZ(magnetCutHZ), fOverlap(overlap)
{}


SNSTargetOuterProtonBox::~SNSTargetOuterProtonBox()
{}

//TODO copy exists in SNSTargetDetector class 
G4SubtractionSolid* SNSTargetOuterProtonBox::SubMultiStepDet(
	G4SubtractionSolid* fromSolid, G4String name, 
	G4ThreeVector pos1, G4RotationMatrix* rot1)

{
	//TODO get these cyliders from calling function which is defined elsewhere 
	assert(fMS1Inner > 0 || fMS2Inner > 0 );
	assert(fMSthick > 0 && fMSHht >0);
	
	G4Tubs* multiStepCyl1  		= new G4Tubs("multiStepCylSolid1",fMS1Inner, 
										fMS1Inner+ fMSthick, fMSHht, 0, twopi);
	G4SubtractionSolid* sub1 	= new G4SubtractionSolid("sub1",
										fromSolid, multiStepCyl1, rot1, pos1);	
	G4SubtractionSolid* sub		= sub1;
	G4SubtractionSolid* sub2	= sub1;

	
	//Det2 and 3 are independent
	if(fMStepDet2)
	{
		G4Tubs* multiStepCyl2     = new G4Tubs("multiStepCylSolid2",fMS2Inner, 
										   fMS2Inner+ fMSthick, fMSHht, 0, twopi);	
		sub2	= new G4SubtractionSolid(name, sub1, multiStepCyl2, rot1, pos1); 
		sub		= sub2;
	}

	return sub;
}

void SNSTargetOuterProtonBox::Place(G4RotationMatrix* rotAssem, 
                        G4ThreeVector* transAssem, G4LogicalVolume* motherLogVol)
{
		//TODO: the current box is to be changed to seperate plates surrounding


		//subtract beam hole
		G4double hdInsideHfX 	= 50.0*cm;
		G4double hdInsideHfZ 	= hdInsideHfX; 
		const G4double ySteel1Half 	= 11.5*m; //23/2
		
		const G4double magCentRad 	= 25.0*cm;	
        G4double magnetHalfL  = 45.0*cm; 		
		
		const G4double magCutHfX 	= hdInsideHfX; //for cut	
		const G4double magCutHfZ 	= hdInsideHfZ;
		
		const G4double outerConcThick 	= 0.5*m;
		const G4double innerHDThick		= 25.*cm;		//SET
		// from caller : fMagnetCutHalfL, fMagnetCutHZ
		
		if(fMagnetCutHalfL < magnetHalfL )
		{
			magnetHalfL = fMagnetCutHalfL;
			G4cout << "Resetting magnet Length "<< G4endl;
		}
		if(fMagnetCutHZ < hdInsideHfX ) //Z ?
		{
			hdInsideHfX = fMagnetCutHZ;
			G4cout << "Resetting magnet Length "<< G4endl;
		}
		
		
		std::vector <G4double> magPosY {2.5*m, 8.5*m+ magnetHalfL, 9.5*m+ magnetHalfL, 
			15.5*m+ magnetHalfL, 16.5*m+ magnetHalfL, 24.0*m+ magnetHalfL};// -ve rel to center of target
		// {2.5*m, 8.5*m, 9.5*m, 15.5*m, 16.5*m, 24.*m};
		
		
		G4Material* steel = G4NistManager::Instance()->FindOrBuildMaterial("Steel");
		G4Material* HDC = G4NistManager::Instance()->FindOrBuildMaterial("HDConcrete");   
        G4Material* concrete = G4NistManager::Instance()->FindOrBuildMaterial("Concrete"); 
        G4ThreeVector  trans(0.,0., 0.); //center
        G4RotationMatrix* rot = new G4RotationMatrix;


        G4Box* fullBox = new G4Box("FullBox", fX, fY, fZ); //8 x 40 x 8 full

        //outer concrete box
		G4Box* steelBoxDummy = new G4Box("SteelBoxfull", fX-outerConcThick, fY, fZ-outerConcThick);
		
        G4SubtractionSolid* boxConc 	= new G4SubtractionSolid("OuterConcrete", 
												fullBox, steelBoxDummy, rot, trans);

		G4RotationMatrix* totalRot1 	= NetRotation(rotAssem, *transAssem,rot,trans);
		G4ThreeVector* totalTrans1 		= NetTrans(rotAssem, *transAssem, rot,trans);
		
		G4ThreeVector invTrans1(- totalTrans1->x(), -totalTrans1->y(), -totalTrans1->z() + fPosMSDetZ);
		
		G4double monoSteelRad 			= -totalTrans1->y() - fY;
		
		G4RotationMatrix* invRot 		= new G4RotationMatrix;
		//invRot->rotateX(-totalRot1->xx()); // get rotation component
		//invRot->rotateZ(-totalRot1->zz());	//NOTE the order
		//Not working !!!
		
		//multiStepRuns		
		G4SubtractionSolid* boxConcMSub = NULL;	
		if(fMSDet)	
		{
			boxConcMSub = SubMultiStepDet(boxConc, "boxConcMSub", invTrans1, invRot); 
		}	
		boxConcMSub = boxConc;
		
		G4LogicalVolume* boxConcLog = new G4LogicalVolume(boxConcMSub,
                                    concrete,fAssemblyBaseName+"ConcreteLog");
        G4VisAttributes* visAtt = new G4VisAttributes(G4Colour::Cyan()); 
        boxConcLog->SetVisAttributes(visAtt);

        new G4PVPlacement( totalRot1, *totalTrans1, boxConcLog, 
               fAssemblyBaseName+"Concrete", motherLogVol, false, 0 ,fOverlap);

		
        //SteelBox inside outer concrete
        //subtract first (middle) of outer	
		G4Box* steelBoxFull = new G4Box("SteelBoxfull", fX-outerConcThick, ySteel1Half, fZ-outerConcThick);
		
		G4Box* hDBoxInsideFull 		= new G4Box("hDBoxInsideFull",hdInsideHfX + innerHDThick,
												ySteel1Half, hdInsideHfZ + innerHDThick);
		
        G4SubtractionSolid* steelBox= new G4SubtractionSolid("steelBox", 
									steelBoxFull, hDBoxInsideFull, rot, trans);
		G4ThreeVector  trans2(0., fY-ySteel1Half, 0.); //push to +y end, then move back half
		G4RotationMatrix* totalRot2 = NetRotation(rotAssem, *transAssem,rot,trans2);
		G4ThreeVector* totalTrans2 	= NetTrans(rotAssem, *transAssem, rot,trans2);
		G4ThreeVector invTrans2(- totalTrans2->x(), -totalTrans2->y(), -totalTrans2->z() + fPosMSDetZ);
		
		//multiStepRuns				
		G4SubtractionSolid* steelBoxMSub = NULL;
			
		if(fMSDet)	
		{
			steelBoxMSub = SubMultiStepDet(steelBox, "steelBoxMSub", invTrans2, invRot);  			
		}	
		else
			steelBoxMSub = steelBox;
		
		
		G4SubtractionSolid* tempSub1		= steelBoxMSub;
		G4SubtractionSolid* steelBoxMagSub 	= NULL;	
		if(fMagnet)
		{		
			G4Box* magCutSteel = new G4Box("magCutSteel", magCutHfX, magnetHalfL+20.*cm, magCutHfZ);	
			assert(hdInsideHfX <= magCutHfX);
			assert(hdInsideHfZ <= magCutHfZ);
			
			for (std::size_t i=0; i< magPosY.size(); i++)
			{			
				G4ThreeVector magcutPos1(0, ySteel1Half + monoSteelRad-magPosY[i], 0); 	
				G4cout <<  " MAGNET_YPOS(m)" << i << ": " << -magPosY[i]/m << G4endl;	
				
				steelBoxMagSub 		= new G4SubtractionSolid("SteelBoxMagSub", 
										tempSub1, magCutSteel, rot, magcutPos1);
				tempSub1				= steelBoxMagSub;
			}
		}
		else
			steelBoxMagSub 			= steelBoxMSub;
		
		
		G4LogicalVolume* steelBoxLog = new G4LogicalVolume(steelBoxMagSub,
                                    steel,fAssemblyBaseName+"SteelBoxLog");
		
		G4VisAttributes* visAtt2 = new G4VisAttributes(G4Colour::Yellow()); 
		steelBoxLog->SetVisAttributes(visAtt2);

        new G4PVPlacement( totalRot2, *totalTrans2,steelBoxLog, 
               fAssemblyBaseName+"SteelBox", motherLogVol, false, 0 ,fOverlap);

		
		
        //HD concrete
        G4Box* beamCutHD1 = new G4Box("beamCutHD1", hdInsideHfX, ySteel1Half, hdInsideHfZ);
        G4SubtractionSolid* hDBox1 = new G4SubtractionSolid("HDBox1", 
										hDBoxInsideFull, beamCutHD1, rot, trans);
		
		//multiStepRuns				
		G4SubtractionSolid* hDBox1MSub = NULL;
	
		if(fMSDet)	
		{
			hDBox1MSub = SubMultiStepDet(hDBox1, "hDBox1MSub", invTrans2, invRot);  			
		}	
		else
			hDBox1MSub = hDBox1;
		G4SubtractionSolid* tempSub		 = hDBox1MSub;
		G4SubtractionSolid* hDBox1MagSub = NULL;	
		if(fMagnet)
		{		
			G4Box* magCutHD1 = new G4Box("magCutHD1", magCutHfX, magnetHalfL+20.*cm, 3/2.*magCutHfX);	
			assert(hdInsideHfX <= magCutHfX);
			
			for (std::size_t i=0; i< magPosY.size(); i++)
			{			
				G4ThreeVector magcutPos(0, ySteel1Half + monoSteelRad-magPosY[i], 0); 	

				hDBox1MagSub 		= new G4SubtractionSolid("HDBox1MagSub", 
										tempSub, magCutHD1, rot, magcutPos);
				tempSub				= hDBox1MagSub;
							
				G4ThreeVector magnetPos(0, fY + monoSteelRad -magPosY[i],0); 
				std::ostringstream ss2;
				ss2 << i+1 ;
				G4String namePart = "_"+ss2.str(); 
				
				PlaceMagnet(namePart, hdInsideHfX, magnetHalfL, magCentRad, magnetPos, 
							rotAssem, transAssem, motherLogVol);
			}
		}
		else
			hDBox1MagSub 			= hDBox1MSub;
		
		G4LogicalVolume* HDBox1Log 	= new G4LogicalVolume(hDBox1MagSub,
                                   HDC,fAssemblyBaseName+"HDBoxLog");
        G4VisAttributes* visAtt3 	= new G4VisAttributes (G4Colour::Red()); 
		HDBox1Log->SetVisAttributes(visAtt3);

        //use the above for steel
        new G4PVPlacement( totalRot2, *totalTrans2, HDBox1Log, 
               fAssemblyBaseName+"HDBox1", motherLogVol, false, 0 ,fOverlap);

		
		//inside of Outermost
		const G4double x2in = (fX-outerConcThick)/2.; 
        const G4double y2in = (2.*fY-2.*ySteel1Half) /2.; //
        const G4double z2in = x2in; 
		G4Box* steelInOuterSolid = new G4Box("steelInOuterSolid", fX-outerConcThick, y2in, fZ-outerConcThick);
        G4Box* inBox2 = new G4Box("inBox2",x2in, y2in, z2in);
        G4SubtractionSolid* steelInOuter = new G4SubtractionSolid("SteelInOuter", 
                                    steelInOuterSolid, inBox2, rot, trans);
		G4ThreeVector  trans4(0., -fY+y2in, 0.);  
		G4RotationMatrix* totalRot4 = NetRotation(rotAssem, *transAssem,rot,trans4);
		G4ThreeVector* totalTrans4 = NetTrans(rotAssem, *transAssem, rot,trans4);  
		G4ThreeVector invTrans4(- totalTrans4->x(), -totalTrans4->y(), -totalTrans4->z()+fPosMSDetZ);
		
		//multiStepRuns				
		G4SubtractionSolid* steelInOutMSub = NULL;
		
		if(fMSDet)	
		{
			steelInOutMSub = SubMultiStepDet(steelInOuter, "steelInOutMSub", invTrans4, invRot);  
			
		}	
		else
			steelInOutMSub = steelInOuter;
		
		G4LogicalVolume* steelInOuterLog = new G4LogicalVolume(steelInOutMSub,
                                   steel,fAssemblyBaseName+"SteelInOuterLog");
		G4VisAttributes* visAtt4 = new G4VisAttributes (G4Colour::Yellow()); 
		steelInOuterLog->SetVisAttributes(visAtt4);

        new G4PVPlacement( totalRot4, *totalTrans4, steelInOuterLog, 
               fAssemblyBaseName+"SteelInOuter", motherLogVol, false, 0 ,fOverlap);

        G4Region* regPBox = new G4Region("ProtBox");
        if(steelInOuterLog)regPBox->AddRootLogicalVolume(steelInOuterLog);
        if(HDBox1Log)regPBox->AddRootLogicalVolume(HDBox1Log);
        if(steelBoxLog)regPBox->AddRootLogicalVolume(steelBoxLog);
        if(boxConcLog)regPBox->AddRootLogicalVolume(boxConcLog);
        G4ProductionCuts* cutPBox = new G4ProductionCuts();
        cutPBox->SetProductionCut(5.*mm);
        regPBox->SetProductionCuts(cutPBox);
}



void SNSTargetOuterProtonBox::PlaceMagnet(const G4String namePart , const G4double hdInsideHfX,  
	const G4double magnetHalfL, const G4double magCentRad, G4ThreeVector magnetPos, 
	G4RotationMatrix* rotAssem, G4ThreeVector* transAssem, G4LogicalVolume* motherLogVol)
{

	G4Material* copper 		= G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
	G4Material* steel 		= G4NistManager::Instance()->FindOrBuildMaterial("Steel");
	G4ThreeVector  trans(0.,0., 0.); //center
	G4RotationMatrix* rot = new G4RotationMatrix;	
	G4Tubs* dummySolid      = new G4Tubs ("DummySolid",0, 2.*mm, 2.*mm, 0, twopi);
	G4Tubs* mag1  			= new G4Tubs("mag1", magCentRad, hdInsideHfX*3/4., magnetHalfL, 0, twopi);
	G4RotationMatrix* rotMag= new G4RotationMatrix;	
	rotMag->rotateX(90.*degree);
	G4Box* mag2   			= new G4Box("mag2", 1.*mm, magnetHalfL, 1.*mm);
	G4UnionSolid* magU1 	= new G4UnionSolid("magU1", mag2, mag1, rotMag, trans);
	
	//value checks
	G4double secStep 		= hdInsideHfX*3/4.+3.*cm;
	if(secStep >= hdInsideHfX) secStep =  hdInsideHfX*3/4.;
	G4Tubs* mag3			= new G4Tubs("mag3",secStep, hdInsideHfX, magnetHalfL, 0, twopi);
	G4UnionSolid* magU2 	= new G4UnionSolid("magSolid", mag2, mag3, rotMag, trans);
	
	
	G4SubtractionSolid* magBoxDSub1 = new G4SubtractionSolid("magBoxDSub1",
															 magU1, dummySolid,  rot, trans);
	G4SubtractionSolid* magBoxDSub2 = new G4SubtractionSolid("magBoxDSub2",
															 magU2, dummySolid,  rot, trans);			
	
	G4RotationMatrix* totalRot21 = NetRotation(rotAssem, *transAssem, rot, magnetPos);	
	G4ThreeVector* totalTrans21  = NetTrans(rotAssem, *transAssem, rot, magnetPos);	
	
	//multiStepRuns				
	G4SubtractionSolid* magBoxMSub1 = NULL;
	G4SubtractionSolid* magBoxMSub2 = NULL;			
	if(fMSDet)	
	{
		G4ThreeVector invTrans21(- totalTrans21->x(), 
								 -totalTrans21->y(), -totalTrans21->z() + fPosMSDetZ);
		
		magBoxMSub1 = SubMultiStepDet(magBoxDSub1, "magBoxMSub1", invTrans21, rot);  
		magBoxMSub2 = SubMultiStepDet(magBoxDSub2, "magBoxMSub2", invTrans21, rot);  				
	}	
	else
	{
		magBoxMSub1 = magBoxDSub1;
		magBoxMSub2 = magBoxDSub2;
	}
	G4LogicalVolume* magBox1Log = new G4LogicalVolume(magBoxMSub1,
													  copper,fAssemblyBaseName+"MagBox1Log");
	G4LogicalVolume* magBox2Log = new G4LogicalVolume(magBoxMSub2,
													  steel,fAssemblyBaseName+"MagBox2Log");
	G4VisAttributes* visAtt31 = new G4VisAttributes (G4Colour::Brown()); 
	magBox1Log->SetVisAttributes(visAtt31);
	G4VisAttributes* visAtt32 = new G4VisAttributes (G4Colour::Grey()); 
	magBox2Log->SetVisAttributes(visAtt32);			
	
	new G4PVPlacement( totalRot21, *totalTrans21, magBox1Log, 
			fAssemblyBaseName+"MagnetCoil"+namePart, motherLogVol, false, 0 ,fOverlap);
	new G4PVPlacement( totalRot21, *totalTrans21, magBox2Log, 
			fAssemblyBaseName+"MagnetPart"+namePart, motherLogVol, false, 0 ,fOverlap);

	
	/*
	G4Region* regPBoxMag = new G4Region("ProtBoxMagnet");
	if(magBox1Log)regPBoxMag->AddRootLogicalVolume(magBox1Log);
	if(magBox2Log)regPBoxMag->AddRootLogicalVolume(magBox2Log);
	G4ProductionCuts* cutPBoxM = new G4ProductionCuts();
	cutPBoxM->SetProductionCut(5.*mm);
	regPBoxMag->SetProductionCuts(cutPBoxM);
	*/
}

