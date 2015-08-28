// MAUS WARNING: THIS IS LEGACY CODE.
// MiceModule.cc
//
// Class that describes the shape, size, position, orientation and other properties of a module of the MICE experiment,
// or a subcomponent.

#include <cstdlib>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Config/MiceModule.hh"
#include "Config/ModuleTextFileIO.hh"

#include "Utils/Exception.hh"
// Constructor takes the name of a file from which information is read in to instantiate this module or sub component

static int _count = 0;
const int MiceModule::precision = 100;

MiceModule::MiceModule( std::string filename )
          : _isroot(true), _mother(NULL), _rotation()
{
  addPropertyString("Volume",  "Box");
  ModuleTextFileIO io(filename);
  *this = *io.getModule();
  checkNames();
}

MiceModule::MiceModule( )
          : _isroot(true), _name(""), _mother(NULL), _rotation()
{
}

MiceModule::MiceModule( MiceModule* parent, std::string name )
          : _isroot(parent==NULL), _name(name), _mother(parent), _rotation()
{
  if(isRoot()) addPropertyString("Volume",  "Box");
}

MiceModule&	MiceModule::operator=( MiceModule& rhs )
{
   _isroot = rhs._isroot;
   _name = rhs._name;
   _mother = rhs._mother;
   _bools = rhs._bools;
   _ints = rhs._ints;
   _doubles = rhs._doubles;
   _strings = rhs._strings;
   _hep3vectors = rhs._hep3vectors;

  // transfer daughters from the rhs to this MiceModule
  _daughters = rhs._daughters;
  rhs._daughters.clear();
  for(unsigned int i=0; i<_daughters.size(); i++) _daughters[i]->setMother(this);

  delete &rhs;

  return *this;
}

MiceModule* MiceModule::deepCopy(const MiceModule& rhs, bool insertInMother)
{
    MiceModule * mod = new MiceModule();
    mod->_isroot      = rhs._isroot;
    mod->_name        = rhs._name;
    mod->_mother      = rhs._mother;
    mod->_rotation    = rhs._rotation;
    mod->_bools       = rhs._bools;
    mod->_ints        = rhs._ints;
    mod->_doubles     = rhs._doubles;
    mod->_strings     = rhs._strings;
    mod->_hep3vectors = rhs._hep3vectors;
    mod->_parameters = rhs._parameters;

    for(unsigned int i=0; i<rhs._daughters.size(); i++)
    {
       mod->_daughters.push_back(deepCopy( *(rhs._daughters[i]) ));
       mod->_daughters[i]->_mother = mod;
    }
    if(insertInMother && mod->_mother) 
      mod->_mother->_daughters.push_back(mod);

    return mod;
}

void	MiceModule::checkNames()
{
  std::vector<const MiceModule*> matches = rootModule()->findModulesByName( fullName() );

  if( matches.size() > 1 )
  {
    std::stringstream str;
    str << _count;
    _name += str.str();
    ++_count;
  }

  for( unsigned int i = 0; i < _daughters.size(); ++i )
    _daughters[i]->checkNames();
}

// Destructor

MiceModule::~MiceModule()
{
  for( unsigned int i = 0; i < _daughters.size(); ++i )
    delete _daughters[i];
}

// Return the full name

std::string		MiceModule::fullName() const
{
  std::string prefix = "";

  if( _mother )
    prefix += _mother->fullName();

  std::string nam;

  if( _mother )
    nam = prefix + "/" + _name;
  else
    nam = _name;

  return nam;
}

// Return a particular daughter volume

MiceModule*		MiceModule::daughter( int d ) const
{
  MiceModule* daught = NULL;

  if( d >= 0 && d < (int) _daughters.size() )
    daught = _daughters[d];

  return daught;
}

// Return true if this module is inside of another

bool                    MiceModule::isInside( const MiceModule* module ) const
{
  bool isInside = false;

  MiceModule* check = _mother;

  while( check && ! isInside )
  {
    if( module == check )
      isInside = true;
    else
      check = check->mother();
  }

  return isInside;
}

// Return the position of this module with respect to another module that it is inside of

HepGeom::Transform3D MiceModule::relativeTransform(const MiceModule* module ) const
{

  if( ! isInside( module ) && module != this )
    std::cerr << "Requested relativeRotation of a module with respect to another that does not contain it! Return value will be nonsense!" << std::endl;

  const MiceModule* moth = _mother;

  std::vector<const MiceModule*> modules;
  const MiceModule* mod = this;
  modules.push_back(mod);

  while( moth && moth != module )
    {
      modules.push_back(moth);
      moth = moth->mother();
    }

  Hep3Vector  newposition = modules[modules.size()-1]->position();
  HepRotation newrotation = modules[modules.size()-1]->rotation();

  for(int i= (modules.size()-1); i>0; i--)
    {
      newposition+=newrotation.inverse()*modules[i-1]->position();
      newrotation=modules[i-1]->rotation()*newrotation;
    }

  HepGeom::Transform3D transform( newrotation, newposition );

  return transform;
}

Hep3Vector	MiceModule::dimensions() const {
  if(propertyExistsThis("Dimensions", "Hep3Vector")) 
    return propertyHep3Vector("Dimensions");
  else return Hep3Vector(0,0,0);
}


Hep3Vector	MiceModule::position() const {
  if(propertyExistsThis("Position", "Hep3Vector")) 
    return propertyHep3Vector("Position");
  else return Hep3Vector(0,0,0);
}

