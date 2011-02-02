// KalmanSeed.hh
//
// Class to hold the seed value determined by the pattern recognition to be used by the RecPack package
// inside the SciFiKalTrack.
//
// M.Ellis 6th October 2006

#ifndef RECON_SCIFI_KALMANSEED_HH
#define RECON_SCIFI_KALMANSEED_HH 1

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"

using CLHEP::Hep3Vector;

class KalmanSeed
{
  public :

    KalmanSeed( Hep3Vector pos, Hep3Vector posE, Hep3Vector mom, Hep3Vector momE, double q )
    {
      _pos = pos; _posE = posE; _mom = mom; _momE = momE; _q = q; 
    };

    double		charge() const		{ return _q; };

    Hep3Vector		position() const	{ return _pos; };

    Hep3Vector		positionError() const	{ return _pos; };

    double		x() const		{ return _pos.x(); };

    double		y() const		{ return _pos.y(); };

    double		z() const		{ return _pos.z(); };

    double		xError() const		{ return _posE.x(); };

    double		yError() const		{ return _posE.y(); };

    double		zError() const		{ return _posE.z(); };

    Hep3Vector		momentum() const	{ return _mom; };

    Hep3Vector		momentumError() const	{ return _momE; };

    double		px() const		{ return _mom.x(); };

    double		py() const		{ return _mom.y(); };

    double		pz() const		{ return _mom.z(); };

    double		pxError() const		{ return _momE.x(); };

    double		pyError() const		{ return _momE.y(); };

    double		pzError() const		{ return _momE.z(); };

    double		pInv() const		{ return 1.0 / _mom.mag(); };

    double		pInvError() const	{ return( fabs( pInv() ) * sqrt( ( _momE.x() / _mom.x() ) * ( _momE.x() / _mom.x() ) + ( _momE.y() / _mom.y() ) * ( _momE.y() / _mom.y() ) + ( _momE.z() / _mom.z() ) * ( _momE.z() / _mom.z() ) ) ); };

    double		xPrime() const		{ return _mom.x() / _mom.z(); };

    double		yPrime() const		{ return _mom.y() / _mom.z(); };

    double		xPrimeError() const	{ return ( fabs( xPrime() ) * sqrt( ( _momE.x() / _mom.x() ) * ( _momE.x() / _mom.x() ) + ( _momE.z() / _mom.z() ) * ( _momE.z() / _mom.z() ) ) ); };

    double		yPrimeError() const	{ return ( fabs( yPrime() ) * sqrt( ( _momE.y() / _mom.y() ) * ( _momE.y() / _mom.y() ) + ( _momE.z() / _mom.z() ) * ( _momE.z() / _mom.z() ) ) ); };

    double		xError2() const		{ double x = xError(); return x* x; };

    double		yError2() const		{ double x = yError(); return x * x; };

    double		pInvError2() const	{ double x = pInvError(); return x * x; };

    double		xPrimeError2() const	{ double x = xPrimeError(); return x * x; };

    double		yPrimeError2() const	{ double x = yPrimeError(); return x * x; };

    std::string		volumeName() const	{ return _volName; };

   void			setVolumeName( std::string name )	{ _volName = name; };

  private :

    Hep3Vector 	_pos;
    Hep3Vector	_posE;
    Hep3Vector	_mom;
    Hep3Vector	_momE;
    double	_q;
    std::string _volName;
};

#endif

