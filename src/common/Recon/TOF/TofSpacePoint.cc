//----------------------------------------------------------------------------
//
//     File :        TofSpacePoint.cpp
//     Purpose :     Serve the "reconstructed" point information for the
//                   TOF.
//     Created :     3-DEC-2002  by Steve Kahn
//
//----------------------------------------------------------------------------

#include "Recon/TOF/TofSpacePoint.hh"
#include "Interface/TofDigit.hh"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/Matrix.h"

using namespace std;
using CLHEP::Hep3Vector;

TofSpacePoint::TofSpacePoint()
{
  miceMemory.addNew( Memory::TofSpacePoint );

  	_station = NULL;
	_hita = NULL;
	_hitb = NULL;
  	_good = false;
}

TofSpacePoint::TofSpacePoint( const MiceModule* station,  TofSlabHit* a )
{
  miceMemory.addNew( Memory::TofSpacePoint );

  _station = station;
  _hita = a;
  _hitb = NULL;
  _good = false;

  //const MiceModule* slabA = findSlabModule( _hita, station );
  const MiceModule* slabA = a->GetModule();

  if( slabA && _hita->isCalibrated() )
  {
    _pos = slabA->globalPosition();
    _posE = Hep3Vector( 10. * CLHEP::cm, 10. * CLHEP::cm, 0. );
    _time = a->CorrectedTime();
    _good = true;
  }
}


TofSpacePoint::TofSpacePoint( const MiceModule* station,  TofSlabHit* a, TofSlabHit* b )
{
  miceMemory.addNew( Memory::TofSpacePoint );

  _hita = a;
  _hitb = b;
  _good = false;

  //const MiceModule* slabA = findSlabModule( _hita, station );
  const MiceModule* slabA = a->GetModule();

  //const MiceModule* slabB = findSlabModule( _hitb, station );
  const MiceModule* slabB = b->GetModule();

  if( slabA && slabB && _hita->isCalibrated() && _hitb->isCalibrated() )
  {
    Hep3Vector posa1 = getSlabEndPos( slabA, +1. );
    Hep3Vector posa2 = getSlabEndPos( slabA, -1. );
    Hep3Vector posb1 = getSlabEndPos( slabB, +1. );
    Hep3Vector posb2 = getSlabEndPos( slabB, -1. );

    Hep3Vector posEa = slabA->dimensions();
    std::vector<double> widths(3);
    widths[0] = posEa.x();
    widths[1] = posEa.y();
    widths[2] = posEa.z();

    std::sort(widths.begin(), widths.end());

    double sigma =  widths[1] / sqrt( 12. );

    _pos = crossingPos( posa1, posa2, posb1, posb2 );
    _posE = Hep3Vector( sigma, sigma, 0. );
    _time = ( a->CorrectedTime() + b->CorrectedTime() )/ 2.* CLHEP::picosecond;
    _good = true;
  }
}

const MiceModule*	TofSpacePoint::findSlabModule( TofSlabHit* hit, const MiceModule* station ) const
{
  int plane = hit->plane();
  int slab = hit->slabNumber();

  const MiceModule* m_plane = NULL;
  if( station->propertyExistsThis( "Plane", "int" ) )
    m_plane = station;
  else
    for( int i = 0; i < station->daughters(); ++i )
    {
      if( station->daughter(i)->propertyExistsThis( "Plane", "int" ) &&
          station->daughter(i)->propertyInt( "Plane" ) == plane )
        m_plane = station->daughter(i);
    }

  if( ! m_plane )
    return NULL;

  const MiceModule* m_slab = NULL;

  if( m_plane->propertyExistsThis( "Slab", "int" ) )
    m_slab = m_plane;
  else
    for( int i = 0; i < m_plane->daughters(); ++i )
    {
      if( m_plane->daughter(i)->propertyExistsThis( "Slab", "int" ) &&
          m_plane->daughter(i)->propertyInt( "Slab" ) == slab )
      m_slab = m_plane->daughter(i);
    }

  return m_slab;
}

