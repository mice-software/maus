// SciFiSeed.hh
//
// Routine that takes three space point positions and a magnetic field value and determines the 3 momentum of the particle
//
// A. Fish 31/8/2006

// ME - for the moment this is assuming a positively charged particle, despite the fact that Q is an input parameter

#include <algorithm>

#include "CLHEP/Matrix/Matrix.h"

#include "Recon/SciFi/SciFiSeed.hh"

using CLHEP::Hep3Vector;

bool  determineCentre( Hep3Vector&, Hep3Vector&, Hep3Vector&, double&, double&, double& );
bool determinePtFromR( double&, double&, double& );
bool determinePhi( double&, double&, Hep3Vector&, double& );

static const double pi = 3.141592654;

bool	momentumFromPoints( Hep3Vector pnt1, Hep3Vector pnt2, Hep3Vector pnt3, double sigx, double sigy, double B, double Q, double& px, double& py, double& pz, int& pos, double& sigpx, double& sigpy, double& sigpz, bool mcs )
{ 
  bool use_p = false;
  double p = pz;

  if( p > 0. )
    use_p = true;

  Hep3Vector p1, p2, p3;

  std::vector<double> pnts(3);
  std::vector<Hep3Vector> spnts(3);

  //ME need to work out what I'm doing wrong when I try to sort a vector of Hep3Vectors...

  pnts[0] = pnt1.z();
  pnts[1] = pnt2.z();
  pnts[2] = pnt3.z();

  std::sort( pnts.begin(), pnts.end() );

  for( int i = 0; i < 3; ++i )
    if( fabs( pnts[i] - pnt1.z() ) < 1. * CLHEP::mm )
      spnts[i] = pnt1;

  for( int i = 0; i < 3; ++i )
    if( fabs( pnts[i] - pnt2.z() ) < 1. * CLHEP::mm )
      spnts[i] = pnt2;

  for( int i = 0; i < 3; ++i )
    if( fabs( pnts[i] - pnt3.z() ) < 1. * CLHEP::mm )
      spnts[i] = pnt3;

  pos = 1;

  if( fabs( pnts[0] - pnt2.z() ) < 1. * CLHEP::mm ) pos = 2;
  else if( fabs( pnts[0] - pnt3.z() ) < 1. * CLHEP::mm ) pos = 3;

  p1 = spnts[0];
  p2 = spnts[1];
  p3 = spnts[2];


  // determine the direction of rotation +1 = clockwise, -1 = anticlockwise

  double direction = B * Q / fabs( B * Q );

  // determine the center and radius of the circle

  double x0, y0, r;

  bool ok = determineCentre( p1, p2, p3, x0, y0, r );

  if( ! ok )
    return ok;

  // calculate the PT from the radius of the circle and the magnetic field

  double pt;

  ok = determinePtFromR( r, B, pt );

  if( ! ok )
    return ok;

  // determine the phi angles of each three points

  double phi1, phi2, phi3;

  ok = determinePhi( x0, y0, p1, phi1 );
  ok &= determinePhi( x0, y0, p2, phi2 );
  ok &= determinePhi( x0, y0, p3, phi3 );

  if( ! ok )
    return ok;

  // determine PX and PY from PT and phi

  px = pt * cos( phi1 );
  py = - pt * sin( phi1 );

  // determine PZ from PT and the slope of the line in s vs z

  double dphi1 = direction * ( phi2 - phi1 );

  if( dphi1 < 0. )
     dphi1 += 2. * pi;

  double dphi2 = direction * ( phi3 - phi1 );

  if( dphi2 < 0. )
    dphi2 += 2. * pi;

  if( dphi2 < dphi1 )
    dphi2 += 2. * pi;

  double R_z = ( p3.z() - p1.z() ) / ( p2.z() - p1.z() );

  double phirat = dphi2 / dphi1;

  // test if this ratio is consistent with the z spacing

  if( ( fabs( phirat - R_z ) / R_z ) > 0.2 )
    return false;

/*
  bool top = true;
  int count = 0;

  while( ( fabs( dphi2 - dphi1 * R_z ) > ( pi / 4. ) ) && ( count < 4 ) )
  {
    if( top )
	dphi2 += 2. * pi;
    else
	dphi1 += 2. * pi;

    top = ! top;

    ++count;
  }


  if( count > 3 )
    return false;
*/

  double dsdz = r * dphi2 / ( p3.z() - p1.z() );

 if( dsdz == 0. )
    return false;

  if( use_p )
  {
    // calculate pz knowing pt and the total momentum

    if( pt > p )
      return false;
    else
      pz = sqrt( p * p - pt * pt );
  }
  else 
    pz = pt / dsdz;

  pz = fabs( pz ); 

  // determine sigpx, sigpy and sigpz
  
  double sigr = sqrt( ( sigx * sigx ) + ( sigy * sigy ) );
 
  double sigpt = pt * ( sigr / r );


  sigpz = pz * ( sigpt / pt );

  sigpx = sigpt / sqrt( 2. );
  sigpy = sigpt / sqrt( 2. );

  //ME corrections from Monte Carlo

  sigpx *= 6.34; //3.17; 
  sigpy *= 6.18; //3.09; 
  sigpz *= 1.88; //0.94; 

  if( ! mcs )
  {
    sigpx *= 0.42;
    sigpy *= 0.44;
    sigpz *= 0.36;
  }

  return true;
}

