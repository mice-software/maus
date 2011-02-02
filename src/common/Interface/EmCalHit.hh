///////////////////////////////////////////////////////////////////////////////
/// EmCalHit
///
/// \brief Monte Carlo hit in the calorimeter
///
/// This class is created by a Geant4 application or read from file.
/// In the case of the Simulation application, it is just a store of information
/// of Geant4 data. During Digitisation, each EmCalHit is associated with the 
/// EmCalDigits that was derived from the hit. For a EmCalDigit to know where 
/// it comes from, what energy deposition etc, it should ask the hits that 
/// took part in creating it.
///
/// \author 2006-03-04 Rikard Sandstrom
////////////////////////////////////////////////////////////////////////////////

#ifndef EmCalHit_h
#define EmCalHit_h 1
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"

#include "Memory.hh"

#include <iostream>

#include "MCHit.hh"

class EmCalDigit;

/////////////////////////////////////////////////////////////////////////////

class EmCalHit : public MCHit
{
public:

  /// The constructor
  EmCalHit();
  
  /// The destructor
  ~EmCalHit();

  /// Assigns the EmCalDigit in the argument to the list of digits 
  /// for this hit. If it already exists do nothing.
  void AssignDigit(EmCalDigit* aDigit);

  /// Removes the EmCalDigit in the argument from the list of digits 
  /// for this hit.
  void UnassignDigit(EmCalDigit* aDigit);

  /// Get/Set the vector of digits this hit was taking part of creating.
  inline std::vector<EmCalDigit*> GetDigits() {return m_digitVector;};
  inline void SetDigits(std::vector<EmCalDigit*>& v ) {m_digitVector = v;};

  /// Get/Set Monte Carlo truth values.
  inline double        GetEdep()             { return Edep(); };
  inline void          SetEdep(double eDep)  { setEdep( eDep ); };
  inline int           GetCopyNumber()  const      {return m_copyNumber;};
  inline void          SetCopyNumber(int i)  {m_copyNumber    = i;};
  inline int           GetCellNumber()       {return m_cellNumber;};
  inline void          SetCellNumber(int i)  {m_cellNumber    = i;};
  inline int           GetLayerNumber()       {return m_layerNumber;};
  inline void          SetLayerNumber(int i)  {m_layerNumber    = i;};
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
  
  std::vector<EmCalDigit*> m_digitVector;
  int         m_copyNumber;
  int         m_cellNumber;
  int         m_layerNumber;

};

#endif
