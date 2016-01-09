// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "VBaseDetector.hh"
#include "globals.hh"
#include "G4LogicalVolume.hh"


VBaseDetector::VBaseDetector(G4String detName)
:fDetLogical(NULL), fSDLogVol(NULL), fTheDetName(detName)
{;}


VBaseDetector::~VBaseDetector()
{;}


VBaseDetector::VBaseDetector(const VBaseDetector& rhs)
:fDetLogical(rhs.fDetLogical),fSDLogVol(rhs.fSDLogVol),
 fTheDetName(rhs.fTheDetName)
{;}


VBaseDetector& VBaseDetector::operator=(const VBaseDetector& rhs)
{
	if(this != &rhs)
	{
		fDetLogical = rhs.fDetLogical;
		fSDLogVol	= rhs.fSDLogVol;
		fTheDetName	= rhs.fTheDetName;
	}
	return *this;
}