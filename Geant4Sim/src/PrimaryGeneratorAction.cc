// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RandomDirection.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

//===============================================================//

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
{
    
	fParticleGun  = new G4GeneralParticleSource();

    // defaults
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    G4ParticleDefinition* particle
        = particleTable->FindParticle(particleName="neutron");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticlePosition(G4ThreeVector(0.0,0.0,0.0));
    //fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
    //fParticleGun->SetParticleEnergy(1000*MeV);
}

//===============================================================//

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

//===============================================================//

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //called at the begining of each event

    fParticleGun->GeneratePrimaryVertex(anEvent);
}

    
    /* 
	# default intensity is 1, 
	# now change to 5.
	/gps/source/intensity 5.
	
	/gps/particle proton
	/gps/pos/type Beam
	
	# the incident surface is 
	# in the y-z plane
	/gps/pos/rot1 0 1 0
	/gps/pos/rot2 0 0 1
	
	# the beam spot is centered 
	# at the origin and is 
	# of 1d gaussian shape 
	# with a 1 mm central plateau
	/gps/pos/shape Circle 
	/gps/pos/centre 0. 0. 0. mm
	/gps/pos/radius 1. mm
	/gps/pos/sigma_r .2 mm
	
	# the beam is travelling 
	# along the X_axis 
	# with 5 degrees dispersion
	/gps/ang/rot1 0 0 1
	/gps/ang/rot2 0 1 0
	/gps/ang/type beam1d 
	/gps/ang/sigma_r 5. deg
	
	# the beam energy is in 
	# gaussian profile centered 
	# at 400 MeV
	/gps/ene/type Gauss
	/gps/ene/mono 400 MeV
	/gps/ene/sigma 50. MeV
	
	/gps/pos/type Plane
	/gps/pos/shape Ellipsoid # Annulus, Circle, Square or Rectangle
	/gps/pos/centre 0. 0. 0. cm
	/gps/pos/halfx 3.5 cm
	/gps/pos/halfy 10.0 cm
	
	*/