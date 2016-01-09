// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#include "SNSParallelTargetDetector.hh"
#include "SNSParallelMessenger.hh"

#include "globals.hh"  
#include <sstream>
#include <map>
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4PVPlacement.hh"



// For Primitive Scorers
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4SDParticleFilter.hh"
#include "G4PSNofCollision.hh"
#include "G4PSPopulation.hh"
#include "G4PSTrackCounter.hh"
#include "G4PSTrackLength.hh"
#include "G4VIStore.hh"
#include "G4IStore.hh"   // for importance biasing
#include "G4WeightWindowStore.hh"    // for weight window technique


SNSParallelTargetDetector::SNSParallelTargetDetector(G4String worldName)
:G4VUserParallelWorld(worldName),
 fParallelWorld(NULL), 
 fIsVariableStep(false), fNumVol(10), fLogicalVolumeVector()
{
    fMessenger = new SNSParallelMessenger(this);
}

SNSParallelTargetDetector::~SNSParallelTargetDetector()
{
	delete fMessenger;
}

void SNSParallelTargetDetector::Construct()
{


	fParallelWorld = GetWorld();
	G4cout << "Constructing Parallel World" << G4endl;
    
    //G4Material* dummyMat  = 0;

    G4cout << " SNSParallelTargetDetector:: ParallelWorldName = " 
                 << fParallelWorld->GetName() << G4endl;
                 
	G4LogicalVolume* parallelWorldLogical = fParallelWorld->GetLogicalVolume();
	G4bool surfaceDet 		= false;
	G4bool shrinkOuter 		= false; //limit par to surface shielding		
	G4bool addInnerHdcBase	= false;
    G4bool surfaceDetSpace     = false; // air above for surface detector
	G4double detHalfHt 		= 0.5*m;	

	
	//Don't change the following keyword in comment, used in script
	fNumVol 			 = 55; //NUMVOL


shrinkOuter     = true; //fit to medium
addInnerHdcBase = true;
surfaceDet      = false;
surfaceDetSpace = false;

    detHalfHt       = 3.5*m;

    if(surfaceDet && !surfaceDetSpace) 
    {
        G4cerr << "\n**** Possible Error!. Parallel Volume set below basement."
               << " No space for Surface Detector\n" << G4endl; 
        std::abort();
    }
	//use exampleRE06.cc to set meesenger
	G4double parCylHalfHt 		= 7.*m; //	
    if(surfaceDet)parCylHalfHt  = 6.5*m;//11.5*m; 
	G4double mainVolRad		    = 22.3*m;	
    if(surfaceDet) mainVolRad   = 10.*m; //25.*m - detToWallHor;
    G4double offBeamZ 			= 0.5*m;	  //protBoxHalfXZ= 2*m;
	G4double offTargetAxis		= 1.*m;
    G4double detToWallHor		= 1.2*m;
    G4double detToWallVert		= detHalfHt + 0.5*m;
	G4double detToTargetRad   	= mainVolRad + detToWallHor;	

	if(fNumVol <1 ) 
	{
		G4cerr << "** Number of parallel volumes is WRONG *** " << G4endl;
		return;
	}
	
	G4double stepCylRad 		= (mainVolRad - offTargetAxis)/fNumVol;
	G4double stepCylHt  		= (parCylHalfHt - detToWallVert)/fNumVol; 	
	G4double posParCylZ        =  -parCylHalfHt -offBeamZ;
    if(surfaceDet) posParCylZ  =  2.*m;//-3.7*m;
	G4ThreeVector posParCyl(0,0, posParCylZ); //cylinder  
	G4ThreeVector posParBox(18.2*m,-5.*m,-parCylHalfHt -offBeamZ);
	
	G4ThreeVector center(0,0,0);
	G4PVPlacement* parallelPhys = NULL;
	G4LogicalVolume* parLog 	= NULL;	
	G4RotationMatrix* rot 	    = new G4RotationMatrix(); 

	
	G4String shape = "cylinder";//"box" 

	if(!shrinkOuter)
	{
		for(G4int i=0; i<fNumVol+1; i++) 
		{
			std::ostringstream ss;
			ss << "parallelSNS"<< i;
			G4String pName = ss.str();
			
			if( shape == "cylinder") //cylinder
			{
				G4SubtractionSolid* cylSolid = NULL;

				G4Tubs* cylFull = NULL;
				G4Tubs* cylInner = NULL;
				

				G4cout << " Full:  rad1 " << (i*stepCylRad+offTargetAxis)/m 
						<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-i*stepCylRad)/m  
						<< " ht "<<(parCylHalfHt-i*stepCylHt)/m  << G4endl;
				
				
				cylFull = new G4Tubs("BasementCyl", i*stepCylRad+ offTargetAxis, 
						2.*detToTargetRad- offTargetAxis-i*stepCylRad, 
						parCylHalfHt-i*stepCylHt, 0., twopi ); // 0, twopi);
				if(i < fNumVol)
				{		
					cylInner = new G4Tubs("CylIn",(i+1)*stepCylRad+offTargetAxis,
						2.*detToTargetRad- offTargetAxis-(i+1)*stepCylRad, 
						parCylHalfHt-(i+1)*stepCylHt, 0., twopi); //0,twopi
					
					G4cout << " Inner:  rad1 " << ((i+1)*stepCylRad+offTargetAxis)/m 
							<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-(i+1)*stepCylRad)/m  
							<< " ht "<<(parCylHalfHt-(i+1)*stepCylHt)/m << G4endl;
				
					cylSolid =  new G4SubtractionSolid("SubCyl",cylFull, cylInner, rot, center);	
					parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
				}
				else if(i == fNumVol)
				{
					parLog = new G4LogicalVolume(cylFull,0,pName+"Log");
				}

				parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
				parallelPhys = new G4PVPlacement(rot, posParCyl, parLog,// NOTE change pos
												pName, parallelWorldLogical, false, 0);
			
			} //if shape cylinder

			SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );//NOTE
			//fPhysicalVolumeVector.push_back(HgBox->GetPartLog());
			//parallelWorldLogical = parLog; // to nest the next volume in this
		}
	}
	
	//******seperate shielding***************
	else if(shrinkOuter)
	{

		if(addInnerHdcBase)
		{

parCylHalfHt = 10.*m ;  //8+7.8+3 NOTE:9.4 is exact, but it causes wrong wt
            //mainVolRad 		= 24.*m; //25.*m - detToWallHor;

            //detHalfHt = 0.75*m+0.1*m;  
            //detToWallVert       = detHalfHt + 0.5*m;
            G4bool limit2HDC = false;
		
			posParCylZ  		  =  8.*m - parCylHalfHt; //-2.*m;	1m		
			detToTargetRad   	  =  mainVolRad + detToWallHor;
			G4double monoRad	  = 5.*m;
			G4double hdcRadMax    = 10.*m;		
					
			G4int numMonoVol 	  = fNumVol * (monoRad - offTargetAxis)/(mainVolRad- offTargetAxis);
			G4int numHdcVol 	  = fNumVol * (hdcRadMax-monoRad)/(mainVolRad- offTargetAxis);
			G4int numBaseVol      = fNumVol - numMonoVol - numHdcVol;
			G4double stepMonoRad  = (monoRad - offTargetAxis)/numMonoVol;
			G4double stepHdcRad   = (hdcRadMax - monoRad)/numHdcVol;
			G4double hdcStart     = monoRad + stepHdcRad;		
			G4double stepBaseRad  = (mainVolRad - hdcRadMax)/(numBaseVol);
			G4double baseStart    = hdcRadMax+ stepBaseRad;	
			
			detToTargetRad = (limit2HDC == true ) ? hdcRadMax: detToTargetRad;
			stepBaseRad = (limit2HDC == true ) ? 0: stepBaseRad;
			numBaseVol  = (limit2HDC == true ) ? 0: numBaseVol;
			
			fNumVol = numMonoVol + numHdcVol + numBaseVol;
			
			if(limit2HDC) 
				G4cout << "\n\n **** No Base Volume Slices:: Detector Should be "
						<< " Placed near HDC shield ***** \n\n" << G4endl;
						
			G4cout << "Mono+Hdc+Base= " << numMonoVol << " + " << numHdcVol
					<< " + " << numBaseVol << " =? " << fNumVol << G4endl;			

			G4double stepMonoHt  = 6.*cm*45./fNumVol;//(parCylHalfHt - detToWallVert)/fNumVol; 	
			G4ThreeVector posParCylOuter(0, 0, posParCylZ);
			G4double hdcHalfHt   = (parCylHalfHt -posParCylZ + 1.5*m- (numMonoVol+1)*stepMonoHt)/2.;
            G4double  posHdcVolZ = 1.5*m - hdcHalfHt;
			G4ThreeVector posHdcVol(0, 0, posHdcVolZ);
			G4double stepHdcHt 	 = stepMonoHt;  // =steplimit 
			G4double drad = 20.*cm; //=	stepHdcHt


            G4double posBaseVolZ  = posHdcVolZ;
            G4double baseSliceHht = hdcHalfHt - numHdcVol*stepHdcHt;// - 0.1*m;

            if(!surfaceDetSpace)
            {
                G4double pushBaseDown = (3.5*m - numHdcVol*stepHdcHt )/2.;
                baseSliceHht -= pushBaseDown;
                posBaseVolZ  -= pushBaseDown;
            }
            G4ThreeVector posBaseVol(0, 0, posBaseVolZ);
            G4double stepBaseHt   = stepHdcHt;
            G4ThreeVector posSubHdcMonoLast(0,0, posHdcVolZ - posParCylZ);
            G4ThreeVector posBaseHdcLast(0, 0, posBaseVolZ - posHdcVolZ);
/*
			G4double baseSliceUpFrom_2mBase = 2.*detHalfHt + numBaseVol*stepBaseHt;
			G4double baseSliceDownFrom_2mBase = parCylHalfHt -posParCylZ -1.5*m - 
					(numMonoVol+1)*stepMonoHt - (numHdcVol+1)*stepHdcHt ;
			G4double baseSliceHht = (baseSliceUpFrom_2mBase + baseSliceDownFrom_2mBase)/2.;
            G4double basePosConst =  fNumVol*(-1.25+0.75)/(100.-40.) -1.25/3.;
                // -1 for 65; -0.75 for 40; -1.25 for 100
            G4double posBaseSliceZ = basePosConst * m + baseSliceUpFrom_2mBase - baseSliceHht;			
            //G4ThreeVector posInnerMostHdc(0, 0, posParCylZ - hdcHalfHt - posBaseSliceZ);
			G4ThreeVector posSliceBaseVol(0, 0, posBaseSliceZ);
*/

			if(stepMonoRad < drad) drad = stepMonoRad;
			if(stepHdcRad < drad) drad = stepHdcRad; //hdc >mono
			
			G4int lastVal = 0; // innermost 
			for(G4int i=0; i< numMonoVol+1; i++) 
			{
				std::ostringstream ss;
				ss << "Mono_"<< i;
				G4String pName = ss.str();
				
				G4Tubs* outerCylFull = NULL;

				G4cout << " MonoFull:  rad1 " << (i*stepMonoRad+offTargetAxis)/m 
				<<  " rad2 "<< (detToTargetRad+detToWallHor*2.+fNumVol*drad-i*drad)/m  
				<< " ht "   << (parCylHalfHt-i*stepMonoHt)/m  
                << " placed_at_z " << posParCylZ/m 
                << " z_top " << (posParCylZ + parCylHalfHt-i*stepMonoHt)/m  
                << " z_bot " << (posParCylZ - (parCylHalfHt-i*stepMonoHt))/m  
                << G4endl;
				
				
				outerCylFull = new G4Tubs("OuterCyl", i*stepMonoRad+ offTargetAxis, 
								detToTargetRad + detToWallHor*2.+ fNumVol*drad-i*drad, 
								parCylHalfHt-i*stepMonoHt, 0, twopi);
						 
					
				if(i < numMonoVol)
				{	
					G4Tubs* outerCylIn = new G4Tubs("OuterIn",(i+1)*stepMonoRad+offTargetAxis,
					detToTargetRad+ detToWallHor*2.+ fNumVol*drad-(i+1)*drad, 
					parCylHalfHt-(i+1)*stepMonoHt, 0, twopi);
					
					G4cout << " MonoInner:  rad1 " << ((i+1)*stepMonoRad+offTargetAxis)/m 
                        <<  " rad2 "<<(detToTargetRad+ detToWallHor*2.+fNumVol*drad-(i+1)*drad)/m  
                        << " ht "<<(parCylHalfHt-(i+1)*stepMonoHt)/m 
                        << " z_top " << (posParCylZ + parCylHalfHt-(i+1)*stepMonoHt)/m  //sub center
                        << " z_bot " << (posParCylZ - (parCylHalfHt-(i+1)*stepMonoHt))/m//sub center  
                        << G4endl;
					
					G4SubtractionSolid* cylSolid =  new G4SubtractionSolid(
								"SubCyl",outerCylFull, outerCylIn, rot, center);	
					parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
				}
				else if(i == numMonoVol)
				{
					if(stepHdcRad>0)
					{
						G4Tubs* monoLast =  new G4Tubs("monoLast", monoRad+stepHdcRad, 
							detToTargetRad + detToWallHor*2.+ fNumVol*drad-(i+1)*drad,
							hdcHalfHt, 0, twopi);
						G4SubtractionSolid* monoLastSub = new G4SubtractionSolid(
							"monoLastSub", outerCylFull, monoLast, rot, posSubHdcMonoLast);
						G4cout << " MonoInLast:  rad1 " << (monoRad+stepHdcRad)/m <<  " rad2 "
						<<(detToTargetRad + detToWallHor*2.+ fNumVol*drad-(i+1)*drad)/m  
						<< " ht "<< hdcHalfHt/m  
                        << " taken_off_at "<< (posHdcVolZ-posParCylZ)/m 
                << " posHdcVolZ " << posHdcVolZ/m
                << " hdcHalfHt " << hdcHalfHt/m
                        << " placed at " << (posHdcVolZ)/m  
                        << " z_top " << (posHdcVolZ + hdcHalfHt)/m  
                        << " z_bot " << (posHdcVolZ - hdcHalfHt)/m 
                        << G4endl;
						
						parLog = new G4LogicalVolume(monoLastSub,0,pName+"Log");
					}
					else
						parLog = new G4LogicalVolume(outerCylFull,0,pName+"Log");
				}
				
				parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
				parallelPhys = new G4PVPlacement(rot, posParCylOuter, parLog,
												pName, parallelWorldLogical, false, 0);
				
				
				SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );
				//fPhysicalVolumeVector.push_back(HgBox->GetPartLog());
				//parallelWorldLogical = parLog; // to nest the next volume in this
			}//for
			for(G4int i=0; i<numHdcVol; i++)
			{			
				std::ostringstream ss;
				ss << "parallelSNShdc"<< i;
				G4String pName = ss.str();
				
				G4SubtractionSolid* cylSolid = NULL;
				
				G4Tubs* hdcCylFull = NULL;
				G4Tubs* hdcCylIn = NULL;
				
				G4cout << " HDCFull:  rad1 " << (i*stepHdcRad+hdcStart)/m 
				<<  " rad2 " << (detToTargetRad+detToWallHor*2.+fNumVol*drad-(numMonoVol+1+i)*drad)/m  
				<< " ht " << (hdcHalfHt-i*stepHdcHt)/m  
                << " placed_at_z " << posHdcVolZ/m 
				<< " z_top " << (posHdcVolZ + hdcHalfHt-i*stepHdcHt)/m  
                << " z_bot " << (posHdcVolZ - (hdcHalfHt-i*stepHdcHt))/m  
				<< G4endl;

				hdcCylFull = new G4Tubs("HdcOuterCyl", i*stepHdcRad+ hdcStart, 
							detToTargetRad + detToWallHor*2.+ fNumVol*drad-(numMonoVol+1+i)*drad, 
							hdcHalfHt-i*stepHdcHt, 0, twopi);
				
				
				if(i < numHdcVol-1)
				{		
					hdcCylIn = new G4Tubs("HdcInerCyl",(i+1)*stepHdcRad+hdcStart,
							detToTargetRad+ detToWallHor*2.+ fNumVol*drad-(numMonoVol+1+i+1)*drad, 
							hdcHalfHt-(i+1)*stepHdcHt, 0, twopi);
					
					G4cout << " HDCInner:  rad1 " << ((i+1)*stepHdcRad+hdcStart)/m 
					<<  " rad2 "<< (detToTargetRad+ detToWallHor*2.+fNumVol*drad
							-(numMonoVol+1+i+1)*drad)/m
					<< " ht " << (hdcHalfHt-(i+1)*stepHdcHt)/m 
					<< " z_top " << (posHdcVolZ + hdcHalfHt-(i+1)*stepHdcHt)/m  //w.r.t center
                    << " z_bot " << (posHdcVolZ - (hdcHalfHt-(i+1)*stepHdcHt))/m  
                    << G4endl;

					cylSolid =  new G4SubtractionSolid("HdcSubCyl",hdcCylFull, 
                                hdcCylIn, rot, center);	
					parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
				}
				else if(i == numHdcVol-1)
				{
					if(stepBaseRad>0)
					{	
						G4Tubs* hdcLast =  new G4Tubs("hdcLast", hdcRadMax+stepBaseRad, 
							detToTargetRad + detToWallHor*2.+ fNumVol*drad-(numMonoVol+1+i+1)*drad,
							baseSliceHht, 0, twopi);
						G4SubtractionSolid* hdcSub = new G4SubtractionSolid(
							"hdcSub", hdcCylFull, hdcLast, rot, posBaseHdcLast);
					
						G4cout << " HDCInnerLast:  rad1 " << (hdcRadMax+stepBaseRad)/m 
					          << " rad2 "<< (detToTargetRad + detToWallHor*2.+ fNumVol*drad-
						      (numMonoVol+1+i+1)*drad)/m << " ht "<< baseSliceHht/m  
							  << " taken_off_at "<<  (posBaseVolZ - posHdcVolZ)/m 
						      << " placed_at "<< posHdcVolZ/m 
                              << " z_top " << (posBaseVolZ  + baseSliceHht)/m
                              << " z_bot " << (posBaseVolZ - baseSliceHht)/m 
                              << G4endl;
					
						parLog = new G4LogicalVolume(hdcSub,0,pName+"Log");
					}
					else
						parLog = new G4LogicalVolume(hdcCylFull, 0, pName+"Log");
				}
				
				parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Yellow()));
                //if(i == numHdcVol-1)parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Green())); 
				parallelPhys = new G4PVPlacement(rot, posHdcVol, parLog,
												 pName, parallelWorldLogical, false, 0);
				
				SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1+numMonoVol+1) );
				
			}//hdcVol
			
			
			lastVal = 0;
			for(G4int i=0; i<numBaseVol; i++)
			{			
				std::ostringstream ss;
				ss << "parallelSNSBase"<< i;
				G4String pName = ss.str();
				
				G4SubtractionSolid* cylSolid = NULL;
				
				G4Tubs* baseCylFull = NULL;
				G4Tubs* baseCylIn = NULL;
				
				G4cout << " BaseFull:  rad1 " << (i*stepBaseRad+baseStart)/m << " rad2 "
                    << (detToTargetRad+detToWallHor*2.+fNumVol*drad-(numHdcVol+numMonoVol+1+i)*drad)/m  
                    << " ht "<<(baseSliceHht-i*stepBaseHt)/m  
                    << " placed_at_z " <<   posBaseVolZ/m  
                    << " z_top " << (posBaseVolZ  + baseSliceHht-i*stepBaseHt)/m
                    << " z_bot " << (posBaseVolZ - (baseSliceHht-i*stepBaseHt))/m 
                    << G4endl;
				
				baseCylFull = new G4Tubs("BaseOuterCyl", i*stepBaseRad+ baseStart, 
					detToTargetRad + detToWallHor*2.+ fNumVol*drad-(numHdcVol+numMonoVol+1+i)*drad, 
					baseSliceHht-i*stepBaseHt, 0, twopi);
				
				if(i < numBaseVol-1)
				{		
					baseCylIn = new G4Tubs("BaseInerCyl",(i+1)*stepBaseRad+baseStart,
						detToTargetRad+ detToWallHor*2.+ fNumVol*drad-(numHdcVol+numMonoVol+1+i+1)*drad, 
						baseSliceHht-(i+1)*stepBaseHt, 0, twopi);
					
					G4cout << " BaseInner:  rad1 " << ((i+1)*stepBaseRad+baseStart)/m 
					<<  " rad2 "<< (detToTargetRad+ detToWallHor*2.+fNumVol*drad
					-(numHdcVol+numMonoVol+1+i+1)*drad)/m
					<< " ht " << (baseSliceHht-(i+1)*stepBaseHt)/m 
                    << " z_top " << (posBaseVolZ  + baseSliceHht-(i+1)*stepBaseHt)/m
                    << " z_bot " << (posBaseVolZ - (baseSliceHht-(i+1)*stepBaseHt))/m 
                    << G4endl;
					
					cylSolid =  new G4SubtractionSolid("BaseSubCyl",baseCylFull, 
													   baseCylIn, rot, center);	
					parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
				}
				else if(i == numBaseVol-1)
				{
					parLog = new G4LogicalVolume(baseCylFull,0,pName+"Log");
					//lastVal = -1; //keep imp value same
				}
				
				parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Blue()));
				parallelPhys = new G4PVPlacement(rot, posBaseVol, parLog,
												 pName, parallelWorldLogical, false, 0);				
				SetImportanceValueForRegion(parallelPhys, 
                            std::pow(2.0, i+1+numMonoVol+1+numHdcVol+lastVal) );
			}//baseVol
			
			G4cout << "\n TOTAL PARALLEL VOLUMES : " << numMonoVol << " + " 
				  << numHdcVol <<  " + " << numBaseVol << " = " 
				  << numMonoVol + numHdcVol + numBaseVol << " \n"<< G4endl;
		}// end of addInnerHdc
		
		else //if not fitToShields/addInnerHdc
		{	
			for(G4int i=0; i<fNumVol+1; i++) 
			{
				std::ostringstream ss;
				ss << "parallelSNS"<< i;
				G4String pName = ss.str();
				
				G4SubtractionSolid* cylSolid = NULL;
				
				G4Tubs* cylFull = NULL;
				G4Tubs* cylInner = NULL;
				G4double drad = 1.*cm;
				G4cout << " Full:  rad1 " << (i*stepCylRad+offTargetAxis)/m 
				<<  " rad2 "<<(detToTargetRad+detToWallHor+fNumVol*drad-i*drad)/m  
				<< " ht "<<(parCylHalfHt-i*stepCylHt)/m  << G4endl;
				
				//G4UnionSolid* unionSolid = NULL;
				
				cylFull = new G4Tubs("BasementCyl", i*stepCylRad+ offTargetAxis, 
							detToTargetRad + detToWallHor+ fNumVol*drad-i*drad, 
							parCylHalfHt-i*stepCylHt, 0., twopi ); // 0, twopi);
				
				
				if(i < fNumVol)
				{		
					cylInner = new G4Tubs("CylIn",(i+1)*stepCylRad+offTargetAxis,
					detToTargetRad+ detToWallHor+ fNumVol*drad-(i+1)*drad, 
					parCylHalfHt-(i+1)*stepCylHt, 0., twopi); //0,twopi
					
					G4cout << " Inner:  rad1 " << ((i+1)*stepCylRad+offTargetAxis)/m 
					<<  " rad2 "<<(detToTargetRad+ detToWallHor+fNumVol*drad-(i+1)*drad)/m  
					<< " ht "<<(parCylHalfHt-(i+1)*stepCylHt)/m << G4endl;
					
					cylSolid =  new G4SubtractionSolid("SubCyl",cylFull, cylInner, rot, center);	
					parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
				}
				else if(i == fNumVol)
				{
					parLog = new G4LogicalVolume(cylFull,0,pName+"Log");
				}
				
				parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
				parallelPhys = new G4PVPlacement(rot, posParCyl, parLog,// NOTE change pos
												pName, parallelWorldLogical, false, 0);
							
			
				SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );//NOTE
				//fPhysicalVolumeVector.push_back(HgBox->GetPartLog());
				//parallelWorldLogical = parLog; // to nest the next volume in this
			}//for
		}//if ! fitToShield
	}//if shrinkouter
}	

