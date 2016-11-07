//---------------------------------------------------------------------------//
//bb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nu//
//                                                                           //
//                                                                           //
//                         MaGe Simulation                                   //
//                                                                           //
//      This code implementation is the intellectual property of the         //
//      MAJORANA and Gerda Collaborations. It is based on Geant4, an         //
//      intellectual property of the RD44 GEANT4 collaboration.              //
//                                                                           //
//                        *********************                              //
//                                                                           //
//    Neither the authors of this software system, nor their employing       //
//    institutes, nor the agencies providing financial support for this      //
//    work  make  any representation or  warranty, express or implied,       //
//    regarding this software system or assume any liability for its use.    //
//    By copying, distributing or modifying the Program (or any work based   //
//    on on the Program) you indicate your acceptance of this statement,     //
//    and all its terms.                                                     //
//                                                                           //
//bb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nu//
//---------------------------------------------------------------------------//
//
// $Id:
//
// CLASS IMPLEMENTATION:
//
//---------------------------------------------------------------------------//



//---------------------------------------------------------------------------//
//

#include "globals.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAnInteger.hh"

//---------------------------------------------------------------------------//
#include "mjdemonstrator/MJDemoParallelWorld.hh"
#include "mjdemonstrator/MJDemoParallelWorldMessenger.hh"

//---------------------------------------------------------------------------//

MJDemoParallelWorldMessenger::MJDemoParallelWorldMessenger(
  MJDemoParallelWorld *demoParallel):fDemoParallelWorld(demoParallel)

{
    // /MG/generator/ParticlesGen
    fParallelWorldDirectory = new G4UIdirectory("/MG/demonstrator/parallel/");
    fParallelWorldDirectory->SetGuidance("Parallel World in MJ Demo");

    //parallel pb+cu shields
    fParallelShieldsCmd = new G4UIcmdWithAnInteger("/MG/demonstrator/parallel/NumberOfParallelShields", this);
    fParallelShieldsCmd->SetGuidance("Number of Parallel Pb+Cu Shields");
    fParallelShieldsCmd->AvailableForStates(G4State_PreInit);

    //parallel cavern air
    fParallelAirCmd = new G4UIcmdWithAnInteger("/MG/demonstrator/parallel/NumberOfParallelAir", this);
    fParallelAirCmd->SetGuidance("Number of Parallel cavern Air volumes");
    fParallelAirCmd->AvailableForStates(G4State_PreInit);

    //parallel poly shields
    fParallelPolyCmd = new G4UIcmdWithAnInteger("/MG/demonstrator/parallel/NumberOfParallelPoly", this);
    fParallelPolyCmd->SetGuidance("Number of Parallel Poly shields");
    fParallelPolyCmd->AvailableForStates(G4State_PreInit);//, G4State_Idle);

}

//---------------------------------------------------------------------------//

MJDemoParallelWorldMessenger::MJDemoParallelWorldMessenger(const MJDemoParallelWorldMessenger & other) : G4UImessenger(other)
{;}

//---------------------------------------------------------------------------//

MJDemoParallelWorldMessenger::~MJDemoParallelWorldMessenger()
{
    delete fParallelShieldsCmd;
    delete fParallelAirCmd;
    delete fParallelPolyCmd;
}

//---------------------------------------------------------------------------//

void MJDemoParallelWorldMessenger::SetNewValue(G4UIcommand *cmd, G4String str)
{
    if( cmd == fParallelShieldsCmd )
      fDemoParallelWorld->SetParallelShieldNums(fParallelShieldsCmd->GetNewIntValue(str));
    else if(cmd == fParallelAirCmd)
      fDemoParallelWorld->SetParallelAirNums(fParallelAirCmd->GetNewIntValue(str));
    else if( cmd == fParallelPolyCmd )
      fDemoParallelWorld->SetParallelPolyNums(fParallelPolyCmd->GetNewIntValue(str));

}
