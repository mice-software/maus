// MiceModule.cc
//
// Class that describes the shape, size, position, orientation and other properties of a module of the MICE experiment,
// or a subcomponent.
//
// 15/5/2006 M.Ellis

#include <cstdlib>
#include <sstream>
#include <vector>
#include "Config/MiceModule.hh"
#include "Config/ModuleTextFileIO.hh"
#include "Interface/Squeal.hh"

#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::HepRotationX;
using CLHEP::HepRotationY;
using CLHEP::HepRotationZ;

MICEUnits          ModuleTextFileIO::_units;
HepTool::Evaluator ModuleTextFileIO::_evaluator;

std::map<std::string, std::string> ModuleTextFileIO::_substitutions = std::map<std::string, std::string>();


// Constructor takes the name of a file from which information is read in to instantiate this module or sub component

ModuleTextFileIO::ModuleTextFileIO(MiceModule* parent, std::string name, std::istream& input)
          : _this(NULL), _hasFile("")
{
	_this = new MiceModule(parent, stripDirs(name));
	parent->addDaughter(_this);

	readModule(name, input);

	std::string a="", b="";//nonsense to remove compiler warning
	parseString(a,b);
}

ModuleTextFileIO::ModuleTextFileIO( std::string fname ) : _this(NULL), _hasFile("")
{
	_this = new MiceModule(NULL, stripDirs(fname));
	_this->setVolumeType("Box");
	if(getenv( "MICEFILES" ) == NULL) 
		throw(Squeal(Squeal::recoverable, "Error - MICEFILES environment variable was not defined", "ModuleTextFileIO::ModuleTextFileIO"));
	std::string fnam = std::string(getenv( "MICEFILES" )) + "/Models/Configurations/" + fname;
	std::ifstream fin(fnam.c_str());
	if(!fin) 
	{
		fin.open(fname.c_str());
		if(!fin) 
		{
			fin.close(); 
			throw(Squeal(Squeal::recoverable, "Failed to open root module file at "+fnam+" or "+fname, "MiceModule::MiceModule(std::string)"));
		}
	}
	std::stringstream inOut1, inOut2, inOut3;
	stripFile (inOut1, fin);
	findSubs  (inOut2, inOut1);
	substitute(inOut3, inOut2);
	readModule(fname,  inOut3);
}

void ModuleTextFileIO::readModule(std::string name, std::istream& in)
{
	while(in)
	{
		std::string line = "";
		std::string key  = "";
		getline(in, line);
		std::stringstream lineStream(line);
		lineStream >> key;
		try
		{
			if     (key.find("Property")     != std::string::npos) readProperty(line);
			else if(key.find("Dimensions")   != std::string::npos) readDimensions(line);
			else if(key.find("Position")     != std::string::npos) readPosition(line);
			else if(key.find("Rotation")     != std::string::npos) readRotation(line);
			else if(key.find("ScaleFactor")  != std::string::npos) readScaleFactor(line);
			else if(key.find("Volume")       != std::string::npos) readVolume(line);
			else if(key.find("Module")       != std::string::npos) newModule(line, in);
			else if(key.find("Substitution") != std::string::npos); //do nothing, should be handled by preprocessor
			else if(key=="" || key =="//" || key=="!"); //ignore white space and comments  
			else throw(Squeal(Squeal::recoverable, "Failed to parse module line "+line+" in module "+name+_hasFile, "MiceModule::setModule(std::istream*)"));
		}
		catch(Squeal squee) { throw squee;}
		catch(...) {throw Squeal(Squeal::recoverable, "Failed to parse module line "+line+" in module "+name+_hasFile, "MiceModule::setModule(std::istream*)");}
	}
	checkRepeats();
}

void ModuleTextFileIO::readPosition(std::string lineIn)
{
    std::stringstream ist(lineIn);
    std::string dummy, pos;
    ist >> dummy;
    getline(ist, pos);
    _this->addPropertyHep3Vector( "Position", pos );
}

void ModuleTextFileIO::readRotation(std::string lineIn)
{
    std::stringstream ist(lineIn);
    std::string dummy, rot;
    ist >> dummy;
    getline(ist, rot);
    _this->addPropertyHep3Vector( "Rotation", rot );
}

void ModuleTextFileIO::readScaleFactor(std::string lineIn)
{
  std::string dummy, scale;
  std::stringstream ist(lineIn);
  ist >> dummy;
  getline(ist, scale);
  _this->addPropertyDouble("ScaleFactor", scale);
}

