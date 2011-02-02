#ifndef _TOFSPACEPOINT_HPP_
#define _TOFSPACEPOINT_HPP_ 1
//----------------------------------------------------------------------------
//
//     File :        TofSpacePoint.hh
//     Purpose :     Serve the "reconstructed" point information for the
//                   TOF.
//     Created :     3-DEC-2002  by Steve Kahn
//
//----------------------------------------------------------------------------

#include "CLHEP/Vector/ThreeVector.h"
#include <iostream>
#include <algorithm>

#include "TofSlabHit.hh"
#include "Config/MiceModule.hh"

#include "Interface/Memory.hh" 

class  TofSpacePoint
{
   public:

	TofSpacePoint();
	TofSpacePoint( const MiceModule*, TofSlabHit* );
	TofSpacePoint( const MiceModule*, TofSlabHit*, TofSlabHit* );
	~TofSpacePoint() 	{ miceMemory.addDelete( Memory::TofSpacePoint ); };
	
	const MiceModule*     stat() const                    { return _station; }

	void			Print( int verbosity = 1 );
	TofSlabHit*		hitA() const			{ return _hita; }
	TofSlabHit*		hitB() const			{ return _hitb; }
	TofSlabHit*		hit( int const plane ) const ;
	bool			isGood() const			{ return _good; }
	Hep3Vector		position() const 		{ return _pos; }
	Hep3Vector		posE() const	 		{ return _posE; }
	double			time() const 			{ return _time; }
	double			pe() const;
	int			station() const			{ return _hita->station(); };
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

	bool		_good;

	const MiceModule*     _station;

	TofSlabHit*	_hita;
	TofSlabHit*	_hitb;

	Hep3Vector	_pos;
	Hep3Vector	_posE;
	double		_time;

};
#endif
