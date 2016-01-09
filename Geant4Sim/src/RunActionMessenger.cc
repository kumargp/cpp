// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

#include "RunActionMessenger.hh"

#include "RunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

//............................................................................

RunActionMessenger::RunActionMessenger(RunAction* run)
:G4UImessenger(),fRun(run)
 ,fRunDir(0)
 ,fSeedCmd(0)
{
  fRunDir = new G4UIdirectory("/Sim/run/");
  fRunDir->SetGuidance("run control");

  fSeedCmd = new G4UIcmdWithAString("/Sim/run/UseSeeds",this);
  fSeedCmd->SetGuidance("Initialize the random number generator with integer seed stream.");
  fSeedCmd->SetParameterName("seed0","seed1",true);
  fSeedCmd->SetRange("seed0>0 && seed1>0");
  fSeedCmd->SetGuidance("Number of integers should be more than 1.");
  fSeedCmd->SetParameterName("IntArray",false);
  fSeedCmd->AvailableForStates(G4State_PreInit,G4State_Idle,G4State_GeomClosed);

}

//............................................................................

RunActionMessenger::~RunActionMessenger()
{
  delete fRunDir;
  delete fSeedCmd;
}

//...........................................................................

void RunActionMessenger::SetNewValue(G4UIcommand* /* command*/, G4String /* newValue*/ )
{      
  /*if( command==seedCmd )
  {
    G4Tokenizer next(newValue);
    G4int idx=0;
    long seeds[100];
    G4String vl;
    while(!(vl=next()).isNull())
    { seeds[idx] = (long)(StoI(vl)); idx++; }
    if(idx<2)
    { G4cerr << "/random/setSeeds should have at least two integers. Command ignored." << G4endl; }
    else
    {
      seeds[idx] = 0;
      G4Random::setTheSeeds(seeds);
    }
  }*/

}
