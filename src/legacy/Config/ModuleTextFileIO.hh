// MAUS WARNING: THIS IS LEGACY CODE.
// MiceModule.hh
//
// Class that describes the shape, size, position, orientation and other properties of a module of the MICE experiment,
// or a subcomponent.
//
// 15/5/2006 M.Ellis

#ifndef MODULETEXTFILEIO_HH
#define MODULETEXTFILEIO_HH 1

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "Utils/Squeak.hh"

#include "src/common_cpp/Utils/MAUSEvaluator.hh"

class MiceModule;

class ModuleTextFileIO
{
  public :
    //Read in root Module
    ModuleTextFileIO( std::string rootName );
    //Read in daughter module and add it to the parent
    ModuleTextFileIO(MiceModule* parent, std::string name, std::istream& input);
    // Destructor
    virtual	~ModuleTextFileIO() {}
    //Return module that this represents
    MiceModule* getModule() {return _this;}

    //strip curly brackets and comments
    static void stripFile(std::ostream& out, std::istream& in);
    //strip text preceding / from name
    static std::string stripDirs(std::string name);
    //preprocessor to search through "in" and find all substitutions
    //put data from "in" into "out"
    //substitutions are defined by key word Substitution <name> <value>
    //needs to be all one line with nothing else on the line
    //throws an exception if <name> does not start with $, or if <name> or <value> can't be found
    static void findSubs(std::ostream& out, std::istream& in);
    //search through "in" and find all occurrences of <name> and replace with <value>
    //using _substitutions as the map of <name> to <value>
    static void substitute(std::ostream& out, std::istream& in);
    //substitute all instances of string s1 with string s2 in string target
    static void substitute(std::string& target, std::string s1, std::string s2);
    //alias $$ and @ to MI_ (in place substitution); this is because evaluator can't handle
    //special characters
    static void MI_alias(std::string& value);

    //convert from a value to a string
    template <class Temp> static std::string toString  (Temp value, int precision);
    //fill target with data from source; class Temp must have a parseString method (below) defined
    template <class Temp> static Temp        fromString(const std::string& source);
    static void parseString(const std::string& source, int&                out);
    static void parseString(const std::string& source, bool&               out);
    static void parseString(const std::string& source, std::string&        out);
    static void parseString(const std::string& source, CLHEP::Hep3Vector&  out);
    static void parseString(const std::string& source, double&             out);


    static void setEvaluator(std::map<std::string, double> parameters);
    static void setEvaluator(std::map<std::string, int> parametersInt);

    //Repeat module first numberOfRepeats times, applying translation, rotation, scalefactor etc
    void        repeatModule (MiceModule* first, CLHEP::Hep3Vector translation, CLHEP::HepRotation rotation, double scaleFactor, unsigned int numberOfRepeats);
    //Repeat module first numberOfRepeats times, loading a variable $$RepeatNumber to the MiceModule each time for evaluation each time
    //$$RepeatNumber starts at 0 and ends at numberOfRepeats
    void        repeatModule2 (MiceModule* first, unsigned int numberOfRepeats);

  private :
    // Construct a module from input stream; input stream *must* contain position, rotation information
    // expect this to be loaded into an iostream object which is then unloaded in this method (iostream inherits from istream) 
    // possibly preprocess that first!
    void readModule(std::string name, std::istream& input);
    //read components
    void readProperty(std::string lineIn);
    void readDimensions(std::string volumeType, std::string lineIn);
    void readDimensions(std::string lineIn);
    void readPosition(std::string lineIn);
    void readRotation(std::string lineIn);
    void readScaleFactor(std::string lineIn);
    void readVolume(std::string lineIn);
    //check for RepeatModule propertyBool and call _this->repeatThis(...) if required
    void checkRepeats();
    //create a submodule from existing input stream
    void newModule(std::string lineIn, std::istream& in);
  
    MiceModule* _this;
    std::string _hasFile;
    static std::map<std::string, std::string> _substitutions;
    static MAUS::MAUSEvaluator* _units;
    static MAUS::MAUSEvaluator* _evaluator;
  
};

template <class Temp> std::string ModuleTextFileIO::toString  (Temp value, int precision) {
  std::stringstream ss;
  ss << std::setprecision(precision);
  ss << value;
  return ss.str();
}



#endif
