// MAUS WARNING: THIS IS LEGACY CODE.
#include "Interface/MagFieldMap.hh"
#include "Interface/SplineInterpolator.hh"
#include "Utils/Exception.hh"
#include "Utils/Squeak.hh"
#include "Interface/STLUtils.hh"

#include <string>
#include <streambuf>
#include <fstream>

//constructor
MagFieldMap::MagFieldMap(const std::string& mapFile, const std::string& fileType, std::string interpolation)
              : myFieldCalculation("interpolation"), myFileName(mapFile), zOffset(0), 
                myInterpolator(NULL)
{
	ReadMap(mapFile, fileType, interpolation);
}


MagFieldMap::MagFieldMap(Interpolator3dSolenoidalTo3d* Interpolator)
              : myFieldCalculation("interpolation"), myFileName(""), zOffset(0), 
                myInterpolator(Interpolator),
                numberOfRCoords(Interpolator->GetGrid()->xSize()), numberOfZCoords(Interpolator->GetGrid()->ySize()), 
                myZMin( Interpolator->GetGrid()->y(1) ), myZMax( Interpolator->GetGrid()->y(numberOfZCoords) ),
                myRMin( Interpolator->GetGrid()->x(1) ), myRMax( Interpolator->GetGrid()->x(numberOfRCoords) )
{
}

MagFieldMap::MagFieldMap(const MagFieldMap & rhs) :
             myFieldCalculation(rhs.myFieldCalculation), mygridtype(rhs.mygridtype), 
             myFileName(rhs.myFileName), zOffset(rhs.zOffset),
             myInterpolator(rhs.myInterpolator),myZMin(rhs.myZMin), myZMax(rhs.myZMax),
             myRMin(rhs.myRMin), myRMax(rhs.myRMax)
{
	SetSheetInformation(rhs.sheetInformation);
}


MagFieldMap::MagFieldMap()
              : myFieldCalculation("interpolation"), zOffset(0), myInterpolator(NULL)
{
}


// Destructor
MagFieldMap::~MagFieldMap()
{
	for(unsigned int i=0; i<sheetInformation.size(); i++)
		delete [] sheetInformation[i];
  delete myInterpolator;
}

void MagFieldMap::ReadMap(const std::string& mapFile, const std::string& fileType, std::string interpolation)
{
	//Open the field map
	myFileName = ReplaceVariables(mapFile);
	std::ifstream fMap;
	if((fileType == "icool") || (fileType == "g4micetext") || (fileType == "g4beamline") || (fileType == "gregoire") || fileType == "parmila")
		fMap.open(myFileName.c_str());
	else if(fileType == "g4micebinary")
		fMap.open(myFileName.c_str(), std::fstream::in | std::fstream::binary);
	else throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Field map type "+fileType+" not supported", "MagFieldMap::ReadMap"));
	if (!fMap.is_open()) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error opening field map file"+myFileName, 
	                                                       "MagFieldMap::ReadMap"));

	//Read the field map; built interpolator; read bounds (needed for bound checking) and grid size (needed for write)
	if((fileType=="icool") && (myFieldCalculation == "interpolation"))
		myInterpolator = new SplineInterpolator(ReadIcoolMap(fMap));
	else if((fileType=="g4micetext") && (myFieldCalculation == "interpolation"))
		myInterpolator = new SplineInterpolator(ReadG4MiceTextMap(fMap));
	else if((fileType == "g4micebinary") && (myFieldCalculation == "interpolation"))
		ReadG4MiceBinaryMap(fMap, interpolation);
	else if((fileType == "parmila") && (myFieldCalculation == "interpolation"))
		myInterpolator = new SplineInterpolator(ReadPandiraMap(fMap));
	else if((fileType == "gregoire") && (myFieldCalculation == "interpolation"))
		myInterpolator = new SplineInterpolator(ReadGregoireMap(fMap));
	else if((fileType == "g4beamline") && (myFieldCalculation == "interpolation"))
		myInterpolator = new SplineInterpolator(ReadG4BeamlineMap(fMap));

	fMap.close();
}


