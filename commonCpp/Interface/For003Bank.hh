
// Modidfied:
// 18.10.02 V. Grichine

#ifndef FOR003BANK_HH
#define FOR003BANK_HH

#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

class For003Bank
{
public:
	For003Bank();
	~For003Bank();

	void Read (std::istream &fin);
	void Write(std::ostream &fout) const;

	static void ReadHeader (std::istream& fin);
	static void WriteHeader(std::ostream& fout);

	int        EventNumber()    {return _eventNumber;}
	int        ParticleNumber() {return _particleNumber;}
	int        ParticleType()   {return _particleType;}
	int        ParticleId()     {return ParticleType();} //interface
	int        ParticleStatus() {return _particleStatus;}
	double     Time()     {return _time;}
	double     Weight()   {return _weight;}
	Hep3Vector Position() {return _position;}
	Hep3Vector Momentum() {return _momentum;}
	Hep3Vector Spin()     {return _spin;}

	void       EventNumber   (int number){_eventNumber    = number;}
	void       ParticleNumber(int number){_particleNumber = number;}
	void       ParticleType  (int type)  {_particleType   = type;}
	void       ParticleStatus(int status){_particleStatus = status;}
	void       Time    (double time)     {_time =time;}
	void       Weight  (double weight)   {_weight = weight;}
	void       Position(Hep3Vector pos)  {_position = pos;}
	void       Momentum(Hep3Vector mom)  {_momentum = mom;}
	void       Spin    (Hep3Vector spin) {_spin = spin;}

	//convert from icool pid to pdg pid and back
	static int IcoolToPdgPid(int icoolPid);
	static int PdgToIcoolPid(int pdgPid);

private:


	int        _eventNumber;
	int        _particleNumber;
	int        _particleType;
	int        _particleStatus;
	double     _time;
	double     _weight;
	Hep3Vector _position;
	Hep3Vector _momentum;
	Hep3Vector _spin;

	static int icoolPid[];
	static int pdgPid[];

};

std::ostream& operator<<(std::ostream&, const For003Bank& bank);
std::istream& operator>>(std::istream&, For003Bank& bank);

#endif
