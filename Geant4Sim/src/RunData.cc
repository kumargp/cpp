// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

// RunData.cc


#include "RunData.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"


//===========================================================================

RunData::RunData()
: G4Run()
,  fPrintModulo(10000)
{} 

//===========================================================================

RunData::~RunData()
{} 
 
//===========================================================================

void RunData::RecordEvent(const G4Event*) //or use EndOfEventAction
{
	/*
    G4int evtNb = event->GetEventID();
  
    if (evtNb%fPrintModulo == 0) 
    { 
        G4cout << "\n---> end of event: " << evtNb << G4endl;
    }      
    
    if (fCollIDscint < 0 ) {
        fCollIDscint = G4SDManager::GetSDMpointer()->GetCollectionID("scint/edep");
        G4cout << " fCollIDscint: " << fCollIDscint << G4endl;   
    }
    
    //Hits collections
    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    if(!HCE) return;
    
    G4THitsMap<G4double>* evtMap = 
        static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollIDscint));
               
    G4double edep = 0;
    G4double eThreshold = 0.;
    std::map<G4int,G4double*>::iterator itr;
    for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) 
    {
        edep = *(itr->second);
        if(edep > eThreshold) AddEdep(edep);
    }
    //call FillPerEvent or similar Analysis method
    G4Run::RecordEvent(event); //??
    */
}

//===========================================================================

void RunData::Merge(const G4Run* run)
{
  //const RunData* localRun = static_cast<const RunData*>(run);
  //fRunEdep  += localRun->fRunEdep;


  G4Run::Merge(run); 
} 

//===========================================================================
/*
void RunData::Reset()
{ 
  for ( G4int i=0; i<kDim; i++) {
    fEdep[i] = 0.;
  }  
}
*/
