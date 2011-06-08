#ifndef _TOFHIT_HH_
#define _TOFHIT_HH_
//--------------------------------------------------------------------------
//
//    File :      TofHit.hh
//    Purpose :   Variable bank for TOF hits
//    Created :   17-SEP-2002  by Steve Kahn and Kevin Lee
//
//--------------------------------------------------------------------------

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <cstdio>
#include <fstream>
#include <string>

#include "MCHit.hh"

#include "Memory.hh"

typedef Hep3Vector ThreeVector;

class TofHit : public MCHit
{
   	public:

    TofHit();

    ~TofHit()		{ miceMemory.addDelete( Memory::TofHit ); };

    TofHit(const TofHit& th);

    const TofHit& operator=(const TofHit& th);

    void DecodeVolumeName();
    void Print() ;

    void SetTrackID   (int tid)        { setTrackID( tid ); }
    void SetStripNo   (int cn)         { _stripNumber = cn; }
    void SetPlaneNo   (int pn)         { _planeNumber = pn; }
    void SetStationNo (int sn)         { _stationNumber = sn; }
    void SetEdep      (double de)      { setEdep( de ); }
    void SetPathLength(double pl)      { _pathLength = pl; }
    void SetPosition  (Hep3Vector xyz){ setPosition( xyz ); }
    void SetMomentum  (Hep3Vector xyz){ setMomentum( xyz ); }
    void SetVolumeName(std::string vn)      { _volName = vn; }
    void SetTime      (double ti)      { setTime( ti ); }
    void SetEnergy    (double en)      { setEnergy( en ); }
    void SetPID       (int id)         { setPdg( id ); }
    void SetMass      (double ma)      { setMass( ma ); }

    void AddEdep      (double);     
    void AddPathLength(double);

    int GetTrackID()      { return trackID(); }
    int GetStripNo()       {  return _stripNumber; }
    int GetPlaneNo()      { return _planeNumber; }
    int GetStationNo()    { return _stationNumber; }
    double GetEdep()      { return Edep(); }
    double GetPathLength(){ return _pathLength; }
    Hep3Vector GetPosition()  { return position(); }
    Hep3Vector GetMomentum()  { return momentum(); }
    std::string GetVolumeName(){ return _volName; }
    double GetTime()      { return time(); }
    double GetEnergy()    { return energy(); }
    int    GetPID()       { return pdg(); }
    double GetMass()      { return mass(); }

  private :

      int  _stationNumber; //  TOF station
      int  _planeNumber;   //  x or y plane
      int  _stripNumber;    // strip number
      double        _pathLength;
      std::string        _volName;  // geant volume name
};
#endif
