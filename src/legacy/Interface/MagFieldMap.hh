// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef MAGFIELDMAP_HH
#define MAGFIELDMAP_HH

#include <iostream>
#include <string>

#include "Interface/SplineInterpolator.hh"

class MagFieldMap
{

public:

	// Map constructor
	// Need fileType as some external maps don't have a file type identifier in the file e.g. ICOOL
	// All lower case fileType please
	// fileTypes are:
	// icool, g4micetext, g4beamline, gregoire, g4micebinary
	MagFieldMap(const std::string& mapFile, const std::string& fileType, std::string intepolation);
	MagFieldMap(Interpolator3dSolenoidalTo3d* Interpolator);
	MagFieldMap(const MagFieldMap& rhs);
	MagFieldMap();

	// Map destructor
	~MagFieldMap();

	// retrieve the field at cartesian Point[4]
	inline void GetFieldValue( const  double Point[4], double *Bfield ) const
	{	myInterpolator->F(Point, Bfield); }

	//read a map file with file name mapFile and type fileType
	void ReadMap(const std::string& mapFile, const std::string& fileType, std::string interpolation);

	//write a map file with file name mapFile and sheet information in sheetInformation
	void WriteG4MiceBinaryMapV1(std::string mapFile);//, vector<float*> sheetInformation);
	//return true if the sheet info in the file is the same as the sheet info in the vector<float*>
	bool IsSameG4MiceBinary(vector<double*> sheetInformation, std::string mapFile);
	// return the length of the map
	inline double zMinf(){return myZMin;}
	inline double zMaxf(){return myZMax;}
	inline double rMinf(){return myRMin;}
	inline double rMaxf(){return myRMax;}
	inline int    nZPt (){return numberOfRCoords;}
	inline int    nRPt (){return numberOfZCoords;}

	// return shallow copy of this->sheetInformation
	vector<double*> GetSheetInformation() {return sheetInformation;}
	//Shallow copy from someSheetInformation to this->sheetInformation
	//delete anything currently in this->sheetInformation
	void SetSheetInformation(vector<double*> someSheetInformation);
	std::string GetFileName() {return myFileName;}
	Interpolator3dSolenoidalTo3d* GetInterpolator() {return myInterpolator;}
	static std::string ReplaceVariables( std::string fileName );

private:
	std::string myFieldCalculation; //  "HardEdge" or "Interpolated" (linear interpolation)
	std::string mygridtype; // "Uniform" or "NonUniform"
	std::string myFileName;
	vector<double*> sheetInformation; // some files contain sheet information
	double zOffset; //the zoffset for eg ICOOL files
	//std::fstream fMap;


	//Spline Interpolator takes the field grid and spits out B-field at any point
	Interpolator3dSolenoidalTo3d* myInterpolator;

	int    numberOfRCoords;
	int    numberOfZCoords;

	double myZMin;
	double myZMax;
	double myRMin;
	double myRMax;

	inline double GetZOffset() {return zOffset;};
	inline double SetZOffset(double offset) {return (zOffset = offset);};

	void               ReadG4MiceBinaryMap(std::ifstream &fMap, std::string interpolation);


	SplineInterpolator ReadIcoolMap(std::ifstream &fMap);
	SplineInterpolator ReadG4MiceTextMap(std::ifstream &fMap);
	SplineInterpolator ReadG4MiceBinaryHeader(std::ifstream &fMap);
	SplineInterpolator ReadGregoireMap(std::ifstream &fMap);
	SplineInterpolator ReadG4BeamlineMap(std::ifstream &fMap);
	SplineInterpolator ReadPandiraMap(std::ifstream &fMap);


}; // class MagFieldMap

#endif  // MAGFIELDMAP _HH
