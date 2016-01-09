// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "SNSTargetSD.hh"
#include "SNSHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4VProcess.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VAnalysisManager.hh"
#include "G4LossTableManager.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"

//#include "TSpline.h"
//#include "TGraph.h"
#include "TMath.h"

#include "G4ParticleTable.hh"
#include <iomanip>
#include <iostream>
#include <vector>
#include "Analysis.hh"
#include "G4ios.hh"         

G4int SNSTargetSD::fTargetHCid = -1; //static
    
//------------------------------------------------------------------------   

SNSTargetSD::SNSTargetSD( const G4String& sdName, const G4String& hitsCollectionName )
: G4VSensitiveDetector(sdName),
fHitsCollection(NULL), fVerbose(0)
{
	collectionName.insert(hitsCollectionName);
	//NOTE: don't get the collection ID here, but do it in Initialize()
    
}

//------------------------------------------------------------------------

SNSTargetSD::~SNSTargetSD() 
{ 
}

//------------------------------------------------------------------------

void SNSTargetSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection 
      = new SNSHitsCollection(SensitiveDetectorName, collectionName[0]); 
	
    if(fTargetHCid < 0)
    {    
        fTargetHCid = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        // TODO: handle collectionName order, if multiple SD are there
    }
    hce->AddHitsCollection( fTargetHCid, fHitsCollection );     
}

