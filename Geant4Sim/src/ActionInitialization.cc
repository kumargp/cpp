// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#include "ActionInitialization.hh"
#include "SteppingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "DetectorConstruction.hh"
#include "StackingAction.hh"

//==========================================================//

ActionInitialization::ActionInitialization(DetectorConstruction* detector)
 : G4VUserActionInitialization(), fDetector(detector)
{}

//==========================================================//

ActionInitialization::~ActionInitialization()
{
	//don't delete detector
}


//==========================================================//

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction);
}

//==========================================================//

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction);
  
  SetUserAction(new RunAction);  
  
  EventAction* evtAction = new EventAction;
  SetUserAction(evtAction);    
  
  TrackingAction* trackingAction = new TrackingAction(evtAction) ;
  SetUserAction(trackingAction);
  
  SetUserAction(new SteppingAction(fDetector, evtAction));
  
  SetUserAction(new StackingAction());
}  

//==========================================================//