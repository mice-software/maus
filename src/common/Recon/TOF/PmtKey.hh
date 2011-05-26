// PmtKey.hh
// Mark Rayner, January 2009


#ifndef PMTKEY_HH
#define PMTKEY_HH

#include <iostream>
using namespace std;


//! A key to identify individual TOF PMTs in maps of calibration constants, cabling etc.
class PmtKey{

public:
   //! Default constructor.
   PmtKey();

	//! Constructor.
	PmtKey( int station, int plane, int slab, int pmt );

	int station() const { return _station; };
	int plane() const { return _plane; };
	int slab() const { return _slab; };
	int pmt() const { return _pmt; };

	void SetStation(int st) 	{ _station = st; };
	void SetPlane(int pl)		{ _plane = pl; };
	void SetSlab(int sl)			{ _slab = sl; };
	void SetPmt(int pmt)			{ _pmt = pmt; };

	//! Is equal to operator.
	bool operator== ( PmtKey key );


	friend ostream& operator<< ( ostream& stream, PmtKey key );

private:
	//! TOF station number.
	int _station;

	//! X or Y plane.
	int _plane;

	//! Slab number.
	int _slab;

	//! PMT number. Can be 0 or 1.
	int _pmt;
};

#endif