Hep3Vector              MiceModule::relativePosition( const MiceModule* module ) const
{
  HepGeom::Transform3D transform = relativeTransform(module);
  return transform.getTranslation();
}

// Return the position of this module with respect to the global coordinate system

Hep3Vector		MiceModule::globalPosition() const
{
  // find the highest level mother

  const MiceModule* mother = this;

  while( mother->mother() )
    mother = mother->mother();

  return relativePosition( mother );
}


HepRotation		MiceModule::rotation() const {
  if(propertyExistsThis("Rotation", "Hep3Vector")) 
    return Hep3VecToRotation(propertyHep3Vector("Rotation"));
  else return Hep3VecToRotation(Hep3Vector(0,0,0));

}

// Return the rotation of this module with respect to another module that it is inside of
HepRotation             MiceModule::relativeRotation( const MiceModule* module ) const
{
  HepGeom::Transform3D transform = relativeTransform(module);
  return transform.getRotation();
}

// Return the rotation of this module with respect to the global coordinate system

HepRotation		MiceModule::globalRotation() const
{
  // find the highest level mother

  const MiceModule* mother = this;

  while( mother->mother() )
    mother = mother->mother();

  return relativeRotation( mother );
}

double MiceModule::scaleFactor() const 
{
  if(propertyExistsThis("ScaleFactor","double"))
    return propertyDouble("ScaleFactor");
  else
    return 1;
}


double   MiceModule::globalScaleFactor() const
{
  const MiceModule*   mother = this;
  double globalScaleFactor = scaleFactor();
  while(!mother->isRoot())
  {
    mother = mother->mother();
    globalScaleFactor *= mother->scaleFactor();
  }
  return globalScaleFactor;

}

// --- Methods to add properties

void MiceModule::addPropertyBool( std::string name, std::string val ) {
  if(_bools.find(name) != _bools.end()) 
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Attempt to add PropertyBool "+name+" twice in module "+this->fullName(), "MiceModule::addPropertyBool")); 
  _bools[name] = val;
}

void MiceModule::addPropertyBool( std::string name, bool val )
{ 
  if(_bools.find(name) != _bools.end()) 
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Attempt to add PropertyBool "+name+" twice in module "+this->fullName(), "MiceModule::addPropertyBool")); 

  std::stringstream myInpStream;
  myInpStream << val;
  _bools[name] = myInpStream.str(); 
}

void MiceModule::addPropertyInt( std::string name, int val )
{ 
  std::stringstream myInpStream;
  myInpStream << val;
  addPropertyInt(name, myInpStream.str());
}

void MiceModule::addPropertyInt( std::string name, std::string val )
{
  if(_ints.find(name) != _ints.end())
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Attempt to add PropertyInt "+name+" twice in module "+this->fullName(), "MiceModule::addPropertyInt")); 
  _ints[name] = val;
}

void MiceModule::addPropertyDouble( std::string name, double val )
{ 
  std::stringstream myInpStream;
  myInpStream << std::setprecision(100);
  myInpStream << val;
  addPropertyDouble(name, myInpStream.str());
}

void MiceModule::addPropertyDouble( std::string name, std::string val )
{ 
  if(_doubles.find(name) != _doubles.end())
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Attempt to add PropertyDouble "+name+" twice in module "+this->fullName(), "MiceModule::addPropertyDouble")); 
  _doubles[name] = val;
}

void MiceModule::addPropertyString( std::string name, std::string val )
{ 
  if(_strings.find(name) != _strings.end()) 
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Attempt to add PropertyString "+name+" twice in module "+this->fullName(), "MiceModule::addPropertyString")); 
  _strings[name] = val;
}

void MiceModule::addPropertyHep3Vector( std::string name, CLHEP::Hep3Vector val )
{ 
  std::stringstream myInpStream;
  myInpStream << std::setprecision(100);
  myInpStream << val[0] << " " << val[1] << " " << val[2];
  _hep3vectors[name] = myInpStream.str(); 
}

void MiceModule::addPropertyHep3Vector( std::string name, std::string val )
{ 
  if(_hep3vectors.find(name) != _hep3vectors.end()) 
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Attempt to add PropertyHep3Vector "+name+" twice in module "+this->fullName(), "MiceModule::addPropertyHep3Vector")); 
  _hep3vectors[name] = val; 
}

// --- Methods to provide "Properties" of a Module

// Return true if this property exists as a particular type
bool                    MiceModule::propertyExists( std::string property, std::string type ) const
{
  bool exists = false;

  if( type == "Bool" || type == "BOOL" || type == "bool" )
    exists = ( _bools.find( property ) != _bools.end() ) || ( _mother && _mother->propertyExists( property, type ) );
  else if( type == "Int" || type == "INT" || type == "int" )
    exists = ( _ints.find( property ) != _ints.end() ) || ( _mother && _mother->propertyExists( property, type ) );
  else if( type == "Double" || type == "DOUBLE" || type == "double" )
    exists = ( _doubles.find( property ) != _doubles.end() ) || ( _mother && _mother->propertyExists( property, type ) );
  else if( type == "String" || type == "STRING" || type == "string" )
    exists = ( _strings.find( property ) != _strings.end() ) || ( _mother && _mother->propertyExists( property, type ) );
  else if( type == "Hep3Vector" || type == "HEP3VECTOR" || type == "hep3vector" )
    exists = ( _hep3vectors.find( property ) != _hep3vectors.end() ) || ( _mother && _mother->propertyExists( property, type ) );
  else throw(MAUS::Exception(MAUS::Exception::recoverable, "Property type "+type+" not recognised", "MiceModule::propertyExists"));

  return exists;
}

