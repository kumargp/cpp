// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

//EventAction.cc

#include "EventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "Randomize.hh"

#include "SNSHit.hh"
#include "RunData.hh"
#include "Analysis.hh"
#include <ctime>
#include <cmath> 
#define EPSILON 0.00001

G4String EventAction::fRandString("");

//==============================================================================

EventAction::EventAction()
: G4UserEventAction()
, fEventEdep(0),fEdepStep(0), fTableNPhot(0)
, fCollIDscint(-2), fCollIDtarget(-2), fTrackingFlag(false)
{} 

//==============================================================================

EventAction::~EventAction()
{}


//==============================================================================
void EventAction::BeginOfEventAction(const G4Event* anEvent)
{    
    G4bool setEventSeeds = false ; //EVTSEEDFLAG 
    	
    static G4int evtID = -1;
    if(setEventSeeds && evtID == -1) // stop further beams
    {
        evtID = 1 ; //EVTSEEDID
        G4long seed0 = 0; //EVTSEEDS0
        G4long seed1 = 0; //EVTSEEDS1
        G4int index = 0 ; //EVTINDEX
        G4long seeds[2] = {seed0, seed1}; 
        G4Random::setTheSeeds(seeds, index);
		G4cout << "\n\n\n\n*****WARNING ***** eventseed is preset\n\n"
			   << " Re-running EventID " << evtID << " seedInd " <<
                  seed0 << " " << seed1 << " " << index << G4endl;
    }

    //time_t  start = time(0); ctime(&start)
    std::ostringstream os;
    os  << "evtIDseedsIndex "<< anEvent->GetEventID() << " " 
        << *G4Random::getTheSeeds() << " " << *(G4Random::getTheSeeds()+1)
        << " " << G4Random::getTheSeed();
    SetRandString(os.str());

	
	/*std::vector <G4int> eventIDstoSelect; //	static. or in RunAction/RunData
	if(eventIDstoSelect.size() >0)
	{	
		G4bool abortThis = true;
		G4int thisEvt    = anEvent->GetEventID();
		for( G4int i=0; i<eventIDstoSelect.size(); i++)
		{
			if(thisEvt == eventIDstoSelect[i])
			{	
				abortThis  = false; 
				break;
			}
		}
		if(abortThis) G4RunManager::GetRunManager()->AbortEvent();
	} */
	//if(421814 != anEvent->GetEventID()) G4RunManager::GetRunManager()->AbortEvent();	
	
    fEdepStep = 0;
    fEventEdep = 0;
    fTableNPhot = 0;
    /*FindSensitiveDetector(G4String dName, G4bool warning = true);
    G4VSensitiveDetector* aSD = SDman->FindSensitiveDetector("SNSTargetDetector", false);
    G4int numberOfCollections = aSD->GetNumberOfCollections();
    for(G4int i=0;i<numberOfCollections;i++)
    {
      G4String SDname = aSD->GetName();
      G4String DCname = aSD->GetCollectionName(i);
    }*/
    
    if(fCollIDscint<-1)
    {
        G4cout << "getting ScintCollection =>  " ;
        G4SDManager* SDman = G4SDManager::GetSDMpointer();
        fCollIDscint = SDman->GetCollectionID("ScintCollection");
        if(fCollIDscint >= 0) G4cout << " ID : " <<  fCollIDscint << G4endl;
        //BEWARE only 1st event, but to avoid warning if det is absent
    }
    if(fCollIDtarget<-1)
    {   G4cout << "getting TargetCollection =>  ";
        G4SDManager* SDman = G4SDManager::GetSDMpointer();
        fCollIDtarget = SDman->GetCollectionID("SNSTargetCollection");
        if(fCollIDtarget >=0) G4cout << " ID : " << fCollIDtarget << G4endl;
        //BEWARE only 1st event, but to avoid warning if det is absent
    }
	//IFF there is array of data accumulated from each step,it has to be reset here.
	//  B4bRunData* runData  = static_cast<B4bRunData*>(
	//      G4RunManager::GetRunManager()->GetNonConstCurrentRun());
	//   runData->Reset();  
	//if(id%1000 ==0) 
	//	G4cout << "\n EVENTID: "<< anEvent->GetEventID() << G4endl;
	
	
	fTrackingFlag = false;
  
}

