


#include "StackingAction.hh"
#include "StackingActionMessenger.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
//--------------------------------------------------------------------------

StackingAction::StackingAction()
:fNeutThresh(0.), fGammaThresh(0.)
{ 
	fStackMessenger = new StackingActionMessenger(this);
}

//--------------------------------------------------------------------------

StackingAction::~StackingAction()
{ 
	delete fStackMessenger;
}

//--------------------------------------------------------------------------

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{

    G4ClassificationOfNewTrack status = fUrgent;
	
	if( fNeutThresh > 0. || fGammaThresh > 0. )
	{	
		G4double energy = aTrack->GetKineticEnergy();
		G4String pname  = aTrack->GetDefinition()->GetParticleName();

		if( (energy < fNeutThresh  && pname == "neutron" ) || (energy < fGammaThresh  && pname == "gamma") ) 
		{ 
			G4bool  verb = false;
			if(verb)
			{
				G4int trk = aTrack->GetTrackID();
				G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
				G4cout << "killing " << pname << " trk " << trk << " " << evt <<
					" E_mev: " << energy/MeV << G4endl;
			}
			status = fKill;
		}
	}

	return status;
}

