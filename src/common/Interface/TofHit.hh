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

#include "Interface/MCHit.hh"

#include "Interface/Memory.hh"

typedef Hep3Vector ThreeVector;

//! TofHit is a description of a simulated hit in a single strip of the TOF detector.
//! TofHit class is used to hold all information recorded.
class TofHit : public MCHit
{
  public:
	 //! Default constructor.
    TofHit();

	 //! Destructor.
    ~TofHit()		{ miceMemory.addDelete( Memory::TofHit ); };

	 //! Copy constructor.
    TofHit(const TofHit& th);

	 //! Assignment operator.
    const TofHit& operator=(const TofHit& th);

	 //! Uses the volume name to set  _stationNumber, _planeNumber and _stripNumber.
    void DecodeVolumeName();

	 //! Dumps some information about this digit to the standard output
    void Print() ;

    void setStripNo   (int cn)         { _stripNumber = cn; }
    void setPlaneNo   (int pn)         { _planeNumber = pn; }
    void setStationNo (int sn)         { _stationNumber = sn; }
    void setPathLength(double pl)      { _pathLength = pl; }
    void setVolumeName(std::string vn) { _volName = vn; }
    void setPID       (int id)         { setPdg( id ); }

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

	 //!  TOF station number.
	 int  _stationNumber;

	 //!  X or Y plane.
	 int  _planeNumber;

	 //! Strip number.
	 int  _stripNumber;

	 //! Path length of the track in the geant4 sensitive detector volume.
	 double _pathLength;

	 //! Geant4 volume name.
	 std::string _volName;
};
#endif
