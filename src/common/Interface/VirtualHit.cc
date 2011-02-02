//----------------------------------------------------------------------------
//
//     File :              VirtualHit.cc
//     Purpose :           Variables used by VirtualHit.  This represents
//                         a sort of generic hit.
//     Created :           29-Sep-2002  by Steve Kahn
//
//----------------------------------------------------------------------------

#include "VirtualHit.hh"

using namespace std;

VirtualHit::VirtualHit()
{
  miceMemory.addNew( Memory::VirtualHit );

  setEdep(0);
  _stationNumber = -1;
  _bField = ThreeVector(0., 0., 0.);
	_eField = ThreeVector(0., 0., 0.);
}

VirtualHit::VirtualHit(const VirtualHit& rhs)
{
  miceMemory.addNew( Memory::VirtualHit );
	
	_stationNumber=rhs._stationNumber;
	_bField=rhs._bField;
	_eField=rhs._eField;
}

const VirtualHit& VirtualHit::operator=(const VirtualHit& rhs)
{
	if(&rhs==this) return *this;

	_stationNumber=rhs._stationNumber;
	_bField=rhs._bField;
	_eField=rhs._eField;
	return *this;
}

int VirtualHit::operator==(const VirtualHit& rhs) const
{
	bool a=( trackID() == rhs.trackID() );
	bool b=( charge() == rhs.charge() );
	bool c=(_stationNumber==rhs._stationNumber);
	bool d=( mass() == rhs.mass() );
	bool e=( position() == rhs.position() );
	bool f=( time() == rhs.time() );
	bool g=( momentum() == rhs.momentum() );
	bool h=( energy() == rhs.energy() );
	bool i=( pdg() == rhs.pdg() );
	bool j=(_bField==rhs._bField);
	bool k=(_eField==rhs._eField);

	return a*b*c*d*e*f*g*h*i*j*k;

}
