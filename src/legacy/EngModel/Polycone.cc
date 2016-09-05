// MAUS WARNING: THIS IS LEGACY CODE.
//
// Polycone class reads in a file of doubles and creates a polycone
//

#include "EngModel/Polycone.hh"
#include "Utils/Exception.hh"
#include "Utils/Squeak.hh"

const double Polycone::_phiStart = 0*degree;
const double Polycone::_phiEnd = 360*degree;


//Constructor with only one material
Polycone::Polycone(MiceModule *theModule) : _zCoordinates(NULL), _rInner(NULL), _rOuter(NULL)
{
	std::string fileName = theModule->propertyStringThis("FileName");

        std::string fullFileName = replaceVariables( fileName );

	std::ifstream fin(fullFileName.c_str());
	if(!fin)
	{
		throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable, "Could not find polycone file "+fileName, "Polycone::Polycone(MiceModule)"));
		return;
	}
	MAUS::Squeak::mout(MAUS::Squeak::debug) << "LOADING POLYCONE " << fileName << std::endl;
	_polyconeType = theModule->propertyStringThis("PolyconeType");
	_name = theModule->fullName() + _polyconeType;
	ReadPolyconeFile(fin);
}

Polycone::Polycone() : _zCoordinates(NULL), _rInner(NULL), _rOuter(NULL)
{}


Polycone::~Polycone()
{
	if(_zCoordinates!= NULL) delete [] _zCoordinates;
	if(_rInner!= NULL) delete [] _rInner;
	if(_rOuter!= NULL) delete [] _rOuter;
}

std::string	Polycone::replaceVariables( std::string fileName )
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
          throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error - "+variable+" environment variable was not defined", "Polycone::replaceVariables"));
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

//File I/O
void Polycone::ReadPolyconeFile(std::istream & fin)
{
	std::string key="";
	GetNumberOfCoordinates(fin); //also sets up the arrays
	//GetDirection(fin);
	GetUnits(fin);
	//Skip next line
	std::getline(fin, key);
	if(_polyconeType == "Shell")
		GetShellCoordinates(fin);
	if(_polyconeType == "Fill")
		GetFillCoordinates(fin);

	if(!fin) std::cerr << "There was a problem reading a Polycone file" << std::endl;
}

void Polycone::GetNumberOfCoordinates(std::istream & fin)
{
	std::string key="";

	//Read number of coords
	while(fin && key!="NumberOfPoints")
		fin >> key;
	fin >> _numberOfCoordinates;
	//skip rest of line
	std::getline(fin, key);
	if(!fin) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable, "Error in polycone input", "Polcone.cc::GetNumberOfCoordinates()"));
}

void Polycone::GetUnits(std::istream & fin)
{
	MAUS::MAUSEvaluator theUnits;
	std::string key="", unitsString="";
	while(fin && key!="Units")
		fin >> key;
	fin >> unitsString;
	_units = theUnits.evaluate(unitsString);
	//skip rest of line
	std::getline(fin, key);
	if(!fin) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable, "Error in polycone input", "Polcone.cc::GetUnits()"));
}

void Polycone::GetShellCoordinates(std::istream & fin)
{
	if(_zCoordinates!= NULL) delete [] _zCoordinates;
	if(_rInner!= NULL)       delete [] _rInner;
	if(_rOuter!= NULL)       delete [] _rOuter;

	_zCoordinates = new double[_numberOfCoordinates];
	_rInner       = new double[_numberOfCoordinates];
	_rOuter       = new double[_numberOfCoordinates];

	for(int i=0; i<_numberOfCoordinates; i++)
	{
		fin >> _zCoordinates[i] >> _rInner[i] >> _rOuter[i];
		_zCoordinates[i] *= _units;
		_rInner[i]       *= _units;
		_rOuter[i]       *= _units;
	}
	if(!fin) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable, "Error in polycone input", "Polcone.cc::GetShellCoordinates()"));

}

void Polycone::GetFillCoordinates(std::istream & fin)
{
	std::string dummy;
	if(_zCoordinates!= NULL) delete [] _zCoordinates;
	if(_rInner!= NULL)       delete [] _rInner;

	_zCoordinates = new double[_numberOfCoordinates];
	_rInner       = new double[_numberOfCoordinates];

	for(int i=0; i<_numberOfCoordinates; i++)
	{
		fin >> _zCoordinates[i] >> _rInner[i];
		getline(fin, dummy);
		_zCoordinates[i] *= _units;
		_rInner[i]      *= _units;
	}
	if(!fin) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable, "Error in polycone input", "Polcone.cc::GetFillCoordinates()"));
}

G4Polycone * Polycone::GetShell()
{
	if(_polyconeType != "Shell")
	{
		std::cerr << "Attempt to use a Fill absorber shape as a Shell" << std::endl;
		return NULL;
	}

	G4Polycone * theCone = new G4Polycone(_name, _phiStart,
	                       _phiEnd, _numberOfCoordinates, _zCoordinates, _rInner, _rOuter);

	return theCone;
}

G4Polycone * Polycone::GetFill()
{
	double * _rInnerNew = new double[_numberOfCoordinates];
	for(int i=0; i<_numberOfCoordinates; i++)
		_rInnerNew[i] = 0.;
	G4Polycone * theCone = new G4Polycone(_name, _phiStart,
	                       _phiEnd, _numberOfCoordinates, _zCoordinates, _rInnerNew, _rInner);

	return theCone;

}

G4Polycone * Polycone::GetPolycone()
{
	if(_polyconeType == "Shell") return GetShell();
	else if(_polyconeType == "Fill" ) return GetFill();
	else
	{
		std::cerr << "Error - Polycone type " << _polyconeType << " not recognised" << std::endl;
		return NULL;
	}
}

void Polycone::Print(std::ostream & out)
{
	out << "z     r";
	if(_polyconeType=="Shell") out << "Inner     rOuter";
	out << std::endl;
	for(int i=0; i<_numberOfCoordinates; i++)
	{
		out << _zCoordinates[i] << "    " << _rInner[i];
		if(_polyconeType=="Shell") out << "    " << _rOuter[i];
		out << std::endl;
	}

}

std::vector<double> Polycone::ZCoords() 
{
	return std::vector<double>(_zCoordinates, _zCoordinates+_numberOfCoordinates);
}

std::vector<double> Polycone::RInner()  
{
	return std::vector<double>(_rInner, _rInner+_numberOfCoordinates);
}

std::vector<double> Polycone::ROuter()  
{
	if(_polyconeType=="Shell") return std::vector<double>(_rOuter, _rOuter+_numberOfCoordinates); 
	else                       return std::vector<double>();
}



