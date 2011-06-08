/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#ifndef SciFiHit_h
#define SciFiHit_h 1

#include "CLHEP/Vector/ThreeVector.h"

#include "MCHit.hh"

#include "Memory.hh"

/**
* SciFiHit is a description of a simulated hit in a single doublet layer of Scintillating Fibre detector.
**/

class SciFiHit : public MCHit
{
  public:

      //! Default constructor
      SciFiHit()		{ miceMemory.addNew( Memory::SciFiHit ); };

      //! Default destructor
     ~SciFiHit()		{ miceMemory.addDelete( Memory::SciFiHit ); };

      //! Copy constructor
      SciFiHit(const SciFiHit&);

      //! Assignment operator
      const SciFiHit& operator=(const SciFiHit&);

      //! Is equal to operator 
      int operator==(const SciFiHit&) const;

      //! Dump some information about this hit to the standard output
      void Print();

  public:

      void mergeHit( const SciFiHit* );

      void SetTrackID   ( int tid )       { setTrackID( tid ); };
      void SetFiberNo   ( int fn)         { fiberNo = fn; }
      void SetPlaneNo   ( int pn)         { planeNo = pn; }
      void SetStationNo ( int sn)         { stationNo = sn; }
      void SetTrackerNo ( int tn)         { trackerNo = tn; }
      void SetEdep      ( double de)      { setEdep( de ); };
      void SetPos       ( Hep3Vector xyz) { setPosition( xyz ); };
      void SetMom       ( Hep3Vector xyz) { setMomentum( xyz ); };
      void SetVolumeName( std::string vn)      { volumeName = vn; }
      void SetTime      ( double ti)      { setTime( ti ); };
      void SetEnergy    ( double en)      { setEnergy( en ); };
      void SetPID       ( int id )        { setPdg( id ); };
      void SetCharge    ( double ch)      { setCharge( ch ); };
      void SetMass      ( double ma)      { setMass( ma ); };
      
      int GetTrackID() const     	{ return trackID(); };
      int GetFiberNo() const     	{ return fiberNo; }
      int GetPlaneNo() const      	{ return planeNo; }
      int GetStationNo() const   	{ return stationNo; }
      int GetTrackerNo() const   	{ return trackerNo; }
      double GetEdep() const     	{ return Edep(); };
      Hep3Vector GetPos() const 	{ return position(); };
      Hep3Vector GetMom() const 	{ return momentum(); };
      std::string GetVolumeName() const	{ return volumeName; }
      double GetTime() const     	{ return time(); };
      double GetEnergy() const   	{ return energy(); };
      int    GetPID() const      	{ return pdg(); };
      double GetCharge() const   	{ return charge(); };
      double GetMass() const     	{ return mass(); };
      
  private:
  
      std::string      volumeName;

      int         trackerNo;
      int         stationNo;
      int         planeNo;
      int         fiberNo;
};

#endif
