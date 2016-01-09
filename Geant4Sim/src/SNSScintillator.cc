// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 



#include "VBaseDetectorPart.hh"
#include "SNSScintillator.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "Materials.hh"
#include "G4NistManager.hh"// 
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


SNSScintillator::SNSScintillator(G4String physName) 
: VBaseDetectorPart(physName)
{;}


SNSScintillator::~SNSScintillator()
{;}


G4LogicalVolume* SNSScintillator::ConstructPart()
{
	G4String logName = fPartPhysName + "Log";
    G4LogicalVolume* partLog = G4LogicalVolumeStore::GetInstance()
				->GetVolume(logName, false);
	if(! partLog)
	{
		G4double thickAl 		= 1.252*mm;
		G4double length 		= 137.*mm; 
		G4double lengthAl      = 167.4*mm;
		G4double bottomRad     = 43.25 *mm;
		G4double zPositions[] 	= {-1/2.*lengthAl +thickAl, 1/2.*length };
		G4double rInner[] 		= {0., 0.};
		G4double rOuter[]		= {bottomRad-thickAl, 60.*mm}; //43.25+tan(6.94)*167  - tan(6.94)*23
		G4Polyhedra * scint 	= new G4Polyhedra("Scintillator", 0, 2*pi, 6, 2, 
												  zPositions, rInner, rOuter);

		//get material
		G4Material* material = G4NistManager::Instance()->FindOrBuildMaterial("EJ301");
		partLog = new G4LogicalVolume(scint, material, logName);
	}
	return partLog;
}

