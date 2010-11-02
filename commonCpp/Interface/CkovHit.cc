/* CkovHit.cc
*/

#include <iostream>
#include <sstream>

#include "CkovHit.hh"

using namespace std;
using CLHEP::eV;
using CLHEP::nm;

CkovHit::CkovHit(const CkovHit& right)
{
   PmtNo  = right.PmtNo;
   VesselNo  = right.VesselNo;
   IncidentAngle  = right.IncidentAngle;
	PolarizationAngle  = right.PolarizationAngle;
	Wavelenght = right.Wavelenght;
	Polarization = right.Polarization;
}

const CkovHit& CkovHit::operator=(const CkovHit& right)
{
   PmtNo  = right.PmtNo;
   VesselNo  = right.VesselNo;
   IncidentAngle  = right.IncidentAngle;
	PolarizationAngle  = right.PolarizationAngle;
	Wavelenght = right.Wavelenght;
	Polarization = right.Polarization;
   return *this;
}


void CkovHit::Print()
{
  	cout << "____________________________" << endl;
  	cout << "  CkovHit" << endl;
	cout << "  Plane Number : " << VesselNo << endl;
  	cout << "  PMT Number : " << PmtNo << endl;
  	cout << "  Photon energy : " << GetEnergy()/eV << " eV "<< endl;
  	cout << "  Photon wavelenght : " << GetWavelenght()/nm << " nm "<< endl;
  	cout << "  Angle of incidence: " << IncidentAngle <<" rad "<< endl;
  	cout << "____________________________" << endl;
}

