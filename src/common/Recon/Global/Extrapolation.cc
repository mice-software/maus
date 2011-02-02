// Extrapolation.cc - stores all information about the extrapolation of a MouseTrack
// to somewhere in MICE using the Kalman package

#include "Extrapolation.hh"

/*
Extrapolation::Extrapolation( IState* st, TrajectoryManager* man )
{ 
  m_good = true;
  state = st;
  manager = man;
}
*/

Extrapolation::~Extrapolation()
{ 
}

Extrapolation::Extrapolation( const Extrapolation& rhs )
{
}

Extrapolation& Extrapolation::operator=( const Extrapolation& rhs )
{
  return *this;
}

CLHEP::Hep3Vector	Extrapolation::position() const
{
  if( ! m_good )
    return CLHEP::Hep3Vector( -9.999e4, -9.999e4, -9.999e4 );

  CLHEP::Hep3Vector pos;
  return CLHEP::Hep3Vector( 10 * pos[0], 10 * pos[1], 10 * pos[2] );
}

CLHEP::Hep3Vector	Extrapolation::positionError() const
{
  if( ! m_good )
    return CLHEP::Hep3Vector( -9.999e4, -9.999e4, -9.999e4 );

  return CLHEP::Hep3Vector( 0, 0, 0 );
}

CLHEP::Hep3Vector	Extrapolation::momentum() const
{
  if( ! m_good )
    return CLHEP::Hep3Vector( -9.999e4, -9.999e4, -9.999e4 );

  CLHEP::Hep3Vector mom;
  return CLHEP::Hep3Vector( 1000 * mom[0], 1000 * mom[1], 1000 * mom[2] );
}

CLHEP::Hep3Vector	Extrapolation::momentumError() const
{
  if( ! m_good )
    return CLHEP::Hep3Vector( -9.999e4, -9.999e4, -9.999e4 );

  return CLHEP::Hep3Vector( 0, 0, 0 );
}

double		Extrapolation::chi2MatchPos( CLHEP::Hep3Vector& pos ) const
{
  if( ! m_good ) // not a good extrapolation
    return 1e11;

  CLHEP::Hep3Vector extrap = position();
  CLHEP::Hep3Vector extrapE = positionError();

  double chi2 = ( extrap.x() - pos.x() ) * ( extrap.x() - pos.x() ); // / ( extrapE.x() * extrapE.x() );
        chi2 += ( extrap.y() - pos.y() ) * ( extrap.y() - pos.y() ); // / ( extrapE.y() * extrapE.y() );
        chi2 += ( extrap.z() - pos.z() ) * ( extrap.z() - pos.z() ); // / ( extrapE.z() * extrapE.z() );

  return chi2;
}

double		Extrapolation::time() const // not implemented at the moment!
{
  return -9.999e4;
}