// Return true if this property exists as a particular type in this instance only

bool                    MiceModule::propertyExistsThis( std::string property, std::string type ) const
{
  bool exists = false;

  if( type == "Bool" || type == "BOOL" || type == "bool" )
    exists = _bools.find( property ) != _bools.end();
  else if( type == "Int" || type == "INT" || type == "int" )
    exists = _ints.find( property ) != _ints.end();
  else if( type == "Double" || type == "DOUBLE" || type == "double" )
    exists = _doubles.find( property ) != _doubles.end();
  else if( type == "String" || type == "STRING" || type == "string" )
    exists = _strings.find( property ) != _strings.end();
  else if( type == "Hep3Vector" || type == "HEP3VECTOR" || type == "hep3vector" )
    exists = _hep3vectors.find( property ) != _hep3vectors.end();
  else throw(MAUS::Exception(MAUS::Exception::recoverable, "Property type "+type+" not recognised", "MiceModule::propertyExists"));

  return exists;
}

// Return the named boolean property
bool                    MiceModule::propertyBoolThis( std::string property ) const
{
  if(propertyExistsThis(property, "bool")) 
    return propertyBool(property);
  else                                       
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyBoolThis");
}

bool                    MiceModule::propertyBool( std::string property ) const
{
  bool prop_exists = true;
  std::string prop;
  bool        prop_bool;

  if( propertyExistsThis( property, "bool" ) )
    prop = _bools.find( property )->second;
  else if( _mother )
    try{prop = _mother->propertyBool( property );} catch(MAUS::Exception exc) {prop_exists = false;}
  else
    prop_exists = false;
  if (!prop_exists)
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyBool");
  ModuleTextFileIO::parseString(prop, prop_bool);

  return prop_bool;
}

//  Return the named integer property
int                    MiceModule::propertyIntThis( std::string property ) const
{
  if(propertyExistsThis(property, "int")) 
    return propertyInt(property);
  else                                       
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyIntThis");
}

// Return the named int property
int                  MiceModule::propertyInt( std::string property ) const
{
  std::string prop_str;
  static std::string real_name="";
  int      prop_int;
  if( propertyExistsThis( property, "int" ) )
  {
    try{
      prop_str = _ints.find( property )->second;
      ModuleTextFileIO::setEvaluator(_parameters);
      ModuleTextFileIO::parseString(prop_str, prop_int);
    }
    catch(MAUS::Exception exc)
    {
      std::string error = exc.GetMessage();
      throw(MAUS::Exception(MAUS::Exception::recoverable, "Error parsing MiceModule "+fullName()+" property "+property+". Error was reported as \'"+error+"\'", "MiceModule::propertyInt"));
    }
  }
  else if( _mother )
  {
    real_name += name()+" that is in ";
    prop_int = _mother->propertyInt( property );
  }
  else
    {
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+real_name+name(), "MiceModule::propertyInt");
    }
  return prop_int;
}

double                  MiceModule::propertyDoubleThis( std::string property ) const
{
  if( propertyExistsThis( property, "double" ) )
    return propertyDouble(property);
  else
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyDoubleThis");
}


// Return the named double property
double                  MiceModule::propertyDouble( std::string property ) const
{
  std::string prop_str;
  double      prop_dbl;
  if( propertyExistsThis( property, "double" ) )
  {
    try{
      prop_str = _doubles.find( property )->second;
      ModuleTextFileIO::setEvaluator(_parameters);
      ModuleTextFileIO::parseString(prop_str, prop_dbl);
    }
    catch(MAUS::Exception exc)
    {
      std::string error = exc.GetMessage();
      throw(MAUS::Exception(MAUS::Exception::recoverable, "Error parsing MiceModule "+fullName()+" property "+property+". Error was reported as \'"+error+"\'", "MiceModule::propertyDouble"));
    }
  }
  else if( _mother )
    prop_dbl = _mother->propertyDouble( property );
  else
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyDouble");
  return prop_dbl;
}

// Return the named string property
std::string                    MiceModule::propertyStringThis( std::string property ) const
{
  if(propertyExistsThis(property, "string")) 
    return propertyString(property);
  else                                       
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyStringThis");
}

std::string             MiceModule::propertyString( std::string property ) const
{
  bool prop_exists = true;
  std::string prop  = "";

  if( propertyExistsThis( property, "string" ) )
    prop = _strings.find( property )->second;
  else if( _mother )
    try{prop = _mother->propertyString( property );} catch(MAUS::Exception exc) {prop_exists = false;}
  else prop_exists = false;
  if (!prop_exists)
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyString");

  return prop;
}

// Return the name Hep3Vector property
CLHEP::Hep3Vector   MiceModule::propertyHep3VectorThis( std::string property ) const
{
  if(propertyExistsThis(property, "hep3vector")) 
    return propertyHep3Vector(property);
  else                                       
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyHep3VectorThis");
}

