#include <stdio.h>
#include <fstream>
#include "CLHEP/Vector/ThreeVector.h"
#include "Interface/For003Bank.hh"
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::second;
using CLHEP::meter;
using CLHEP::GeV;

int For003Bank::icoolPid[11] = {0,  1,  2,  3,  4,   5,-1,-2, -3,   -4,   -5};
int For003Bank::pdgPid[11]   = {0,-11,-13,211,321,2212,11,13,-211,-321,-2212};

For003Bank::For003Bank()
          : _eventNumber(0), _particleNumber(0), _particleType(0), _particleStatus(0), _time(0), 
            _weight(0), _position(0,0,0), _momentum(0,0,0), _spin(0,0,0)
{;}

For003Bank::~For003Bank() {}

//    evt par typ  flg   reg    time        x           y           z           Px          Py          Pz          Bx          By          Bz          wt          Ex          Ey          Ez          arclength   polX        polY        polZ

void For003Bank::Read(std::istream &fin)
{
	fin >> _eventNumber >> _particleNumber >> _particleType >> _particleStatus >> _time >> _weight;
	fin >> _position[0] >> _position[1] >> _position[2];
	fin >> _momentum[0] >> _momentum[1] >> _momentum[2];
	fin >> _spin[0] >> _spin[1] >> _spin[2];

	_time     *= second;
	_position *= meter;
	_momentum *= GeV;
	_particleType = IcoolToPdgPid(_particleType);
}

void For003Bank::Write(std::ostream &fout) const
{
	fout << _eventNumber << " " << _particleNumber << " " << PdgToIcoolPid(_particleType) << " ";
	fout << _particleStatus << " " << _time << " " << _weight << " ";
	fout << _position[0] << " " << _position[1] << " " << _position[2] << " ";
	fout << _momentum[0] << " " << _momentum[1] << " " << _momentum[2] << " ";
	fout << _spin[0] << " " << _spin[1] << " " << _spin[2];
}

int For003Bank::IcoolToPdgPid(int icoolID)
{
	for(unsigned int i=0; i<11; i++)
		if(icoolPid[i] == icoolID) return pdgPid[i];
	return 0;
}

int For003Bank::PdgToIcoolPid(int pdgID)
{
	for(unsigned int i=0; i<11; i++)
		if(pdgPid[i] == pdgID) return icoolPid[i];
	return 0;
}

void For003Bank::ReadHeader(std::istream& fin)
{
	std::string header;
	getline(fin, header);
	getline(fin, header);
}

void For003Bank::WriteHeader(std::ostream& fout)
{
	fout << "ICOOL input file from G4MICE\n0. 0. 0. 0. 0. 0. 0. 0.\n"; 
}


std::ostream& operator<<(std::ostream& out, const For003Bank& bank)
{
	bank.Write(out);
	return out;
}

std::istream& operator>>(std::istream& in, For003Bank& bank)
{
	bank.Read(in);
	return in;
}