bool  determineCentre( Hep3Vector& p1, Hep3Vector& p2, Hep3Vector& p3, double& x0, double& y0, double& r )
{
  CLHEP::HepMatrix a( 3, 3 );
  CLHEP::HepMatrix d( 3, 3 );
  CLHEP::HepMatrix e( 3, 3 );
  CLHEP::HepMatrix f( 3, 3 );

  a[0][0] = p1.x();		a[0][1] = p1.y();		a[0][2] = 1.;
  a[1][0] = p2.x();		a[1][1] = p2.y();		a[1][2] = 1.;
  a[2][0] = p3.x();		a[2][1] = p3.y();		a[2][2] = 1.;

  d[0][0] = p1.x() * p1.x() + p1.y() * p1.y();		d[0][1] = p1.y();		d[0][2] = 1.;
  d[1][0] = p2.x() * p2.x() + p2.y() * p2.y();		d[1][1] = p2.y();		d[1][2] = 1.;
  d[2][0] = p3.x() * p3.x() + p3.y() * p3.y();		d[2][1] = p3.y();		d[2][2] = 1.;
  
  e[0][0] = p1.x() * p1.x() + p1.y() * p1.y();		e[0][1] = p1.x();		e[0][2] = 1.;
  e[1][0] = p2.x() * p2.x() + p2.y() * p2.y();		e[1][1] = p2.x();		e[1][2] = 1.;
  e[2][0] = p3.x() * p3.x() + p3.y() * p3.y();		e[2][1] = p3.x();		e[2][2] = 1.;

  f[0][0] = p1.x() * p1.x() + p1.y() * p1.y();		f[0][1] = p1.x();		f[0][2] = p1.y();
  f[1][0] = p2.x() * p2.x() + p2.y() * p2.y();		f[1][1] = p2.x();		f[1][2] = p2.y();
  f[2][0] = p3.x() * p3.x() + p3.y() * p3.y();		f[2][1] = p3.x();		f[2][2] = p3.y();
  
  double detA = a.determinant();
  double detD = - d.determinant();
  double detE = e.determinant();
  double detF = - f.determinant();

  if( detA == 0. )
    return false;

  x0 = - detD / (2. * detA);
  y0 = - detE / (2. * detA);

  double det = ( detD * detD + detE * detE ) / ( 4. * detA * detA ) - ( detF / detA );

  if( det < 0. )
    return false;

  r = sqrt ( det );

  return true;
}


bool determinePtFromR( double& r, double& B, double& pt )
{
  pt = r / CLHEP::m * B / CLHEP::tesla * 0.3 * CLHEP::GeV;

  return true;
}


bool determinePhi( double& x0, double& y0, Hep3Vector& pos, double& phi )
{
  phi = atan2( pos.x() - x0, pos.y() - y0 );
  
  if( phi < 0. )
    phi += 2. * pi;

  return true;
}

