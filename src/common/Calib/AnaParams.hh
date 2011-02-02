/*
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#ifndef ANAPARAMS_HH
#define ANAPARAMS_HH

#include <cstdio>
#include "CLHEP/Vector/ThreeVector.h"
#include "Interface/dataCards.hh"
#include <string>
#include <iostream>
//#include <strstream>

using namespace std;

class AnaParams
{
public:
	AnaParams();
	~AnaParams();

	static  AnaParams* getInstance();
	int     nEvts()         { return _nEvts; }
	int     startEvt()      { return _startEvt;}
	int     particleID()    { return _particleID;}
	double  particleMass()  { return _mass; }
	string  simOutput()     { return _simOutput; }
	string  recOutput()     { return _recOutput; }
	string  anaInput()      { return _anaInput; }
	string  for009Input()   { return _for009Input; }
	string  virtualInput()  { return _virtualInput; }
	string  turtleInput()   { return _turtleInput; }
	string  pidInput()      { return _pidInput; }
	string  pidBranch()     { return _pidBranch; }
	void    setTurtleInput(string turtleInput) { _turtleInput=turtleInput;}
private:
	double getMassFromPdgPid();

	string _recOutput;
	string _simOutput;
	string _anaInput;
	string _for009Input;
	string _virtualInput;
	string _turtleInput;
	string _pidInput;
	string _pidBranch;
	double _mass;
	int    _particleID;
	int    _nEvts; //number of events in the run
	int    _startEvt;
	static AnaParams* _instance;
	static const double m_e, m_mu, m_pi, m_K, m_p; //MeV/c2

};

#endif
