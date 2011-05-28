// MAUS WARNING: THIS IS LEGACY CODE.
/* Ckovhit.hh
*/
#ifndef CkovHit_h
#define CkovHit_h 1

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "Interface/MCHit.hh"

/**
* CkoviHit is a description of a simulated hit in  the aerogel radiator of the CKOV detector
**/

class CkovHit : public MCHit
{
  public:

      //! Default constructor
  CkovHit()		{};

      //! Default destructor
  ~CkovHit()		{};

      //! Copy constructor
      CkovHit(const CkovHit&);

      //! Assignment operator
      const CkovHit& operator=(const CkovHit&);

      //! Dump some information about this hit to the standard output

  public:

      void DecodeVolumeName();

      void SetTrackID   ( int tid )        { setTrackID( tid ); };
      void SetPmtNo     ( int pmt )        { PmtNo = pmt; }
      void SetVesselNo  ( int Vessel )     { VesselNo = Vessel; }
      void SetPlaneNo   ( int Vessel )     { VesselNo = Vessel; }
      void SetAngle     ( double angle )   { IncidentAngle = angle; }
      void SetPAngle     ( double angle )  { PolarizationAngle = angle; }
      void SetEdep      ( double de )      { setEdep( de ); };
      void SetWavelenght( double wl )      { Wavelenght = wl; };
      void SetPol       ( Hep3Vector  pol ){ Polarization = pol; };

      void SetPos       ( Hep3Vector xyz) { setPosition( xyz ); };
      void SetMom       ( Hep3Vector xyz) { setMomentum( xyz ); };
      void SetVolumeName( std::string vn) { volumeName = vn; }
      void SetTime      ( double ti)      { setTime( ti ); };
      void SetEnergy    ( double en)      { setEnergy( en ); };
      void SetPID       ( int id )        { setPdg( id ); };
      void SetCharge    ( double ch)      { setCharge( ch ); };
      void SetMass      ( double ma)      { setMass( ma ); };

      int GetTrackID() const     { return trackID(); };
      int GetPmtNo() const      	{ return PmtNo; }
      int GetVesselNo() const    { return VesselNo; }
      int GetPlaneNo() const     { return VesselNo; }
      double GetAngle() const    { return IncidentAngle; }
      double GetPAngle() const   { return PolarizationAngle; }
      double GetWavelenght() const { return Wavelenght; };
      Hep3Vector GetPol() const { return Polarization;}

      double GetEdep() const     	{ return Edep(); };
      Hep3Vector GetPos() const 	{ return position(); };
      Hep3Vector GetMom() const 	{ return momentum(); };
      std::string GetVolumeName() const	{ return volumeName; }
      double GetTime() const     	{ return time(); };
      double GetEnergy() const   	{ return energy(); };
      int    GetPID() const      	{ return pdg(); };
      double GetCharge() const   	{ return charge(); };
      double GetMass() const     	{ return mass(); };

      void Print();

  private:
  
      std::string      volumeName;

      int         PmtNo;
      int         VesselNo;
      double      Wavelenght;
      double      IncidentAngle;
      double      PolarizationAngle;
      Hep3Vector  Polarization;
};

#endif
