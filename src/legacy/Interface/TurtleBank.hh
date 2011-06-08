
// Modidfied:
// 18.10.02 V. Grichine

#ifndef TURTLEBANK_HH
#define TURTLEBANK_HH

#include <stdio.h>
#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
typedef Hep3Vector ThreeVector;
using namespace std;

class TurtleBank
{
public:
	TurtleBank();
	~TurtleBank();

	void Read(std::istream &in);

  int ParticleId()             {return 0;}
  int EventNumber()            {return _evt;}
  CLHEP::Hep3Vector Position() {return CLHEP::Hep3Vector(_x,_y,0.);}
  CLHEP::Hep3Vector Momentum() {return CLHEP::Hep3Vector(_xprime*_pz,_yprime*_pz,_pz);}
  CLHEP::Hep3Vector Spin()     {return CLHEP::Hep3Vector(0,0,0);}
  double            Time()     {return 0.;}
  double            Weight()   {return _wt;}

	//event number, Turtle particle ID, particle flag (e.g. reference particle), spatial region
	int _evt;
	//statistical weight, x[mm], dx/dz, y[mm], dy/dz, pz[Mev/c]
	double _wt, _x, _xprime, _y, _yprime, _pz;


private:

};

std::istream& operator >>(std::istream& in, TurtleBank t);


#endif
