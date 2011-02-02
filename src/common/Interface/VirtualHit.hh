#ifndef _VIRTUALHIT_HH_
#define _VIRTUALHIT_HH_
//----------------------------------------------------------------------------
//
//     File :              VirtualHit.hh
//     Purpose :           Variables used by VirtualHit.  This represents
//                         a sort of generic hit.
//     Created :           29-Sep-2002  by Steve Kahn
//
//----------------------------------------------------------------------------

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <string>

#include "Memory.hh"

#include "MCHit.hh"

typedef Hep3Vector  ThreeVector;

class VirtualHit : public MCHit
{
  private :

    int          _stationNumber;  //  ID assigned to location
    ThreeVector  _bField;         //  magnetic field
    ThreeVector  _eField;         //  electric field

  public:

    VirtualHit();

    ~VirtualHit() 		{ miceMemory.addDelete( Memory::VirtualHit); };

    VirtualHit(const VirtualHit& rhs);

    const VirtualHit& operator=(const VirtualHit& rhs);
    int operator==(const VirtualHit& rhs) const;


    void SetTrackID   (int tid)        { setTrackID( tid ); }
    void SetStationNumber (int sn)         { _stationNumber = sn; }
    void SetPos  (Hep3Vector xyz){ setPosition( xyz ); }
    void SetMomentum  (Hep3Vector xyz){ setMomentum( xyz ); }
    void SetTime      (double ti)      { setTime( ti ); }
    void SetEnergy    (double en)      { setEnergy( en ); }
    void SetPID       (int id)         { setPdg( id ); }
    void SetMass      (double ma)      { setMass( ma ); }
    void SetCharge    (double ch)      { setCharge( ch ); }
    void SetBField     (Hep3Vector b)  {  _bField = b; }
    void SetBField     ( double b[3]) { _bField.setX( b[0] ); _bField.setY( b[1] ); _bField.setZ( b[2] ); };
    void SetEField     (Hep3Vector e)  {  _eField = e; }
    void SetEField     ( double e[3]) { _eField.setX( e[0] ); _eField.setY( e[1] ); _eField.setZ( e[2] ); };

    int GetTrackID() const     { return trackID(); }
    int GetStationNumber() const   { return _stationNumber; }
    Hep3Vector GetPos() const  { return position(); }
    Hep3Vector GetMomentum() const { return momentum(); }
    double GetTime() const     { return time(); }
    double GetEnergy() const   { return energy(); }
    int    GetPID() const      { return pdg(); }
    double GetMass() const     { return mass(); }
    double GetCharge() const   { return charge(); }
    Hep3Vector GetBField() const {  return _bField; }
    Hep3Vector GetEField() const {  return _eField; }

};
#endif
