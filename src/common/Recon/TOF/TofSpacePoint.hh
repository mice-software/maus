#ifndef _TOFSPACEPOINT_HPP_
#define _TOFSPACEPOINT_HPP_ 1
//----------------------------------------------------------------------------
//
//     File :        TofSpacePoint.hh
//     Created :     3-DEC-2002  by Steve Kahn
//
//----------------------------------------------------------------------------

#include "CLHEP/Vector/ThreeVector.h"
#include <iostream>
#include <algorithm>

#include "Recon/TOF/TofSlabHit.hh"
#include "Config/MiceModule.hh"

#include "Interface/Memory.hh"

//! The class holds the information for the reconstructed point in the TOF detectors.
//! TofSpacePoint objects are created from two TofSlabHit objects.
class  TofSpacePoint
{
   public:
	 //! Default constructor.
	 TofSpacePoint();

	 //! Constructor.
	 TofSpacePoint( const MiceModule*, TofSlabHit* );

	 //! Constructor.
	 TofSpacePoint( const MiceModule*, TofSlabHit*, TofSlabHit* );

	 //! Destructor.
	 ~TofSpacePoint() 	{ miceMemory.addDelete( Memory::TofSpacePoint ); };

	 //! Dumps some information about this space point to the standard output.
	 void			Print( int verbosity = 1 );

	const MiceModule*     stat() const                    { return _station; }
	TofSlabHit*		hitA() const			{ return _hita; }
	TofSlabHit*		hitB() const			{ return _hitb; }
	TofSlabHit*		hit( int const plane ) const ;
	bool				isGood() const			{ return _good; }
	Hep3Vector		position() const 		{ return _pos; }
	Hep3Vector		posE() const	 		{ return _posE; }
	double			time() const 			{ return _time; }
	double			pe() const;
	int			station() const			{ return _hita->station(); };

	//! Returns the difference between the times of the two slab hits.
	double      dt()                    { return hitA()->time() - hitB()->time(); };

	void 			SetTime( double t )				{ _time = t; }
	void 			SetPosition( Hep3Vector r )		{ _pos = r; }
	void 			SetModule( MiceModule* m )		{ _station = m; }
	void 			SetGood( bool g )				{ _good = g; }

	void 			SetHitA( TofSlabHit*	hitA )	{ _hita = hitA; }
	void 			SetHitB( TofSlabHit*	hitB )	{ _hitb = hitB; }

  private :

	const MiceModule*	findSlabModule( TofSlabHit*, const MiceModule* ) const;

	Hep3Vector		getSlabEndPos( const MiceModule*, double ) const;
	Hep3Vector		crossingPos( Hep3Vector, Hep3Vector, Hep3Vector, Hep3Vector ) const;

	//! True if this space point is good.
	bool		_good;

	const MiceModule*     _station;

	//! Pointer to the first TofSlabHit used to create this space point.
	TofSlabHit*	_hita;

	//! Pointer to the second TofSlabHit used to create this space point.
	TofSlabHit*	_hitb;

	//! Position of the space point.
	Hep3Vector	_pos;

	//! Position error of the space point.
	Hep3Vector	_posE;

	//! Time of the space point.
	double		_time;

};
#endif
