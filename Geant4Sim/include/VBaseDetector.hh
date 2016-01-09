// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



//VBaseDetector.hh

#ifndef VBASEDETECTOR_HH
#define VBASEDETECTOR_HH

#include "globals.hh"

class G4LogicalVolume;

//All the detectors have to be derived from this baseclass, so that they can be
//selected and constructed in the DetectorConstruction class.



class VBaseDetector
{
    public:
        VBaseDetector(G4String );
        virtual ~VBaseDetector();
		
		VBaseDetector (const VBaseDetector& );
		VBaseDetector& operator= (const VBaseDetector& );	
	
   public:
        //construct geometry.
		virtual void ConstructDet() {;}

        //add set methods
        inline G4LogicalVolume *GetDetLog() { return fDetLogical; }
        inline G4String GetDetName() {return fTheDetName;}
        inline G4LogicalVolume *GetSDLog() { return fSDLogVol; }       
          
    protected:
	
        G4LogicalVolume* fDetLogical;
        G4LogicalVolume* fSDLogVol;
      
    private:
        G4String fTheDetName;
};

#endif