// MAUS WARNING: THIS IS LEGACY CODE.
#include <cmath>
#include "Utils/Exception.hh"
#include "Interface/RFFieldMap.hh"

// Constructor to read in a RF cavity field map
// Only supported filetype is "SuperFishSF7" at the moment
RFFieldMap::RFFieldMap(const std::string& mapFile, const std::string& fileType)
{
	if(CLHEP::electron_charge); //get rid of compiler warning
	ReadMap(mapFile, fileType);
}



// retrieve the cartesian 6-vector field=(bx,by,bz,ex,ey,ez) at cartesian 4-vector Point[]=(x,y,z,t)
// needs to be wrapped in phasing/timing etc by the BTRFFieldMap class
void RFFieldMap::GetFieldValue( const  double Point[4], double *Bfield ) const
{
	double r   = sqrt(Point[1]*Point[1] + Point[0]*Point[0]);
	if(r > _rMax || r < _rMin || Point[2] > _zMax || Point[2] < _zMin) return;

	std::vector<double> interpolatedField = LinearInterpolation(Point[2], r); //bphi = 0, ez=1, er=2

	//Bfield = B_theta
	if(r != 0)
	{
		Bfield[0] = interpolatedField[0]*Point[1]/r; //bx = bphi*y/r
		Bfield[1] = interpolatedField[0]*Point[0]/r; //by = bphi*x/r
		Bfield[3] = interpolatedField[2]*Point[0]/r; //ex = er*x/r
		Bfield[4] = interpolatedField[2]*Point[1]/r; //ey = er*y/r
	}
	else
	{
		Bfield[0] = 0.;
		Bfield[1] = 0.;
		Bfield[3] = 0.;
		Bfield[4] = 0.;
	}
	Bfield[2] = 0;
	Bfield[5] = interpolatedField[1]; //ez
}

//read a map file with file name mapFile and type fileType
void RFFieldMap::ReadMap(const std::string& mapFile, const std::string& fileType)
{
	try
	{
	_fileName = ReplaceVariables(mapFile);
	std::ifstream fin;
	fin.open(_fileName.c_str());

	if(!fin)
	{
		std::cerr << "Error opening RF Map File " << mapFile << std::endl;
		exit(1);
	}
		if(fileType=="SuperFishSF7" || fileType == "") ReadSuperFishSF7Map(fin);
		else if(fileType == "SuperFishBinary") std::cerr << "SFBinary not implemented" << std::endl;
	}
	catch(...)
	{
		throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "There was a problem accessing the RFFieldMap", "RFFieldMap::ReadMap"));
	}
}