//----------------------------------------------------------------------------

//void SNSParallelTargetDetector::ConstructSD()
//{}                        

//G4VIStore*
void SNSParallelTargetDetector::CreateImportanceStore()
{
	G4cout << "SNSParallelTargetDetector:: Creating Importance Store "<< G4endl;

	G4IStore *istore = G4IStore::GetInstance(GetName());
	
	G4GeometryCell parallelVolCell(GetConstWorldVolumeAddress(), 0);
	istore->AddImportanceGeometryCell(1, parallelVolCell);
	
   for(std::map<G4VPhysicalVolume*, std::pair<G4double, G4int>* >::iterator i =
      fImportanceMap.begin();i != fImportanceMap.end(); i++)
   {
      istore->AddImportanceGeometryCell((i->second)->first,
         *(i->first), (i->second)->second);

      G4cout <<" Volume added: "<< (i->first)->GetName() << " (Replica "
     <<(i->second)->second<< ") of imp value: "<< (i->second)->first <<G4endl;
   }
   // return istore;
}

//---------------------------------------------------------------------------//

void SNSParallelTargetDetector::SetImportanceValueForRegion(
  G4VPhysicalVolume* aVol,  G4double anImpValue,   G4int aRepNum)
{
   // sets importance value (default kMinImpValue)
   // anImpValue: 0 -> particle will be killed
   //           : > 0 -> allowed (higher means greater importance)
   //           : < 0 -> not allowed!

  if (anImpValue< 0) G4cout << "Error in importance value..."<< G4endl;
  else
  {
    std::map<G4VPhysicalVolume*, std::pair<G4double, G4int>* > :: const_iterator anIt;
    anIt = fImportanceMap.find(aVol);
    if(anIt == fImportanceMap.end())
    {
      if(anImpValue < kMinImpValue && anImpValue > 0.) anImpValue = kMinImpValue;
      std::pair<G4double, G4int>* aPair =
        new std::pair<G4double, G4int>(anImpValue, aRepNum);
      fImportanceMap.insert(
        std::pair<G4VPhysicalVolume*, std::pair<G4double, G4int>* >(aVol,aPair));
      G4cout << "Set " << aVol->GetName() << " (: " << aRepNum
        << ") of importance: " << anImpValue << G4endl;
    }
    else
    {
      G4cout << "WARNING: Importance numberfor: " << aVol->GetName()
        << " had been set" << G4endl;
    }
  }
}