//-----------------------------------------------------------------------
//only SD vol hits
G4bool SNSTargetSD::ProcessHits( G4Step* aStep, G4TouchableHistory* )
{  
	G4Track *track = aStep->GetTrack();

fVerbose = 0;

	G4double eDep = (aStep->GetTotalEnergyDeposit()
					- aStep->GetNonIonizingEnergyDeposit() )/MeV;
	if ( eDep == 0 ) return false;


	G4ParticleDefinition* particleType = track->GetDefinition();
	G4String particleName 	=  particleType->GetParticleName();
	G4double partKE			=  track->GetKineticEnergy();//track->GetDynamicParticle()->GetKineticEnergy();
	//G4double partKE		=  aStep->GetPreStepPoint()->GetKineticEnergy();
    //G4double charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
	// post-step point = for currentstep 
	G4StepPoint* preStep = aStep->GetPreStepPoint();
	G4StepPoint* postStep = aStep->GetPostStepPoint();
	const G4VProcess *postProcStep = postStep->GetProcessDefinedStep();
	G4double secTotKE = 0.;	
	G4String postProc = "";
	G4String preProc = "";
	G4String trackCreatorProc = "";
	
	if(postProcStep)
	{
		postProc = postProcStep->GetProcessName();
		if(fVerbose>2) G4cout << " SD:postprcess: " << postProc << G4endl;
	} else 	{ if(fVerbose>2) G4cout << " SD:postprcess: NONE "; 	}
	
	if(preStep->GetProcessDefinedStep())
	{
		preProc = preStep->GetProcessDefinedStep()->GetProcessName();
		if(fVerbose>2) G4cout << " SD:prestep.proc:" << preProc << " ";		
	}else {if(fVerbose>2) G4cout << " SD:prestep.proc: NONE ";}
	
	const G4VProcess* creatorProc = track->GetCreatorProcess();
	if(creatorProc)
	{
		trackCreatorProc = creatorProc->GetProcessName();
		if(fVerbose>2)G4cout<< " SD:track:creator.proc:" << trackCreatorProc << " ";
	}else { if(fVerbose>2) G4cout << " SD:track:creator.proc: NONE ";}
	
	
	//track->GetDynamicParticle()->GetTotalEnergy();

	const std::vector<const G4Track*>* secondaries = aStep->GetSecondaryInCurrentStep();
    if(fVerbose>2) G4cout << " SD:sec.loop.size: " <<secondaries->size() << G4endl; 
    
    //G4bool flagPhot = true;
    
	if (secondaries->size()>0) 
	{
		for(std::size_t i=0; i<secondaries->size(); ++i) 
		{
				secTotKE = secTotKE +  secondaries->at(i)->
							GetDynamicParticle()->GetKineticEnergy();
				if(fVerbose>1) 
                    G4cout<< " part: " << secondaries->at(i)->GetDefinition()->GetParticleName()
                          << " KE: " << secondaries->at(i)->
                            GetDynamicParticle()->GetKineticEnergy()
                          << G4endl;
		}//for
	}//secondaries  
	
	
    G4TouchableHistory* touchable 	=
            (G4TouchableHistory*)(aStep->GetPostStepPoint()->GetTouchable());
    G4VPhysicalVolume* thePostPV 	= touchable->GetVolume();
    G4ThreeVector globalPos 		= postStep->GetPosition();
    G4ThreeVector localPos			=
			touchable->GetHistory()->GetTopTransform().TransformPoint(globalPos);
    //G4LogicalVolume* logVolume = thePostPV->GetLogicalVolume();      
	//G4ThreeVector position = preStep->GetPosition() + thePostStepPoint->GetPosition();
    // position = position/2.;
	SNSHit* newHit = new SNSHit(); //not for optG4std::icalphoton ?
	newHit->SetParticle(particleName);
	newHit->SetEdep(eDep);  
	newHit->SetPostProc(postProc);
	newHit->SetPreProc(preProc);
	newHit->SetTrackCreatorProc(trackCreatorProc);	
	newHit->SetPartKE(partKE);
	newHit->SetSecTotKE(secTotKE);
    newHit->SetTime(postStep->GetGlobalTime());	
    newHit->SetGlobalPos(globalPos);
	newHit->SetLocalPos(localPos);
    newHit->SetPhysVol(thePostPV);
	fHitsCollection->insert(newHit);

	G4int p = 5;
    G4int w = 10;
G4bool part = true;
//if(particleName != "proton") part = false;
fVerbose = 1;
    if(fVerbose && part)
    {
        //G4double trL = track->GetTrackLength()/um;
        //G4double stL = aStep->GetStepLength()/um;
        
        G4cout<< std::setw(w) << std::fixed << std::right << particleName
          << "   E: "
		  << std::setw(w) << std::fixed << std::right << std::setprecision(p) << eDep 

          << "  post: "  
          << std::setw(w) << std::fixed << std::right 
          << postProc
          << " preProc: " << preProc
          << "  cr: "
          << std::setw(w) << std::fixed << std::right 
          << trackCreatorProc 
         << std::setw(w) << std::fixed << std::right 
          << " pKE: "
          << std::setw(w) << std::fixed << std::right << std::setprecision(p) 
		  << partKE
          << std::setw(w) << std::fixed << std::right 
          << " secKE: " 
          << std::setw(w) << std::fixed << std::right << std::setprecision(p) 
		  << secTotKE
		<< " time: "  << postStep->GetGlobalTime()	
          << G4endl;
    }
     //If the option is false in SteppingAction::UserSteppingAction
     /*
     if(! fWriteAllSteps) 
	{
		Analysis* man = Analysis::GetInstance();
		if(man) man->WriteAllSteps(aStep);
	}


	 store particles and KE for 1st step
	 G4int StepNo = aStep->GetTrack()->GetCurrentStepNumber();
	if (secondaries->at(i)->GetParentID()>0) 
	NOTE: dynamic particle has no position or geometrical info.
	 G4ProcessManager* pm
	 = aStep->GetTrack()->GetDefinition()->GetProcessManager();
	 G4int nprocesses = pm->GetProcessListLength();
	 G4ProcessVector* pv = pm->GetProcessList();
     
     
     
     RE05CalorimeterHit* calHit
      = new RE05CalorimeterHit
          (touchable->GetVolume()->GetLogicalVolume(),copyIDinZ,copyIDinPhi);
    calHit->SetEdep( edep );
    G4AffineTransform aTrans = touchable->GetHistory()->GetTopTransform();
    aTrans.Invert();
    calHit->SetPos(aTrans.NetTranslation());
    calHit->SetRot(aTrans.NetRotation());
     
     
     
	 */
	
	return true;
}

//--------------------------------------------------------------------------
void SNSTargetSD::EndOfEvent(G4HCofThisEvent*)
{
	if ( verboseLevel>1 ) { 
		G4int nofHits = fHitsCollection->entries();
		G4cout << "\n------>Hits Collection: in this event there are " << nofHits 
		<< " hits in the sensitive volume: " << G4endl;
		for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
	}
}