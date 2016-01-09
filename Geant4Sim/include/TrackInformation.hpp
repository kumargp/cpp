#ifndef TRACKINFORMATION_HH
#define TRACKINFORMATION_HH

#include "G4VUserTrackInformation.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"


class TrackInformation : public G4VUserTrackInformation 
{
	public:
		TrackInformation();
  		TrackInformation(const G4Track* aTrack);
		TrackInformation(const TrackInformation& aTrackInfo);
  		TrackInformation(const TrackInformation* aTrackInfo);
  		virtual ~TrackInformation();
   
  		inline void *operator new(size_t);
  		inline void operator delete(void *aTrackInfo);

  		TrackInformation& operator =(const TrackInformation& right);
  
  		void SetSourceTrackInformation(const G4Track* aTrack);
  		virtual void Print() const;

	public:
  		inline G4int GetTrackingStatus() const {return fTrackingStatus;}
  		inline void  SetTrackingStatus(G4int i) {fTrackingStatus = i;}
  		inline G4int GetSourceTrackID() const {return fSourceTrackID;}

	private:
  		G4int                 fOriginalTrackID;
  		G4ParticleDefinition* fParticleDefinition;
  		G4ThreeVector         fOriginalPosition;
  		G4ThreeVector         fOriginalMomentum;
  		G4double              fOriginalEnergy;
		G4double              fOriginalTime;

  		G4int                 fTrackingStatus;
  		G4int                 fSourceTrackID;
  		G4ParticleDefinition* fSourceDefinition;
  		G4ThreeVector         fSourcePosition;
  		G4ThreeVector         fSourceMomentum;
  		G4double              fSourceEnergy;
  		G4double              fSourceTime;

};

extern G4ThreadLocal G4Allocator<TrackInformation> * aTrackInformationAllocator;
//extern  G4Allocator<TrackInformation> aTrackInformationAllocator;

inline void* TrackInformation::operator new(size_t)
{
  if(!aTrackInformationAllocator)
    aTrackInformationAllocator = new G4Allocator<TrackInformation>;
  return (void*)aTrackInformationAllocator->MallocSingle();
}

inline void TrackInformation::operator delete(void *aTrackInfo)
{ aTrackInformationAllocator->FreeSingle((TrackInformation*)aTrackInfo);}

#endif
