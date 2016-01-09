// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


//RunAction.cc

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh" //
#include "RunData.hh"
#include "Analysis.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include <fstream> //
#include <sstream>
#include "G4RunManager.hh"
#include "Randomize.hh"
//#include "G4UImanager.hh"
//#include "G4VVisManager.hh"
#include <ctime>
#include <sys/time.h>


size_t  RunAction::nNeutronsRun = 0;

//============================================================================

RunAction::RunAction()
: G4UserRunAction()
, fSaveRndm(false), fReadRndm(false)
, fSeed0(-1), fSeed1(-1), fIndex(-1)
, fRndmFileName("run0.rndm"), fVerboseLevel(1), start(0)
{}

//============================================================================

RunAction::~RunAction()
{}

//============================================================================

G4Run* RunAction::GenerateRun()
{
	return new RunData; 
}

//============================================================================

void RunAction::BeginOfRunAction(const G4Run* run)
{ 
	nNeutronsRun = 0;

	start = time(0);
	G4cout << "@@@Run_Begin: " << ctime(&start)<< G4endl;

	time_t systime = time(NULL);
	timespec ts;
	timespec ts2;
	clock_gettime(CLOCK_REALTIME, &ts);
	clock_gettime(CLOCK_REALTIME, &ts2);

	/* NOTE DON't call  G4UniformRand() before setting seeds, it gives the default always.
	so Don't use it for setting seeds. Don't use systime, as for batch it doesn't 
	change fast, use nano-time instead. 
	How it works: When G4UniformRand()gets called the seeds change to next in the sequence.
	use G4Random:: inplace of CLHEP::HepRandom::, for the MT.
	showEngineStatus() gives the same as G4Random::getTheSeed(), *G4Random::getTheSeeds(), 
		*(G4Random::getTheSeeds()+1).  
	Is there a limit of 900,000,000,otherwise they get modified and the seeds may repeat?.
	http://mu2e.fnal.gov/public/hep/computing/Random.shtml 

	long t1 = (long) (ts.tv_nsec);
	long ss = systime*1000+ t1%1000;
	*/

	fSeed0 = 0; fSeed1 = 0; fIndex = -1; //SET_SEED_HERE
	if(fIndex ==-1)
	{
		G4long t1 = (G4long) (ts.tv_nsec);
		G4long t2 = (G4long) (ts2.tv_nsec);
		fIndex = t1;
		fSeed0 = systime*100+ t1%100; //(long) (ts.tv_nsec/11);
		fSeed1 = systime*10+ t2%10;//(long) (ts2.tv_nsec); 
	}
	G4long seeds[2];
	seeds[0] = fSeed0;
	seeds[1] = fSeed1;
	G4Random::setTheSeeds(seeds, fIndex);//CLHEP::HepRandom::
	G4Random::showEngineStatus(); //NOTE Dont take off. Used from out file to re-run

	G4cout<< "***** seedNumbers " << fSeed0 << " " << fSeed1 << " index: " << fIndex << G4endl;

	G4bool randTest = false; 
	if(randTest)
	{//test using code from RanecuEngine.cc	
		int theSeed=0,shift1=147483563, shift2=2147483399;
		if (fIndex != -1) {  theSeed = std::abs(int(fIndex%215));  }
		G4cout << " shifted:ind:seeds " << theSeed << " " << abs(seeds[0])%shift1
				<< " " << abs(seeds[1])%shift2 << G4endl;
			
		G4cout << "  set::ind:seeds " << fIndex << " " << seeds[0] << " " << seeds[1] << G4endl;
		for (G4int ii=0; ii<100000;ii++)
		{ //test sequence for same initial seed(s)
			G4cout << " G4randtest: " << *G4Random::getTheSeeds()
				<< " " << *(G4Random::getTheSeeds()+1) 
				<< " uniform " << (long)(1000000000*G4UniformRand()) << G4endl;
		} 
	}//end of test


	if ( fReadRndm ) { //set default as false 
		G4cout << "\n---> rndm status restored from file: " << fRndmFileName << G4endl;
		G4Random::restoreEngineStatus(fRndmFileName);
		G4Random::showEngineStatus();
	}   


	// save Rndm status
	if ( fSaveRndm ) { 
		G4int runNumber = run->GetRunID();
		std::ostringstream rndmFileName;
		rndmFileName << "run" << runNumber << "Begin.rndm";
		G4Random::saveEngineStatus(rndmFileName.str().c_str()); 
	}  
	/*
	if ( G4VVisManager::GetConcreteInstance() )  {
	G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");
	}  
	*/

	Analysis* man = Analysis::GetInstance();
	man->Book();
  
}

//============================================================================

void RunAction::EndOfRunAction(const G4Run* run)
{

	
  G4int nofEvents = run->GetNumberOfEvent();
  //if (nofEvents == 0) return;
  
  //particle name
  G4String partName;
  const PrimaryGeneratorAction* generatorAction
    = static_cast<const PrimaryGeneratorAction*>(
        G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  if (generatorAction) 
  {
    G4ParticleDefinition* particle 
      = generatorAction->GetParticleGun()->GetParticleDefinition();
    partName = particle->GetParticleName();
  }  
  G4double energy = generatorAction->GetParticleGun()->GetParticleEnergy();
  
  //const RunData* localRun = static_cast<const RunData*>(run);

  // save Rndm status
  if ( fSaveRndm ) { 
	  G4int runNumber = run->GetRunID();
	  std::ostringstream rndmFileName;
	  rndmFileName << "run" << runNumber << "End.rndm";
	  G4Random::saveEngineStatus(rndmFileName.str().c_str()); 
  }  

  time_t end = time(0);
  time_t dur = end - start;
  G4cout << "\n@@@Run_duration " << dur << " sec;End: "  << ctime(&end)
  << "\n Number of events: " << nofEvents 
  << "\n @@@Particle: " << partName << " :: " << energy/MeV << " MeV "
  <<  G4endl;
  
  G4Random::showEngineStatus();
  // Save histograms
  Analysis* man = Analysis::GetInstance();
  man->Finish();
  
}
