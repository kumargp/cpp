// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

class G4Event;
//class G4GeneralParticleSource;

/// The primary generator action class

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
    virtual ~PrimaryGeneratorAction();

    // Method from base class
    virtual void GeneratePrimaries(G4Event*);         

	 G4GeneralParticleSource* GetParticleGun() const { return fParticleGun; }

    
  private:
	  PrimaryGeneratorAction (const  PrimaryGeneratorAction&);
	  PrimaryGeneratorAction& operator= (const  PrimaryGeneratorAction&);	 
	  
    G4GeneralParticleSource*	fParticleGun;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif