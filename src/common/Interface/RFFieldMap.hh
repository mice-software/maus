#ifndef RFFIELDMAP_HH
#define RFFIELDMAP_HH

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cstdlib>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "Interface/Spline1D.hh"

class RFFieldMap
{

public:

	// Constructor to read in a RF cavity field map
	// Only supported filetype is "SuperFish" at the moment
	// Calls the read method
	RFFieldMap(const std::string& mapFile, const std::string& fileType);

	// retrieve the field at cartesian Point[4]
	void GetFieldValue( const  double Point[4], double *Bfield ) const;

	//read a map file with file name mapFile and type fileType
	void ReadMap(const std::string& mapFile, const std::string& fileType);
	//Return the file name
	std::string GetFileName() {return _fileName;}
	std::vector<double> BoundingBoxMin() {std::vector<double> bbMin(3); bbMin[0]=-_rMax; bbMin[1]=-_rMax; bbMin[2]=_zMin; return bbMin;}
	std::vector<double> BoundingBoxMax() {std::vector<double> bbMax(3); bbMax[0]=+_rMax; bbMax[1]=+_rMax; bbMax[2]=_zMax; return bbMax;}
private:
	//Spline 1Ds and then a linear interpolation
	//Thinking about integrating this with the SplineInterpolator
	std::vector<Spline1D> _bphi, _er, _ez;

	//Assume a regular, rectangular grid
	//Boundaries of the map
	double _zMin, _zMax, _rMin, _rMax, _dr;
	//Number of coordinates in r and z
	int    _nZCoords, _nRCoords;
	std::string _fileName;
	//Load field map values into the Spline1Ds above
	void ReadSuperFishSF7Map(std::ifstream &fin);
	void ReadSuperFishBinaryMap(std::ifstream &fin);

	//Do the linear interpolation across splines
	std::vector<double> LinearInterpolation(double z, double r) const;
	std::string	        ReplaceVariables( std::string fileName );


}; // class RFFieldMap

#endif  // RFFIELDMAP_HH
