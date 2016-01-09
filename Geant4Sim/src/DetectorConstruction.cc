// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "Materials.hh"
#include "SNSSD.hh"
//#include "SNSTargetSD.hh"
//#include "CosISD.hh"
//#include "CosIDetector.hh"
#include "SNSDetector.hh"
#include "SNSTargetDetector.hh"
//#include "SimpleSNSDet.hh"
//#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh" //
#include "G4NistManager.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh" 
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh" 
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4VExceptionHandler.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4UserLimits.hh"
#include "G4GeometrySampler.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4UnitsTable.hh"


DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
  , fTheDetector(NULL), fSDdetector(NULL), fLogVol(NULL), fWorldPhysical(NULL)
  , fDetName("detector"), fCheckOverlaps(true), fHalfLen(0.), fGeometrySampler(NULL)
{
    fDetMessenger = new DetectorMessenger(this);
	new Materials;
	new G4UnitDefinition ( "inch", "inch", "Length", 2.54*cm );
    //G4UnitDefinition::BuildUnitsTable(); 

}


DetectorConstruction::~DetectorConstruction()
{
	delete fTheDetector;
	delete fDetMessenger;
}
	



G4VPhysicalVolume* DetectorConstruction::Construct()
{
	
//TODO: messenger and default constructor ?
	
	//fTheDetector = new SimpleSNSDet("SimpleSNSDet");  
	fTheDetector = new SNSTargetDetector("SNSTargetDetector");

	//fDetName = "SimpleSNSDet";
	fDetName = "SNSTargetDetector";
	
    fHalfLen = 36.*m;  
	
	//fTheDetector = new SNSDetector("SNSDet");
    //fDetName = "SNSDet";

	if( !fTheDetector )
	{
		G4Exception("DetectorConstruction::Construct()",
					"", FatalException,  "Detector Not Set");
		return 0;
	}
	else
	{
		
        if (fWorldPhysical)
        {
            G4GeometryManager::GetInstance()->OpenGeometry();
            G4PhysicalVolumeStore::GetInstance()->Clean();
            G4LogicalVolumeStore::GetInstance()->Clean();
            G4SolidStore::GetInstance()->Clean();
        }
        
            
      //G4GeometryManager:SD:GetInstance()->SetWorldMaximumExtent(worldLength);
      
	    G4NistManager* nist = G4NistManager::Instance();
	
	    //World
		G4Box* worldBox = new G4Box("World",fHalfLen, fHalfLen, fHalfLen);
	    G4Material *worldMat = nist->FindOrBuildMaterial("G4_Galactic");
	    //G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
	    G4LogicalVolume* worldLog
	    				= new G4LogicalVolume(worldBox, worldMat, "World");
	 
		fWorldPhysical = new G4PVPlacement(0, G4ThreeVector(), "World",
							worldLog, 0, false, 0); 

        //(re)construct detector
		fTheDetector->ConstructDet();
		G4ThreeVector fDetTrans = G4ThreeVector(0.,0.,0.);
		new G4PVPlacement(0, fDetTrans, fTheDetector->GetDetName(), 
            fTheDetector->GetDetLog(), fWorldPhysical, false, 0);
            
		
		//set max time for neutrons ??????????????
		G4double maxTime = 10*ms;
		G4PhysicalVolumeStore* physVolStore = G4PhysicalVolumeStore::GetInstance();
		for(size_t i=0; i<physVolStore->size(); i++) {
			G4VPhysicalVolume* vol = (*physVolStore)[i];   //vol->GetName()
			vol->GetLogicalVolume()->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,maxTime));
		}
		
		
		return fWorldPhysical;
	}

}


//Invoked for each thread. SD objects should be thread-local
void DetectorConstruction::ConstructSDandField()
{
    if(! fTheDetector) //if detOpenGeometryector
    {
            G4Exception("DetectorConstruction::ConstructSDandField()",
            "", FatalException,  "Detector Not Found");
    }
    
  
  //todo : handle multiple SD volumes,loop over see below
   	//G4String fSDname = fTheDetector->GetSDLogVolName();
   	//G4String hitsCollectionName = fTheDetector->GetSDHitscollectionName();
    
	G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
    
    if ( fDetName == "SNSDet" )
    {
        fSDdetector = new SNSSD("/SNS/Scintillator", "ScintCollection");
        fLogVol     = store->GetVolume( "ScintillatorLogical", false);
        SetSensitiveDetector( fLogVol, fSDdetector);
        G4cout << "SD set as SNSSD " << G4endl; 
    }
    /*
    else if ( fDetName == "SNSTargetDetector" )
    {
        fSDdetector = new SNSTargetSD("/SNS/TargetDetector", "SNSTargetCollection");
        fLogVol     = store->GetVolume( "DetectorSphereLog", false); 
        SetSensitiveDetector( fLogVol, fSDdetector);
        G4cout << "SD set as SNSTargetDetector" << G4endl; 
    }
    */

    /*
    G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
	 for(G4LogicalVolumeStore::iterator pos=store->begin(); pos!=store->end(); pos++)
	   {
		     if((*pos)->GetName()==logVolName)
			 G4LogicalVolume lVol = *pos;
	*/
	/*G4LogicalVolume* lVol = store->GetVolume( "ScintillatorLogical", false); 	
	if ( lVol && fDetName == "SNSDet" ) // if (vol->IsSensitive())
	{
		SNSSD* detectorSD = new SNSSD("/SNS/Scintillator", "ScintCollection");
		//Register the sensitive detector to G4SDManager
		SetSensitiveDetector( lVol, detectorSD );
		
		G4cout << "SD set as SNSSD " << G4endl; 
	}
	*/	/*
    	if(fDetName == "CosIDet")//todo:get logVol for all SDs.
    	{
			CosISD* detectorSD = new CosISD("CosI/Scintillator", 
									"ScintCollection");
			//Register the sensitive detector to G4SDManager
			SetSensitiveDetector( fSDLogVol, detectorSD );
		}*/
        /*for (vector<G4LogicalVolume*>::iterator iter = store->begin();
        iter != store->end(); i_volume++)  {
        G4LogicalVolume* lVolume = *iter;
        lVolume->GetName() == "ScintillatorLogical"
        }*/
	
}

