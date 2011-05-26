#include <stdio.h>
#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
#include "Interface/For009Bank.hh"


typedef Hep3Vector ThreeVector;
using namespace std;

For009Bank::For009Bank()
{
	_evt = 0;
	_par = 0;
	_typ = 0;
	_flg = 0;
	_reg = 0;
	_time = 0;
	_wt = 0;
	_arclength = 0;
	_pos = ThreeVector(0,0,0);
	_mom = ThreeVector(0,0,0);
	_pol = ThreeVector(0,0,0);
	_Bfield = ThreeVector(0,0,0);
	_E = ThreeVector(0,0,0);

}

For009Bank::~For009Bank() {}

//    evt par typ  flg   reg    time        x           y           z           Px          Py          Pz          Bx          By          Bz          wt          Ex          Ey          Ez          arclength   polX        polY        polZ

void For009Bank::Read(istream &fin)
{
	double x, y, z, px, py, pz, Bx, By, Bz, Ex, Ey, Ez, polx, poly, polz;
	fin >> _evt >> _par >> _typ >> _flg >> _reg;
	fin >> _time >> x >> y >> z >> px >> py >> pz;
	fin >> Bx >> By >> Bz >> _wt >> Ex >> Ey >> Ez;
	fin >> _arclength >> polx >> poly >> polz;
	_time *= 1e9; // convert to ns
	_pos = ThreeVector(x*1000,y*1000,z*1000); //convert to mm
	_mom = ThreeVector(px*1000,py*1000,pz*1000); //convert to MeV
	_pol = ThreeVector(polx,poly,polz);
	//CAUTION - units not sure
	_Bfield = ThreeVector(Bx,By,Bz);
	_E = ThreeVector(Ex,Ey,Ez);
}

void For009Bank::Write(ostream &fout)
{
	fout << "  " << _evt << "  " << _par << "  " << _typ << "  " << _flg << "  " << _reg;
	fout << "  " << _time/1e9 << "  " << _pos.x()/1000. << "  " << _pos.y()/1000. << "  " << _pos.z()/1000.; 
	fout << "  " << _mom.x()/1000. << "  " << _mom.y()/1000. << "  " << _mom.z()/1000.;
	fout << "  " << _Bfield.x() << "  " << _Bfield.y() << "  " << _Bfield.z() << "  " << _wt ;
	fout << "  " << _E.x() << "  " << _E.y() << "  " << _E.z();
	fout << "  " << " 0 0 0 0\n";
}

