// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef SNSSD_HH
#define SNSSD_HH

#include "G4VSensitiveDetector.hh"
#include "SNSHit.hh"
#include "G4ios.hh"
#include <vector> //Note


class G4Step;
class G4HCofThisEvent;



class SNSSD : public G4VSensitiveDetector
{
public:
	SNSSD(const G4String& name, const G4String& hitsCollectionName );
	virtual ~SNSSD();
	
private:
	SNSSD (const SNSSD& );
	SNSSD& operator= (const SNSSD& );	
	
public:
	// From base class
	virtual void   Initialize(G4HCofThisEvent* hitCollection);
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
	//ProcessHits() is called by G4 kernel at each step.
	virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);
	
    G4double LinearInterp(G4double, std::vector <G4double>,std::vector <G4double>, G4bool);
    G4int CrossCheckLY(G4String, G4double);
	G4int CalculateLY(G4String, G4double);
    G4double BirksAttenuation(const G4Step*);
	G4double GLG4Scint( const G4Step*);
    
private:
	SNSHitsCollection* fHitsCollection;
	G4double fThresholdEDep;
	static G4int fHCid;
    G4int fVerbose;
    const G4ParticleDefinition* proton;
};

//--------------------------------------------------------------------------

#endif
