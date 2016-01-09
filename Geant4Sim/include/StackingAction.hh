



#ifndef STACKINGACTION_HH
#define STACKINGACTION_HH

#include "G4UserStackingAction.hh"
#include "globals.hh"

class StackingActionMessenger;

// Stacking action class : manage the newly generated particles

//-------------------------------------------------------------------------

class StackingAction : public G4UserStackingAction
{
  public:
    StackingAction();
    virtual ~StackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);   
	
	inline void SetNeutronThreshold(G4double en) { fNeutThresh = en; }
	inline void SetGammaThreshold(G4double en) { fGammaThresh = en; }

private:
	StackingAction (const StackingAction&);
	StackingAction& operator= (const StackingAction&);
	
	StackingActionMessenger* fStackMessenger;
	G4double fNeutThresh;
	G4double fGammaThresh;
	
};

//----------------------------------------------------------------------

#endif