void ModuleTextFileIO::readVolume(std::string lineIn)
{
  std::string dummy, vol;
  std::stringstream ist(lineIn);
  ist >> dummy >> vol;
  _this->addPropertyString("Volume", vol);
}

//MiceModule(MiceModule* parent, std::string name, std::istream& input);
void ModuleTextFileIO::newModule(std::string lineIn, std::istream & in)
{
	std::string       modName = "";
	std::string       modLine = "";
	std::stringstream nameStream;
	nameStream << lineIn;
	nameStream >> modName >> modName;
	std::stringstream modStream1, modStream2;
	while(in && modLine.find("{")==std::string::npos) getline(in, modLine);

	modLine = "";
	int nLeftCurlies  = 0;
	int nRightCurlies = 0;
	while(in && nLeftCurlies >= nRightCurlies)
	{
		modStream1 << modLine << "\n";
		getline(in, modLine);
		if(modLine.find("{")!=std::string::npos) nLeftCurlies++;
		if(modLine.find("}")!=std::string::npos) nRightCurlies++;
	}

	modLine       = "";
	if(getenv( "MICEFILES" ) == NULL) 
		throw(Squeal(Squeal::recoverable, "Error - MICEFILES environment variable was not defined", "ModuleTextFileIO::ModuleTextFileIO"));
	std::string file = std::string(getenv( "MICEFILES" )) + "/Models/Modules/" + modName;
	std::ifstream fin(file.c_str());
	if(!fin) 
	{
		fin.open(modName.c_str());
		if(!fin) 
		{
			fin.close(); 
			_hasFile = " Note: I could not find a file associated with this module."; 
			Squeak::mout(Squeak::debug) << "Warning - failed to open module "+file+" - I assume it is not required" << std::endl;
		}
	}
	stripFile(modStream1, fin);
	substitute(modStream2, modStream1);

	ModuleTextFileIO daughter(_this, modName, modStream2);
}

void ModuleTextFileIO::checkRepeats()
{
	if     (_this->propertyExistsThis("RepeatModule",  "bool") && _this->propertyExistsThis("RepeatModule2",  "bool"))
		throw(Squeal(Squeal::recoverable, "Multiple repeat module types defined in MiceModule "+_this->fullName()+_hasFile, "ModuleTextFileIO::checkRepeats"));
	if     (_this->propertyExistsThis("RepeatModule",  "bool")) {if(!_this->propertyBool("RepeatModule"))  return;}
	else if(_this->propertyExistsThis("RepeatModule2", "bool")) {if(!_this->propertyBool("RepeatModule2")) return;}
	else return;
	int                nReps = _this->propertyInt("NumberOfRepeats");
	if     (_this->propertyExistsThis("RepeatModule2",  "bool")) 
	{
		Squeak::mout(Squeak::debug) << "Repeating module "+_this->name() << " " << nReps << " times" << std::endl;
		repeatModule2(_this, nReps); 
		return;
	}

	double             scale(1);
	CLHEP::Hep3Vector  trans(0,0,0);
	CLHEP::HepRotation rot;
	if(_this->propertyExistsThis("RepeatTranslation", "Hep3Vector")) trans = _this->propertyHep3Vector("RepeatTranslation");	
	if(_this->propertyExistsThis("RepeatRotation", "Hep3Vector"))
	{
		CLHEP::Hep3Vector rTemp = _this->propertyHep3Vector("RepeatRotation");
		rot = HepRotationX( rTemp.x() ) * HepRotationY( rTemp.y() ) * HepRotationZ( rTemp.z() );
	}
	if(_this->propertyExistsThis("RepeatScaleFactor", "double")) scale = _this->propertyDouble    ("RepeatScaleFactor");	
	Squeak::mout(Squeak::debug) << "Repeating module "+_this->name() << " " << nReps << " times with scale " << scale 
	                            << " rot " << _this->propertyHep3Vector("RepeatRotation") << " trans " << trans << std::endl;
	repeatModule(_this, trans, rot, scale, nReps);
}

