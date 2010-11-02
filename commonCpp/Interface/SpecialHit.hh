//-----------------------------------------------------------------------------
/*
** MICE special virual detector for the MICE proposal
** This hit bank is designed to be functionallly similar to the SciFiHit
** bank by E. McKigney for the simulation
** Steve Kahn - Sept 16, 2002
** Re-written for re-design, M. Ellis, October 2005
*/
//-----------------------------------------------------------------------------
#ifndef _SPECIALHIT_hh
#define _SPECIALHIT_hh 1

#include "CLHEP/Vector/ThreeVector.h"
#include <string>
#include <string.h>

#include "MCHit.hh"
#include <iostream>

#include "Memory.hh"

class SpecialHit : public MCHit
{
  public:

      SpecialHit();

      ~SpecialHit()		{ miceMemory.addDelete( Memory::SpecialHit ); };

      SpecialHit(const SpecialHit&);

      const SpecialHit& operator=(const SpecialHit&);

      int operator==(const SpecialHit&) const;

      void addEdep(double);

      void DecodeVolumeName();

      void SetTrackID      (int tid)        { setTrackID( tid ); }
      void SetStationNo    (int sn)         { _stationNumber = sn; }
      void SetCellNo       (int cn)         { _cellNumber = cn; }
      void SetPID          (int id)         { setPdg( id ); }
      void SetParenTrackID (int tid)        { _parentrackID = tid; }
      void SetAncesTrackID (int tid)        { _ancestrackID = tid; }
      void SetStepType     (int tid)        { _stepType = tid; }
      void SetPosition     (Hep3Vector xyz) { setPosition( xyz ); }
      void SetMomentum     (Hep3Vector xyz) { setMomentum( xyz ); }
      void SetBField       (Hep3Vector b)   { _bfield = b; }
      void SetEField       (Hep3Vector e)   { _efield = e; }
      void SetVtxMomentum  (Hep3Vector vp)  { _vtxmom = vp; }
      void SetVtxPosition  (Hep3Vector vtx) { _vtxpos = vtx; }
      void SetVolumeName   (std::string vn) { _volName = vn; }
      void SetEdep         (double edep)    { setEdep( edep ); }
      void SetTime         (double ti)      { setTime( ti ); }
      void SetEnergy       (double en)      { setEnergy( en ); }
      void SetMass         (double ma)      { setMass( ma ); }
      void SetCharge       (double ch)      { setCharge( ch ); }
      void SetBField       (double b[3]);
      void SetEField       (double e[3]);
      void SetVtxTime      (double vt)      { _vtxtime = vt; }
      void SetStepLength   (double l)       { _stepLength = l; }
      void SetPathLength   (double l)       { _pathLength = l; }
      void SetProperTime   (double t)       { _propTime   = t; }
      void SetCellNo       (int iz, int iaz, int ir);

      int         GetTrackID()      const { return trackID(); }
      int         GetStationNo()    const { return _stationNumber; }
      int         GetCellNo()       const { return _cellNumber; }
      int         GetPID()          const { return pdg(); }
      int         GetZnumber()      const { return _iax; }
      int         GetRnumber()      const { return _irad; }
      int         GetPhiNumber()    const { return _iaz; }
      int         GetParenTrackID() const { return _parentrackID; }
      int         GetAncesTrackID() const { return _ancestrackID; }
      int         GetStepType()     const { return _stepType; }
      Hep3Vector  GetPosition()     const { return position(); }
      Hep3Vector  GetMomentum()     const { return momentum(); }
      Hep3Vector  GetBField()       const { return _bfield; }
      Hep3Vector  GetEField()       const { return _efield; }
      Hep3Vector  GetVtxPosition()  const { return _vtxpos; }
      Hep3Vector  GetVtxMomentum()  const { return _vtxmom; }
      std::string GetVolumeName()   const { return _volName; }
      double      GetTime()       const   { return time(); }
      double      GetEnergy()     const   { return energy(); }
      double      GetMass()       const   { return mass(); }
      double      GetCharge()     const   { return charge(); }
      double      GetEdep()       const   { return Edep(); }
      double      GetVtxTime()    const   { return _vtxtime; }
      double      GetStepLength() const   { return _stepLength; }
      double      GetPathLength() const   { return _pathLength; }
      double      GetProperTime() const   { return _propTime; }

  private :

     int  stationIDfromName( std::string ) const;

     int     _iax, _iaz, _irad; // cell numbers along axial, azimuthal
                                // and radial directions
     int     _stepType;   //  0 = internal, +1 = entering volume,
                          // -1 = leaving, +2 = single step across
     double  _stepLength; // size of the step

     int _stationNumber;
     int _cellNumber;
     std::string _volName;
     Hep3Vector _bfield;
     Hep3Vector _efield;
     int    _parentrackID;
     int    _ancestrackID;
     Hep3Vector _vtxpos;
     double _vtxtime;
     Hep3Vector _vtxmom;
     double     _pathLength;
     double     _propTime;
};

inline void SpecialHit::SetCellNo(int iz, int iphi, int ir)
{   _iax = iz;    _iaz = iphi;   _irad = ir; }

#endif
