// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#include "SNSParallelTargetDetector.hh"
#include "SNSParallelMessenger.hh"

#include <iomanip>
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
		
	//Don't change the following keyword in comment, used in script
	fNumVol 			    = 50; //NUMVOL
	

	
	if(fNumVol <1 ) 
	{
		G4cerr << "** parallel volumes WRONG *** " << G4endl;
		return;
	}
		
	//use exampleRE06.cc to set meesenger
	G4bool twoStackTracking	       	= false;
	G4bool twoStackCont		       	= false;
	G4bool skipPBeam				= false;
	G4bool pBeamLossBox				= false;
	G4bool pBeamLossCyl				= false;
	G4double detHalfHt 		    	= 0.5*m;	//including vert space
	G4double topStackThick      	= 1.*m;  // top to stack1 bottom
	G4double innerStackWidth    	= 1.*m;  // radial: stack2 start to max
	G4double mainVolRad		    	= 22.3*m;	
	const G4double beamHalfHt		= 10.*cm;
	G4double stepRatio          	= 1.; // stackOuter step size /stackInner step size
	const G4double offBeamZ 		= 1.5*m;	  //protBoxHalfXZ= 2*m;
	const G4double offTargetAxis	= 1.*m;
	G4double detToWallHor			= 1.*m;
	const G4double detToBaseBottom	= 4.*m;
	const G4double detToTargetFullZ = 7.6*m;
	G4double posParCylZ 			= -7.6*m;//detToTargetFullZ
	G4double yPosBeamLoss 			= -25.*m;
	

twoStackTracking    = false; //preference	

twoStackCont		= true;
	
pBeamLossBox		= false; 
pBeamLossCyl		= false;

yPosBeamLoss 		= -24.0*m; //-24*m;

mainVolRad		    = 18.7*m; //20m
innerStackWidth     = 15.*m;
stepRatio          	= 1.; // < 1 NOTE
detHalfHt 		    = 0.5*m;
topStackThick       = 0.5*m;
detToWallHor		= 0.5*m;
	
	
	G4bool twoStack					= false;
	if(twoStackTracking || twoStackCont) 
		twoStack = true;
	G4double parCylHalfHt 			= -posParCylZ + offBeamZ; //note -ve
	G4double detToTargetRad   		= mainVolRad + detToWallHor;	
	G4double stepRad 				= (mainVolRad - offTargetAxis)/fNumVol;
	G4double stepHt  				= (parCylHalfHt - detHalfHt)/fNumVol;
	G4ThreeVector posParCyl(0,0, posParCylZ); 	
	
	//if multiple stacks		
	G4double effectiveRad			= mainVolRad - offTargetAxis;
	G4double radDistStack1      	= stepRatio*(mainVolRad - offTargetAxis- innerStackWidth);
	G4double vertDistStack2     	= parCylHalfHt - topStackThick - detHalfHt;
	if(twoStackTracking) effectiveRad = radDistStack1 + vertDistStack2; //note the vertical part
	
	G4int numStackOuter         	= fNumVol * radDistStack1/effectiveRad;
	//fNumVol* (innerStackWidth *1/stepRatio)/( innerStackWidth *1/stepRatio + radDistStack1);	
	G4int numStackInner	        	= fNumVol - numStackOuter;
	G4double stepCylOutRad 			= 1./stepRatio*radDistStack1/numStackOuter;
	G4double stepCylInRad 			= innerStackWidth/numStackInner;
	G4double stepCylOutHt  			= topStackThick/numStackOuter; 	//6.*cm*45./fNumVol;
