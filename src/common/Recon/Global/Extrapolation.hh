// Extrapolation.hh - stores all information about the extrapolation of a MouseTrack
// to somewhere in MICE using the Kalman package

#ifndef REC_EXTRAPOLATION_HH
#define REC_EXTRAPOLATION_HH

#include "CLHEP/Vector/ThreeVector.h"

class Extrapolation
{
  public :

  Extrapolation() { m_good = false; };

//  Extrapolation( IState*, TrajectoryManager* );

  ~Extrapolation();

  Extrapolation( const Extrapolation& );

  Extrapolation& operator=( const Extrapolation& );

  CLHEP::Hep3Vector	position() const;
  CLHEP::Hep3Vector	positionError() const;

  CLHEP::Hep3Vector	momentum() const;
  CLHEP::Hep3Vector	momentumError() const;

  double		chi2MatchPos( CLHEP::Hep3Vector& ) const;

  double		time() const;

  bool			good() const { return m_good; };

  private :

  bool			m_good;
};

#endif
