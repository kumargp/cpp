// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 




#ifndef SNSTARGETDETECTOR_HH 
#define SNSTARGETDETECTOR_HH

#include "VBaseDetector.hh"
#include "G4LogicalVolume.hh"
#include <vector>

//#include "SNSTargetDetectorMessenger.hh"


class G4LogicalVolume;
class G4VPhysicalVolume;
class G4SubtractionSolid;

class SNSTargetDetector :  public VBaseDetector
{

public:
    SNSTargetDetector(G4String );
    virtual ~SNSTargetDetector();
    virtual void ConstructDet();
	
	G4SubtractionSolid* SubMultiStepDet(G4SubtractionSolid* fromSolid, 
					G4String name, G4ThreeVector pos, G4RotationMatrix* rot);
	void CreateInnerNTube( G4double nTubeHalfDx1, 
		G4double nTubeHalfDx2, G4double nTubeHalfDy1, G4double nTubeHalfDy2,
		G4double nTubeHalfDz, G4double thickNTubeAlX, G4double thickNTubeAlY,
		G4RotationMatrix* rotNTube, G4ThreeVector posNTube, G4String tubeName,
		G4double outerSteelR2, G4double outerSteelHt, G4bool overlap);

	std::vector <G4double> GetPosNOuter( G4int i, G4double dT, G4double lH3posX, 
			G4double lH3posY, G4double lH1halfX, G4double nBeamOuterZhLen, 
			G4double lH1posY, G4double lH1Rot, G4double nBeamOuterStart  );

private:
	SNSTargetDetector(const SNSTargetDetector&);
	SNSTargetDetector& operator=(const SNSTargetDetector&);	
	
  //SNSTargetDetectorMessenger*  fMessenger;
	G4double	fMSDet1Inner;
	G4double	fMSDet2Inner;
	G4double	fMSDetCylThick; 
	G4double	fMSDetCylHht;
	G4bool		fIsMSDet2; 
};

#endif