//==============================================================================

void EventAction::EndOfEventAction(const G4Event* anEvent)
{   
    //RunData* localRun = static_cast<RunData*>(
	//		G4RunManager::GetRunManager()->GetNonConstCurrentRun() );
	//localRun->Recordevent(anEvent); 
	G4int evtID = anEvent->GetEventID();
	G4int nPrint = 1000; //localRun->GetPrintModulo(); 

	time_t ttt = time(0);
	if ( evtID>0 && evtID % nPrint == 0) 
		G4cout << "\n######end_of_event: " << evtID 
               << " at " << ctime(&ttt) <<  G4endl;
	
	
	if(fCollIDscint<0 && fCollIDtarget<0) return;


	//Hits collections
	G4HCofThisEvent* HCE = anEvent->GetHCofThisEvent();
	if(!HCE) return;
	
	SNSHitsCollection* snsHC= NULL;

    if(fCollIDscint>=0 && HCE->GetHC(fCollIDscint))
                  snsHC = (SNSHitsCollection*)(HCE->GetHC(fCollIDscint));
    else if(fCollIDtarget>=0 && HCE->GetHC(fCollIDtarget))
                  snsHC = (SNSHitsCollection*)(HCE->GetHC(fCollIDtarget));
    if(!snsHC) return;
    
    //Hits in scintillator
    G4int nHit = snsHC->entries();     
    G4int g4nPhot = 0;
	G4int glg4nPhot = 0;
	G4int g4MethodNPhot = 0;
	G4String particle("");
	G4String crProcName("");
	G4String postProcName("");
	G4double energy = 0;
	G4double eDepProt = 0;
	G4double eDepElec =0;
	G4double eDepAlpha = 0;
	G4double eDepC12 = 0;
	G4double eDepBe = 0;
	G4double nCapE = 0; 
	G4double nInelasticE = 0. ; 
	G4double iInelasticE =0. ; 
	G4double hIoniE = 0.;
	G4double eIoniE = 0.; 
	G4double ionIoniE= 0.; 
	G4double comptE = 0.; 
	G4double eBremE =0.; 
	G4double hadElasticE =0.; 
	G4double photE = 0.; 
	G4double convE = 0.; 
	G4double annihilE = 0.;  
	G4double mscE = 0. ; 
	G4double decayE = 0.; 

	G4double cr_nCapE = 0.; 
	G4double cr_nInelasticE = 0.;
	G4double cr_iInelasticE =0.;
	G4double cr_hIoniE = 0.;
	G4double cr_eIoniE = 0.;
	G4double cr_ionIoniE= 0.;
	G4double cr_comptE = 0.;
	G4double cr_eBremE =0.;
	G4double cr_hadElasticE =0.; 
	G4double  cr_photE = 0.;
	G4double cr_convE = 0.;
	G4double cr_annihilE = 0.;
	G4double cr_mscE = 0.;
	G4double cr_decayE = 0.; 

    if(snsHC)
    {
    	for(int i=0; i< nHit; i++)
        { 
			energy = (*snsHC)[i]->GetEdep();
        	fEventEdep += energy; 
            //same as adding edep in SteppingAction, and stored in EventAction. ??

			particle  = (*snsHC)[i]->GetParticle();
			crProcName  = (*snsHC)[i]->GetTrackCreatorProc();
            postProcName  = (*snsHC)[i]->GetPostProc();

            fTableNPhot     += (*snsHC)[i]->GetTableNPhot();          
            g4nPhot         += (*snsHC)[i]->GetG4nPhot();
            glg4nPhot       += (*snsHC)[i]->GetGLG4nPhot(); 
            g4MethodNPhot  += (*snsHC)[i]->GetG4MethodNPhot(); 
            
            
			if(particle.compare("proton")==0 ) eDepProt += energy;
			else if(particle.compare("e-")==0) eDepElec += energy;
			else if(particle.compare("alpha")==0) eDepAlpha += energy;
			else if(particle.compare("C12")==0) 	eDepC12 += energy;
			else if(particle.compare("Be9")==0) 	eDepBe += energy;
			
			if(crProcName.compare("nCapture")==0) cr_nCapE += energy;
			else if(crProcName.compare("neutronInelastic")==0) cr_nInelasticE += energy;
			else if(crProcName.compare("ionInelastic")==0) cr_iInelasticE += energy;
			else if(crProcName.compare("compt")==0) cr_comptE += energy;
			else if(crProcName.compare("phot")==0) cr_photE += energy;
			else if(crProcName.compare("eIoni")==0) cr_eIoniE += energy;
			else if(crProcName.compare("hIoni")==0) cr_hIoniE += energy;
			else if(crProcName.compare("ionIoni")==0)cr_ionIoniE += energy;
			else if(crProcName.compare("eBrem")==0) cr_eBremE += energy;
			else if(crProcName.compare("hadElastic")==0) cr_hadElasticE += energy;
			else if(crProcName.compare("conv")==0) cr_convE += energy;
			else if(crProcName.compare("msc")==0) cr_mscE += energy;
			else if(crProcName.compare("annihil")==0) cr_annihilE += energy;	
			else if(crProcName.compare("Decay")==0) cr_decayE += energy;	

            if(postProcName.compare("nCapture")==0) nCapE += energy;
            else if(postProcName.compare("neutronInelastic")==0) nInelasticE += energy;
            else if(postProcName.compare("ionInelastic")==0) iInelasticE += energy;
            else if(postProcName.compare("compt")==0) comptE += energy;
            else if(postProcName.compare("phot")==0) photE += energy;
            else if(postProcName.compare("eIoni")==0) eIoniE += energy;
            else if(postProcName.compare("hIoni")==0) hIoniE += energy;
            else if(postProcName.compare("ionIoni")==0)ionIoniE += energy;
            else if(postProcName.compare("eBrem")==0) eBremE += energy;
            else if(postProcName.compare("hadElastic")==0) hadElasticE += energy;
            else if(postProcName.compare("conv")==0) convE += energy;
            else if(postProcName.compare("msc")==0) mscE += energy;
            else if(postProcName.compare("annihil")==0) annihilE += energy;    
            else if(postProcName.compare("Decay")==0) decayE += energy;
            // other defined proc:
			// anti_neutronInelastic, anti_protonInelastic, hFritiofCaptureAtRest, 
			//positronNuclear, electronNuclear, photonNuclear, kaon+Inelastic, 
			//kaon-Inelastic, hBertiniCaptureAtRest, lambdaInelastic, 
			//muMinusCaptureAtRest,  nFission, pi+Inelastic, 
		}
	}

    if(fEventEdep>0)
    {
        Analysis* man = Analysis::GetInstance();
		man->FillPartE(fEventEdep, eDepProt, eDepElec, eDepAlpha, eDepC12, 
						  eDepBe,fTableNPhot, g4nPhot, glg4nPhot, g4MethodNPhot, evtID);	
		man->FillProcessE(fEventEdep, nCapE, nInelasticE, iInelasticE, comptE, photE, eIoniE, 
						  hIoniE, ionIoniE, eBremE, hadElasticE, convE, mscE, 
					      annihilE, decayE, evtID);
        man->FillCrProcessE(fEventEdep, cr_nCapE, cr_nInelasticE, cr_iInelasticE, cr_comptE, 
                          cr_photE, cr_eIoniE, cr_hIoniE, cr_ionIoniE, cr_eBremE, 
                          cr_hadElasticE, cr_convE, cr_mscE, cr_annihilE, cr_decayE, evtID);
		man->WriteAscii( fEventEdep, eDepProt, eDepElec, eDepAlpha, eDepC12, 
						 eDepBe,fTableNPhot, g4nPhot, glg4nPhot, g4MethodNPhot , evtID);
    }	
    
    time_t systime = time(NULL);
    nPrint=1000;   
    if ( evtID % nPrint == 0) 
	{ 
		//time_t now = time(0);
		G4cout << "\n######end_of_event: " << evtID //<< "  TotEDep: "<<  fEventEdep
		       <<  "  at: " << ctime(&systime) 
               << G4endl;
	}   
	
}
	
//==============================================================================

// genAction->GetEventSeeds();
