

#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "EventAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include <string>

//------------------------------------------------------------------------------

TrackingAction::TrackingAction( EventAction* evtAct)
:G4UserTrackingAction(), fEventAction(evtAct)
{ }

//------------------------------------------------------------------------------

TrackingAction::~TrackingAction()
{ }

//------------------------------------------------------------------------------

void TrackingAction::PreUserTrackingAction(const G4Track*)
{ }

//------------------------------------------------------------------------------

void TrackingAction::PostUserTrackingAction(const G4Track*)
{

}