// MAUS WARNING: THIS IS LEGACY CODE.
#include "Interface/ThreeDFieldMap.hh"
#include "Utils/Squeak.hh"
#include <iostream>
#include <sstream>
#include <map>
#include <fstream>
#include "Utils/Exception.hh"

ThreeDFieldMap::ThreeDFieldMap (std::string interpolation, std::string fileName, std::string fileType,
                               std::string symmetry)
              : _symmetry(none)
{
	SetSymmetry(symmetry);
	MAUS::Squeak::mout(MAUS::Squeak::debug) << "Loading 3d field map file \'"+fileName+"\'" << std::endl;
	if(fileType!="g4bl3dGrid") throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise file type \'"+fileType+"\'", "ThreeDFieldMap::ThreeDFieldMap"));
	fileName = ReplaceVariables(fileName);
	std::ifstream in(fileName.c_str());
	if(!in) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Could not open file \'"+fileName+"\'", "ThreeDFieldMap::ThreeDFieldMap"));
	myInterpolator = LoadBeamlineMap(in, Interpolator3dGridTo3d::triLinear);
}

ThreeDFieldMap::~ThreeDFieldMap() {delete myInterpolator;}

Interpolator3dGridTo3d* ThreeDFieldMap::LoadBeamlineMap(std::istream& fin, Interpolator3dGridTo3d::interpolationAlgorithm algo)
{
	std::stringstream in;
	StripG4BLComments(in, fin);

	int nx(0), ny(0), nz(0), unknown2(0);
	double*** Bx(NULL);
	double*** By(NULL);
	double*** Bz(NULL); 
	double*** x(NULL);
	double*** y(NULL);
	double*** z(NULL);
	double globalScale;

	in >> nx >> ny >> nz >> globalScale;
	std::map<std::string, double> scale;

	x  = SetArray(nx, ny, nz);
	y  = SetArray(nx, ny, nz);
	z  = SetArray(nx, ny, nz);
	Bx = SetArray(nx, ny, nz);
	By = SetArray(nx, ny, nz);
	Bz = SetArray(nx, ny, nz);
	
	for(int i=0; i<6; i++)
	{
		std::string name;
		std::string scaleStr;
		double      localScale;
		in >> name;
		in >> name >> scaleStr;
		if(scaleStr.size() < 2) 
			throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Scale \'"+scaleStr+"\' formatting not recognised", "ThreeDFieldMap::LoadBeamlineMap") );
		scaleStr = scaleStr.substr(1,scaleStr.size()-2);
		std::stringstream scaleStream(scaleStr);
		scaleStream >> localScale;
		scale[name] = localScale*globalScale;
	}
	in >> unknown2;

	for(int i=0; i<nx; i++)
	{
		for(int j=0; j<ny; j++)
			for(int k=0; k<nz; k++)
			{
				in >> x[i][j][k] >> y[i][j][k] >> z[i][j][k] >> Bx[i][j][k] >> By[i][j][k] >> Bz[i][j][k];
				x[i][j][k]  *= scale["X"];
				y[i][j][k]  *= scale["Y"];
				z[i][j][k]  *= scale["Z"];
				Bx[i][j][k] *= scale["BX"];
				By[i][j][k] *= scale["BY"];
				Bz[i][j][k] *= scale["BZ"];
			}
	}


	std::vector<double> xV(nx), yV(ny), zV(nz);
	for(int i=0; i<nx; i++) xV[i] = x[i][0][0];
	for(int i=0; i<ny; i++) yV[i] = y[0][i][0];
	for(int i=0; i<nz; i++) zV[i] = z[0][0][i];

	Interpolator3dGridTo3d* interpol = new Interpolator3dGridTo3d(new ThreeDGrid(xV,yV,zV), Bx, By, Bz, algo);

	MAUS::Squeak::mout(MAUS::Squeak::debug) << "Loaded field map with\n  min (x, y, z): "
                              << interpol->GetMesh()->MinX() << " "
                              << interpol->GetMesh()->MinY() << " "
                              << interpol->GetMesh()->MinZ() << " "
                              << "\n  max (x, y, z): "
                              << interpol->GetMesh()->MaxX() << " "
                              << interpol->GetMesh()->MaxY() << " "
                              << interpol->GetMesh()->MaxZ() << std::endl;

	DeleteArray(x,  nx, ny, nz);
	DeleteArray(y,  nx, ny, nz);
	DeleteArray(z,  nx, ny, nz);
	return interpol;
}