SplineInterpolator MagFieldMap::ReadG4MiceTextMap(std::ifstream &fMap)
{
	double **inputBr=0, **inputBz=0;
	double minZ, maxZ, minR, maxR, dZ, dR;
	std::string dummy;

	//read in header info
    fMap >> dummy >> dummy >> dummy >> mygridtype >> dummy >> dummy >> dummy;
	fMap >> dummy >> dummy >> dummy >> minZ >> dummy >> dummy >> maxZ >> dummy >> dummy >> dZ;
	fMap >> dummy >> dummy >> dummy >> minR >> dummy >> dummy >> maxR >> dummy >> dummy >> dR;

	numberOfRCoords=(int)((maxR-minR)/dR)+1;
	numberOfZCoords=(int)((maxZ-minZ)/dZ)+1;

	//awkward way of setting these pointers to 2D arrays
	//To avoid loss of information at the Grid fringe, our spline is always a little bit bigger than the grid.
	inputBr = new double*[numberOfRCoords];
	inputBz = new double*[numberOfRCoords];
	for(int i=0; i<numberOfRCoords; i++)
	{
		inputBr[i] = new double[numberOfZCoords];
		inputBz[i] = new double[numberOfZCoords];
	}

	//Get the b values from the grid
	for(int i=0; i<numberOfZCoords; i++)
		for(int j=0; j<numberOfRCoords; j++)
			fMap >> inputBz[j][i] >> inputBr[j][i];

	//Set the z length parameters
	myZMin = minZ;
	myZMax = maxZ;
	//Set the r width parameters
	myRMin = minR;
	myRMax = maxR;

	return SplineInterpolator(dR, dZ, minZ, numberOfRCoords, numberOfZCoords,
	                          inputBz, inputBr);
}

SplineInterpolator MagFieldMap::ReadG4BeamlineMap(std::ifstream &fMap)
{
	double **inputBr=0, **inputBz=0;
	double minZ, minR, dZ, dR;
	std::string dummy;
	char cha;
	getline(fMap, dummy);
	getline(fMap, dummy);

	//ME set value of minR
        minR = 0;

	//read in header info
  fMap >> dummy >>
          cha >> cha >> cha >> minZ >>
          cha >> cha >> cha >> dZ >>
          cha >> cha >> cha >> numberOfZCoords >>
          cha >> cha >> cha >> dR >>
          cha >> cha >> cha >> numberOfRCoords;

	getline(fMap, dummy);
	getline(fMap, dummy);

	//awkward way of setting these pointers to 2D arrays
	//To avoid loss of information at the Grid fringe, our spline is always a little bit bigger than the grid.
	inputBr = new double*[numberOfRCoords];
	inputBz = new double*[numberOfRCoords];
	for(int i=0; i<numberOfRCoords; i++)
	{
		inputBr[i] = new double[numberOfZCoords];
		inputBz[i] = new double[numberOfZCoords];
	}

	//Get the b values from the grid
	for(int i=0; i<numberOfZCoords; i++)
		for(int j=0; j<numberOfRCoords; j++)
		{
			fMap >> dummy >> dummy >> inputBr[j][i] >> inputBz[j][i];
			inputBr[j][i]*=1e-3; //convert T to kT
			inputBz[j][i]*=1e-3; //convert T to kT
		}

	//Set the z length parameters
	myZMin = minZ;
	myZMax = minZ + dZ * (double)(numberOfZCoords-1);
	//Set the r width parameters
	myRMin = minR;
	myRMax = minR + dR * (double)(numberOfRCoords-1);

	if(!fMap) 
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "There was a problem while reading a fieldmap.", "MagFieldMap::ReadG4BeamlineMap" ));


	return SplineInterpolator(dR, dZ, minZ, numberOfRCoords, numberOfZCoords,
	                          inputBz, inputBr);
}



