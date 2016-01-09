// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef SSNSDETECTOR_HH
#define SSNSDETECTOR_HH

#include "VBaseDetector.hh"
#include "G4LogicalVolume.hh"

#include "SNSDetectorMessenger.hh"


class SNSDetector: public VBaseDetector
{
    public:
        SNSDetector(G4String );
        virtual ~SNSDetector();
        
		virtual void ConstructDet();

    private:
		SNSDetector (const SNSDetector&);
		SNSDetector& operator= (const SNSDetector&);
		//SNSDetectorMessenger*  fMessenger;
        
};

#endif 
