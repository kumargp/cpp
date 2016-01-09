// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "G4Event.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

G4int PrimaryGeneratorAction::partProperty = 0; //static

//===============================================================//

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fRndmBeam(false),f2DGaus(false), fSigmaX(0.), fSigmaY(0.)
, fSourceOnSphere(0), fSphereRad(0)
, fSourceInBox(false), fBoxSize(G4ThreeVector(0,0,0))
, fPartPos(G4ThreeVector(0,0,0))
{
    fParticleMessenger = new PrimaryGeneratorMessenger(this);
    
    G4int numOfParticles = 1;
	fParticleGun  = new G4ParticleGun(numOfParticles); //new G4GeneralParticleSource();

    // defaults
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    G4ParticleDefinition* particle
        = particleTable->FindParticle(particleName="neutron");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticlePosition(G4ThreeVector(0.0,0.0,0.0));
    fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
    fParticleGun->SetParticleEnergy(1000*MeV);
}

//===============================================================//

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    delete fParticleMessenger;
}

//====================================================================//

void PrimaryGeneratorAction::SetParticleName(G4String name)
{
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(name);
    //TODO: verify particle
    fParticleGun->SetParticleDefinition(particle);
    partProperty++;
}    

void PrimaryGeneratorAction::SetParticleEnergy(G4double energy)
{
    fParticleGun->SetParticleEnergy(energy);
    partProperty++;
}

void PrimaryGeneratorAction::SetParticlePos(G4ThreeVector pos)
{
    fPartPos = pos;
    fParticleGun->SetParticlePosition(pos);
    partProperty++;
}  


void PrimaryGeneratorAction::SetParticleMomentum(G4ThreeVector mom)
{
    if(! fRndmBeam)
    {
       //convert it to unit vector
      G4double px = mom.getX();
      G4double py = mom.getY();
      G4double pz = mom.getZ();      
      G4double ResMag = std::sqrt((px*px) + (py*py) + (pz*pz));
      if(std::abs(ResMag) >0.000001 )
      {
        px = px/ResMag;
        py = py/ResMag;
        pz = pz/ResMag;
      }
      G4cout << "## SETTING momentum: " << px << " " << py << " " << pz << G4endl;
      fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px,py,pz));
      partProperty++;
    }
}  

void PrimaryGeneratorAction::Apply2DGaussian(G4double sigmaX, G4double sigmaY )
{ 
      f2DGaus   = true; 
      fSigmaX   = sigmaX; 
      fSigmaY   = sigmaY;
      G4cout << "NOTE: Applied 2D Gaussian: sigmaX,Y = "
             << sigmaX << " " <<  sigmaY << G4endl;    
}

void PrimaryGeneratorAction::DistributeSourceOnSphere(G4double radius)
{ 
      fSphereRad = radius; 
      fSourceOnSphere = true; 
      
      G4cout << "NOTE: Source sampled on a spher of radius : "
             << fSphereRad/cm << " cm"<< G4endl;    
}

void PrimaryGeneratorAction::DistributeSourceInABox(G4ThreeVector boxSize)
{
    fBoxSize = boxSize;
    fSourceInBox = true;
    G4cout << "NOTE: Source sampled in a box of size : "
           << fBoxSize.getX()/cm << "  "
           << fBoxSize.getY()/cm << "  "
           << fBoxSize.getZ()/cm << "  cm "
           << G4endl;
}
   
void PrimaryGeneratorAction::DirectParticleToVolume(G4String)
{
    //TODO:
  
}

