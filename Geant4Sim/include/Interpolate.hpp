
#ifndef INTERPOLATE_HH
#define INTERPOLATE_HH

class Interpolate
{
  public:
      Interpolate();
      ~Interpolate();
      
      Spline(const G4double&, const G4double&);
      G4double SplineValue(const G4double);
      G4double Integrate();
      void DHeapSort(G4double&, G4double&);
      void DRemakeHeap(G4double&, G4double&, G4int,G4int);
  
  private:
      G4int n, last_interval;
      G4double* x, f, b, c, d;
      static G4bool uniform;
      G4bool debug;
  
};

#endif