SplineInterpolator MagFieldMap::ReadIcoolMap(std::ifstream &fMap)
{
	double **inputBr=0, **inputBz=0;
	numberOfRCoords=0, numberOfZCoords=0;
	std::string dummy;

	//read in header info
	getline(fMap, dummy);
	fMap >> numberOfZCoords >> numberOfRCoords;

	//work out the dR and dZ

	//awkward way of setting these pointers to 2D arrays
	inputBr = new double*[numberOfRCoords];
	inputBz = new double*[numberOfRCoords];
	for(int i=0; i<numberOfRCoords; i++)
	{
		inputBr[i] = new double[numberOfZCoords];
		inputBz[i] = new double[numberOfZCoords];
	}

	double *inputZ = new double[numberOfZCoords];
	double *inputR = new double[numberOfRCoords];

	//Get the b values from the grid
	for(int i=0; i<numberOfZCoords; i++)
		for(int j=0; j<numberOfRCoords; j++)
		{
			fMap >> dummy >> dummy >> inputZ[i] >> inputR[j] >> inputBz[j][i] >> inputBr[j][i];
			inputBz[j][i] *= 1e-3;
			inputBr[j][i] *= 1e-3;
		}

	double minZ = GetZOffset();

	double dZ = (inputZ[1] - inputZ[0]) * 1.e3;
	double dR = (inputR[1] - inputR[0]) * 1.e3;
	//Set the z length parameters
	myZMin = inputZ[0]*1.e3;
	myZMax = inputZ[numberOfZCoords-1]*1.e3;
	//Set the r width parameters
	myRMin = inputR[0]*1.e3;
	myRMax = inputR[numberOfRCoords-1]*1.e3;

//	MAUS::Squeak::mout(MAUS::Squeak::debug) << "icool field parameters nZ:" << numberOfZCoords << " dZ: " << dZ << " zMin: " << myZMin << " zMax: " << myZMax << " nR: "
//                                    << numberOfRCoords << " dR: " << dR << " rMin: " << myRMin << " rMax: " << myRMax << std::endl;
	delete [] inputZ;
	delete [] inputR;
	return SplineInterpolator(dR, dZ, minZ, numberOfRCoords, numberOfZCoords,
	                          inputBz, inputBr);
}

SplineInterpolator MagFieldMap::ReadGregoireMap(std::ifstream &fMap)
{
	std::string dummy="";
	numberOfRCoords=0, numberOfZCoords=0;
	double dR, r1, r2, dZ, z1, z2, minZ;
	double ** inputBr, ** inputBz;

	//skip header
	getline(fMap, dummy);
	getline(fMap, dummy);

	//work out the dR and dZ - not easy!
	fMap >> dummy >> dummy >> dummy >> z1 >> r1 >> dummy >> dummy;
	getline(fMap, dummy);

	z2 = z1;
	//scan to the point on the grid one dr and one dz away
	while(z2==z1)
	{
		fMap >> dummy >> dummy >> dummy >> z2 >> r2 >> dummy >> dummy;
		getline(fMap, dummy);
	}
	fMap >> dummy >> dummy >> dummy >> z2 >> r2 >> dummy >> dummy;
	getline(fMap, dummy);

	//Now extract dr, dz
	dR = (r2-r1)*10.; // remembering to convert from cm to mm
	dZ = (z2-z1)*10.;
	//Work out the extent of the map
	while(fMap)
		fMap >> dummy >> dummy >> dummy >> z2 >> r2 >> dummy >> dummy;

	numberOfRCoords = (int)((r2 - r1 / dR) + 1);
	numberOfZCoords = (int)((z2 - z1 / dZ) + 1);

	minZ = GetZOffset();

	//awkward way of setting these pointers to 2D arrays
	inputBr = new double*[numberOfRCoords];
	inputBz = new double*[numberOfRCoords];
	for(int i=0; i<numberOfRCoords; i++)
	{
		inputBr[i] = new double[numberOfZCoords];
		inputBz[i] = new double[numberOfZCoords];
	}

	//return to the beginning of the mapfile
	fMap.seekg(0, std::ios::beg);
	fMap.clear();

	//skip header
	getline(fMap, dummy);
	getline(fMap, dummy);

	//now read in the bfields
	for(int i=0; i<numberOfZCoords; i++)
		for(int j=0; j<numberOfRCoords; j++)
		{
			fMap >> dummy >> dummy >> dummy >> dummy >> dummy >> inputBr[j][i] >> inputBz[j][i];
			getline(fMap, dummy);
			inputBz[j][i] *= 1e-7; //convert from Gauss to kT
			inputBr[j][i] *= 1e-7; //convert from Gauss to kT
		}

	//Set the z length parameters
	myZMin = minZ;
	myZMax = minZ + dZ*(numberOfZCoords-1);
	//Set the r width parameters
	myRMin = 0;
	myRMax = (numberOfRCoords - 1) * dR;

	if(!fMap)
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "There was a problem while loading a field map", "MagFieldMap::LoadGregoireMap"));

	return SplineInterpolator(dR, dZ, minZ, numberOfRCoords, numberOfZCoords,
	                          inputBz, inputBr);
}


