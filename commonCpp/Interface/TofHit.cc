//--------------------------------------------------------------------------
//
//    File :      TofHit.cc
//    Purpose :   Variable bank for TOF hits
//    Created :   17-SEP-2002  by Steve Kahn and Kevin Lee
//
//--------------------------------------------------------------------------

#include "TofHit.hh"
#include <iostream>
#include <fstream>

using namespace std;
using CLHEP::ns;
using CLHEP::cm;
using CLHEP::MeV;
TofHit::TofHit()
{
  miceMemory.addNew( Memory::TofHit );
  //std::cout << "TofHit created" << std::endl;
  _pathLength = 0.;
  _stationNumber = -1;
  _planeNumber = -1;
}

TofHit::TofHit(const TofHit& th):MCHit(th)
{
  miceMemory.addNew( Memory::TofHit );
        //std::cout << "TofHit copied" << std::endl;
        _planeNumber =   th._planeNumber;
        _stripNumber =   th._stripNumber;
        _stationNumber = th._stationNumber;
        _pathLength =    th._pathLength;
        _volName =       th._volName;
}

const TofHit& TofHit::operator=(const TofHit& th)
{
	MCHit::operator=(th);

        _planeNumber =   th._planeNumber;
        _stripNumber =    th._stripNumber;
        _stationNumber = th._stationNumber;
        _pathLength =    th._pathLength;
        _volName =       th._volName;
        return *this;
}

void TofHit::DecodeVolumeName()
{       int iplane;
        _planeNumber = -1;
        _stationNumber = -1;
        _stripNumber = -1;
        sscanf(_volName.c_str(), "TOFstrip%d", &iplane);
        _stationNumber = iplane/1000;
        iplane -= 1000*_stationNumber;
        _planeNumber = iplane/100;
        iplane -= 100*_planeNumber;
        _stripNumber = iplane/10;
}

void TofHit::AddEdep(double moreEdep)
{
	double E_dep = GetEdep() + moreEdep;
	SetEdep(E_dep);
}

void TofHit::AddPathLength(double moreLenght)
{
	double Path_Length = GetPathLength() + moreLenght;
	SetPathLength(Path_Length);
}

void TofHit::Print()
{
  	cout << "____________________________" << endl;
  	cout << "  TofHit" << endl;
	cout << "  Station Number : " << _stationNumber << endl;
	cout << "  Plane Number : "<<  _planeNumber<< endl;
  	cout << "  Strip Number : "<< _stripNumber << endl;
  	cout << "  Time : " << GetTime()/ns <<" ns" << endl;
  	cout << "  Energy deposit : " << GetEdep()/MeV <<" MeV"<< endl;
  	cout << "  Path Length : " << _pathLength/cm <<" cm"<< endl;
  	cout << "____________________________" << endl;
}