void PrimaryGeneratorAction::ConfineSourceToVolume(G4String vol)
{
  //TODO: not complete !!
  //G4cout << vol << G4endl;

	//command in macro
	// /gps/pos/ confine your_physical_volume_name
	
  // checks if selected volume exists
  G4VPhysicalVolume *tempPV      = NULL;
  G4PhysicalVolumeStore *PVStore = 0;
  G4String theRequiredVolumeName = vol;
  PVStore = G4PhysicalVolumeStore::GetInstance();
  G4int i = 0;
  G4bool found = false;
  // G4cout << PVStore->size() << G4endl;

  while (!found && i<(G4int)PVStore->size())
    {
      tempPV = (*PVStore)[i];
      found  = tempPV->GetName() == theRequiredVolumeName;
      //G4cout << i << " " << " " << tempPV->GetName() 
      //     << " " << theRequiredVolumeName << " " << found << G4endl;
      if (!found) { i++; }
    }
    
  G4ThreeVector null(0.,0.,0.);
  G4ThreeVector *ptr;
  ptr = &null;

  // Check particle_position is within VolName
  G4VPhysicalVolume *theVolume;
  G4Navigator *gNavigator = G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking();
  theVolume=gNavigator->LocateGlobalPointAndSetup(fPartPos,ptr,true);
  G4String theVolName = theVolume->GetName();
//  if(theVolName != vol) { G4cout << "Particle NOT in " << VolName << G4endl; }
    
    
    
  /*
  // found = true then the volume exists else it doesnt.
  if(found == true) {
     // G4cout << "Volume " << VolName << " exists" << G4endl;
        Confine = true;
  }
  else if(VolName=="NULL")
    Confine = false;
  else {
    G4cout << " **** Error: Volume does not exist **** " << G4endl;
    G4cout << " Ignoring confine condition" << G4endl;
    VolName = "NULL";
    Confine = false;
  }
  */
}
//===============================================================//

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //called at the begining of each event

    if(fSourceOnSphere && fSphereRad >0)
    {
        // sphere
        G4double th = 2.*pi*G4UniformRand(); //[0, 2*pi]
        G4double phi = pi*G4UniformRand(); //[0, pi]
        G4double x = fSphereRad* cos(th)*sin(phi);
        G4double y = fSphereRad* sin(th)*sin(phi);
        G4double z = fSphereRad * cos(phi); 
        
        x += fPartPos.getX();
        y += fPartPos.getY();        
        z += fPartPos.getZ();        
        fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
    }
    else if (fSourceInBox)
    {               
        G4double x = fBoxSize.getX()* (2.*G4UniformRand() - 1.); //[-1, 1]
        G4double y = fBoxSize.getY()* (2.*G4UniformRand() - 1.);
        G4double z = fBoxSize.getZ()* (2.*G4UniformRand() - 1.);        
        
        x += fPartPos.getX();
        y += fPartPos.getY();        
        z += fPartPos.getZ();  
        fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));        
    }
 
    if(f2DGaus)
    {
        G4double x = exp(-(G4UniformRand()*G4UniformRand()/(2.*fSigmaX*fSigmaX) +
                    G4UniformRand()*G4UniformRand()/(2.*fSigmaX*fSigmaX)));
		G4double y = 0;
		G4double z = 0;

        fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));  
		
		/*
		/gps/pos/type Plane
		/gps/pos/shape Ellipsoid # Annulus, Circle, Square or Rectangle
		/gps/pos/centre 0. 0. 0. cm
		/gps/pos/halfx 3.5 cm
		/gps/pos/halfy 10.0 cm
		  
		https://root.cern.ch/root/html/TRandom.html
		math/testrandom.C
		
		TRandom3, is based on the "Mersenne Twister generator", and is the recommended one
		
		TF1 *f1 = new TF1("f1","abs(sin(x)/x)*sqrt(x)",0,10);
		double r = f1->GetRandom();
		TF2 *f2 = new TF2("f2",
			
		Double_t Gaus(Double_t mean = 0, Double_t sigma = 1)
		TRandom3::Gaus(
		Samples a random number from the standard Normal (Gaussian) Distribution
		with the given mean and sigma.
		  
		void SetSeed(UInt_t seed = 0)		
		Set the random generator seed. Note that default value is zero, which is
		different than the default value used when constructing the class.
		If the seed is zero the seed is set to a random value
		which in case of TRandom depends on the lowest 4 bytes of TUUID
		The UUID will be identical if SetSeed(0) is called with time smaller than 100 ns
		Instead if a different generator implementation is used (TRandom1, 2 or 3)
		the seed is generated using a 128 bit UUID. This results in different seeds
		and then random sequence for every SetSeed(0) call.		
		*/
    }

    if (fRndmBeam) 
    {
     //distribution uniform in solid angle
     //
     G4double cosTheta = 2.*G4UniformRand() - 1.;
     G4double phi = twopi*G4UniformRand();
     G4double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
     G4double ux = sinTheta*std::cos(phi);
     G4double uy = sinTheta*std::sin(phi);
     G4double uz = cosTheta;
     fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));    
	 
    }


    //G4ThreeVector vec = fParticleGun->GetParticleMomentumDirection();
    //G4ThreeVector vec = fParticleGun->GetParticlePosition();
    //G4cout << "## " << vec.getX()/cm<< " " << vec.getY()/cm << " " << vec.getZ()/cm << G4endl;
	
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

    
    /* //Top and bottom lids, proportionally
    //2*πr^2/2πrh = r/h 
    if(G4UniformRand() < r/h)
    {
        z = h/2.* floor(G4UniformRand());
        if(z < 0.1) z = -h/2.;
        x = r * (2. * G4UniformRand()-1.);
        y = sqrt(r*r - x*x);
    }*/
    //x = 1.5*m; y = 1.5*m; z = 0;

    /*G4double px,py,pz;
	 G 4double mag;                                                        *
	 do {
		 px = (G4UniformRand()-0.5)/0.5;
		 py = (G4UniformRand()-0.5)/0.5;
		 pz = (G4UniformRand()-0.5)/0.5;
		 mag = px*px + py*py + pz*pz;
		 } while (mag > 1 || mag == 0.0);
		 mag = std::sqrt(mag);
		 px /= mag;
		 py /= mag;
		 pz /= mag;
		 particleGun->SetParticleMomentumDirection(G4ThreeVector(px,py,pz));
	
	
	
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
	
	*/