// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

#ifndef ANALYSIS_HH
#define ANALYSIS_HH

#include "globals.hh"
#include "G4ios.hh"
#include "g4root.hh"
#include <fstream>

class G4Step;

class Analysis
{
    public:
      ~Analysis();
        
        
    public:
      void Book(); // Create the root file of ntuples, histo
      void Finish();
      
	  void FillPartE( G4double,G4double, G4double, G4double, G4double, G4double,
						 G4int, G4int, G4int, G4int, G4int);
	  void FillProcessE(G4double, G4double, G4double, G4double, G4double, G4double, 
						G4double, G4double, G4double, G4double, G4double, G4double, 
					   G4double, G4double, G4double, G4int);
      void FillCrProcessE(G4double, G4double, G4double, G4double, G4double, G4double, 
                        G4double, G4double, G4double, G4double, G4double, G4double, 
                       G4double, G4double, G4double, G4int); 
                       
	  void FillPerStep( G4double, G4double, G4double, G4double, G4double, 
						G4double, G4double, G4double, G4int, G4int );
      void WriteAllSteps(const G4Step*);

	  void WriteAscii( G4double,G4double, G4double, G4double, G4double, G4double,
					 G4int, G4int, G4int,G4int, G4int);	  
	  
	  void FillNFluxDet(G4double, G4double, G4double,  G4double , G4double, G4int);
	  void FillNFluxOther(G4double, G4double, G4double,  G4double , G4double, G4int);
	  void FillEdep(G4double, G4int, G4double, G4double, G4double, G4double, G4double,
		G4int, G4int, G4int, G4int, G4double, G4double, G4int, G4int, G4double, G4int);
	  
	  
      static Analysis* GetInstance();
	  inline void SetFileNamePattern(G4String part)
	  { 
		if(part.find(".") != std::string::npos) {
			histFileName = part+".root";
		}
		else
		{
			histFileName = part;
		}
        textFileName = "Hits_" + part + ".txt";
      }
	  
      
    private:
      Analysis();
	  Analysis(const Analysis& rhs);             
	  Analysis& operator=(const Analysis& rhs);  
      static Analysis* fInstance;
      
      G4String textFileName;
      G4String histFileName;
      
      static std::ofstream *textFile;     

};

#endif