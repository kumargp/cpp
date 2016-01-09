
#ifndef SNSTARGETOuterPROTONBOX_HH 
#define SNSTARGETOuterPROTONBOX_HH

#include "VBaseDetPartAssembly.hh"
#include "globals.hh"
#include "G4Colour.hh"


class G4LogicalVolume;
class G4SubtractionSolid;

class SNSTargetOuterProtonBox : public VBaseDetPartAssembly
{
  public:
    SNSTargetOuterProtonBox(G4String, G4double, G4double, G4double, 
		G4bool, G4bool, G4double, G4double, G4double, G4double, 
		G4double, G4bool, G4double, G4double, G4bool);
    virtual ~SNSTargetOuterProtonBox();
	
	void Place(G4RotationMatrix* rotAssem, G4ThreeVector* transAssem, 
                                    G4LogicalVolume* motherLogVol);
	G4SubtractionSolid* SubMultiStepDet(G4SubtractionSolid* fromSolid, 
		G4String name, G4ThreeVector pos, G4RotationMatrix* rot);
	void PlaceMagnet(const G4String namePart, const G4double xBeamInHalf, const G4double magnetHalfL, 
		const G4double magCentRad, G4ThreeVector magnetPos, G4RotationMatrix* rotAssem,
		G4ThreeVector* transAssem, G4LogicalVolume* motherLogVol);
	
  private:
	  
	  SNSTargetOuterProtonBox(const SNSTargetOuterProtonBox&);
	  SNSTargetOuterProtonBox& operator=(const SNSTargetOuterProtonBox&);
	  
    G4double    fX;
    G4double    fY;
    G4double    fZ;
    G4bool      fMSDet;
	G4bool		fMStepDet2;
	G4double	fMS1Inner; 
	G4double	fMS2Inner; 
	G4double 	fMSthick; 
	G4double	fMSHht;
	G4double	fPosMSDetZ;
	G4bool		fMagnet;	
	G4double 	fMagnetCutHalfL;
	G4double	fMagnetCutHZ;
	G4bool		fOverlap;
};
 
#endif