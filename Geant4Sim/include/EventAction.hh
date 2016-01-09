// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"


/// Event action class
///

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();
	
    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    inline void AddStepEdep(G4double edep) { fEdepStep += edep; }
    inline void SetRandString(G4String str) { fRandString = str; }
    inline static G4String GetRandString() { return fRandString;}

    inline void SetTrackingFlag(){ fTrackingFlag = true;}   
    inline G4bool GetTrackingFlag(){ return fTrackingFlag;}
    
  private:
	EventAction(const EventAction&);
	EventAction& operator=(const EventAction&);
	  
    G4double    fEventEdep;
    G4double    fEdepStep;
    G4int       fTableNPhot;
    G4int       fCollIDscint;
    G4int       fCollIDtarget;
    static G4String    fRandString;
	G4bool 		fTrackingFlag;
};


#endif