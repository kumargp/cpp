// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 




#include "SNSDetector.hh"

//#include "SNSDetectorMessenger.hh"
#include "SNSHousingAl.hh"
#include "SNSScintillator.hh"
#include "G4RunManager.hh" //?
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

SNSDetector::SNSDetector(G4String detName)
:VBaseDetector(detName)//, fStepLimit(NULL)
{ 
	G4cout << "constructed SNSDet" <<G4endl;
    //fMessenger = new SNSDetectorMessenger();
}


SNSDetector::~SNSDetector()
{
    //delete fMessenger;
    //delete fStepLimit;
}

void SNSDetector::ConstructDet()
{

	//check if already constructed ???
    
	G4NistManager* nist = G4NistManager::Instance();
	
	G4double halfX = 0.50*m; //set messenger
	G4double halfY = 0.50*m;
	G4double halfZ = 0.50*m;
	
	//World
	G4Box* snsBox = new G4Box("SNSScintDetBox", halfX, halfY, halfZ);
	//G4Material* expMat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material *expMat = nist->FindOrBuildMaterial("G4_Galactic");
    
	G4String detLogName = "SNSScintDetLogical";

	fDetLogical	= new G4LogicalVolume(snsBox, expMat, detLogName );

  	SNSHousingAl* housing = new SNSHousingAl("HousingAl");
	G4ThreeVector* posHousing = new G4ThreeVector(0, 0, 0); // change it
	G4RotationMatrix* rotHousing = new G4RotationMatrix(); //change it
	housing->Place(rotHousing, posHousing, fDetLogical);


	SNSScintillator *scint = new SNSScintillator("LiquidScint");
	G4ThreeVector* posScint = new G4ThreeVector(0, 0, 0); // change it
	G4RotationMatrix* rotScint = new G4RotationMatrix(); //change it
	scint->Place(rotScint, posScint, fDetLogical);//housing->GetPartLog());
    
    //Apply steplimits
    /*
    G4double volWidth = scint->GetStepLimitWidth();
    G4double maxStep = 0.5*volWidth;
    fStepLimit = new G4UserLimits(maxStep);
    scint->GetPartLog()->SetUserLimits(fStepLimit);
  */
}