Hep3Vector      TofSpacePoint::getSlabEndPos( const MiceModule* slab, double dir ) const
{
  double dx = slab->dimensions().x();
  double dy = slab->dimensions().y();
  double dz = slab->dimensions().z();

  Hep3Vector norm;

  if( dx > dy && dx > dz )
     norm = Hep3Vector( dir * dx / 2., 0., 0. );
  else if( dy > dx && dy > dz )
     norm = Hep3Vector( 0., dir * dy / 2., 0. );
  else
     norm = Hep3Vector( 0., 0., dir * dz / 2. );

  Hep3Vector pos = slab->globalPosition() + slab->globalRotation() * norm;

  return pos;
}

Hep3Vector	TofSpacePoint::crossingPos( Hep3Vector posa1, Hep3Vector posa2,
    					    Hep3Vector posb1, Hep3Vector posb2 ) const
{
  // Start of Calc. of distance between lines

  Hep3Vector d1 = ( posa2 - posa1) / (posa2 - posa1).mag();
  Hep3Vector d2 = ( posb2 - posb1) / (posb2 - posb1).mag();

  CLHEP::HepMatrix m1( 3, 3, 0);

    m1[0][0] = ( posb1 - posa1 ).x();
    m1[0][1] = ( posb1 - posa1 ).y();
    m1[0][2] = ( posb1 - posa1 ).z();

    m1[1][0] = ( d2 ).x();
    m1[1][1] = ( d2 ).y();
    m1[1][2] = ( d2 ).z();

    m1[2][0] = ( d1.cross(d2) ).x();
    m1[2][1] = ( d1.cross(d2) ).y();
    m1[2][2] = ( d1.cross(d2) ).z();

  CLHEP::HepMatrix m2( 3, 3, 0);

    m2[0][0] = ( posb1 - posa1 ).x();
    m2[0][1] = ( posb1 - posa1 ).y();
    m2[0][2] = ( posb1 - posa1 ).z();

    m2[1][0] = ( d1 ).x();
    m2[1][1] = ( d1 ).y();
    m2[1][2] = ( d1 ).z();

    m2[2][0] = ( d1.cross(d2) ).x();
    m2[2][1] = ( d1.cross(d2) ).y();
    m2[2][2] = ( d1.cross(d2) ).z();

  double t1 = m1.determinant() / pow( ( d1.cross(d2) ).mag(), 2. );
  double t2 = m2.determinant() / pow( ( d1.cross(d2) ).mag(), 2. );

  Hep3Vector p1 = posa1 + t1 * d1;
  Hep3Vector p2 = posb1 + t2 * d2;

  Hep3Vector pos = ( p1 + p2 ) / 2.;

  return pos;
}

double		TofSpacePoint::pe() const
{
  double pe = 0;

  if( _hita )
    pe += _hita->pe();

  if( _hitb )
    pe += _hitb->pe();

  return pe;
}

void TofSpacePoint::Print( int verbosity )
{
	if ( verbosity == 1 ) {
	  	cout << "____________________________" << endl;
	  	cout << "  TofSpacePoint" << endl;
		cout << "  Station Number : " << station() << endl;
		cout << "  Time : " << time()/CLHEP::ns <<" ns" << endl;
		cout << "  Position : "  << position()/CLHEP::cm <<" cm" << endl;
  		cout << "____________________________" << endl;
	}
	if ( verbosity == 0 ) {
		cout << "TofSpacePoint: station " << station() << "slabs (" << hit(0)->slabNumber() << "," << hit(1)->slabNumber() << ")" << endl;
	}
}

TofSlabHit * TofSpacePoint::hit( int const plane ) const {
	TofSlabHit* slabhit = 0;
	if ( hitA()->plane() == plane ) slabhit = hitA();
	if ( hitB()->plane() == plane ) slabhit = hitB();
	return slabhit;
}
