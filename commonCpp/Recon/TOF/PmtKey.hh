// PmtKey.hh
// Mark Rayner, January 2009	
// A key to identify individual PMTs in maps of calibration constants etc

#ifndef PMTKEY_HH
#define PMTKEY_HH

#include <iostream>
using namespace std;

class PmtKey {

public:
	PmtKey( int station, int plane, int slab, int pmt );
	bool operator== ( PmtKey key );
	int station() const { return _station; };
	int plane() const { return _plane; };
	int slab() const { return _slab; };
	int pmt() const { return _pmt; };

	//Not a member of the class but a friend
	//So you need to supply the PmtKey as there is no *this
	//However as a friend it can reference private data
	//"std::cout << someKey" prints the st, pl, sl, pm
	friend ostream& operator<< ( ostream& stream, PmtKey key );

private:
	int _station;
	int _plane;
	int _slab;
	int _pmt;

};

#endif
