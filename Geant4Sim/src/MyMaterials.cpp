

//Materials.cc

#include "globals.hh"
#include "MyMaterials.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Isotope.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"



MyMaterials::MyMaterials()
{
    DefineMaterials();
}

MyMaterials::~MyMaterials()
{
}
   

void MyMaterials::DefineMaterials()
{

    G4Element* elDD  = new G4Element("Deut", "DD" , 1., 2.*g/mole);
    G4Element* elOO = new G4Element("Oxy", "OO", 8., 16.00*g/mole);

    // define material for Be plugs - Be(90%)+D2O(10%)
    G4Material* d2O = new G4Material("D2O2",1.1*g/cm3, 2);
    d2O->AddElement(elDD,2);
    d2O->AddElement(elOO,1);
 
 }