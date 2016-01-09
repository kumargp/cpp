


#include "StackingAction.hh"
#include "G4Track.hh"

//--------------------------------------------------------------------------

StackingAction::StackingAction()
{ }

//--------------------------------------------------------------------------

StackingAction::~StackingAction()
{ }

//--------------------------------------------------------------------------

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* track)
{

    G4ClassificationOfNewTrack status = fUrgent;
    G4double energy = aTrack->GetKineticEnergy();
    G4String pname  = aTrack->GetDefinition()->GetParticleName();

    G4cout << " killing "
           << pname << " E_mev: " << energy/MeV
           << G4endl;
  
 // CAREFUL !!!!!!
  if( (pname == "neutron"|| pname == "gamma") && energy < 1.*MeV  ) 
    { 
        status = fKill;
    }

  return status;

}