Hep3Vector		MiceModule::propertyHep3Vector( std::string property ) const
{
  ModuleTextFileIO::setEvaluator(_parameters);
  bool prop_exists = true;
  Hep3Vector  prop_h3v = Hep3Vector( -1, -1, -1 );
  std::string prop_str;
  if( propertyExistsThis( property, "Hep3Vector" ) )
    prop_str = _hep3vectors.find( property )->second;
  else if(  _mother )
    try{return _mother->propertyHep3Vector( property );} catch(MAUS::Exception exc) {prop_exists = false;}
  else prop_exists = false;
  if (!prop_exists)
    throw MAUS::Exception(MAUS::Exception::recoverable, "Couldn't find property "+property+" in module "+fullName(), "MiceModule::propertyHep3Vector");
  try{
    ModuleTextFileIO::parseString(prop_str, prop_h3v);
  }
  catch(MAUS::Exception exc)
  {
    std::string error = exc.GetMessage();
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Error parsing MiceModule "+fullName()+" property "+property+". Error was reported as \'"+error+"\'", "MiceModule::propertyHep3Vector"));
  }

  return prop_h3v;
}

/// Print out the tree of this module and all its daughters

void MiceModule::printTree( int level, std::ostream& Cout ) const
{
  std::string indent;

  for( int i = 0; i < level; ++i )
    indent += "\t";

  if( ! _isroot )
  {
    Cout << indent << "Module " << _name << " is at relative position " << position() << " and global position " << globalPosition() << std::endl;
    Cout << indent << "Module has relative rotation " 
              << HepRotationTo3Vec(rotation()).x() << " " << HepRotationTo3Vec(rotation()).y() << " " << HepRotationTo3Vec(rotation()).z() << " and global rotation " 
              << HepRotationTo3Vec(globalRotation()).x() << " " << HepRotationTo3Vec(globalRotation()).y() << " " << HepRotationTo3Vec(globalRotation()).z() << "\n"; 
    Cout << indent << "Volume is a " << volType() << " with";
    if( volType() == "Cylinder" ) Cout << " Radius " << dimensions().x() << " and Length " << dimensions().y();
    else if( volType() == "Box" ) Cout << " Width " << dimensions().x() << ", Height " << dimensions().y() << " and Length " << dimensions().z();
    else if( volType() == "Tube" ) Cout << " Inner Radius " << dimensions().x() << ", outer Radius " << dimensions().y() << " and Length " << dimensions().z();

    Cout << std::endl;
    printProperties( indent, Cout );
  }
  else
    Cout << indent << "Configuration " << _name << std::endl;

  if( _daughters.size() )
  {
    Cout << indent << "Has " << _daughters.size() << " daughter";
    if( _daughters.size() > 1 )
      Cout << "s";
    Cout << ":" << std::endl;
  }

  for( unsigned int i = 0; i < _daughters.size(); ++i )
    _daughters[i]->printTree( level + 1, Cout );
}

void MiceModule::printProperties( std::string indent, std::ostream& Cout ) const
{
  // print out any boolean properties of this module

  for( std::map<std::string,std::string>::const_iterator it = _bools.begin(); it != _bools.end(); ++it )
     Cout << indent << "Boolean property " << (*it).first << " has the value " << (*it).second << std::endl;

  // print out any integer properties of this module
  for( std::map<std::string,std::string>::const_iterator it = _ints.begin(); it != _ints.end(); ++it )//reads strings
    {
      Cout << indent << "Integer property " << (*it).first << " has the value " << (*it).second << std::endl;
    }
  
  // print out any double properties of this module

  for( std::map<std::string,std::string>::const_iterator it = _doubles.begin(); it != _doubles.end(); ++it )
    Cout << indent << "Double property " << (*it).first << " has the value " << (*it).second << std::endl;

  // print out any string properties of this module

  for( std::map<std::string,std::string>::const_iterator it = _strings.begin(); it != _strings.end(); ++it )
     Cout << indent << "String property " << (*it).first << " has the value " << (*it).second << std::endl;

  // print out any Hep3Vector properties of this module

  for( std::map<std::string,std::string>::const_iterator it = _hep3vectors.begin(); it != _hep3vectors.end(); ++it )
    Cout << indent << "Hep3Vector property " << (*it).first << " has the value " << (*it).second << std::endl;
}

void MiceModule::DumpMiceModule(std::string & Indent,
                                std::ostream & Cout) const {
  std::string MyIndent = Indent + " ";

  if (! _isroot) {
    std::string Volume = volType();
    Cout << Indent << "Module " << _name << std::endl << Indent << "{"
         << std::endl << MyIndent << "Volume " << volType() << std::endl;
    if (Volume == "Cylinder") { 
      Cout << MyIndent << "Dimensions " << dimensions().x() << " "
           << dimensions().y() << std::endl;
    } else {
      if ((Volume == "Box") || (Volume == "Tube") || (Volume == "Wedge")) {
	Cout << MyIndent << "Dimensions " << dimensions().x() << " "
	     << dimensions().y() << " " << dimensions().z() << std::endl;
      }
    }
    Cout << MyIndent << "Position " << position().x() << " " << position().y()
         << " " << position().z() << std::endl;
    Cout << MyIndent << "Rotation " << HepRotationTo3Vec(rotation()).x()
         << " " << HepRotationTo3Vec(rotation()).y()
         << " " << HepRotationTo3Vec(rotation()).z() << std::endl;
    Cout << MyIndent << "ScaleFactor " << scaleFactor() << std::endl;
    DumpProperties(MyIndent, Cout);
  } else {
    Cout <<"Configuration " << _name << std::endl << "{" << std::endl;
    Cout << MyIndent << "Dimensions " << dimensions().x() << " "
         << dimensions().y() << " " << dimensions().z() << std::endl;
    DumpProperties(MyIndent, Cout);
  }

    for( unsigned int i = 0; i < _daughters.size(); ++i )
      _daughters[i]->DumpMiceModule(MyIndent, Cout);
    Cout << Indent << "}" <<std::endl; 
}

