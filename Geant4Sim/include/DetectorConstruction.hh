// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class VBaseDetector;
class G4GeometrySampler;
class DetectorMessenger; //
//class G4VSensitiveDetector;


class DetectorConstruction: public G4VUserDetectorConstruction
{
    public:
    	DetectorConstruction();
    	virtual ~DetectorConstruction();
			
    	virtual G4VPhysicalVolume* Construct();    
        virtual void ConstructSDandField();

        void SetDetector(const G4String&);
		inline void SetCheckOverlaps(G4bool val) { fCheckOverlaps = val; }
		inline void SetWorldHalfLen(G4double len) { fHalfLen = len; }
		//inline G4LogicalVolume* GetSDvolume(){ return fDetectorSD;}

        void SetImportanceSampling();
        
  private:
		DetectorConstruction( const DetectorConstruction&);
		DetectorConstruction& operator=( const DetectorConstruction&);
	  
        DetectorMessenger*     fDetMessenger;   
		VBaseDetector*         fTheDetector;		
		G4VSensitiveDetector*  fSDdetector; 
		G4LogicalVolume*	   fLogVol;
		G4VPhysicalVolume*     fWorldPhysical;
		G4String               fDetName; //for setdetector
		G4bool                 fCheckOverlaps;
		G4double               fHalfLen;
        
        G4GeometrySampler *fGeometrySampler;
        
};

#endif