//Load field map values into the Spline1Ds above
void RFFieldMap::ReadSuperFishSF7Map(std::ifstream &fin)
{
	std::string    string1="", string2="";
	while(fin && string2!="Electromagnetic") //skip to the line with first word "Electromagnetic"
	{
		std::getline(fin, string1); // create a strstream with one line of data in it
		std::stringstream line(string1);
		line >> string2; // load the first word into string2
	}
	//next line is something like "(ZMin, RMin) = (1,1)"
	std::getline(fin, string1, '(');
	std::getline(fin, string1, '(');
	std::getline(fin, string1, ','); //ZMin
	std::stringstream number1(string1);
	number1 >> _zMin; // load the first word into string2
	std::getline(fin, string1, ')'); //RMin
	std::stringstream number2(string1);
	number2 >> _rMin; // load the first word into string2

	//next line is something like "(ZMax, RMax) = (1,1)"
	std::getline(fin, string1, '(');
	std::getline(fin, string1, '(');
	std::getline(fin, string1, ','); //ZMax
	std::stringstream number3(string1);
	number3 >> _zMax; // load the first word into string2
	std::getline(fin, string1, ')'); //RMax
	std::stringstream number4(string1);
	number4 >> _rMax; // load the first word into string2

	_zMin *= CLHEP::cm;
	_zMax *= CLHEP::cm;
	_rMin *= CLHEP::cm;
	_rMax *= CLHEP::cm;

	//read in nZCoords, nRcoords (file stores number of "increments" so need to add 1)
	fin >> string1 >> string1 >> string1 >> string1 >> _nZCoords >> _nRCoords;
	_nRCoords++;
	_nZCoords++;
	//skip now to the actual data
	for(int i=0; i<4; i++)
	{
		std::getline(fin, string1);
		//std::cout << string1 <<std::endl;
	}

	//Set up arrays (DO WE WANT THE SPLINE IN R OR Z?)
	std::vector<double*> erVec(_nRCoords), ezVec(_nRCoords), bphiVec(_nRCoords);
	for(int i=0; i<_nRCoords; i++)
	{
		ezVec[i] = new double[_nZCoords];
		bphiVec[i] = new double[_nZCoords];
		erVec[i] = new double[_nZCoords];
	}
	//now read in the data
	double* z = new double[_nZCoords];
	double* r = new double[_nRCoords];

	double e;
	int zCounter=0, rCounter=0;
	for(rCounter=0; rCounter < _nRCoords; rCounter++)
		for(zCounter=0; zCounter < _nZCoords; zCounter++)
		{
			std::getline(fin, string1); // create a strstream with one line of data in it
			std::stringstream line(string1);
			line >> z[zCounter] >> r[rCounter] >> ezVec[rCounter][zCounter] >> erVec[rCounter][zCounter]
			     >> e >> bphiVec[rCounter][zCounter];
			//E is dimensionless (normalised to 1)
			erVec[rCounter][zCounter]   *= 1.; 
			ezVec[rCounter][zCounter]   *= 1.;
			//B = mu0 H [A/m]
			//But need extra factor of 1.e3 to account for E in MV/m (need to keep ratio B/E correct)
			bphiVec[rCounter][zCounter] *= CLHEP::ampere/CLHEP::m*CLHEP::mu0*1.e3; 
			z[zCounter]                 *= CLHEP::cm;
			r[rCounter]                 *= CLHEP::cm;
		}


	if(!fin) std::cerr << "There was a problem while accessing RF Field map file " << std::endl;

	for(rCounter = 0; rCounter < _nRCoords; rCounter++)
	{
		_bphi.push_back(Spline1D(_nZCoords, z, bphiVec[rCounter]));
		_er.push_back(  Spline1D(_nZCoords, z, erVec[rCounter]));
		_ez.push_back(  Spline1D(_nZCoords, z, ezVec[rCounter]));
	}

	_dr = (_rMax - _rMin)/(double)(_nRCoords-1);

	for(int i=0; i<_nRCoords; i++)
	{
		delete[] bphiVec[i];
		delete[] erVec[i];
		delete[] ezVec[i];
	}

}

//Do the linear interpolation across splines
std::vector<double> RFFieldMap::LinearInterpolation(double z, double r) const
{
	int rLowerIndex = (int)((r-_rMin)/_dr);
	int rUpperIndex = rLowerIndex+1;

	std::vector<double> field;
	//E = E(r0) + [E(r1)-E(r0)]*(r-r0)/dr
	double ezLower   = _ez  [rLowerIndex](z);
	double erLower   = _er  [rLowerIndex](z);
	double bphiLower = _bphi[rLowerIndex](z);
	double deltaR    = r-_rMin;
	//This is an (hopefull optimised) expression e = e1 + de/dr * delta_r = e1 + de * delta_r/dr
	//and delta_r/dr = (r2 - r1)/dr = (r-rMin)/dr - rLower

	field.push_back( bphiLower+(_bphi[rUpperIndex](z) - bphiLower )*(deltaR/_dr - (double)rLowerIndex) );
	field.push_back( ezLower  +(_ez[rUpperIndex](z)   -   ezLower )*(deltaR/_dr - (double)rLowerIndex) );
	field.push_back( erLower  +(_er[rUpperIndex](z)   -   erLower )*(deltaR/_dr - (double)rLowerIndex) );

	return field;
}

std::string	RFFieldMap::ReplaceVariables( std::string fileName )
{
  unsigned int pos = 0;
  std::string fullName;

  while( pos < fileName.size() )
  {
    if( fileName[pos] == '$' ) 	// we've found an environment variable 
    {
      ++pos;
      if( fileName[pos] == '{' )
	++pos;
      int end = pos +1;
      while( fileName[end] != '}' )
        ++end;
      std::string variable;
      for( int vpos = pos; vpos < end; ++vpos )
        variable += fileName[vpos];
      if(getenv( variable.c_str() ) == NULL) 
          throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error - "+variable+" environment variable was not defined", "RFFieldMap::ReplaceVariables"));
      fullName += std::string( getenv( variable.c_str() ) );
      pos = end + 1;
    }
    else
    {
      fullName += fileName[pos];
      ++pos;
    }
  }

  return fullName;
}