G4double stepCylInHt  			= vertDistStack2/numStackInner; 	
	
	
	G4ThreeVector center(0,0,0);
	G4PVPlacement* parallelPhys 	= NULL;
	G4LogicalVolume* parLog 		= NULL;	
	G4RotationMatrix* rot 	    	= new G4RotationMatrix(); 

	G4cout << "Outer+Inner: " << numStackOuter << " + " << numStackInner << " =? " << fNumVol << G4endl;		

	G4double drad 		= 6.*cm; 
	G4double offRadius	= 0;
	G4double offHt		= 0;	
	G4int	nn			= 0;
	
	if(! (pBeamLossBox || pBeamLossCyl) )
	{
		for(G4int i=0; i< fNumVol; i++) 
		{
			std::ostringstream ss;
			ss << "Par_"<< i;
			G4String pName 		= ss.str();
			
			G4Tubs* outerCylFull= NULL;

			
			if(twoStack)
			{
				if(i==0)
				{
					offRadius 	= offTargetAxis;
					offHt		= 0;
					stepRad   	= stepCylOutRad;
					stepHt	  	= stepCylOutHt;
				}
				else if(i == numStackOuter) //seting for the rest
				{
					nn			= 0;
					offRadius 	= offTargetAxis + numStackOuter*stepCylOutRad;
					offHt		= numStackOuter*stepCylOutHt;
					if(skipPBeam && offHt < parCylHalfHt+posParCylZ+beamHalfHt )
						offHt	= parCylHalfHt+posParCylZ+beamHalfHt;
					stepRad   	= stepCylInRad;
					stepHt	  	= stepCylInHt;
				}
			}
			
			
			G4cout << " CylFull:  rad1 " <<  (nn*stepRad+ offRadius)/m
			<<  " rad2 "<<  (detToTargetRad + detToWallHor + fNumVol*drad-i*drad)/m
			<< " ht:from "   << (posParCylZ-(parCylHalfHt - offHt -nn*stepHt) )/m
			<< " ht:to " << (posParCylZ+(parCylHalfHt- offHt -nn*stepHt) )/m
			<< " centZ: " << posParCylZ/m << G4endl;
			
			
			outerCylFull = new G4Tubs("OuterCylfull", nn*stepRad+ offRadius, 
								detToTargetRad + detToWallHor + fNumVol*drad-i*drad, 
								parCylHalfHt-offHt -nn*stepHt, 0, twopi);
			
			
			if(i < fNumVol-1)
			{	
				G4Tubs* outerCylIn = new G4Tubs("OuterCylIn", (nn+1)*stepRad+ offRadius, 
									detToTargetRad + detToWallHor + fNumVol*drad-(i+1)*drad, 
									parCylHalfHt-offHt -(nn+1)*stepHt, 0, twopi);
				
				G4cout << " CylIn:  rad1 " << ( (nn+1)*stepRad+ offRadius)/m
				<<  " rad2 "<<  (detToTargetRad + detToWallHor + fNumVol*drad-(i+1)*drad)/m
				<< " ht:from "   << (posParCylZ-(parCylHalfHt-offHt-(nn+1)*stepHt) )/m
				<< " ht:to " << (posParCylZ+(parCylHalfHt-offHt-(nn+1)*stepHt))/m << G4endl;
				
				G4SubtractionSolid* cylOutSolid =  new G4SubtractionSolid(
					"SubCyl",outerCylFull, outerCylIn, rot, center);	
				parLog = new G4LogicalVolume(cylOutSolid,0,pName+"Log");
			}
			else // if(i == fNumVol-1)
			{
				parLog = new G4LogicalVolume(outerCylFull,0,pName+"Log");
			}
			
			nn++;
			
			parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
			parallelPhys = new G4PVPlacement(rot, posParCyl, parLog,
											pName, parallelWorldLogical, false, 0);
			
			
			SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );
			
			//fPhysicalVolumeVector.push_back(HgBox->GetPartLog());
			//parallelWorldLogical = parLog; // to nest the next volume in this
		}//for
	} // regular
	
	else if(pBeamLossCyl) // parallel cyl centered at 25m
	{
		offRadius 		= 5.*cm;
		mainVolRad 		= 31.*m;
		detToTargetRad  = mainVolRad + detToWallHor;	
		stepRad 		= (mainVolRad - offTargetAxis)/fNumVol;
		G4ThreeVector posParCylBLoss(0, yPosBeamLoss, posParCylZ); 	
		
		for(G4int i=0; i< fNumVol; i++) 
		{
			std::ostringstream ss;
			ss << "Par_"<< i;
			G4String pName 		= ss.str();
			
			G4Tubs* outerCylFull= NULL;
			
			G4cout << " CylFull:  rad1 " <<  (i*stepRad+ offRadius)/m
			<<  " rad2 "<<  (detToTargetRad + detToWallHor + fNumVol*drad-i*drad)/m
			<< " ht:from "   << (posParCylZ-(parCylHalfHt - offHt -i*stepHt) )/m
			<< " ht:to " << (posParCylZ+(parCylHalfHt- offHt -i*stepHt) )/m
			<< " centZ: " << posParCylZ/m << G4endl;
			
			
			outerCylFull = new G4Tubs("OuterCylfull", i*stepRad+ offRadius, 
									  detToTargetRad + detToWallHor + fNumVol*drad-i*drad, 
							 parCylHalfHt-offHt -i*stepHt, 0, twopi);
			
			
			if(i < fNumVol-1)
			{	
				G4Tubs* outerCylIn = new G4Tubs("OuterCylIn", (i+1)*stepRad+ offRadius, 
												detToTargetRad + detToWallHor + fNumVol*drad-(i+1)*drad, 
												parCylHalfHt-offHt -(i+1)*stepHt, 0, twopi);
				
				G4cout << " CylIn:  rad1 " << ( (i+1)*stepRad+ offRadius)/m
				<<  " rad2 "<<  (detToTargetRad + detToWallHor + fNumVol*drad-(i+1)*drad)/m
				<< " ht:from "   << (posParCylZ-(parCylHalfHt-offHt-(i+1)*stepHt) )/m
				<< " ht:to " << (posParCylZ+(parCylHalfHt-offHt-(i+1)*stepHt))/m << G4endl;
				
				G4SubtractionSolid* cylOutSolid =  new G4SubtractionSolid(
					"SubCyl",outerCylFull, outerCylIn, rot, center);	
				parLog = new G4LogicalVolume(cylOutSolid,0,pName+"Log");
			}
			else // if(i == fNumVol-1)
			{
				parLog = new G4LogicalVolume(outerCylFull,0,pName+"Log");
			}
			
			parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
			parallelPhys = new G4PVPlacement(rot, posParCylBLoss, parLog,
											 pName, parallelWorldLogical, false, 0);
			
			SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );
		}//for
	} // pBeamLossCyl
	
	else if (pBeamLossBox) // parallel box volumes
	{
		for(G4int i=0; i< fNumVol; i++) 
		{

	const G4double offBeam				= 10.*cm;				
			
			const G4double raiseParZ 	= 1.*m;
			const G4double detToWallVert= 0.5*m;
			const G4double facMainVolY	= 1.5; //multiple of mainVolRad in y dir
			
			//parCylHalfHt 				= 8.5*m;//- offBeam/2.;				
			
			std::ostringstream ss;
			ss << "Par_"<< i;
			G4String pName 				= ss.str();
			
			G4Box* outerBox				= NULL;

			const G4double stepX		= (mainVolRad - offBeam)/fNumVol;
			const G4double stepY		= drad;
			const G4double stepZ		= (raiseParZ +detToTargetFullZ - detToWallVert )/fNumVol;		
			const G4double stepZBot		= (detToBaseBottom - detToWallVert )/fNumVol;	
			
			const G4double boxXFull	= mainVolRad - offBeam - i*stepX + 2.*detToWallHor + fNumVol*drad - i*drad;
			const G4double boxYFull	= facMainVolY*mainVolRad - 2.*i*stepY;
			const G4double boxZFull	= raiseParZ +detToTargetFullZ + detToBaseBottom - i*stepZ - i*stepZBot;
								
			outerBox = new G4Box("OuterBox", boxXFull/2., boxYFull/2.,boxZFull/2.);
			
			const G4double parXpos	= boxXFull/2. + offBeam + i*stepX;		
			const G4double parYpos	= (facMainVolY/2. - 1)*mainVolRad;			
			const G4double parZPos	= raiseParZ -boxZFull/2. - i*stepZ;
			G4ThreeVector posParBox(parXpos, parYpos, parZPos);
			
			int wid = 6;
			
			G4cout 	<< "Outer:: i: " << i << " Len " << std::setw(wid) << boxXFull/m   
					<< "  X: "	<< std::setw(wid) << (parXpos -boxXFull/2.)/m << " : "
					<< std::setw(wid) << (parXpos +boxXFull/2.)/m
					<< " || Len " << std::setw(wid) << boxYFull/m   
					<< ":: Y: " << std::setw(wid) << (parYpos -boxYFull/2.)/m << " : " 
					<< std::setw(wid) << (parYpos +boxYFull/2.)/m
					<< " || Len " << std::setw(wid) << boxZFull/m   
					<< ":: Z: " << std::setw(wid) << (parZPos -boxZFull/2.)/m << " : " 
					<< std::setw(wid) << (parZPos +boxZFull/2.)/m << G4endl;
			
			if(i < fNumVol-1)
			{	
				
				const G4double boxXInFull	= mainVolRad - offBeam - (i+1)*stepX 
									+ 2.*detToWallHor + fNumVol*drad - (i+1)*drad;
				const G4double boxYInFull	= facMainVolY*mainVolRad - 2.*(i+1)*stepY;
				const G4double boxZInFull	= raiseParZ + detToTargetFullZ 
									+ detToBaseBottom - (i+1)*stepZ - (i+1)*stepZBot;
				
				G4Box* innerBox = new G4Box("InnerBox", boxXInFull/2., boxYInFull/2.,boxZInFull/2.);
				
				const G4double parXInpos	= boxXInFull/2. + offBeam + (i+1)*stepX;			
				const G4double parYInpos	= (facMainVolY/2. - 1)*mainVolRad;			
				const G4double parZInPos	= raiseParZ -boxZInFull/2. - (i+1)*stepZ;
				
				G4cout 	
					<< "Inner:: i: " 	<< i << " Len " << std::setw(wid) << boxXInFull/m
					<<"  X: " << std::setw(wid) << (parXInpos -boxXInFull/2.)/m << " : " 
					<< std::setw(wid)<< (parXInpos +boxXInFull/2.)/m
					<< i << " || Len " << std::setw(wid) << boxYInFull/m
					<< ":: Y: " << std::setw(wid) << (parYInpos -boxYInFull/2.)/m << " : " 
					<< std::setw(wid) << (parYInpos +boxYInFull/2.)/m
					<< i << " || Len " << std::setw(wid) << boxZInFull/m
					<< ":: Z: " << std::setw(wid) << (parZInPos -boxZInFull/2.)/m << " : " 
					<< std::setw(wid) << (parZInPos +boxZInFull/2.)/m << G4endl;
				
				G4SubtractionSolid* subBox =  new G4SubtractionSolid(
										"SubBox",outerBox, innerBox, rot, center);	
				parLog = new G4LogicalVolume(subBox,0,pName+"Log");
			}
			else // if(i == fNumVol-1)
			{
				parLog = new G4LogicalVolume(outerBox,0,pName+"Log");
			}
			
			nn++;
			
			parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
			parallelPhys = new G4PVPlacement(rot, posParBox, parLog,
											 pName, parallelWorldLogical, false, 0);
			
			
			SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );
		}//for
		
	}// if pBeamLoss
	
	
	/*
	if(testDetector1)
	{
		G4double mainVolRadius     = 4.9*m;
		G4double offTargetAx  = 0.*cm;
		G4double stepCylRad2         = (mainVolRadius - offTargetAx)/fNumVol;
		G4double drad2               = 3.*cm;
		G4double detToWallHor2       = 15.*cm;		
		G4double detToTargetRad2     = mainVolRadius+detToWallHor2;
		
		G4PVPlacement* parallelPhysical = NULL;
		G4LogicalVolume* parLogical     = NULL; 
		
		
		for(G4int i=0; i<fNumVol+1; i++) 
		{
			std::ostringstream ss;
			ss << "paralleTest"<< i;
			G4String pName = ss.str();
			G4SubtractionSolid* sphSolid = NULL;
			
			G4Sphere* sphFull = NULL;
			G4Sphere* sphInner = NULL;
			
			//drad2                      
			sphFull = new G4Sphere("testSph", i*stepCylRad2+ offTargetAx, 
								   detToTargetRad2 + detToWallHor2+ fNumVol*drad2-i*drad2, 0., twopi, 0., pi); 
			G4cout << " Full:  rad1 " << (i*stepCylRad2+offTargetAx)/m 
			<<  " rad2 "<<(detToTargetRad2 + detToWallHor2+ fNumVol*drad2-i*drad2)/m  
			<< G4endl;             
			if(i < fNumVol)
			{   
				
				sphInner = new G4Sphere("sphIn",(i+1)*stepCylRad2+offTargetAx,
										detToTargetRad2+ detToWallHor2+ fNumVol*drad2-(i+1)*drad2,0., twopi, 0., pi); 
				G4cout << " Full:  rad1 " << ((i+1)*stepCylRad2+offTargetAx)/m 
				<<  " rad2 "<<(detToTargetRad2+ detToWallHor2+ fNumVol*drad2-(i+1)*drad2)/m  
				<< G4endl;  
				sphSolid =  new G4SubtractionSolid("SubCyl",sphFull, sphInner, rot, center);    
				parLogical = new G4LogicalVolume(sphSolid,0,pName+"Log");
			}   
			else if(i == fNumVol)
			{
				parLogical = new G4LogicalVolume(sphFull,0,pName+"Log");
			}
			parallelPhysical = new G4PVPlacement(rot, center, parLogical,// NOTE change pos
												 pName, parallelWorldLogical, false, 0);
			parLogical->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
			SetImportanceValueForRegion(parallelPhysical, std::pow(2.0, i+1.0) );//NOTE
			
		}
		return;
	}//end testDetector1
	*/
	
	
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
