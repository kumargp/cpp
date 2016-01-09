// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 





#ifndef SNSTARGETSD_HH
#define SNSTARGETSD_HH

#include "G4VSensitiveDetector.hh"
#include "SNSHit.hh"
#include "G4ios.hh"
#include <vector> //Note

class G4Step;
class G4HCofThisEvent;



class SNSTargetSD : public G4VSensitiveDetector
{
public:
	SNSTargetSD(const G4String& name, const G4String& hitsCollectionName );
	virtual ~SNSTargetSD();
	
	// From base class
	virtual void   Initialize(G4HCofThisEvent* hitCollection);
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
	//ProcessHits() is called by G4 kernel at each step.
	virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);
	
private:
	SNSTargetSD(const SNSTargetSD &);
	SNSTargetSD& operator=(const SNSTargetSD &);
	
	SNSHitsCollection* fHitsCollection;
	static G4int fTargetHCid;
    G4int fVerbose;
    //const G4ParticleDefinition* proton;
};

//--------------------------------------------------------------------------

#endif