void MiceModule::DumpProperties(std::string & indent, std::ostream & Cout) const {

  for (std::map<std::string,std::string>::const_iterator it = _hep3vectors.begin();
       it != _hep3vectors.end(); ++it) {
    std::string HEP = (*it).first;
    if ((HEP != "Dimensions") && (HEP != "Position") && (HEP != "Rotation") &&
        (HEP.find("Repeat") == std::string::npos)) {
      Cout << indent << "PropertyHep3Vector " << (*it).first << " " << (*it).second << std::endl;
    }
  }

  for (std::map<std::string,std::string>::const_iterator it = _doubles.begin();
       it != _doubles.end(); ++it) {
    if (((*it).first != "ScaleFactor") && ((*it).first != "RepeatScaleFactor")) {
      Cout << indent << "PropertyDouble " << (*it).first << " " << (*it).second << std::endl;
    }
  }

  for (std::map<std::string,std::string>::const_iterator it = _bools.begin();
       it != _bools.end(); ++it) {
    if ((*it).first.find("RepeatModule") == std::string::npos) {
      Cout << indent << "PropertyBool " << (*it).first << " " << (*it).second << std::endl;
    }
  }

  for (std::map<std::string,std::string>::const_iterator it = _ints.begin();
       it != _ints.end(); ++it) {
    if ((*it).first != "NumberOfRepeats") {
      Cout << indent << "PropertyInt " << (*it).first << " " << (*it).second << std::endl;
    }
  }
  
  for (std::map<std::string,std::string>::const_iterator it = _strings.begin();
       it != _strings.end(); ++it) {
    if ((*it).first != "Volume") {
      Cout << indent << "PropertyString " << (*it).first << " " << (*it).second << std::endl;
    }
  }
}

// Return all daughters (and self) that have the string property refered to of a specific value

std::vector<const MiceModule*>    MiceModule::findModulesByPropertyString( std::string name, std::string val ) const
{
  std::vector<const MiceModule*> modules;

  if( propertyExistsThis( name, "string" ) && propertyString( name ) == val )
    modules.push_back( this );

  for( unsigned int i = 0; i < _daughters.size(); ++i )
  {
    std::vector<const MiceModule*> tmp = _daughters[i]->findModulesByPropertyString( name, val );
    for( unsigned int i = 0; i < tmp.size(); ++i )
      modules.push_back( tmp[i] );
  }

  return modules;
}

// Return all daughters (and self) that have the property refered to of a specific type

std::vector<const MiceModule*>    MiceModule::findModulesByPropertyExists( std::string property, std::string name ) const
{
  std::vector<const MiceModule*> modules;

  if( propertyExistsThis( name, property ) )
    modules.push_back( this );

  for( unsigned int i = 0; i < _daughters.size(); ++i )
  {
    std::vector<const MiceModule*> tmp = _daughters[i]->findModulesByPropertyExists( property, name );
    for( unsigned int i = 0; i < tmp.size(); ++i )
      modules.push_back( tmp[i] );
  }

  return modules;
}

std::vector<const MiceModule*>    MiceModule::findModulesByName( std::string name ) const
{
  std::vector<const MiceModule*> modules;

  if( fullName() == name )
    modules.push_back( this );

  for( unsigned int i = 0; i < _daughters.size(); ++i )
  {
    std::vector<const MiceModule*> tmp = _daughters[i]->findModulesByName( name );
    for( unsigned int i = 0; i < tmp.size(); ++i )
      modules.push_back( tmp[i] );
  }

  return modules;
}

//as above but NON CONST
std::vector<MiceModule*> MiceModule::findModulesByPropertyExistsNC( std::string property, std::string name)
{
  std::vector<MiceModule*> modules;
  if( propertyExistsThis( name, property ) )
    modules.push_back( this );
  for( unsigned int i = 0; i < _daughters.size(); ++i )
  {
    std::vector<MiceModule*> tmp = _daughters[i]->findModulesByPropertyExistsNC( property, name );
    for( unsigned int i = 0; i < tmp.size(); ++i )
      modules.push_back( tmp[i] );
  }
  return modules;
}


// < operator compares the global Z position of this module with another

bool                		MiceModule::operator<( const MiceModule& rhs ) const
{
  return( globalPosition().z() < rhs.globalPosition().z() );
}

void MiceModule::printThis(std::ostream& out) const
{
  out << _name << " with " << _daughters.size() << " daughters\n";
  out << volType() << " dimensions: ";
  for(int i=0; i<3; i++)
    out << dimensions() << ' ';
	out << "position ";
  for(int i=0; i<3; i++)
    out << globalPosition()[i] << ' ';
	out << "rotation ";
  out << globalRotation().thetaX() << ' ' << globalRotation().thetaY()
      << ' ' << globalRotation().thetaZ() << std::endl;
}

void MiceModule::print(std::ostream& out) const
{
  printThis(out);
  for(unsigned int i=0; i<_daughters.size(); i++)
  	_daughters[i]->print(out);
}

void MiceModule::ithBool( int i, std::string& name, bool& val ) const
{
  int count = 0;

  for( std::map<std::string,std::string>::const_iterator it = _bools.begin(); it != _bools.end(); ++it )
  {
    if( count == i )
    {
      name = it->first;
      val = propertyBool(it->first);
      break;
    }
    ++count;
  }
}

