// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "PrimaryGeneratorMessenger.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Navigator.hh"
#include "G4ParticleMomentum.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"


//class G4ParticleGun;
class G4Event;
class PrimaryGeneratorMessenger;

/// The primary generator action class

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
    virtual ~PrimaryGeneratorAction();

    // Method from base class
    virtual void GeneratePrimaries(G4Event*);         
  
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
	//const G4GeneralParticleSource* GetParticleGun() const { return fParticleGun; }
    
    void SetParticleName(G4String); 
    void SetParticleEnergy(G4double);
    void SetParticlePos(G4ThreeVector);
    void SetParticleMomentum (G4ThreeVector);   
    void DistributeSourceInABox(G4ThreeVector);
    void DirectParticleToVolume(G4String);
    void ConfineSourceToVolume(G4String);
    void DistributeSourceOnSphere(G4double);
    void Apply2DGaussian(G4double, G4double);

    inline void SetRandomBeamDir() { fRndmBeam = true;}

    
  private:
    G4ParticleGun*              fParticleGun; // pointer a to G4 gun class
    //G4GeneralParticleSource*	fParticleGun;
    PrimaryGeneratorMessenger*  fParticleMessenger;
    static G4int                partProperty;
    G4bool                      fRndmBeam;
    G4bool                      f2DGaus;
    G4double                    fSigmaX;
    G4double                    fSigmaY;
    G4bool                      fSourceOnSphere;
    G4double                    fSphereRad;
    G4bool                      fSourceInBox;
    G4ThreeVector               fBoxSize;
    G4ThreeVector               fPartPos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif