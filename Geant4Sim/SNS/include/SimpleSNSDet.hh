// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 




#ifndef SIMPLESNSDET_HH 
#define SIMPLESNSDET_HH

#include "VBaseDetector.hh"
#include "G4LogicalVolume.hh"
#include <vector>

//#include "SNSTargetDetectorMessenger.hh"


class G4LogicalVolume;
class G4VPhysicalVolume;
class G4SubtractionSolid;

class SimpleSNSDet :  public VBaseDetector
{

public:
    SimpleSNSDet(G4String );
    virtual ~SimpleSNSDet();
    virtual void ConstructDet();
	
	G4SubtractionSolid* SubMultiStepDet(G4SubtractionSolid* fromSolid, 
					G4String name, G4ThreeVector pos, G4RotationMatrix* rot);

private:
	SimpleSNSDet(const SimpleSNSDet& );
	SimpleSNSDet& operator= (const SimpleSNSDet& );
	
  //SNSTargetDetectorMessenger*  fMessenger;
	G4double	fMSDet1Inner;
	G4double	fMSDet2Inner;
	G4double	fMSDet3Inner;
	G4double	fMSDetCylThick; 
	G4double	fMSDetCylHht;
	G4bool		fIsMSDet2; 
	G4bool 		fIsMSDet3;
};

#endif
