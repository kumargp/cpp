// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef SNSHOUSINGAL_HH
#define SNSHOUSINGAL_HH

#include "VBaseDetectorPart.hh"
#include "globals.hh"

class G4LogicalVolume;


class SNSHousingAl : public VBaseDetectorPart
{
	
	public:
		SNSHousingAl(G4String);
		virtual ~SNSHousingAl();

	
		virtual G4LogicalVolume* ConstructPart();

	private:
		SNSHousingAl(const SNSHousingAl& );
		SNSHousingAl& operator=(const SNSHousingAl& );		
	
};

#endif