void MiceModule::ithInt( int i, std::string& name, int& val ) const
{
  int count = 0;

  for( std::map<std::string, std::string>::const_iterator it = _ints.begin(); it != _ints.end(); ++it )
  {
    if( count == i )
    {
      name = it->first;
      val = propertyInt(name);
      break;
    }
    ++count;
  }
}

void MiceModule::ithDouble( int i, std::string& name, double& val ) const
{
  int count = 0;

  for( std::map<std::string,std::string>::const_iterator it = _doubles.begin(); it != _doubles.end(); ++it )
  {
    if( count == i )
    {
      name = it->first;
      val = propertyDouble(name);
      break;
    }
    ++count;
  }
}

void MiceModule::ithString( int i, std::string& name, std::string& val ) const
{
  int count = 0;

  for( std::map<std::string,std::string>::const_iterator it = _strings.begin(); it != _strings.end(); ++it )
  {
    if( count == i )
    {
      name = it->first;
      val = it->second;
      break;
    }
    ++count;
  }
}

void MiceModule::ithHep3Vector( int i, std::string& name, Hep3Vector& val ) const
{
  int count = 0;

  for( std::map<std::string,std::string>::const_iterator it = _hep3vectors.begin(); it != _hep3vectors.end(); ++it )
  {
    if( count == i )
    {
      name = it->first;
      val = propertyHep3Vector(name);
      break;
    }
    ++count;
  }
}

const MiceModule*	MiceModule::rootModule() const
{
  if( _mother == NULL )
    return this;
  else
    return _mother->rootModule();
}

int			MiceModule::depth() const
{
  if( _mother == NULL )
    return 0;
  else
    return( _mother->depth() + 1 );
}


bool			MiceModule::checkGeometry() const
{
  bool ok = true;
  
  for( unsigned int i = 0; i < _daughters.size(); ++i )
    ok &= checkGeometry( _daughters[i] );

  return ok;
}