//---------------------------------------------------------------------

void SNSParallelTargetDetector::SetNumVolumes( G4int numVol)
{ 
	fNumVol = numVol;
	//G4RunManager::GetRunManager()->ReinitializeGeometry();
}
//-------------------------------------------------------------------
void SNSParallelTargetDetector::SetIsVariableStep ( G4bool val)
{ 
	fIsVariableStep = val;
	//G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//-----------------------------------------------------------------------

		/*
		else if (shape == "box")
		{
			G4SubtractionSolid* boxSub = NULL;
			G4Box* boxFull = NULL;
			B4Box* boxIn   = NULL;
			G4cout << " Full:  rad1 " << (i*stepCylRad+offTargetAxis)/m 
			<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-i*stepCylRad)/m  
			<< " ht "<<(parCylHalfHt-i*stepCylHt)/m << G4endl;
			
			
			boxFull = new G4Tubs("BasementCyl", i*stepCylRad+ offTargetAxis, 
								 2.*detToTargetRad- offTargetAxis-i*stepCylRad, 
						parCylHalfHt-i*stepCylHt, 0, twopi);
			if(i < fNumVol)
			{		
				cylInner = new G4Tubs("CylIn",(i+1)*stepCylRad+offTargetAxis,
				2.*detToTargetRad- offTargetAxis-(i+1)*stepCylRad, 
				parCylHalfHt-(i+1)*stepCylHt, 0, twopi);
				
				G4cout << " Inner:  rad1 " << ((i+1)*stepCylRad+offTargetAxis)/m 
				<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-(i+1)*stepCylRad)/m  
				<< " ht "<<(parCylHalfHt-(i+1)*stepCylHt)/m << G4endl;
				
				cylSolid =  new G4SubtractionSolid("SubCyl",cylFull, cylInner, rot, center);
				parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
			}
			else if(i == fNumVol)
			{
				parLog = new G4LogicalVolume(cylFull,0,pName+"Log");
			}
			parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));
			parallelPhys = new G4PVPlacement(rot, posParBox, parLog, 
										pName, parallelWorldLogical, false, 0);
		}*/

    /*
    //G4double deltaFraction         = 0.2; //for variable step 
    //fNumVol/2*(2*stepCylRadVar+ (fNumVol-1)*delta) = detToTargetRad-detToWallHor;
    //stepCylRadVar+ 0.5*(fNumVol-1)*(deltaFraction*stepCylRadVar) = (detToTargetRad-detToWallHor)/fNumVol;
    //stepCylRadVar(1+ 0.5*(fNumVol-1)*deltaFraction) = (detToTargetRad-detToWallHor)/fNumVol;  
    //G4double stepCylRadVar      = (detToTargetRad - detToWallHor)/fNumVol*1/(1+ 0.5*(fNumVol-1)*deltaFraction);
    if(fIsVariableStep)
    {
        if(i == fNumVol-1)
        {
            cylFull  = new G4Tubs("BasementCyl", fNumVol*stepCylRadVar*(1+ deltaFraction), 
                        fNumVol*stepCylRadVar*(1+ deltaFraction)+2.*detToWallHor, 
                        parCylHalfHt-i*stepCylHt, 0, twopi);
            G4cout << "last:rad1 " <<   fNumVol*stepCylRadVar*(1+ deltaFraction)/m << " rad2 "<< 
            (fNumVol*stepCylRadVar*(1+ deltaFraction)+2.*detToWallHor)/m << " ht "<< 
            (parCylHalfHt-i*stepCylHt)/m << G4endl;
        }
        else
        {
            cylFull = new G4Tubs("BasementCyl",  i*stepCylRadVar*(1+ deltaFraction), 
                        2.*detToTargetRad-i*stepCylRadVar*(1 +deltaFraction),
                        parCylHalfHt-i*stepCylHt, 0, twopi);//cylinder                    
        }
        cylInner = new G4Tubs("CylIn",(i+1)*stepCylRadVar*(1+ deltaFraction),
                        2.*detToTargetRad-(i+1)*stepCylRadVar*(1+ deltaFraction), 
                        parCylHalfHt-(i+1)*stepCylHt, 0, twopi);
        cylSolid =  new G4SubtractionSolid("SubCyl",cylFull, cylInner, rot, center);
        parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
    }//if variable step
    */

	/*
	
	if(fitToShields)
	{
		G4double baseRad    = mainVolRad;//22.3*m;
		G4double baseHalfHt = 4.4*m; // 7.8-2+3 /2
		G4double monoHalfHt = 4.*m;//8/2
		G4double monoRad    = 5.*m;
		G4double hdcHalfHt  = 1.75*m;//3.5/2
		G4double hdcRadMax  = 10.*m;		
		G4double posHdcZ	 = -0.25*m; // 1.5 - 1.75
		G4ThreeVector posParFit2Shields(0, 0, posHdcZ);	
		
		G4double stepMonoRad = (monoRad - offTargetAxis)/fNumVol;
		G4double stepHdcRad  = (hdcRadMax - offTargetAxis)/fNumVol;
		G4double stepBaseRad = (baseRad - offTargetAxis)/fNumVol;
		//G4double stepMonoHalfHt = (monoHalfHt - detToWallVert)/fNumVol;	
		G4double stepHdcHt = 1.*cm;
		
		G4UnionSolid* fullCyl = NULL;				
		G4double drad = 1.*cm;			
		
		for(G4int i=0; i<fNumVol; i++) 
		{
			std::ostringstream ss;
			ss << "parallelSNS"<< i;
			G4String pName = ss.str();
			
			//center is at target
			//G4double posHdc (0, 0, -(fNumVol-i)*stepHdcHt/2.);
			G4double baseAddHht =  (parCylHalfHt-2.*m - i*stepCylHt/2.)/2.;	
			G4double baseAddHhtIn =  (parCylHalfHt-2.*m -(i+1)*stepCylHt/2.)/2.;	
			G4ThreeVector posMonoAdd(0, 0, hdcHalfHt + monoHalfHt- i*stepCylHt/2.);	
			G4ThreeVector posMonoAddIn(0, 0, hdcHalfHt + monoHalfHt- (i+1)*stepCylHt/2.);
			G4ThreeVector posBaseAdd(0, 0, -baseAddHht + i*stepCylHt/2.);
			G4ThreeVector posBaseAddIn(0, 0, -baseAddHhtIn + (i+1)*stepCylHt/2.);
			G4ThreeVector posHdcCut(0,0, hdcHalfHt-(fNumVol-1-i)*stepHdcHt/2.);
			G4ThreeVector posBaseCut(0,0, -hdcHalfHt +(fNumVol-1-i)*stepHdcHt/2.);				
			G4double radMax = detToTargetRad + detToWallHor+ fNumVol*drad-i*drad;
			G4double radMaxIn = detToTargetRad + detToWallHor+ fNumVol*drad-(i+1)*drad;			
			
			G4Tubs* cylHdc = new G4Tubs("cylHdc", i*stepHdcRad+ offTargetAxis,
										radMax, hdcHalfHt-(fNumVol-1-i)*stepHdcHt, 0, twopi); 
			
			G4Tubs* cylMono = new G4Tubs("cylMono", i*stepMonoRad+ offTargetAxis, 
										 radMax, monoHalfHt- i*stepCylHt/2. , 0, twopi);
			G4UnionSolid* cylHdcAdd = new G4UnionSolid( "cylHdcAdd",
														cylHdc, cylMono, rot, posMonoAdd);
			
			G4Tubs* cylBase = new G4Tubs("cylBase", i*stepBaseRad+ offTargetAxis,
										 radMax, baseAddHht, 0, twopi);	
			
			if(i<fNumVol-1)
			{
				G4UnionSolid* cylBaseAdd  = new G4UnionSolid( "cylBaseAdd",
															  cylHdcAdd, cylBase, rot, posBaseAdd);
				G4Tubs* hdcCut = new G4Tubs("hdcCut", i*stepMonoRad+ offTargetAxis,
											radMax, (fNumVol-1-i)*stepHdcHt/2., 0, twopi);
				fullCyl = new G4UnionSolid("hdcCutAdd", cylBaseAdd, 
										   hdcCut, rot, posHdcCut);
			}
			else
			{
				fullCyl  = new G4UnionSolid( "cylBaseAdd",
											 cylHdcAdd, cylBase, rot, posBaseAdd);
			}
			
			if(i < fNumVol-1)
			{		
				
				G4Tubs* cylHdcIn = new G4Tubs("cylHdcIn", (i+1)*stepHdcRad+ 
				offTargetAxis, radMaxIn, hdcHalfHt-(fNumVol-1-i)*stepHdcHt, 0, twopi); 
				
				G4Tubs* cylMonoIn = new G4Tubs("cylMonoIn", (i+1)*stepMonoRad+  
				offTargetAxis, radMaxIn, monoHalfHt- i*stepCylHt/2., 0, twopi);
				G4UnionSolid* cylHdcAddIn = new G4UnionSolid( "cylHdcAddIn",
															  cylHdcIn, cylMonoIn, rot, posMonoAddIn);
				
				G4Tubs* cylBaseIn = new G4Tubs("cylBaseIn", (i+1)*stepBaseRad+ 
				offTargetAxis, radMaxIn, baseAddHht, 0, twopi);
				G4UnionSolid* cylBaseAddIn = new G4UnionSolid( "cylBaseAddIn",
															   cylHdcAddIn, cylBaseIn, rot, posBaseAddIn);	
				
				G4SubtractionSolid* cylSubSolid =  NULL;
				if(i<fNumVol-1)
				{
					G4Tubs* hdcCutIn = new G4Tubs("hdcCutIn", (i+1)*stepMonoRad+ 
					offTargetAxis, radMaxIn, (fNumVol-1-i)*stepHdcHt/2., 0, twopi);
					G4UnionSolid* hdcCutAddIn = new G4UnionSolid("hdcCutAddIn", 
																 cylBaseAddIn, hdcCutIn, rot, posHdcCut);
					cylSubSolid =  new G4SubtractionSolid("SubCylIn",
														  fullCyl, hdcCutAddIn, rot, center);							
					
				}
				else
					cylSubSolid =  new G4SubtractionSolid("SubCylIn",
														  fullCyl, cylBaseAddIn, rot, center);	
					
					parLog = new G4LogicalVolume(cylSubSolid,0,pName+"Log");
			}
			else if(i == fNumVol-1)
			{
				parLog = new G4LogicalVolume(fullCyl,0,pName+"Log");
			}
			
			if(i==0)parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
											 if(i==1)parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Green()));
											 if(i==2)parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Blue()));
											 if(i==3)parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Yellow()));
											 parallelPhys = new G4PVPlacement(rot, posParFit2Shields, parLog,// NOTE change pos
																			  pName, parallelWorldLogical, false, 0);
											 
											 
											 SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );//NOTE				
		}//for 			
	}//if fitToShields
	*/