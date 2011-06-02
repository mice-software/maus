// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef G4BLBANK_HH
#define G4BLBANK_HH

#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

class G4BLBank
{
public:
	G4BLBank();
	~G4BLBank();

	void Read (std::istream &fin);
	void Write(std::ostream &fout) const;

	static void ReadHeader (std::istream& fin);
	static void WriteHeader(std::ostream& fout);

	int        EventNumber()    {return _eventNumber;}
	int        TrackId()        {return _trackId;}
	int        ParticleId()     {return _particleId;}
	int        Parent()         {return _parent;}
	double     Time()     {return _time;}
	double     Weight()   {return _weight;}
	Hep3Vector Position() {return _position;}
	Hep3Vector Momentum() {return _momentum;}
	Hep3Vector Spin()     {return Hep3Vector(0,0,0);}

	void       EventNumber   (int number){_eventNumber = number;}
	void       TrackId       (int id)    {_trackId     = id;}
	void       ParticleId    (int id)    {_particleId  = id;}
	void       Parent        (int parent){_parent = parent;}
	void       Time    (double time)     {_time =time;}
	void       Weight  (double weight)   {_weight = weight;}
	void       Position(Hep3Vector pos)  {_position = pos;}
	void       Momentum(Hep3Vector mom)  {_momentum = mom;}

private:
	Hep3Vector _position;
	Hep3Vector _momentum;
	double     _time;
	int        _particleId;
	int        _eventNumber;
	int        _trackId;
	int        _parent;
	double     _weight;
};

std::ostream& operator<<(std::ostream&, const G4BLBank& bank);
std::istream& operator>>(std::istream&, G4BLBank& bank);

#endif