bool			MiceModule::checkGeometry( MiceModule* daught ) const
{
  bool ok = true;

  HepRotation TestRot( 0.0, 0.0, 0.0 );

  if( daught->volType() == "Box" )
  {
    // Find the eight corners of the daughter's box
   
    Hep3Vector origVect( daught->dimensions().x() / 2., daught->dimensions().y() / 2., daught->dimensions().z() / 2. );

    double dx = daught->relativePosition( this ).x() + origVect.x();
    double sx = daught->relativePosition( this ).x() - origVect.x();
    double dy = daught->relativePosition( this ).y() + origVect.y();
    double sy = daught->relativePosition( this ).y() - origVect.y();
    double dz = daught->relativePosition( this ).z() + origVect.z();
    double sz = daught->relativePosition( this ).z() - origVect.z();
   
    Hep3Vector p1( dx, dy, dz );  
    Hep3Vector p2( dx, dy, sz );
    Hep3Vector p3( sx, dy, dz );
    Hep3Vector p4( sx, dy, sz );
    Hep3Vector p5( dx, sy, dz );
    Hep3Vector p6( dx, sy, sz );
    Hep3Vector p7( sx, sy, dz );
    Hep3Vector p8( sx, sy, sz );
    
    if( daught->relativeRotation( this ) != TestRot )
    {
      p1.transform( daught->relativeRotation( this ) );
      p2.transform( daught->relativeRotation( this ) );
      p3.transform( daught->relativeRotation( this ) );
      p4.transform( daught->relativeRotation( this ) );
      p5.transform( daught->relativeRotation( this ) );
      p6.transform( daught->relativeRotation( this ) );
      p7.transform( daught->relativeRotation( this ) );
      p8.transform( daught->relativeRotation( this ) );
    }    

    // check in case any one of these points is outside of the mother volume

    ok &= isInside( p1, daught );
    ok &= isInside( p2, daught );
    ok &= isInside( p3, daught );
    ok &= isInside( p4, daught );
    ok &= isInside( p5, daught );
    ok &= isInside( p6, daught );
    ok &= isInside( p7, daught );
    ok &= isInside( p8, daught );
  }
  else if( daught->volType() == "Cylinder" )
  {
    // For now, just test the centre of the two end points of the cylinder, this is by no means
    // complete coverage, but is a start...

    Hep3Vector origVect( daught->dimensions().x(), daught->dimensions().x(), daught->dimensions().y() / 2. );

    double dr = daught->relativePosition( this ).x() + origVect.x();
    double sr = daught->relativePosition( this ).x() - origVect.x();
    double dp = daught->relativePosition( this ).y() + origVect.y();
    double sp = daught->relativePosition( this ).y() - origVect.y();
    double dl = daught->relativePosition( this ).z() + origVect.z();
    double sl = daught->relativePosition( this ).z() - origVect.z();

    Hep3Vector p1( daught->relativePosition( this ).x(), daught->relativePosition( this ).y(), dl );
    Hep3Vector p2( daught->relativePosition( this ).x(), daught->relativePosition( this ).y(), sl );
    Hep3Vector p3( dr, daught->relativePosition( this ).y(), dl );
    Hep3Vector p4( sr, daught->relativePosition( this ).y(), dl );
    Hep3Vector p5( dr, daught->relativePosition( this ).y(), sl );
    Hep3Vector p6( sr, daught->relativePosition( this ).y(), sl );
    Hep3Vector p7( daught->relativePosition( this ).x(), dp, dl );
    Hep3Vector p8( daught->relativePosition( this ).x(), sp, dl );
    Hep3Vector p9( daught->relativePosition( this ).x(), dp, sl );
    Hep3Vector p10( daught->relativePosition( this ).x(), sp, sl );

    if( daught->relativeRotation( daught ) != TestRot )
    {
      p1.transform( daught->relativeRotation( this ) );
      p2.transform( daught->relativeRotation( this ) );
      p3.transform( daught->relativeRotation( this ) );
      p4.transform( daught->relativeRotation( this ) );
      p5.transform( daught->relativeRotation( this ) );
      p6.transform( daught->relativeRotation( this ) );
      p7.transform( daught->relativeRotation( this ) );
      p8.transform( daught->relativeRotation( this ) );
      p9.transform( daught->relativeRotation( this ) );
      p10.transform( daught->relativeRotation( this ) );
    }

    // check in case any one of these points is outside of the mother volume

    ok &= isInside( p1, daught );
    ok &= isInside( p2, daught );

    ok &= isInside( p3, daught );
    ok &= isInside( p4, daught );
    ok &= isInside( p5, daught );
    ok &= isInside( p6, daught );
    ok &= isInside( p7, daught );
    ok &= isInside( p8, daught );
    ok &= isInside( p9, daught );
    ok &= isInside( p10, daught );
  }
  else if( daught->volType() == "Tube" )
  {
    // For now, just test the centre of the two end points of the cylinder, this is by no means
    // complete coverage, but is a start...

    double r = daught->dimensions().x();
    if( daught->dimensions().y() > r )
      r = daught->dimensions().y();

    Hep3Vector origVect( r, r, daught->dimensions().z() / 2. );

    double dr = daught->relativePosition( this ).x() + origVect.x();
    double sr = daught->relativePosition( this ).x() - origVect.x();
    double dp = daught->relativePosition( this ).y() + origVect.y();
    double sp = daught->relativePosition( this ).y() - origVect.y();
    double dl = daught->relativePosition( this ).z() + origVect.z();
    double sl = daught->relativePosition( this ).z() - origVect.z();
 
    Hep3Vector p3( dr, daught->relativePosition( this ).y(), dl );
    Hep3Vector p4( sr, daught->relativePosition( this ).y(), dl );
    Hep3Vector p5( dr, daught->relativePosition( this ).y(), sl );
    Hep3Vector p6( sr, daught->relativePosition( this ).y(), sl );
    Hep3Vector p7( daught->relativePosition( this ).x(), dp, dl );
    Hep3Vector p8( daught->relativePosition( this ).x(), sp, dl );
    Hep3Vector p9( daught->relativePosition( this ).x(), dp, sl );
    Hep3Vector p10( daught->relativePosition( this ).x(), sp, sl );
    
    if( daught->relativeRotation( this ) != TestRot )
    {
      p3.transform( daught->relativeRotation( this ) );
      p4.transform( daught->relativeRotation( this ) );
      p5.transform( daught->relativeRotation( this ) );
      p6.transform( daught->relativeRotation( this ) );
      p7.transform( daught->relativeRotation( this ) );
      p8.transform( daught->relativeRotation( this ) );
      p9.transform( daught->relativeRotation( this ) );
      p10.transform( daught->relativeRotation( this ) );
    }
 
    // check in case any one of these points is outside of the mother volume

    ok &= isInside( p3, daught );
    ok &= isInside( p4, daught );
    ok &= isInside( p5, daught );
    ok &= isInside( p6, daught );
    ok &= isInside( p7, daught );
    ok &= isInside( p8, daught );
    ok &= isInside( p9, daught );
    ok &= isInside( p10, daught );
  }

  if( ! ok ) 
    std::cerr << "The daughter volume " << daught->fullName() << " is not completely inside the volume " << fullName() << std::endl;

  for( unsigned int i = 0; i < daught->_daughters.size(); ++i )
    ok &= daught->checkGeometry();

  return ok;
}

bool	MiceModule::isInside( const Hep3Vector& pos, const MiceModule* daught,  bool quiet ) const
{
  // firstly, get this position (which is in global coordinate) and convert it to the system local to this module

  HepRotation TestRot( 0.0, 0.0, 0.0 );
  Hep3Vector localPos( pos.x() - daught->relativePosition( this ).x(), pos.y() - daught->relativePosition( this ).y(), pos.z() - daught->relativePosition( this ).z() );
//  if( globalRotation() != TestRot )
//    localPos.transform( inverseOf( globalRotation() ) );  
  MiceModule* Mother = daught->mother();
  std::string VolType = Mother->volType();
  bool ok = true;
  
  if( VolType == "Box" )
  {
    if( localPos.x() < - Mother->dimensions().x() / 2. || localPos.x() > Mother->dimensions().x() / 2. ||
        localPos.y() < - Mother->dimensions().y() / 2. || localPos.y() > Mother->dimensions().y() / 2. ||
        localPos.z() < - Mother->dimensions().z() / 2. || localPos.z() > Mother->dimensions().z() / 2. )
      ok = false;
  }
  else if( VolType == "Cylinder" )
  {
    double R = sqrt( localPos.x() * localPos.x() + localPos.y() * localPos.y() );
  
      if( R > Mother->dimensions().x() ||  localPos.z() > Mother->dimensions().y() / 2. || localPos.z() < - Mother->dimensions().y() / 2. )
        ok = false;
  }
  else if( VolType == "Tube" )
  {
    double R = sqrt( localPos.x() * localPos.x() + localPos.y() * localPos.y() ); 
    double Rin = Mother->dimensions().x();
    double Rout = Mother->dimensions().y();
 
    if( Rin > Rout )
    {
      Rin = Mother->dimensions().y();
      Rout = Mother->dimensions().x();
    }

      if( R > Rout || localPos.z() < - Mother->dimensions().z() / 2. || localPos.z() > Mother->dimensions().z() / 2. )
        ok = false;
  }

  if( ! ok && ! quiet )
    std::cerr << "The position " << localPos << " is not inside the volume " << fullName() << " which is a " << volType() << " the dimentions are: " << dimensions() << std::endl;
  
  return ok;
}

