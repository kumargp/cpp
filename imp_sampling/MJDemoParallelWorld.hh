//---------------------------------------------------------------------------//
//bb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nu//
//                                                                           //
//                                                                           //
//                         MaGe Simulation                                   //
//                                                                           //
//      This code implementation is the intellectual property of the         //
//      MAJORANA and Gerda Collaborations. It is based on Geant4, an         //
//      intellectual property of the RD44 GEANT4 collaboration.              //
//                                                                           //
//                        *********************                              //
//                                                                           //
//    Neither the authors of this software system, nor their employing       //
//    institutes, nor the agencies providing financial support for this      //
//    work  make  any representation or  warranty, express or implied,       //
//    regarding this software system or assume any liability for its use.    //
//    By copying, distributing or modifying the Program (or any work based   //
//    on on the Program) you indicate your acceptance of this statement,     //
//    and all its terms.                                                     //
//                                                                           //
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
// Begin description of class here
/**
 *
 *Geometry
 *
 *
 */
// End class description
//
/**
 * SPECIAL NOTES:
 *
 */
//
// --------------------------------------------------------------------------//
/**
  */
// --------------------------------------------------------------------------//

#ifndef _MJDEMOPARALLELWORLD_HH
#define _MJDEMOPARALLELWORLD_HH

//---------------------------------------------------------------------------//

#include "globals.hh"
#include "geometry/MGGeometryParallelWorld.hh"

class MJDemoParallelWorldMessenger;
class G4VPhysicalVolume;

using namespace std;
//---------------------------------------------------------------------------//

class MJDemoParallelWorld: public MGGeometryParallelWorld
{

public:
  MJDemoParallelWorld();
  virtual ~MJDemoParallelWorld();

// overloaded from G4VUserParallelWorld:
  virtual void Construct();

  virtual G4VPhysicalVolume* GetWorldVolume() const { return fWorldVolume; }

  void SetParallelAirNums(G4int num){ fNParallelVolumesAir = num;}
  void SetParallelShieldNums(G4int num){fNParallelVolumesShield = num;}
  void SetParallelPolyNums(G4int num){fNParallelVolumesPoly = num;}


private:
  MJDemoParallelWorldMessenger *fMessenger;
  G4VPhysicalVolume* fWorldVolume;
  G4int fNParallelVolumesShield;
  G4int fNParallelVolumesAir;
  G4int fNParallelVolumesPoly;

};
//
#endif
