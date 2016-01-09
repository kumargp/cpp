// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef MATERIALS_HH
#define MATERIALS_HH

#include <map>
#include "globals.hh"
#include "G4Material.hh"


class Materials
{
    public:
		Materials(); //constructor
		~Materials();	
		void DefineMaterials();
		Materials(const Materials&);
		Materials& operator=(const Materials&);   
};


#endif