CLHEP::HepRotation MiceModule::Hep3VecToRotation(CLHEP::Hep3Vector vec)
{
  CLHEP::HepRotation rot = HepRotationX( vec.x() ) * HepRotationY( vec.y() ) * HepRotationZ( vec.z() );
  return rot;
}

CLHEP::Hep3Vector MiceModule::HepRotationTo3Vec(CLHEP::HepRotation rot)
{
  CLHEP::Hep3Vector vec(0.,0.,0.);
  double flt_tol = 1e-9; //floating point tolerance

  vec[1] = asin(rot[0][2]);
  vec[2] = -asin(rot[0][1]/cos(vec[1]));
  if(fabs(vec[2]+atan(rot[0][1]/rot[0][0])) >flt_tol) vec[2] = CLHEP::pi-vec[2];
  if(fabs(rot[0][1])<flt_tol && rot[0][0] < 0 )       vec[2] = CLHEP::pi-vec[2];
  vec[0] = -asin(rot[1][2]/cos(vec[1]));
  if(fabs(vec[0]+atan(rot[1][2]/rot[2][2])) >flt_tol) vec[0] = CLHEP::pi-vec[0];
  if(fabs(rot[1][2])<flt_tol && rot[2][2] < 0 )       vec[0] = CLHEP::pi-vec[0];
  if(fabs(rot[0][2]) > 1.-flt_tol && fabs(rot[1][2]) < flt_tol && fabs(rot[0][1]) < flt_tol )
  {
    vec[0] = asin(rot[2][1]); 
    if(fabs(rot[1][1] - cos(vec[0])) > flt_tol) vec[0] = CLHEP::pi-vec[0];
    vec[2] = 0.;
  }
  return vec;
}

std::string MiceModule::rotationString(HepRotation rot)
{
	std::stringstream myStr("");
	myStr << rot.thetaX() << " " << rot.thetaY() << " " << rot.thetaZ();
	return myStr.str();
}

MiceModule* MiceModule::copyDisplaced(Hep3Vector translation, HepRotation rotation, double scaleFactor)
{
  MiceModule* copy = deepCopy(*this, true);
  copy->_hep3vectors["Position"]     = ModuleTextFileIO::toString(copy->position() + copy->rotation().inverse()*translation, precision);
  copy->_hep3vectors["Rotation"]     = ModuleTextFileIO::toString(HepRotationTo3Vec(rotation*copy->rotation()),              precision);
  copy->_doubles    ["ScaleFactor"]  = ModuleTextFileIO::toString(copy->scaleFactor()*scaleFactor,                           precision);
  return copy;
}

void MiceModule::addParameter(std::string key, double value)
{
  _parameters[key] = value;
}
void MiceModule::setRotation(HepRotation rotation)     
{
  addPropertyHep3Vector( "Rotation", HepRotationTo3Vec(rotation) );
}


void MiceModule::removeDaughter(MiceModule* daughter)
{
  for(unsigned int i=0; i<_daughters.size(); i++) if(_daughters[i] == daughter) {_daughters.erase(_daughters.begin()+i); i--;}
}

//idr 27/10/10

//! Get the map of boolean properties for the MiceModule
template <> 
std::map<std::string, bool> MiceModule::getListOfProperties<bool>()
{
  std::map<std::string, bool> r;
  for( std::map<std::string,std::string>::const_iterator it = _bools.begin(); it != _bools.end(); ++it )
    r[it->first] = propertyBool( it->first );
  return r;
}

//! Get the map of integer properties for the MiceModule
template <> 
std::map<std::string, int> MiceModule::getListOfProperties<int>()
{
  std::map<std::string, int> r;
  for( std::map<std::string,std::string>::const_iterator it = _ints.begin(); it != _ints.end(); ++it )
    r[it->first] = propertyInt( it->first );

  return r;
}

//! Get the map of std::string properties for the MiceModule
template <> 
std::map<std::string, std::string> MiceModule::getListOfProperties<std::string>()
{
  std::map<std::string, std::string> r = _strings;
  return r;
}

//! Get the map of double properties for the MiceModule

//! Use the evaluator to convert from string and account for units
template <> 
std::map<std::string, double> MiceModule::getListOfProperties<double>()
{
  std::map<std::string, double> r;
  for( std::map<std::string,std::string>::const_iterator it = _doubles.begin(); it != _doubles.end(); ++it )
    r[it->first] = propertyDouble( it->first );

  return r;
}

//! Get the map of Hep3Vector properties for the MiceModule

//! Use the evaluator to convert from string and account for units
template <> 
std::map<std::string, Hep3Vector> MiceModule::getListOfProperties<Hep3Vector>()
{
  std::map<std::string, Hep3Vector> r;
  for( std::map<std::string, std::string>::const_iterator it = _hep3vectors.begin(); it != _hep3vectors.end(); ++it )
    r[it->first] = propertyHep3Vector( it->first );

  return r;
}
