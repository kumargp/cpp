// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


// RunAction.hh


#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

//class G4LogicalVolume;

//----------------------------------------------------------------------


class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();
	
  private:
	RunAction(const RunAction&);
	RunAction & operator =(const RunAction&);	
	
 public:	
    virtual G4Run* GenerateRun();
    virtual void  BeginOfRunAction(const G4Run*);
    virtual void  EndOfRunAction(const G4Run*);

    // set methods
    void  SetRndmFileName(const G4String& fileName);
    void  SetVerboseLevel(G4int level);
    void  SetSaveRndm(G4bool value);
    void  SetReadRndm(G4bool value);
    void  SetAutoSeed(G4bool value);
    inline static void CountParticles(){ nNeutronsRun++;}
    inline static size_t GetParticles(){ return nNeutronsRun;}

    //inline void SetSeeds(G4ThreeVector seeds) 
    //{
    //    fIndex = seeds.x(); fSeed0 = seeds.y(); fSeed1 = seeds.z();
    //}

  private:
    G4bool      fSaveRndm;
    G4bool      fReadRndm;
    G4long      fSeed0;
    G4long      fSeed1;
    G4long      fIndex;
    G4String    fRndmFileName;
    G4int       fVerboseLevel;
    time_t      start;
    static size_t  nNeutronsRun;
};


//------------------------------------------------------------------------
// inline functions

inline void RunAction::SetVerboseLevel(G4int level) {
  fVerboseLevel = level;
}  

inline void RunAction::SetRndmFileName(const G4String& fileName) {
  fRndmFileName = fileName;
}  

inline void RunAction::SetSaveRndm(G4bool value) {
  fSaveRndm = value;
}

inline void RunAction::SetReadRndm(G4bool value) {
  fReadRndm = value;
}

#endif