SplineInterpolator MagFieldMap::ReadPandiraMap(std::ifstream &fMap)
{
	std::string dummy="";
	numberOfRCoords=0, numberOfZCoords=0;

	double rD=0, rOld=1, zD=0, zOld=1, brD=0, bzD=0;
	std::vector< std::vector<double> > brV, bzV, rV,  zV;

	//skip header
	while(fMap && dummy!="Index")
		fMap >> dummy;
	//work out the dR and dZ - not easy!
	while(fMap)
	{
		fMap >> rD >> zD >> brD >> bzD;
		if(rOld > rD || zOld > zD)
		{
			brV.push_back(std::vector<double>());
			bzV.push_back(std::vector<double>());
			rV .push_back(std::vector<double>());
			zV .push_back(std::vector<double>());
		}
		brV.back().push_back(brD*1e-7);
		bzV.back().push_back(bzD*1e-7);
		rV .back().push_back(rD*10.);
		zV .back().push_back(zD*10.);
		rOld = rD;
		zOld = zD;
		getline(fMap, dummy);
	}

	double ** bzA = new double*[bzV[0].size()];
	double ** brA = new double*[brV[0].size()];
	for(unsigned int i=0; i<bzV[0].size(); i++)
	{
		bzA[i] = new double[bzV.size()];
		brA[i] = new double[bzV.size()];
		for(unsigned int j=0; j<bzV.size(); j++)
		{
			bzA[i][j] = bzV[j][i];
			brA[i][j] = brV[j][i];
//                        std::cout << i << "  " << j << "  " << rV[j][i] << " " << zV[j][i] << " " << brV[j][i] << "  " << bzV[j][i] << std::endl;
		}
	}

	//Set the z length parameters
	myZMin = zV[0][0];
	myZMax = zV.back()[0];
	//Set the r width parameters
	myRMin = rV[0][0];
	myRMax = zV[0].back();
	return SplineInterpolator(rV[0][1]-rV[0][0], zV[1][0]-zV[0][0], zV[0][0], zV[0].size(), zV.size(), bzA, brA);
//	return SplineInterpolator(dR, dZ, minZ, numberOfRCoords, numberOfZCoords, inputBz, inputBr);
}


void MagFieldMap::ReadG4MiceBinaryMap(std::ifstream &fMap, std::string algorithm)
{
	double **inputBr=0, **inputBz=0;
	double *rCoordinates=0, *zCoordinates=0;
	numberOfRCoords=0, numberOfZCoords=0;
	std::string dummy;

	int nSheet; // number of sheets
	fMap.read(reinterpret_cast<char*>(&nSheet), sizeof(nSheet));
	//floats for backward compatibility
	float aSheet[9]; // array to store all sheet related data

	for (int i=0; i<nSheet; i++)
	{
		fMap.read(reinterpret_cast<char*>(&aSheet), sizeof(aSheet));
		sheetInformation.push_back(new double[9]);
		for(int j=0; j<9; j++)
			sheetInformation[i][j] = (double)aSheet[j];
	}

	fMap.read(reinterpret_cast<char*>(&numberOfRCoords), sizeof(numberOfRCoords));
	fMap.read(reinterpret_cast<char*>(&numberOfZCoords), sizeof(numberOfZCoords));

	numberOfRCoords++;
	numberOfZCoords++;

	rCoordinates = new double[numberOfRCoords];
	zCoordinates = new double[numberOfZCoords];
	inputBr = new double*[numberOfRCoords];
	inputBz = new double*[numberOfRCoords];
	for(int i=0; i<numberOfRCoords; i++)
	{
		inputBr[i] = new double[numberOfZCoords];
		inputBz[i] = new double[numberOfZCoords];
	}

	double outArray[4];
	//Get the b values from the grid
	for(int i=0; i<numberOfRCoords; i++)
	{
		for(int j=0; j<numberOfZCoords; j++)
		{
			fMap.read(reinterpret_cast<char*>(outArray), sizeof(outArray));
			zCoordinates[j] =  outArray[1];
			inputBr[i][j]   = outArray[2];
			inputBz[i][j]   = outArray[3];
		}
		rCoordinates[i] = outArray[0];
	}

	myZMin = zCoordinates[0];
	myRMin = rCoordinates[0];
	myZMax = zCoordinates[numberOfZCoords-1];
	myRMax = rCoordinates[numberOfRCoords-1];
	TwoDGrid* grid = new TwoDGrid(numberOfRCoords, rCoordinates, numberOfZCoords, zCoordinates);
	myInterpolator = new Interpolator3dSolenoidalTo3d(grid, inputBr, inputBz, algorithm);
  // myInterpolator does not take ownership of memory so we now need to clean up
	for(int i=0; i<numberOfRCoords; i++)
	{
		delete [] inputBr[i];
		delete [] inputBz[i];
	}
	delete [] inputBr;
	delete [] inputBz;


//	myInterpolator = new SplineInterpolator(rCoordinates[1]-rCoordinates[0], zCoordinates[1]-zCoordinates[0], myZMin, numberOfRCoords, numberOfZCoords,
//	                   inputBz, inputBr);
}


