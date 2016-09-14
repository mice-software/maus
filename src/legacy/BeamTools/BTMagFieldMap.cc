// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: BTMagFieldMap.cc,v 1.26 2010-03-08 09:50:21 rogers Exp $  $Name:  $
// Gutted and rewritten by CTR, October 2005

#include "BTMagFieldMap.hh"
#include "Interface/SplineInterpolator.hh"
#include "Interface/MagFieldMap.hh"

#include <string>
#include <iostream>
#include <fstream>
using namespace std;


// constructor assumes type is "g4micetext"
BTMagFieldMap::BTMagFieldMap(const std::string& mapFileName, std::string algorithm)
              : myFileType("g4micetext"), myMapFileName(mapFileName), myFieldMap(NULL), myField(this)
{
	//cout << myFileType << endl;
	myFieldMap = new MagFieldMap(myMapFileName, myFileType, algorithm);
}

// constructor takes a type
BTMagFieldMap::BTMagFieldMap(const std::string& mapFileName, const std::string& fileType, std::string algorithm)
              : myFileType(fileType), myMapFileName(mapFileName), myFieldMap(NULL), myField(this)
{
	myFieldMap = new MagFieldMap(myMapFileName, myFileType, algorithm);
}

// "Write mode" constructor
BTMagFieldMap::BTMagFieldMap(const BTField *aField)
              : myFileType("g4micetext"), myMapFileName(""), myFieldMap(), myField(aField)
{}


// Destructor
BTMagFieldMap::~BTMagFieldMap()
{
	delete myFieldMap;
}

//Only g4micetext format
void BTMagFieldMap::WriteG4MiceTextMap(const std::string& fileName, double Z1,
                    double Z2, double dZ, double R1, double R2, double dR)
{
  //open the output stream
  std::ofstream fMap;
  fMap.open(MagFieldMap::ReplaceVariables(fileName).c_str());
  if (!fMap.is_open()) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error opening field map file"+fileName, 
                                                         "BTMagFieldMap::WriteG4MiceTextMap"));

  double z, r, b[6], pos[4];
  int N = int((Z2-Z1)/dZ+1.5) * int((R2-R1)/dR+1.5);

  fMap << "# GridType = Uniform "     << "N = "         << N  << endl
       << "# Z1 = " << Z1 << " Z2 = " << Z2 << " dZ = " << dZ << endl
       << "# R1 = " << R1 << " R2 = " << R2 << " dR = " << dR << endl;
  pos[0] = 0.; pos[3] = 0.;
  for (z = Z1; z <= Z2; z += dZ)
  {
    pos[2] = z;
    for (r = R1; r <= R2; r += dR)
    {
      pos[1] = r;
      myField->GetFieldValue(pos, b);
      fMap << b[2] << "\t" << b[1] << endl;
    }
  }
  fMap.close();
}

void BTMagFieldMap::Print(std::ostream & out) const
{
	Hep3Vector position = GetGlobalPosition();
	out << "MagneticFieldMap FileType: " << myFileType << " FileName: " << myMapFileName << " ZMin: "
	    << myFieldMap->zMinf() << " ZMax: " << myFieldMap->zMaxf() << " RMin: "
	    << myFieldMap->rMinf() << " RMax: " << myFieldMap->rMaxf();
	BTField::Print(out);
}

CLHEP::HepLorentzVector BTMagFieldMap::GetVectorPotential(CLHEP::HepLorentzVector position) const
{
	CLHEP::HepLorentzVector A(0,0,0,0);
	double point[4] = {0,0,position[2],0};
	double field[6] = {0,0,0,0,0,0};
	GetFieldValue(point, field);
	double f        = 1/2.*field[2];
	A.setX(-position.y()*f);
	A.setY( position.x()*f);
	return A;
}



