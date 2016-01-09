

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
	

	
	//Don't change the following keyword in comment, used in script
	fNumVol 					= 65; //NUMVOL
	G4bool basementSection 		= false;
	
	//use exampleRE06.cc to set meesenger

	G4double baseHalfHt 		= 7.*m; //	
	G4double basementRad		= 22.3*m;	

    G4double offBeamZ 			= 0.5*m;	  //protBoxHalfXZ= 2*m;
	G4double offTargetAxis		= 1.*m;
	G4double detToWallGap		= 1.5*m;
	G4double detToTargetRad   	= basementRad + detToWallGap;	
	
	G4double stepCylRad 		= (basementRad - offTargetAxis)/fNumVol-1;
	G4double stepCylHt  		= (baseHalfHt - detToWallGap)/fNumVol-1; 	
	
	G4ThreeVector posBaseCyl(0,0, -baseHalfHt -offBeamZ); //cylinder  
	G4ThreeVector posParBox(18.2*m,-5.*m,-baseHalfHt -offBeamZ);
	
	G4ThreeVector center(0,0,0);
	G4PVPlacement* parallelPhys = NULL;
	G4LogicalVolume* parLog 	= NULL;	
	G4RotationMatrix* rot 	    = new G4RotationMatrix(); 

	G4double dA = 15.*degree/fNumVol; 
	G4double theta1 = 0, theta2 = 0, theta3 = 0, theta4 = 0;	
	
	G4String shape = "cylinder";//"box" 
	
	theta1 = theta3 = 0;
	theta2 = theta4 = twopi;
	
	for(G4int i=0; i<fNumVol+1; i++) 
    {
        std::ostringstream ss;
		ss << "parallelSNS"<< i;
        G4String pName = ss.str();
		
		if(shape == "cylinder") //cylinder
		{
            G4SubtractionSolid* cylSolid = NULL;

            G4Tubs* cylFull = NULL;
			G4Tubs* cylInner = NULL;
			
			if(basementSection)
			{
				theta1 = 3./2.*pi+ i*dA;
				theta2 = pi/2.-2.*i*dA;
				theta3 = 3./2.*pi+ (i+1)*dA;
				theta4 = pi/2.-2.*(i+1)*dA;
			}
            G4cout << " Full:  rad1 " << (i*stepCylRad+offTargetAxis)/m 
					<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-i*stepCylRad)/m  
					<< " ht "<<(baseHalfHt-i*stepCylHt)/m  << G4endl;
            
            
            cylFull = new G4Tubs("BasementCyl", i*stepCylRad+ offTargetAxis, 
                    2.*detToTargetRad- offTargetAxis-i*stepCylRad, 
                    baseHalfHt-i*stepCylHt, theta1, theta2 ); // 0, twopi);
            if(i < fNumVol)
            {		
                cylInner = new G4Tubs("CylIn",(i+1)*stepCylRad+offTargetAxis,
                    2.*detToTargetRad- offTargetAxis-(i+1)*stepCylRad, 
                    baseHalfHt-(i+1)*stepCylHt, theta3, theta4); //0,twopi
                
                G4cout << " Inner:  rad1 " << ((i+1)*stepCylRad+offTargetAxis)/m 
						<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-(i+1)*stepCylRad)/m  
						<< " ht "<<(baseHalfHt-(i+1)*stepCylHt)/m << G4endl;
            
                cylSolid =  new G4SubtractionSolid("SubCyl",cylFull, cylInner, rot, center);	
                parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
            }
            else if(i == fNumVol)
            {
                parLog = new G4LogicalVolume(cylFull,0,pName+"Log");
            }

			parLog->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));
			parallelPhys = new G4PVPlacement(rot, posBaseCyl, parLog,// NOTE change pos
											 pName, parallelWorldLogical, false, 0);
		
		}/*
		else if (shape == "box")
		{
			G4SubtractionSolid* boxSub = NULL;
			G4Box* boxFull = NULL;
			B4Box* boxIn   = NULL;
			G4cout << " Full:  rad1 " << (i*stepCylRad+offTargetAxis)/m 
			<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-i*stepCylRad)/m  
			<< " ht "<<(baseHalfHt-i*stepCylHt)/m << G4endl;
			
			
			boxFull = new G4Tubs("BasementCyl", i*stepCylRad+ offTargetAxis, 
								 2.*detToTargetRad- offTargetAxis-i*stepCylRad, 
						baseHalfHt-i*stepCylHt, 0, twopi);
			if(i < fNumVol)
			{		
				cylInner = new G4Tubs("CylIn",(i+1)*stepCylRad+offTargetAxis,
				2.*detToTargetRad- offTargetAxis-(i+1)*stepCylRad, 
				baseHalfHt-(i+1)*stepCylHt, 0, twopi);
				
				G4cout << " Inner:  rad1 " << ((i+1)*stepCylRad+offTargetAxis)/m 
				<<  " rad2 "<<(2.*detToTargetRad-offTargetAxis-(i+1)*stepCylRad)/m  
				<< " ht "<<(baseHalfHt-(i+1)*stepCylHt)/m << G4endl;
				
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

        SetImportanceValueForRegion(parallelPhys, std::pow(2.0, i+1.0) );//NOTE
        //fPhysicalVolumeVector.push_back(HgBox->GetPartLog());
        //parallelWorldLogical = parLog; // to nest the next volume in this
    }
}	


    /*
    //G4double deltaFraction         = 0.2; //for variable step 
    //fNumVol/2*(2*stepCylRadVar+ (fNumVol-1)*delta) = detToTargetRad-detToWallGap;
    //stepCylRadVar+ 0.5*(fNumVol-1)*(deltaFraction*stepCylRadVar) = (detToTargetRad-detToWallGap)/fNumVol;
    //stepCylRadVar(1+ 0.5*(fNumVol-1)*deltaFraction) = (detToTargetRad-detToWallGap)/fNumVol;  
    //G4double stepCylRadVar      = (detToTargetRad - detToWallGap)/fNumVol*1/(1+ 0.5*(fNumVol-1)*deltaFraction);
    if(fIsVariableStep)
    {
        if(i == fNumVol-1)
        {
            cylFull  = new G4Tubs("BasementCyl", fNumVol*stepCylRadVar*(1+ deltaFraction), 
                        fNumVol*stepCylRadVar*(1+ deltaFraction)+2.*detToWallGap, 
                        baseHalfHt-i*stepCylHt, 0, twopi);
            G4cout << "last:rad1 " <<   fNumVol*stepCylRadVar*(1+ deltaFraction)/m << " rad2 "<< 
            (fNumVol*stepCylRadVar*(1+ deltaFraction)+2.*detToWallGap)/m << " ht "<< 
            (baseHalfHt-i*stepCylHt)/m << G4endl;
        }
        else
        {
            cylFull = new G4Tubs("BasementCyl",  i*stepCylRadVar*(1+ deltaFraction), 
                        2.*detToTargetRad-i*stepCylRadVar*(1 +deltaFraction),
                        baseHalfHt-i*stepCylHt, 0, twopi);//cylinder                    
        }
        cylInner = new G4Tubs("CylIn",(i+1)*stepCylRadVar*(1+ deltaFraction),
                        2.*detToTargetRad-(i+1)*stepCylRadVar*(1+ deltaFraction), 
                        baseHalfHt-(i+1)*stepCylHt, 0, twopi);
        cylSolid =  new G4SubtractionSolid("SubCyl",cylFull, cylInner, rot, center);
        parLog = new G4LogicalVolume(cylSolid,0,pName+"Log");
    }//if variable step
    */


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