void MagFieldMap::WriteG4MiceBinaryMapV1(std::string mapFile)
{
  mapFile = STLUtils::ReplaceVariables(std::string(mapFile)).c_str();
	myFileName = mapFile;
	std::ofstream fout;
	fout.open(mapFile.c_str(), std::fstream::out | std::fstream::binary);
	if(!fout.is_open())
	{
		std::cerr << "Failed to open field map output file " << mapFile << std::endl;
		fout.close();
		return;
	}

	float aSheetInformation[9];
	//should be G4double
	double outArray[4] = {0, 0, 0, 0};
	//should be G4int
	int numberOfSheets=(int)sheetInformation.size();
	if(numberOfSheets==0) std::cerr << "Need sheet information to write field map" << std::endl;

	fout.write(reinterpret_cast<char*>(&numberOfSheets),sizeof(numberOfSheets));
	for(int i=0; i<numberOfSheets;i++)
	{
		for(int j=0; j<9; j++)
			aSheetInformation[j] = (double)sheetInformation[i][j];
		fout.write(reinterpret_cast<char*>(aSheetInformation),sizeof(aSheetInformation));
	}

	numberOfRCoords--; //horrible! Backward compatibility
	numberOfZCoords--;

	fout.write(reinterpret_cast<char*>(&numberOfRCoords),sizeof(numberOfRCoords));
	fout.write(reinterpret_cast<char*>(&numberOfZCoords),sizeof(numberOfZCoords));


	Mesh*   myMesh       = myInterpolator->GetMesh();
	double position[2] = {0., 0.};
	double value[3] = {0., 0., 0.};
	Mesh::Iterator itEnd = myMesh->End();
	for (Mesh::Iterator it = myMesh->Begin(); it < itEnd; ++it)
	{
		it.Position(position);
		myInterpolator->F(it, value);
		outArray[0] = position[0];
		outArray[1] = position[1];
		outArray[2] = value[0];
		outArray[3] = value[2];
		fout.write(reinterpret_cast<char*>(outArray), sizeof(outArray));
	}

	if(!fout) std::cerr << "There was a problem writing the field map " << mapFile << std::endl;
	fout.close();
}

void MagFieldMap::SetSheetInformation(vector<double*> someSheetInformation)
{
	for(unsigned int i=0; i<sheetInformation.size(); i++)
		delete [] sheetInformation[i];
	sheetInformation.clear();
	sheetInformation = someSheetInformation;

//	for(unsigned int i=0; i<someSheetInformation.size(); i++)
//	{
//		sheetInformation[i] = someSheetInformation[i];
//	}
}

bool MagFieldMap::IsSameG4MiceBinary(vector<double*> inpSheets, std::string mapFile)
{
	std::string dummy;

	mapFile = STLUtils::ReplaceVariables(std::string(mapFile)).c_str();
	std::ifstream fMap;
	fMap.open(mapFile.c_str(), std::fstream::in | std::fstream::binary);
	if(!fMap) return false;

	int nSheet; // number of sheets
	fMap.read(reinterpret_cast<char*>(&nSheet), sizeof(nSheet));
	//floats for backward compatibility
	vector<double*> sheetInformation;
	float aSheet[9]; // array to store all sheet related data

	for (int i=0; i<nSheet; i++)
	{
		fMap.read(reinterpret_cast<char*>(&aSheet), sizeof(aSheet));
		sheetInformation.push_back(new double[9]);
		for(int j=0; j<9; j++)
			sheetInformation[i][j] = (double)aSheet[j];
	}

	if(inpSheets.size()!=sheetInformation.size()) return false;

	for(unsigned int i=0; i<sheetInformation.size(); i++)
	{
		for(int j=0; j<9; j++)
			if(sheetInformation[i][j]!=inpSheets[i][j])
				return false;
		delete sheetInformation[i];
	}
	fMap.close();
	return true;
}

std::string	MagFieldMap::ReplaceVariables( std::string fileName )
{
  return STLUtils::ReplaceVariables(fileName);
}