double*** ThreeDFieldMap::SetArray(int nx, int ny, int nz)
{
	double*** array;
	array = new double**[nx];
	for(int i=0; i<nx; i++)
	{
		array[i] = new double*[ny];
		for(int j=0; j<ny; j++) 
		{
			array[i][j] = new double[nz];
			for(int k=0; k<nz; k++)
				array[i][j][k] = 0;
		}
	}
	return array;
}

void ThreeDFieldMap::DeleteArray(double*** array, int nx, int ny, int nz)
{
	for(int i=0; i<nx; i++)
	{
		for(int j=0; j<ny; j++) delete[] array[i][j];
		delete [] array[i];
	}
	delete [] array;
}

void ThreeDFieldMap::StripG4BLComments(std::ostream& out, std::istream& in)
{
	std::string       line;
	std::string       word;
	MAUS::Squeak::mout(MAUS::Squeak::debug) << "Preprocessing beamline map" << std::endl;
	while(in)
	{
		getline(in, line);
		std::stringstream lineIn(line);
		lineIn >> word;
		if(word.size() == 0);
		else if(word[0] == '#')      ;
		else if(word[0] == '*') MAUS::Squeak::mout(MAUS::Squeak::debug) << line << "\n";
		else                    out << line << "\n";
	}
}

std::string	ThreeDFieldMap::ReplaceVariables( std::string fileName )
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
          throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error - "+variable+" environment variable was not defined", "ThreeDFieldMap::ReplaceVariables"));
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

#include <iostream>

void ThreeDFieldMap::GetFieldValue( const  double Point[4], double *Bfield ) const
{
	const double* symPoint = Symmetry(Point);
	myInterpolator->F(symPoint, Bfield);
//	for(int i=0; i<3; i++) std::cout << Point[i]  << " ";
//	std::cout << " ** ";
//	for(int i=0; i<3; i++) std::cout << Bfield[i] << " ";
//	std::cout << " ** ";
	for(int i=0; i<3; i++) Bfield[i] *= symPoint[i+4];
//	for(int i=0; i<7; i++) std::cout << symPoint[i] << " ";
//	std::cout << " ** ";
//	for(int i=0; i<3; i++) std::cout << Bfield[i] << " ";
	delete [] symPoint;
//	std::cout << "\n";
}


void ThreeDFieldMap::SetSymmetry(std::string symmetry)
{
	if(symmetry == "Quadrupole" )      _symmetry = quadrupole;
	else if(symmetry == "Dipole")      _symmetry = dipole;
	else if(symmetry == "Solenoid")    _symmetry = solenoid;
	else                               _symmetry = none;
}

const double* ThreeDFieldMap::Symmetry(const double Point[4]) const
{
	double* pointAndScale = new double[7];
	for(int i=0; i<4; i++) pointAndScale[i] = Point[i];
	for(int i=4; i<7; i++) pointAndScale[i] = 1.;
	if(_symmetry == quadrupole)
	{
		pointAndScale[2] = fabs(Point[2]);
		pointAndScale[0] = fabs(Point[0]);
		pointAndScale[1] = fabs(Point[1]);
		if(Point[0] < 0 && Point[1] < 0)      pointAndScale[6] *= -1;
		else if(Point[0] > 0 && Point[1] > 0) pointAndScale[6] *= -1;
		if(Point[0] < 0) pointAndScale[5] = -1;
		if(Point[1] < 0) pointAndScale[4] = -1;
	}
	else if(_symmetry == dipole)
	{
		pointAndScale[0] = fabs(Point[0]);
		pointAndScale[1] = fabs(Point[1]);
		pointAndScale[2] = fabs(Point[2]);
		if(Point[0] < 0) pointAndScale[4] = -1;
		if(Point[1] < 0) pointAndScale[6] = -1;
	}
	else if(_symmetry == solenoid)
	{
		pointAndScale[0] = fabs(Point[0]);	// f(x) = f(-x) in the map
		pointAndScale[1] = fabs(Point[1]);	// f(y) = f(-y) in the map
		pointAndScale[2] = Point[2];		// The symetry is radial, not along z
		if(Point[0] < 0) pointAndScale[4] = -1; // Bx(-x) = -Bx(x)
		if(Point[1] < 0) pointAndScale[5] = -1; // By(-y) = -By(y)
	}

//	for(int i=0; i<7; i++) std::cout << " @" << pointAndScale[i] << "@ ";
	return pointAndScale;
}


