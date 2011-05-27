///////////////////////////////////////////////////////////////////////////////
/// KLHit
///
/// \brief Monte Carlo hit in the calorimeter
///
/// This class is created by a Geant4 application or read from file.
/// In the case of the Simulation application, it is just a store of information
/// of Geant4 data. During Digitisation, each KLHit is associated with the 
/// KLDigits that was derived from the hit. For a KLDigit to know where 
/// it comes from, what energy deposition etc, it should ask the hits that 
/// took part in creating it.
///
/// \author 2006-03-04 Rikard Sandstrom
////////////////////////////////////////////////////////////////////////////////

#ifndef KLHit_h
#define KLHit_h 1
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"

#include <iostream>

#include "Interface/MCHit.hh"

class KLDigit;

/////////////////////////////////////////////////////////////////////////////

class KLHit : public MCHit
{
public:

  /// The constructor
  KLHit();
  
  /// The destructor
  ~KLHit();

  /// Get/Set Monte Carlo truth values.
  inline double        GetEdep()             { return Edep(); };
  inline void          SetEdep(double eDep)  { setEdep( eDep ); };
  inline int           GetCellNumber()       {return m_cellNumber;};
  inline void          SetCellNumber(int i)  {m_cellNumber    = i;};
  inline int           GetCopyNumber()       {return m_copyNumber;};
  inline void          SetCopyNumber(int i)  {m_copyNumber    = i;};
  inline int           GetTrackID()          { return trackID(); };
  inline void          SetTrackID(int tid)   { setTrackID( tid ); }
  inline Hep3Vector    GetMomentum()                 { return momentum(); };
  inline void          SetMomentum (Hep3Vector xyz)  { setMomentum( xyz ); };
  inline Hep3Vector    GetPosition()                 { return position(); };
  inline void          SetPosition(Hep3Vector pos)   { setPosition( pos ); };
  inline double        GetTime()             { return time(); };
  inline void          SetTime(double ti)    { setTime( ti ); };
  inline double        GetEnergy()           { return energy(); };
  inline void          SetEnergy(double en)  { setEnergy( en ); };
  inline int           GetPID()              { return pdg(); };
  inline void          SetPID(int pid)       { setPdg( pid ); };
  inline void          SetCharge( double ch) { setCharge( ch ); };
  inline double        GetCharge() const     { return charge(); };
  inline void          SetMass( double ma)   { setMass( ma ); };
  inline double        GetMass() const       { return mass(); };

  inline double GetX()           { return position().x(); };
  inline double GetY()           { return position().y(); };
  inline double GetZ()           { return position().z(); };

private:
 int         m_cellNumber;
 int         m_copyNumber;
  
};

#endif