void ModuleTextFileIO::stripFile(std::ostream& out, std::istream& in)
{
	std::string modLine = "";
	while(in && modLine.find("{")==std::string::npos) getline(in, modLine);

	modLine = "";
	int nLeftCurlies  = 0;
	int nRightCurlies = 0;
	while(in && nLeftCurlies >= nRightCurlies)
	{
		getline(in, modLine);
		if(modLine.find("//")!=std::string::npos) modLine = modLine.substr(0, modLine.find("//"));
		if(modLine.find("!") !=std::string::npos) modLine = modLine.substr(0, modLine.find("!"));
		if(modLine.find("{") !=std::string::npos) nLeftCurlies++;
		if(modLine.find("}") !=std::string::npos) nRightCurlies++;
		if(modLine.find("}") !=std::string::npos && nLeftCurlies < nRightCurlies) 
			modLine = modLine.substr(0, modLine.find("}"));
		out << modLine << "\n";
	}
}

void ModuleTextFileIO::readDimensions(std::string volumeType, std::string lineIn)
{
      Hep3Vector        _dimensions;
      std::stringstream ist(lineIn);
      std::string       dimensions;  
      ist >> dimensions;
      if(dimensions != "Dimensions") 
        throw(Squeal(Squeal::recoverable, "Did not recognise dimensions input in module "+_this->fullName()+_hasFile, "MiceModule::setDimensions(string, string)"));
      if( volumeType == "Cylinder" || volumeType == "Sphere" || volumeType == "Polycone")
      {
        std::stringstream dimStream;
        std::string radius, length, units;
        ist >> radius >> length >> units;
        //for consistency I force 3rd element to really be -1 independent of units - but this is just fluff
        dimStream << radius << " " << length << " " << -1/_units.GetUnits(units) << " " << units; 
        _this->addPropertyHep3Vector("Dimensions", dimStream.str());
      }
      else if( volumeType == "Box" || volumeType == "Wedge" || volumeType == "Tube"  || volumeType == "EllipticalCone")
      {
        std::stringstream dimStream;
        std::string x1, x2, x3, units;
        ist >> x1 >> x2 >> x3 >> units;
        dimStream << x1 << " " << x2 << " " << x3 << " " << units;
        _this->addPropertyHep3Vector("Dimensions", dimStream.str());
      }
      else if( volumeType == "Multipole" || volumeType == "Quadrupole" 
               || volumeType == "None" || volumeType == "Boolean")
        _this->setDimensions(CLHEP::Hep3Vector(0,0,0));
      else throw(Squeal(Squeal::recoverable, "Did not recognise volume type "+volumeType+" in module "+_this->fullName()+_hasFile, "MiceModule::setDimensions(std::string)"));
}

template <class Temp> Temp ModuleTextFileIO::fromString(const std::string& source)
{
  Temp target;
  parseString(source, target);
  return target;
}


void ModuleTextFileIO::parseString(const std::string& source, int&                out)
{
  std::stringstream ss(source);
  ss >> out;
}

void ModuleTextFileIO::parseString(const std::string& source, double&             out)
{
  std::string value="", units="";
  std::stringstream ss(source);
  ss >> value >> units;
  substitute(value, "$$", "MI_"); //Evaluator can't do $$ so I alias it to MI_ (for internal mice variables)
  out = _evaluator.evaluate(value.c_str());
  if(_evaluator.status() != HepTool::Evaluator::OK && _evaluator.status())
      throw(Squeal(Squeal::recoverable, "Could not convert "+source+" to a double", "ModuleTextFileIO::parseString(const std::string&, double&)"));
  out *= _units.GetUnits(units);
}

void ModuleTextFileIO::parseString(const std::string& source, bool&               out)
{
  std::string source_lower = source;
  for(unsigned int i=0; i<source.size(); i++) source_lower[i] = std::tolower(source[i]);
  if(source_lower == "1" || source_lower == "true" || source_lower == ".true.")
    out = true;
  else if(source_lower == "0" || source_lower == "false" || source_lower == ".false.")
    out = false;
  else throw(Squeal(Squeal::recoverable, "Could not convert "+source+" to a boolean", "ModuleTextFileIO::parseString"));
}

void ModuleTextFileIO::parseString(const std::string& source, std::string&        out) 
{ out = source;}


void ModuleTextFileIO::parseString(const std::string& source, CLHEP::Hep3Vector&  out)
{
  std::stringstream ss(source);
  std::string eval="", units="";
  for(int i=0; i<3; i++)
  {
    ss >> eval;
    parseString(eval, out[i]); //will call evaluator on each in turn
  }
  ss >> units;
  out *= _units.GetUnits(units);
}