void DetectorConstruction::SetDetector(const G4String& detName)
{
	//G4GeometryManager::GetInstance()->OpenGeometry();

	fDetName = detName;
	//if(! fTheDetector)
	//{
		//if(detName == "CosIDet")
		//	fTheDetector = new CosIDetector(detName);
		//else 
		if(detName == "SNSDet")
			fTheDetector = new SNSDetector("SNSDet");
        else if(detName == "SNSTargetDetector")
            fTheDetector = new SNSTargetDetector("SNSTargetDetector");
		else
			G4Exception("DetectorConstruction::SetDetector()",
						"", FatalException,  "Detector Name not found");
	//}
			
	if(fTheDetector) G4cout << "setup Detector: " <<detName << G4endl;
    //G4GeometryManager::GetInstance()->CloseGeometry();
	//G4RunManager::GetRunManager()->SetUserInitialization(detector);
	//G4RunManager::GetRunManager()->GeometryHasBeenModified();
    //G4RunManager::GetRunManager()->PhysicsHasBeenChanged();
	//G4RunManager::GetRunManager()->ReinitializeGeometry();
}



void DetectorConstruction::SetImportanceSampling()
{
    if(! fWorldPhysical)
    {
       G4Exception("DetectorConstruction::SetImportanceSampling()",
                        "", FatalException,  "PhysicalWorld not found");
    }

    static int samplingBegin = 0;
    if(samplingBegin ==1) fGeometrySampler->ClearSampling();

    //fGeometrySampler = new G4GeometrySampler(fParallelWorld->GetWorldVolume(),
}

/*

    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    if(fSDdetector)
    {
     delete fSDdetector;
    }
    fSDdetector = new SNSDetector("domPMT", this);
    sdManager->AddNewDetector(fSDdetector);
    fLogVol->SetSensitiveDetector(fSDdetector);
    
    
    void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}








    G4Material *DetectorConstruction::SetMaterial(G4VPhysicalVolume *physVol,G4String materialName)
{

 if(!physVol){
   G4cerr << "Universe has not yet been constructed." << G4endl;
   return 0;
 }

  // search the material by its name 
  G4Material* pttoMaterial = nist->FindOrBuildMaterial(materialName);  
  G4LogicalVolume *pLogVol = physVol->GetLogicalVolume();
  G4String oldMaterial=pLogVol->GetMaterial()->GetName();
  if (pttoMaterial){    
    pLogVol->SetMaterial(pttoMaterial); 
    G4cout << "\n----> The material for " << pLogVol->GetName() 
       << " was changed from " << oldMaterial << " to " << materialName 
       << G4endl;

    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    G4RunManager::GetRunManager()->GeometryHasBeenModified(); //UpdateGeometry();
  } 
  else{
    pttoMaterial=nist->FindOrBuildMaterial(oldMaterial); //Returns old material, no changes made
  }  
  
  return pttoMaterial;
}

    
    
 void DetectorConstruction::SetMaterial(G4String materialChoice)
 {
	// G4GeometryManager::GetInstance() -> OpenGeometry();
	 
	 G4Material* pttoMaterial =
	 G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);   
	 
	 if (pttoMaterial) { 
		 if(fMaterial != pttoMaterial) {
			 fMaterial = pttoMaterial;
			 if(fLBox) { fLBox->SetMaterial(pttoMaterial); }
			 G4RunManager::GetRunManager()->PhysicsHasBeenModified();
			 //G4RunManager::GetRunManager() -> GeometryHasBeenModified();
 }
 } else {
	 G4cout << "\n--> warning from DetectorConstruction::SetMaterial : "
	 << materialChoice << " not found" << G4endl;
 }              
 }
 
void DetectorConstruction::SetSize(G4double value)
{
	fBoxSize = value;
	G4RunManager::GetRunManager()->ReinitializeGeometry();
}
*/