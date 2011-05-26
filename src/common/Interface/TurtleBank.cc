#include <stdio.h>
#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
#include "Interface/TurtleBank.hh"


typedef Hep3Vector ThreeVector;
using namespace std;

TurtleBank::TurtleBank()
{
	_evt = 0;
	_wt = 0;
	_x = 0;
	_xprime = 0;
	_y = 0;
	_yprime = 0;
	_pz = 0;

}

TurtleBank::~TurtleBank() {}

//    evt par typ  flg   reg    time        x           y           z           Px          Py          Pz          Bx          By          Bz          wt          Ex          Ey          Ez          arclength   polX        polY        polZ

void TurtleBank::Read(std::istream &in)
{
	double pTot = 0;
	in >> _x >> _xprime >> _y >> _yprime >> pTot >> _wt >> _evt;
	pTot*=1000; // GeV/c->MeV/c
	_xprime/=1000.; //mrad->rad
	_yprime/=1000.; //mrad->rad
	_pz = pTot/(sqrt(1+_xprime*_xprime+_yprime*_yprime));

	_x*=10; //cm->mm
	_y*=10; //cm->mm

}

std::istream& operator >>(std::istream& fin, TurtleBank t)
{
  t.Read(fin);
  return fin;
}


