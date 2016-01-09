// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

//SteppingAction.hh


#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class G4Step;
class G4LogicalVolume;
class DetectorConstruction;
class EventAction;

/// Stepping action class
/// 

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(DetectorConstruction*, EventAction*);
    virtual ~SteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);
	
	G4int CalculateLY(G4String particle, G4double inputE);
	G4double LinearInterp( G4double input, std::vector <G4double> dataX, 
					std::vector <G4double>dataY, G4bool extrapolate);
	void VerbMinimum(const G4Step* step, G4String key, G4bool part=false);
	void VerbMore(const G4Step* step, G4String key, G4bool detailed=false);
	//find particles cross outer surface into/from volumes
	G4int IsBoxSurfaceFlux(const G4Step* aStep);
	G4int IsCylSurfaceFlux(const G4Step* aStep, G4double fRadius, G4double fHalfHt);
	void CallFillDetection(const G4Step*);
	void CallFillNFlux(const G4Step*, bool);
	void GetProcVolIDs( G4int&, G4int&, const G4String, const G4String= "NULL");
	
	void GetNParams(const G4Step*, G4double&, G4double&, G4double&, G4double&, 
					G4double&, G4int&, G4int& , G4int&, G4int&);
	
  private:
	  SteppingAction (const SteppingAction& );
	  SteppingAction& operator= (const SteppingAction& );	  
	  
    EventAction* fEventAction;
    DetectorConstruction* fDetector;
    G4LogicalVolume* fSDvolume;
    G4bool fWriteAllSteps;
    G4int fScintillationCounter;
    G4int fCerenkovCounter;
    G4int fEventID;
    G4bool fSteppingVerbose;
};


#endif