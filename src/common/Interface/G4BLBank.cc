// MAUS WARNING: THIS IS LEGACY CODE.
#include <stdio.h>
#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
#include "Interface/G4BLBank.hh"
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::ns;
using CLHEP::mm;
using CLHEP::MeV;

G4BLBank::G4BLBank()
	: _position(0,0,0), _momentum(0,0,0), _time(0), _particleId(0), _eventNumber(0), _trackId(0), _parent(0), _weight(0)

{;}

G4BLBank::~G4BLBank() {}

void G4BLBank::Read(std::istream &fin)
{
	fin >> _position[0] >> _position[1] >> _position[2];
	fin >> _momentum[0] >> _momentum[1] >> _momentum[2]  >> _time;
	fin >> _particleId >> _eventNumber >> _trackId >>  _parent >> _weight;

	//I don't think these units are dynamic
	_time     *= ns;
	_position *= mm;
	_momentum *= MeV;
}

void G4BLBank::Write(std::ostream &fout) const
{
	fout << _position[0]/mm << " " << _position[1]/mm  << " " <<  _position[2]/mm << " ";
	fout << _momentum[0]/MeV  << " " <<  _momentum[1]/MeV  << " " <<  _momentum[2]/MeV  << " " <<  _time/ns << " ";
	fout << _particleId  << " " <<  _eventNumber  << " " <<  _trackId  << " " <<   _parent  << " " <<  _weight;
}

void G4BLBank::ReadHeader(std::istream& fin)
{
	std::string header;
	getline(fin, header);
	getline(fin, header);
	getline(fin, header);
}

void G4BLBank::WriteHeader(std::ostream& fout)
{
	fout << "#Beam file by G4MICE\n";
	fout << "#x y z Px Py Pz t PDGid Ev# TrkId Parent wt" << std::endl;
}


std::ostream& operator<<(std::ostream& out, const G4BLBank& bank)
{
	bank.Write(out);
	return out;
}

std::istream& operator>>(std::istream& in, G4BLBank& bank)
{
	bank.Read(in);
	return in;
}




