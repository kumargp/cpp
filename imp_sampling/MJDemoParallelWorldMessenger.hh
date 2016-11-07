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
/**
* $Id:
*
* CLASS DECLARATION:
*
*---------------------------------------------------------------------------//
*
* DESCRIPTION:
*
*/

// --------------------------------------------------------------------------//
/**
 * AUTHOR: Gopan Perumpilly
 */
//---------------------------------------------------------------------------//
//

#ifndef _MJDEMOPARALLELWORLDMESSENGER_HH
#define _MJDEMOPARALLELWORLDMESSENGER_HH

//---------------------------------------------------------------------------//

#include "G4UImessenger.hh"

//---------------------------------------------------------------------------//
class MJDemoParallelWorld;

class G4UIdirectory;
class G4UIcmdWithAnInteger;


class MJDemoParallelWorldMessenger : public G4UImessenger
{
public:
	MJDemoParallelWorldMessenger(MJDemoParallelWorld *demoParallel);
	MJDemoParallelWorldMessenger(const MJDemoParallelWorldMessenger &);
	~MJDemoParallelWorldMessenger();

	void SetNewValue(G4UIcommand *cmd, G4String str);

protected:

private:
	MJDemoParallelWorld	*fDemoParallelWorld;
	G4UIdirectory		*fParallelWorldDirectory;
	G4UIcmdWithAnInteger	*fParallelShieldsCmd;
	G4UIcmdWithAnInteger	*fParallelAirCmd;
	G4UIcmdWithAnInteger	*fParallelPolyCmd;

};
#endif