void ModuleTextFileIO::readProperty(std::string lineIn)
{
    std::stringstream ist(lineIn);
    std::string key, propName, prop, prop2="";
    ist >> key >> propName >> prop >> prop2;
    while(ist){ prop += " "+prop2; ist >> prop2; }

    if( key == "PropertyHep3Vector" )
      _this->addPropertyHep3Vector(propName, prop);
    else if( key == "PropertyBool" )
      _this->addPropertyBool( propName, fromString<bool>(prop) );
    else if( key == "PropertyInt" )
      _this->addPropertyInt( propName, fromString<int>(prop) );
    else if( key == "PropertyString")
      _this->addPropertyString( propName, prop );
    else if( key == "PropertyDouble" )
      _this->addPropertyDouble(propName, prop);
    else throw(Squeal(Squeal::recoverable, "Did not recognise property type "+key+" in module "+_this->fullName()+_hasFile, "ModuleTextFileIO::readProperty(std::string)"));
}

std::string ModuleTextFileIO::stripDirs(std::string name)
{
	while(name.find("/")!=std::string::npos) 
		name = name.substr(name.find("/")+1, std::string::npos);
	return name;
}

void ModuleTextFileIO::findSubs(std::ostream& out, std::istream& in)
{
  std::string       line;
  while(in)
  {
    getline(in, line);
    std::stringstream linestream(line);
    std::string       word;
    linestream >> word;
    if(word == "Substitution")
    {
      std::string  name,value;
      linestream >> name >> value;
      if(!linestream)
        throw(Squeal(Squeal::recoverable, "Failed to parse substitution "+line, "ModuleTextFileIO::findSubs"));
      if(name[0] != '$')
        throw(Squeal(Squeal::recoverable, "Substitution name "+name+" must start with $", "ModuleTextFileIO::findSubs"));
      if(name[1] == '$')
        throw(Squeal(Squeal::recoverable, "Substitution name "+name+" must not start with $$ - this is reserved for internal parameters", "ModuleTextFileIO::findSubs"));
      _substitutions[name] = value;
      Squeak::mout(Squeak::debug) << "Substitution "+name+" "+value << std::endl;
    }
    out << line << "\n";
  }
}

void ModuleTextFileIO::substitute(std::ostream& out, std::istream& in)
{
  std::string       line;
  typedef std::map<std::string, std::string>::const_iterator map_it;

  while(in)
  {
    getline(in, line);
    std::stringstream linestream(line);
    std::string       word;
    linestream >> word;
    if(word != "Substitution")
    {
      for(map_it it=_substitutions.begin(); it != _substitutions.end(); it++)
        substitute(line, it->first, it->second);
      out << line << "\n";
    }
  }
}

void ModuleTextFileIO::substitute(std::string& target, std::string s1, std::string s2)
{
  size_t      name_pos = target.find(s1);
  while(name_pos != std::string::npos)
  {
    target.replace(name_pos, s1.length(), s2);
    name_pos = target.find(s1);
  }
}

void ModuleTextFileIO::repeatModule (MiceModule* first, Hep3Vector translation, HepRotation rotation, double scaleFactor, int numberOfRepeats)
{
  MiceModule * mod = NULL;
  if(numberOfRepeats > 0) 
    mod = first->copyDisplaced(translation, rotation, scaleFactor);
  for(double i=1; i<numberOfRepeats; i++)
    mod = mod->copyDisplaced(translation, rotation, scaleFactor);
}

void ModuleTextFileIO::repeatModule2 (MiceModule* first, int numberOfRepeats)
{
  MiceModule * mod = NULL;
  first->addParameter("$$RepeatNumber", 0);
  if(numberOfRepeats > 0) 
  {
    mod = MiceModule::deepCopy(*first, true);
    mod->addParameter("$$RepeatNumber", 1);
  }
  for(double i=1; i<numberOfRepeats; i++)
  {
    std::stringstream istr;
    istr << i+1;
    mod = MiceModule::deepCopy(*mod, true);
    mod->addParameter("$$RepeatNumber", i+1);
  }
}

void ModuleTextFileIO::setEvaluator(std::map<std::string, double> parameters)
{
  _evaluator.clear();
  _evaluator.setStdMath();
  for(std::map<std::string, double>::iterator it=parameters.begin(); it!=parameters.end(); it++)
  {
    std::string name  = it->first;
    double      value = it->second;
    substitute(name, "$$", "MI_"); //evaluator can't do $$ so I alias it to MI_ (for internal variables)
    _evaluator.setVariable(name.c_str(), value);
    if(_evaluator.status() != _evaluator.OK)
      throw(Squeal(Squeal::recoverable, "Failed to parse variable "+it->first+" from MiceModules", "ModuleTextFileIO::setEvaluator"));
  }
}


