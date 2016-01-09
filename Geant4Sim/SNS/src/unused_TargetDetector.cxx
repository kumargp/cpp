	/*
	if(basementDet && !insideDet && ! boxDet)
	{
		G4double detCsIRad          = 5.5*cm;
		G4double detCsIRadInner     = detAxialDist - detCsIRad;
		G4double detCsIRadOuter     = detAxialDist + detCsIRad; 
		G4double detCsIHalfHt       = 16.5*cm;//33/2 

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

		G4double detPbRad           = detCsIRad +18.*cm;
		G4double detPbRadInner      = detAxialDist - detPbRad;
		G4double detPbRadOuter      = detAxialDist + detPbRad; 
		G4double detPbHalfHt        = detCsIHalfHt + 18.*cm; 
		G4Tubs* detPbCylFull        = new G4Tubs ("DetPbCylFull", detPbRadInner, 
									detPbRadOuter, detPbHalfHt,0., twopi);
		G4SubtractionSolid* detPbcyl = new G4SubtractionSolid("detPbcyl", 
								detPbCylFull, detCsICyl, rotCenter, *posCenter);
		G4LogicalVolume* detPbLog = new G4LogicalVolume(detPbcyl, lead, "DetPbLog");
		detPbLog->SetVisAttributes(new G4VisAttributes (G4Colour::Brown()));    
		new G4PVPlacement( rotCenter, *posDetector, detPbLog, "DetLeadShield", 
													fDetLogical, false, 0 , fCheckOverlap); 

		G4double detCdRad           = detPbRad + 0.5*mm;
		G4double detCdInner         = detAxialDist - detCdRad;
		G4double detCdOuter         = detAxialDist + detCdRad; 
		G4double detCdHalfHt        = detPbHalfHt + 0.5*mm; 
		G4Tubs* detCdCylFull        = new G4Tubs ("DetCdCylFull", detCdInner, 
									detCdOuter, detCdHalfHt, 0., twopi);
		G4SubtractionSolid* detCdCyl= new G4SubtractionSolid("detCdCyl", 
								detCdCylFull, detPbCylFull, rotCenter, *posCenter);
		G4LogicalVolume* detCdLog = new G4LogicalVolume(detCdCyl, cadmium, "DetCdLog");
		detCdLog->SetVisAttributes(new G4VisAttributes (G4Colour::Grey())); 
		new G4PVPlacement( rotCenter, *posDetector, detCdLog, "DetCdShield", 
													fDetLogical, false, 0 , fCheckOverlap);

		G4double detWaterRad        = detCdRad + 12.*2.54*cm;
		G4double detWaterInner      = detAxialDist - detWaterRad;
		G4double detWaterOuter      = detAxialDist + detWaterRad; 
		G4double detWaterHalfHt     = detPbHalfHt + 12.*2.54*cm; 
		detWaterCylFull    = new G4Tubs ("DetPbCylFull", detWaterInner, 
							detWaterOuter, detWaterHalfHt, 0., twopi);
		G4SubtractionSolid* detWaterCyl = new G4SubtractionSolid("detWaterCyl", 
								detWaterCylFull, detCdCylFull, rotCenter, *posCenter);

		G4LogicalVolume* detWaterLog = new G4LogicalVolume(detWaterCyl, water, "DetWaterLog");
		detWaterLog->SetVisAttributes(new G4VisAttributes (G4Colour::Red())); 
		new G4PVPlacement( rotCenter, *posDetector, detWaterLog, "DetWaterShield", 
													fDetLogical, false, 0 , fCheckOverlap);
		//Cuts
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
		
		G4cout << "\n\nConstructed around Basement a **BASEMENT DETECTOR** at z: "
		<<  -detToTargetFullZ/m << " m, radius: "  << detAxialDist/m 
		<< " CsI:: rad= "<< detCsIRad/cm << " cm, ht= " << 2.*detCsIHalfHt/cm 
		<< " cm:\nLeadShield:: rad= "<< detPbRad/cm << " cm, ht= " 
		<< 2.*detPbHalfHt/cm << " cm.\nCd Shield:: rad= " << detCdRad/cm << " cm, ht= "
		<< 2.*detCdHalfHt/cm << " cm.\nWATER:: rad= " << detWaterRad/cm << " cm, ht= "
		<< 2.*detWaterHalfHt/cm << G4endl;
		
	}
	else if(insideDet && ! boxDet)
	{
		G4double waterShieldHht = 65*cm;  //around 7.5*m
	
		G4double basementRoofHht = baseRoofStoneHalf + baseRoofInHalf + roofDipZ/2.; //5/2ft + 18/2 inch. excl instr. floor
		G4double nDetIn = basementRad + tunnelWall; //baseRadiusAll
		G4Tubs* detWaterIn = new G4Tubs("detWaterInside", nDetIn, nDetIn + thickNDet, 
										waterShieldHht, 0, twopi);
		G4LogicalVolume* detWaterInLog = new G4LogicalVolume(detWaterIn, water, "detWaterInLog");
		detWaterInLog->SetVisAttributes(new G4VisAttributes (G4Colour::Blue())); 
		G4ThreeVector posNPadDet(0, 0, -detToTargetFullZ );
		new G4PVPlacement( rotCenter, posNPadDet, detWaterInLog, "DetWaterInside",
							fDetLogical, false, 0 , fCheckOverlap);
		
		G4Tubs* outerPad = new G4Tubs("outerPadLayer", nDetIn, nDetIn + thickNDet + padThick, 
							basementHalfHt - basementRoofHht, 0, twopi);
		G4ThreeVector posDetWaterInSub(0, 0, basementPushZ + basementRoofHht - detToTargetFullZ );
		G4SubtractionSolid* outerPadSub = new G4SubtractionSolid("OuterPadSub",
								outerPad, detWaterIn, rotCenter, posDetWaterInSub);
		G4LogicalVolume* outerPadLog = new G4LogicalVolume(outerPadSub, lightConcrete, "outerPadLayerLog");
		outerPadLog->SetVisAttributes(new G4VisAttributes (G4Colour::Magenta())); 
		G4ThreeVector posNPad(0, 0, -basementPushZ - basementRoofHht); //-8.0906
		new G4PVPlacement( rotCenter, posNPad, outerPadLog, "DetOuterPad",
							fDetLogical, false, 0 , fCheckOverlap);	
	}
	//else if(boxDet)
	*/
			

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
                                                fDetLogical, false, 0 , fCheckOverlap);
    */

	
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





	/*
	 G4double nBeamOuterStart   = outerSteelR2;                                    *
	 G4double nBeamOuterX 		= 5.*cm+thickNTubeAlX[0];
	 G4double nBeamOuterY 		= 6.*cm+thickNTubeAlY[0];
	 G4double nBeamOuterZhLen 	= 4.*m;//15.*m;
	 //TODO: change length
	 G4double dTheta = 8.*degree;
	 G4double posNTubeZ	= IRPhalfHt + nTubeHalfDy1[0]; //rotated y->z	
	 G4RotationMatrix* rotnNTube = new G4RotationMatrix; 
	 rotnNTube->rotateZ(rotateZ[0]);
	 rotnNTube->rotateX(rotateX[0]);
	 
	 G4ThreeVector posNTubeOuter11(lH3posX + (lH1halfX+nBeamOuterZhLen)*std::cos(lH1Rot), 
	 lH3posY-(lH1halfX +nBeamOuterZhLen)*std::sin(lH1Rot), posNTubeZ);
	 CreateOuterNTube( nBeamOuterX, nBeamOuterY, nBeamOuterZhLen, thickNTubeAlX[0],
	 thickNTubeAlY[0], outerSteelR2, rotnNTube,  posNTubeOuter11, "NTubeOuter11");
	 
	G4RotationMatrix* rotNTube12 = new G4RotationMatrix; 
	rotNTube12->rotateZ(lH1Rot - 90.*degree-dTheta);
	rotNTube12->rotateX(90.*degree);
	G4ThreeVector posNTubeOuter12(lH3posX + (lH1halfX+nBeamOuterZhLen)*std::cos(lH1Rot-dTheta), 
		lH3posY-(lH1halfX +nBeamOuterZhLen)*std::sin(lH1Rot-dTheta), posNTubeZ);
	CreateOuterNTube( nBeamOuterX, nBeamOuterY, nBeamOuterZhLen, thickNTubeAlX[0],
		thickNTubeAlY[0], outerSteelR2, rotNTube12,  posNTubeOuter12, "NTubeOuter12");
	
	
	G4RotationMatrix* rotNTube13 = new G4RotationMatrix; 
	rotNTube13->rotateZ(lH1Rot - 90.*degree+dTheta);
	rotNTube13->rotateX(90.*degree);
	
	G4ThreeVector posNTubeOuter13(lH3posX + (lH1halfX+nBeamOuterZhLen)*std::cos(lH1Rot+dTheta), 
		lH3posY-(lH1halfX +nBeamOuterZhLen)*std::sin(lH1Rot+dTheta), posNTubeZ);
	CreateOuterNTube( nBeamOuterX, nBeamOuterY, nBeamOuterZhLen, thickNTubeAlX[0],
					thickNTubeAlY[0], outerSteelR2, rotNTube13,  posNTubeOuter13, "NTubeOuter13");
		
		
	

   //tube1, top decoupled +x
	G4String tubeName			= "nTube1";
	G4double nTube1HalfDx1     = 21.59*cm;
	G4double nTube1HalfDx2     = 6.91*cm;
	G4double nTube1HalfDy1     = 7.235*cm; 
	G4double nTube1HalfDy2     = 7.235*cm;
	G4double nTubeHalfDz 		= outerSteelR2/2.+ 10.*cm;//extra 
	G4double thickNTubeAlX[1]      = 1.31*cm;
	G4double thickNTube1AlY      = 0.635*cm;
    //cavity  20.136*cm, 5.456*cm, 6.456*cm, 6.456*cm 
    //Cd liner 20.28*cm, 5.6*cm, 6.6*cm, 6.6*cm
    //Al     21.59*cm, 6.91*cm, 7.235*cm, 7.235*cm 
	G4double posNTube1Z	  = IRPhalfHt + nTube1HalfDy1; //rotated y->z	
	
	G4RotationMatrix* rotNTube1 = new G4RotationMatrix; 
	rotNTube1->rotateZ(lH1Rot - 90.*degree);
	rotNTube1->rotateX(90.*degree);
	G4double posNTube1X = lH3posX + (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot);
	G4double posNTube1Y	 = lH3posY-(lH1halfX +nTubeHalfDz)*std::sin(lH1Rot);
	G4double bePlugZ	 = topBePlugZ;

	G4SubtractionSolid* BePlugTopSub3 = NULL;
	BePlugTopSub3 = CreateInnerNTube(	nTube1HalfDx1, nTube1HalfDx2, 
			nTube1HalfDy1, nTube1HalfDy2, nTubeHalfDz, thickNTube1AlX, 
			thickNTube1AlY, rotNTube1, posNTube1X, posNTube1Y, posNTube1Z,
			tubeName, outerSteelR2, outerSteelHt, bePlugZ, BePlugTopSub2);

	G4ThreeVector posNTube1(lH3posX + (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot), 
							lH3posY-(lH1halfX +nTubeHalfDz)*std::sin(lH1Rot), posNTube1Z);
	G4Trd* nTube1Out     = new G4Trd("nTube1Out", nTube1HalfDx1, nTube1HalfDx2, 
									 nTube1HalfDy1, nTube1HalfDy2, nTubeHalfDz);
	G4SubtractionSolid* outerSteelCylSub2 = new G4SubtractionSolid("outerSteelCylSub2", 
			outerSteelCylSub1, nTube1Out, rotNTube1, posNTube1);
		

	//Tube2, top decoupled -x
	tubeName			= "nTube2";
	G4double nTube2HalfDx1     = 31.38*cm;
	G4double nTube2HalfDx2     = 6.91*cm;
	G4double nTube2HalfDy1     = 7.235*cm; 
	G4double nTube2HalfDy2     = 7.235*cm;

	G4double thickNTube2AlX      = 1.454*cm;
	G4double thickNTube2AlY      = 0.869*cm;
	G4double posNTube2Z	  = IRPhalfHt + nTube2HalfDy1; //rotated y->z	

	G4ThreeVector posNTube2(lH3posX - (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot), 
							lH3posY+(lH1halfX +nTubeHalfDz)*std::sin(lH1Rot), posNTube2Z);

	G4RotationMatrix* rotNTube2 = new G4RotationMatrix; 
	rotNTube2->rotateZ(lH1Rot - 90.*degree);
	rotNTube2->rotateX(-90.*degree);

	G4double posNTube2X = lH3posX - (lH1halfX+nTubeHalfDz)*std::cos(lH1Rot);
	G4double posNTube2Y = lH3posY+(lH1halfX +nTubeHalfDz)*std::sin(lH1Rot);
	
	G4SubtractionSolid* BePlugTopSub4 = NULL;
	BePlugTopSub4 = CreateInnerNTube(	nTube2HalfDx1, nTube2HalfDx2, 
										nTube2HalfDy1, nTube2HalfDy2, nTubeHalfDz, thickNTube2AlX, 
								   thickNTube2AlY, rotNTube2, posNTube2X, posNTube2Y, posNTube2Z,
								   tubeName, outerSteelR2, outerSteelHt, bePlugZ, BePlugTopSub3);
	G4Trd* nTube2Out     = new G4Trd("nTube2Out", nTube2HalfDx1, nTube2HalfDx2, 
									 nTube2HalfDy1, nTube2HalfDy2, nTubeHalfDz);
	G4SubtractionSolid* outerSteelCylSub3 = new G4SubtractionSolid("outerSteelCylSub3", 
		outerSteelCylSub2, nTube2Out, rotNTube2, posNTube2);
	
	
	
	G4Tubs* outerSteelDummy = new G4Tubs ("outerSteelDummy", 0, 
										  outerSteelR2, outerSteelHt, 0, twopi);
	G4Box * nBeamOuter1Out = new G4Box("nBoxOuter1Out", nBeamOuterX, nBeamOuterY, nBeamOuterZhLen);
	G4Box * nBeamOuter1In  = new G4Box("nBoxOuter1In", nBeamOuterX-thickNTube1AlX,
									   nBeamOuterY-thickNTube1AlY, nBeamOuterZhLen);	
	G4Tubs* nTubeOuterCylDummy	= new G4Tubs("nTubeOuterCylDummy",  
											 outerSteelR2, 2.*m + nBeamOuterZhLen, 5.*m, 0, twopi);
	
	
	
	
	
	//Tube3 top coupled 90deg +x
	G4double nTube3HalfDx1     = 21.59*cm;
	G4double nTube3HalfDx2     = 6.91*cm;
	G4double nTube3HalfDy1     = 8.035*cm; 
	G4double nTube3HalfDy2     = 8.035*cm; 
	
	G4double thickNTube3AlX      = 1.62*cm;
	G4double thickNTube3AlY      = 0.635*cm;
	G4double nTube3Z	  = IRPhalfHt + nTube3HalfDy1; //rotated y->z	
	
	G4Trd* nTube3Out     = new G4Trd("nTube3Out", nTube3HalfDx1, nTube3HalfDx2, 
									 nTube3HalfDy1, nTube3HalfDy2, nTubeHalfDz);
	G4Trd* nTube3In      = new G4Trd("nTube3In", nTube3HalfDx1-thickNTube3AlX,  
									 nTube3HalfDx2-thickNTube3AlX, nTube3HalfDy1-thickNTube3AlY, 
								  nTube3HalfDy2-thickNTube3AlY, nTubeHalfDz);
	
	G4RotationMatrix* rotNTube3 = new G4RotationMatrix; 
	rotNTube3->rotateZ(- 90.*degree);
	rotNTube3->rotateX(90.*degree);
	G4ThreeVector posNTube3(lH1halfX +nTubeHalfDz, lH1posY, nTube3Z);
	
	G4IntersectionSolid* nTube3XouterSteel1 = new G4IntersectionSolid(
		"nTube3XouterSteel1", outerSteelDummy, nTube3Out, rotNTube3, posNTube3);
	G4IntersectionSolid* nTube3XouterSteel2 = new G4IntersectionSolid(
		"nTube3XouterSteel2", outerSteelDummy, nTube3In, rotNTube3, posNTube3);
	G4SubtractionSolid* nTube3Sub = new G4SubtractionSolid("nTube3OutSub", 
			nTube3XouterSteel1, nTube3XouterSteel2, rotCenter, *posCenter);
	
	G4LogicalVolume* nTube3Log  = new G4LogicalVolume(nTube3Sub, Al, "NTube3Log");
	new G4PVPlacement( rotCenter, *posCenter, nTube3Log, "NTube3",
					   fDetLogical, false, 0 , fCheckOverlap);  
	
	G4ThreeVector posNTube3BePlugTop(lH1halfX +nTubeHalfDz, lH1posY, nTube3Z - topBePlugZ);

	G4SubtractionSolid* BePlugTopSub5 = new G4SubtractionSolid("BePlugTopSub5", 
															   BePlugTopSub4, nTube3Out, rotNTube3, posNTube3BePlugTop);
	G4SubtractionSolid* outerSteelCylSub4 = new G4SubtractionSolid("outerSteelCylSub4", 
																   outerSteelCylSub3, nTube3Out, rotNTube3, posNTube3);
	
	
	
	//Tube4, bottom decoupled  side +x
	G4double nTube4HalfDx1     = 21.59*cm;
	G4double nTube4HalfDx2     = 6.91*cm;
	G4double nTube4HalfDy1     = 7.235*cm; 
	G4double nTube4HalfDy2     = 7.235*cm;
	G4double thickNTube4AlX      = 1.454*cm;
	G4double thickNTube4AlY      = 0.779*cm;
	G4double nTube4Z      = IRPhalfHt + nTube4HalfDy1; //rotated y->z   
	
	G4Trd* nTube4Out     = new G4Trd("nTube4Out", nTube4HalfDx1, nTube4HalfDx2, 
									 nTube4HalfDy1, nTube4HalfDy2, nTubeHalfDz);
	G4Trd* nTube4In      = new G4Trd("nTube4In", nTube4HalfDx1-thickNTube4AlX,  
									 nTube4HalfDx2-thickNTube4AlX, nTube4HalfDy1-thickNTube4AlY, 
								  nTube4HalfDy2-thickNTube4AlY, nTubeHalfDz);
	
	G4RotationMatrix* rotNTube4 = new G4RotationMatrix; 
	rotNTube4->rotateZ(90.*degree - lH1Rot);
	rotNTube4->rotateX(-90.*degree);
	
	G4ThreeVector posNTube4(-lH3posX +(lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot), 
							lH3posY+(lH1halfX +nTubeHalfDz)*std::cos(90.*degree -lH1Rot), -nTube4Z);
	
	G4IntersectionSolid* nTube4XouterSteel1 = new G4IntersectionSolid(
		"nTube4XouterSteel1", outerSteelDummy, nTube4Out, rotNTube4, posNTube4);
	G4IntersectionSolid* nTube4XouterSteel2 = new G4IntersectionSolid(
		"nTube4XouterSteel2", outerSteelDummy, nTube4In, rotNTube4, posNTube4);
	G4SubtractionSolid* nTube4Sub = new G4SubtractionSolid("nTube4OutSub", 
														   nTube4XouterSteel1, nTube4XouterSteel2, rotCenter, *posCenter);
	
	
	G4LogicalVolume* nTube4Log  = new G4LogicalVolume(nTube4Sub, Al, "NTube4Log");
	new G4PVPlacement( rotCenter, *posCenter, nTube4Log, "NTube4",
					   fDetLogical, false, 0 , fCheckOverlap);  
	
	G4ThreeVector posNTube4BePlugBot(-lH3posX + (lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot), 
			lH3posY+(lH1halfX +nTubeHalfDz)*std::cos(90.*degree -lH1Rot) , -nTube4Z - botBePlugZ);
	G4SubtractionSolid* BePlugBotSub3 = new G4SubtractionSolid("BePlugBotSub3", 
			BePlugBotSub2, nTube4Out, rotNTube4, posNTube4BePlugBot);
	G4SubtractionSolid* outerSteelCylSub5 = new G4SubtractionSolid("outerSteelCylSub5", 
			outerSteelCylSub4, nTube4Out, rotNTube4, posNTube4);
	
	
	
    //Tube 5 , bottom decoupled side+x
	G4double nTube5HalfDx1     = 21.59*cm;
	G4double nTube5HalfDx2     = 6.91*cm;
	G4double nTube5HalfDy1     = 7.235*cm; 
	G4double nTube5HalfDy2     = 7.235*cm;
	G4double thickNTube5AlX      = 1.454*cm;
	G4double thickNTube5AlY      = 0.779*cm;
	G4double nTube5Z      = IRPhalfHt + nTube5HalfDy1; //rotated y->z   
	
	G4Trd* nTube5Out     = new G4Trd("nTube5Out", nTube5HalfDx1, nTube5HalfDx2, 
									 nTube5HalfDy1, nTube5HalfDy2, nTubeHalfDz);
	G4Trd* nTube5In      = new G4Trd("nTube5In", nTube5HalfDx1-thickNTube5AlX,  
									 nTube5HalfDx2-thickNTube5AlX, nTube5HalfDy1-thickNTube5AlY, 
								  nTube5HalfDy2-thickNTube5AlY, nTubeHalfDz);
	
	G4RotationMatrix* rotNTube5 = new G4RotationMatrix; 
	rotNTube5->rotateZ(90.*degree - lH1Rot);
	rotNTube5->rotateX(90.*degree);
	
	G4ThreeVector posNTube5(-lH3posX -(lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot), 
							lH3posY-(lH1halfX +nTubeHalfDz)*std::cos(90.*degree -lH1Rot), -nTube5Z);
	
	G4IntersectionSolid* nTube5XouterSteel1 = new G4IntersectionSolid(
		"nTube5XouterSteel1", outerSteelDummy, nTube5Out, rotNTube5, posNTube5);
	G4IntersectionSolid* nTube5XouterSteel2 = new G4IntersectionSolid(
		"nTube5XouterSteel2", outerSteelDummy, nTube5In, rotNTube5, posNTube5);
	G4SubtractionSolid* nTube5Sub = new G4SubtractionSolid("nTube5OutSub", 
				nTube5XouterSteel1, nTube5XouterSteel2, rotCenter, *posCenter);
	
	
	G4LogicalVolume* nTube5Log  = new G4LogicalVolume(nTube5Sub, Al, "NTube5Log");
	new G4PVPlacement( rotCenter, *posCenter, nTube5Log, "NTube5",
					   fDetLogical, false, 0 , fCheckOverlap);  
	
	G4ThreeVector posNTube5BePlugBot(-lH3posX - (lH1halfX+nTubeHalfDz)*std::sin(90.*degree -lH1Rot), 
		lH3posY-(lH1halfX +nTubeHalfDz)*std::cos(90.*degree -lH1Rot) , -nTube5Z - botBePlugZ);
	G4SubtractionSolid* BePlugBotSub4 = new G4SubtractionSolid("BePlugBotSub4", 
					BePlugBotSub3, nTube5Out, rotNTube5, posNTube5BePlugBot);
	G4SubtractionSolid* outerSteelCylSub6 = new G4SubtractionSolid("outerSteelCylSub6", 
					outerSteelCylSub5, nTube5Out, rotNTube5, posNTube5);
	
	

	//Tube6, bottom coupled 90 deg, side -x
	G4double nTube6HalfDx1     = 21.59*cm;
	G4double nTube6HalfDx2     = 6.91*cm;
	G4double nTube6HalfDy1     = 8.035*cm; 
	G4double nTube6HalfDy2     = 8.035*cm; 
	
	G4double thickNTube6AlX    = 1.62*cm;
	G4double thickNTube6AlY    = 0.635*cm;
	G4double nTube6Z	  = IRPhalfHt + nTube6HalfDy1; //rotated y->z	
	
	G4Trd* nTube6Out     = new G4Trd("nTube6Out", nTube6HalfDx1, nTube6HalfDx2, 
									 nTube6HalfDy1, nTube6HalfDy2, nTubeHalfDz);
	G4Trd* nTube6In      = new G4Trd("nTube6In", nTube6HalfDx1-thickNTube6AlX,  
					nTube6HalfDx2-thickNTube6AlX, nTube6HalfDy1-thickNTube6AlY, 
					nTube6HalfDy2-thickNTube6AlY, nTubeHalfDz);
	
	G4RotationMatrix* rotNTube6 = new G4RotationMatrix; 
	rotNTube6->rotateZ(90.*degree);
	rotNTube6->rotateX(90.*degree);
	G4ThreeVector posNTube6(-lH1halfX -nTubeHalfDz, lH1posY, -nTube6Z);
	
	G4IntersectionSolid* nTube6XouterSteel1 = new G4IntersectionSolid(
		"nTube6XouterSteel1", outerSteelDummy, nTube6Out, rotNTube6, posNTube6);
	G4IntersectionSolid* nTube6XouterSteel2 = new G4IntersectionSolid(
		"nTube6XouterSteel2", outerSteelDummy, nTube6In, rotNTube6, posNTube6);
	G4SubtractionSolid* nTube6Sub = new G4SubtractionSolid("nTube6OutSub", 
					nTube6XouterSteel1, nTube6XouterSteel2, rotCenter, *posCenter);
	
	G4LogicalVolume* nTube6Log  = new G4LogicalVolume(nTube6Sub, Al, "NTube6Log");
	new G4PVPlacement( rotCenter, *posCenter, nTube6Log, "NTube6",
					fDetLogical, false, 0 , fCheckOverlap);  
	
	G4ThreeVector posNTube6BePlugBot(-lH1halfX -nTubeHalfDz, lH1posY, -nTube6Z - botBePlugZ);
	
	G4SubtractionSolid* BePlugBotSub5 = new G4SubtractionSolid("BePlugBotSub5", 
					BePlugBotSub4, nTube6Out, rotNTube6, posNTube6BePlugBot);
	G4SubtractionSolid* outerSteelCylSub7 = new G4SubtractionSolid("outerSteelCylSub7", 
					outerSteelCylSub6, nTube6Out, rotNTube6, posNTube6);
	*/

	/*
std::vector <G4SubtractionSolid*> SNSTargetDetector::CreateOuterNTube(G4double nBeamOuterX, 
	G4double nBeamOuterY, G4double nBeamOuterZhLen, G4double thickNTubeAlX,
	G4double thickNTubeAlY, G4double outerSteelR2, G4RotationMatrix* rotNTube,
	 G4ThreeVector posNTubeOuter, G4String outerTubeName, G4bool placeIt)
{

	G4RotationMatrix* rotCenter = new G4RotationMatrix();
	G4ThreeVector* posCenter = new G4ThreeVector(0,0,0);
	G4Box * nBeamOuterOut = new G4Box(outerTubeName+"Out", nBeamOuterX, 
									  nBeamOuterY, nBeamOuterZhLen);
	G4Box * nBeamOuterIn  = new G4Box(outerTubeName+"In", 
		nBeamOuterX-thickNTubeAlX, 	nBeamOuterY-thickNTubeAlY, nBeamOuterZhLen);	
	
	G4Tubs* nTubeOuterCylDummy	= new G4Tubs("nTubeOuterCylDummy",  
		outerSteelR2, 2.*m + nBeamOuterZhLen, 5.*m, 0, twopi);
	G4IntersectionSolid* nTubeOuterXouterOut = new G4IntersectionSolid(
		outerTubeName+"XOuterOut", nTubeOuterCylDummy, nBeamOuterOut, 
				rotNTube, posNTubeOuter);
	G4IntersectionSolid* nTubeOuterXouterIn = new G4IntersectionSolid(
		outerTubeName+"XOuterIn", nTubeOuterCylDummy, nBeamOuterIn, 
				rotNTube, posNTubeOuter);	
	G4SubtractionSolid* nBeamOuterSub = new G4SubtractionSolid(outerTubeName+"Sub", 
		 nTubeOuterXouterOut, nTubeOuterXouterIn, rotCenter, *posCenter);

    //shields
    G4double shieldThick = 2.*m;
    G4Box* nBeamShield = new G4Box(outerTubeName+"Shield",  
                            shieldThick, shieldThick, nBeamOuterZhLen);
    G4IntersectionSolid* nBeamShieldXDummy = new G4IntersectionSolid(
        outerTubeName+"XHdc", nTubeOuterCylDummy, nBeamShield, 
                rotNTube, posNTubeOuter);   
    G4SubtractionSolid* nTubeHdcSubShield = new G4SubtractionSolid(
        "nTubeSubShield", nBeamShieldXDummy, nTubeOuterXouterOut, rotCenter, *posCenter);

    if(placeIt)
    {
        G4Material* Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"); 
        G4LogicalVolume* nBeamOuterLog = new G4LogicalVolume(nBeamOuterSub,
											Al, outerTubeName+"Log");		
        new G4PVPlacement( rotCenter, *posCenter, nBeamOuterLog, outerTubeName,
					   fDetLogical, false, 0 , true);  
    }
    std::vector <G4SubtractionSolid*> subVec(2);
	subVec[0] = nBeamOuterSub;
	subVec[1] = nBeamOuterSub;//nTubeHdcSubShield;

	return subVec;
}
*/

