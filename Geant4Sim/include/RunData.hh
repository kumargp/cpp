// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


//RunData.hh


#ifndef RUNDATA_HH
#define RUNDATA_HH

#include "G4Run.hh"
#include "globals.hh"

class G4Event;

//----------------------------------------------------------------------------

class RunData : public G4Run
{
	public:
    	RunData();
    	virtual ~RunData();

    	// method from the base class
    	virtual void RecordEvent(const G4Event*);
    	virtual void Merge(const G4Run*);
		inline void  SetPrintModulo(G4int num) { fPrintModulo = num;}
		inline G4int GetPrintModulo() {return fPrintModulo;}

	private:
		RunData( const RunData& );
		RunData& operator=( const RunData& );
		
		G4int fPrintModulo;

};

//---------------------------------------------------------------------------
#endif