

#ifndef MYMATERIALS_HH
#define MYMATERIALS_HH

#include <map>
#include "globals.hh"
#include "G4Material.hh"


class MyMaterials
{
    public:
		MyMaterials(); //constructor
        ~MyMaterials();

		void DefineMaterials();
		
    private:
		MyMaterials (const MyMaterials&);
		MyMaterials& operator= (const MyMaterials&);

    
};


#endif