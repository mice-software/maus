
// Modidfied:
// 18.10.02 V. Grichine

#ifndef FOR009BANK_HH
#define FOR009BANK_HH

#include <stdio.h>
#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

typedef Hep3Vector ThreeVector;
using namespace std;

class For009Bank
{
public:
	For009Bank();
	~For009Bank();

	void Read(istream &fin);
	void Write(ostream &fout);

	//event number, ICOOL particle ID, particle flag (e.g. reference particle), spatial region
	int _evt, _typ, _par, _flg, _reg;
	//time, statistical weight, step arclength
	double _time, _wt, _arclength;
	//position, momentum, B-field, E-field, pol?
	ThreeVector _pos;
        ThreeVector _mom;
	ThreeVector _Bfield;
	ThreeVector _E;
	ThreeVector _pol;

private:

};

#endif
