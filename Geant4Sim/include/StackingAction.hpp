



#ifndef STACKINGACTION_HH
#define STACKINGACTION_HH

#include "G4UserStackingAction.hh"
#include "globals.hh"

// Stacking action class : manage the newly generated particles

//-------------------------------------------------------------------------

class StackingAction : public G4UserStackingAction
{
  public:
    StackingAction();
    virtual ~StackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);        
};

//----------------------------------------------------------------------

#endif
