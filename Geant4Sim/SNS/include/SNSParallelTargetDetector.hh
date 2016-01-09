// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 




//SNSParallelTargetDetector.hh

#ifndef SNSPARALLELTARGETDETECTOR_HH
#define SNSPARALLELTARGETDETECTOR_HH

//---------------------------------------------------------------------------//


#include "G4VUserParallelWorld.hh"
#include "globals.hh"

#include <map>
#include <vector>
//#include "G4GeometryCell.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4IStore;
class G4VIStore;
class G4VWeightWindowStore;
class SNSParallelMessenger;


//---------------------------------------------------------------------------//

class SNSParallelTargetDetector: public G4VUserParallelWorld
{
	
public:
    enum EMGGeometryImportanceConsts {kMinImpValue = 1};
    
	SNSParallelTargetDetector(G4String);
	virtual ~SNSParallelTargetDetector();
	
	// overloaded from G4VUserParallelWorld:
	virtual void Construct();
	//virtual void ConstructSD();
    
    //G4VIStore* 
    void CreateImportanceStore(); //caller should delete it
    //G4VWeightWindowStore *CreateWeightWindowStore();//caller should delete it
    //see B02ImportanceDetectorConstruction

    //void CreateImportanceStore();
    void SetImportanceValueForRegion(G4VPhysicalVolume*,
                        G4double = kMinImpValue, G4int aRepNum = 0);
    G4VPhysicalVolume* GetWorldVolume() const { return fParallelWorld; }
    G4VPhysicalVolume& GetConstWorldVolumeAddress() const { return *fParallelWorld; }
    void SetNumVolumes( G4int );
    void SetIsVariableStep ( G4bool);
	
private:
	SNSParallelTargetDetector(const SNSParallelTargetDetector&);
	SNSParallelTargetDetector& operator=(const SNSParallelTargetDetector&);
	SNSParallelMessenger *fMessenger;
	G4VPhysicalVolume* fParallelWorld;
	G4bool fIsVariableStep;	
	G4int fNumVol;
    std::vector< G4LogicalVolume * > fLogicalVolumeVector;
	std::map<G4VPhysicalVolume* , std::pair<G4double, G4int>* > fImportanceMap;
};
//
#endif