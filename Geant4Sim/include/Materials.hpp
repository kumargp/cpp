

#ifndef MATERIALS_HH
#define MATERIALS_HH

#include <map>
#include "globals.hh"
#include "G4Material.hh"


class Materials
{
    public:
        ~Materials();

        static G4Material* GetMaterial(G4String);
		static void DefineMaterials();
		
    private:
		Materials(); //constructor
		Materials(const Materials &) {;}
		Materials &operator=(const Materials &rhs);
		int operator==(const Materials &rhs) const;
		int operator<(const Materials &rhs) const;
		
		static std::map <G4String, G4Material*> mList;
		static G4bool fInitialized;
    
};


#endif