// MAUS WARNING: THIS IS LEGACY CODE.
// MiceModule.hh
//
// Class that describes the shape, size, position, orientation and other properties of a module of the MICE experiment,
// or a subcomponent.
//
// 15/5/2006 M.Ellis

#ifndef CONFIG_MICEMODULE_HH
#define CONFIG_MICEMODULE_HH 1

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "Interface/MICEUnits.hh"

#include "Config/ModuleTextFileIO.hh"

//! MiceModule object handles geometry information and other relevant parameters for G4MICE
//! 
//! MiceModule is essentially a wrapper for a std::map from string property name to int, 
//! Hep3Vector, string and double property types. Caveat is that numerical property types 
//! are stored as strings until they are needed. This is so that we can evaluate them as 
//! equations.
//!
//! MiceModules sit in a nested tree structure, so that one module will sit inside another
//! module with relative position and rotation to the parent module. MiceModule is stores 
//! position, rotation and scale information, including a few functions to get e.g. position 
//! relative to another module, or relative to the global volume. ScaleFactor is a property
//! for the field maps, giving a linear scaling to the fields represented by the MiceModule
//! and all fields below it.
//!

using namespace CLHEP;

class MiceModule
{
  public :

    // Constructor; if parent is null assume root module
    MiceModule( std::string name ); //root module
    MiceModule( );
    MiceModule( MiceModule* parent, std::string name );
    virtual	~MiceModule();

    // < operator compares the global Z position of this module with another
    bool		operator<( const MiceModule& ) const;

    MiceModule&		operator=( MiceModule& ); //shallow copy which moves daughters with it deleting daughters in the original
    static MiceModule* deepCopy(const MiceModule &, bool insertInMother=false); //deep copy which makes a duplicate of the module tree beneath it; set insertInMother to True to add to daughter mods of mother module (i.e. add it to the module tree)

    std::string	name() const		{ return _name; };
    std::string	fullName() const; // Return the concatenated name of this module with all its mothers
    bool		isRoot() const		{ return _isroot; };
  /// Print out the module tree
    void		printTree( int, std::ostream& Cout = std::cout ) const;
  /// Dump the whole geometry tree in MiceModule format for local usage.
  /// Note that Repeat factors aren't output as each module is explicitly given.
  /// For comparisons, expect some rounding errors and some angles to be
  /// different by 2*PI.
  /// There is a warning for every Module since ModuleTextFileIO looks for
  /// a corresponding file in $(MICEFILES)/Models/Modules but won't find it
  /// since the subdirectory is stripped off the Module name.  This is not
  /// an error because we don't want any parameters read from files.
  void		DumpMiceModule(std::string & Indent, std::ostream & Cout = std::cout) const;
    MiceModule*	mother() const		{ return _mother; };
    MiceModule*	mother()	      	{ return _mother; };
 
    int			daughters() const	{ return _daughters.size(); };
    MiceModule*	daughter( int ) const;
    std::vector<MiceModule*> allDaughters() const 	{ return _daughters; };

    // Return the type of volume this module is describing
    std::string volType()    const {return propertyString("Volume");}
    Hep3Vector	dimensions() const;
    bool        isInside( const MiceModule* ) const;

    // Return the position of this module with respect to the parent module
    Hep3Vector		position() const;
    // Return the position of this module with respect to another module that it is inside of
    Hep3Vector		relativePosition( const MiceModule* ) const;
    // Return the position of this module with respect to the global coordinate system
    Hep3Vector		globalPosition() const;
    // Return the rotation of this module with respect to the module that it is inside
    HepRotation		rotation() const;
    // Return the rotation of this module with respect to another module that it is inside of
    const HepRotation*	rotationPointer() { _rotation = rotation(); return &_rotation; };
    HepRotation		relativeRotation( const MiceModule* ) const;

    // Return the rotation of this module with respect to the global coordinate system
    HepRotation		globalRotation() const;

    // Do the relative transformation
    HepGeom::Transform3D relativeTransform( const MiceModule* ) const;

    // Return the scale factor (or default 1.)
    double scaleFactor() const;

    // Return the product of this scale factor and all parents' scale factors
    double globalScaleFactor() const;


    //Set the named property - doesn't throw an exception if property already exists, just silently overwrites
    template <typename T> 
    void setProperty(std::string property, T val);

    // --- Methods to provide "Properties" of a Module
    // Return true if this property exists as a particular type
    bool    propertyExists( std::string property, std::string type) const;
    // Return true if this property exists in this instance
    bool    propertyExistsThis( std::string property, std::string type) const;

    //Return the named int property
    bool    propertyBoolThis( std::string propertyName ) const;
    //Return propertyBool in the closest ancestor, including this one, that has the property
    //throw a Squeal if ancestor tree does not have the property
    bool    propertyBool( std::string propertyName ) const;

    unsigned int	numPropertyBool() const 						{ return _bools.size(); };
    void		ithBool( int i, std::string& name, bool& val ) const;
    void		addPropertyBool( std::string name, bool val );
    void		addPropertyBool( std::string name, std::string val );

    //Return the named int property
    int			propertyIntThis( std::string propertyName ) const;
    //Return propertyInt in the closest ancestor, including this one, that has the property
    //throw a Squeal if ancestor tree does not have the property
    int			propertyInt( std::string propertyName ) const;

    unsigned int	numPropertyInt() const 							{ return _ints.size(); };
    void		ithInt( int i, std::string& name, int& val ) const;
    void		addPropertyInt( std::string name, int val );
    void		addPropertyInt( std::string name, std::string val );

    //Return the named double property
    double		propertyDoubleThis( std::string propertyName ) const;
    //Return propertyDouble in the closest ancestor, including this one, that has the property
    //throw a Squeal if ancestor tree does not have the property
    double		propertyDouble( std::string propertyName ) const;

    unsigned int	numPropertyDouble() const 						{ return _doubles.size(); };
    void		ithDouble( int i, std::string& name, double& val ) const;
    void		addPropertyDouble( std::string name, double val );
    void		addPropertyDouble( std::string name, std::string val );

    //Return the named double property
    std::string		propertyStringThis( std::string propertyName) const;
    //Return propertyString in the closest ancestor, including this one, that has the property
    //throw a Squeal if ancestor tree does not have the property
    std::string		propertyString( std::string  propertyName) const;

    unsigned int	numPropertyString() const 						{ return _strings.size(); };
    void		ithString( int i, std::string& name, std::string& val ) const;
    void		addPropertyString( std::string name, std::string val );

    //Return the named double propertyHep3Vector
    Hep3Vector		propertyHep3VectorThis( std::string propertyName) const;
    //Return propertyHep3Vector in the closest ancestor, including this one, that has the property
    //throw a Squeal if ancestor tree does not have the property
    Hep3Vector		propertyHep3Vector( std::string ) const;
    unsigned int	numPropertyHep3Vector() const 						{ return _hep3vectors.size(); };
    void		ithHep3Vector( int i, std::string& name, Hep3Vector& val ) const;
    void		addPropertyHep3Vector( std::string name, Hep3Vector val );
    void		addPropertyHep3Vector( std::string name, std::string val );

    // Return all daughters (and self) that have the string property refered to of a specific value
    std::vector<const MiceModule*>	findModulesByPropertyString( std::string, std::string ) const;
    std::vector<const MiceModule*>	findModulesByPropertyExists( std::string, std::string ) const;
    std::vector<const MiceModule*>	findModulesByName( std::string ) const;
    //Somehow gcc doesnt overload from const to non-const... so add nc suffix (to mean NON CONST)
    std::vector<MiceModule*>	findModulesByPropertyExistsNC( std::string, std::string );

    /// print all daughters
    void print( std::ostream& ) const;
    /// print only this class
    void printThis(std::ostream& ) const;
    const MiceModule*	rootModule() const;
    int			depth() const;
    bool		checkGeometry() const;
    bool		checkGeometry( MiceModule* daught ) const;
    bool		isInside( const Hep3Vector&, const MiceModule*,  bool quiet = false ) const;

    //setup methods
    void        addDaughter(MiceModule* daughter)     {_daughters.push_back(daughter); daughter->_mother=this;}
    void        removeDaughter(MiceModule* daughter);
    void        setRotation(HepRotation rotation);
    void        setName(std::string name)             {_name        = name;}
    void        setMother(MiceModule* mother)         {_mother      = mother;}
    //convert from HepRotation to string "xTheta yTheta zTheta"
    static std::string        rotationString(HepRotation rot);
    static CLHEP::HepRotation Hep3VecToRotation(CLHEP::Hep3Vector  vec);
    static CLHEP::Hep3Vector  HepRotationTo3Vec(CLHEP::HepRotation rot);

    MiceModule* copyDisplaced(Hep3Vector translation, HepRotation rotation, double scaleFactor);

    void        addParameter(std::string key, double value);

  //idr 27/10/10
  //! Template the code to get lists of properties from the MiceModule
  template <typename T>
  std::map<std::string, T> getListOfProperties();

private :

   //return the map of the appropriate type
   std::map<std::string, std::string>* getPropertyMap(bool        dummy) {return &_bools;}
   std::map<std::string, std::string>* getPropertyMap(int         dummy) {return &_ints;}
   std::map<std::string, std::string>* getPropertyMap(std::string dummy) {return &_strings;}
   std::map<std::string, std::string>* getPropertyMap(double      dummy) {return &_doubles;}
   std::map<std::string, std::string>* getPropertyMap(CLHEP::Hep3Vector dummy) {return &_hep3vectors;}

  /// Print module properties
   void			printProperties( std::string, std::ostream& Cout = std::cout ) const;
  void DumpProperties(std::string & indent, std::ostream & Cout) const;

   void			checkNames();

   bool					      _isroot;
   std::string				_name;
   MiceModule*				_mother;
   std::vector<MiceModule*>		_daughters;
   HepRotation				_rotation;

   std::map<std::string,std::string> _bools;
   std::map<std::string,std::string> _ints;//Oct2010. Now ints will be read as strings
   std::map<std::string,std::string> _strings;
   //I store doubles as strings so that I can use evaluator on them...
   //It would be nice if I could always use evaluator at ModuleTextFileIO time, but that's not always possible
   //In particular, when doing repeats, I need to parse the propertyInt number of repeats, then run the
   //evaluator against the @RepeatNumber, so I have to run the evaluator after doing IO
   std::map<std::string,std::string> _doubles; 
   std::map<std::string,std::string> _hep3vectors;
   //parameters to be passed to the evaluator. Note that @ is aliased to MI_ as evaluator can only handle alphanumerics
   std::map<std::string,double>		_parameters;
   static const int precision;
};

//Set the named property - doesn't throw an exception if property already exists, just silently overwrites
template <typename T>
void MiceModule::setProperty(std::string property, T val) {
  std::string val_string = ModuleTextFileIO::toString(val, precision);
  std::map<std::string, std::string>* prop_map = getPropertyMap(val);
  (*prop_map)[property] = val_string;
}





#endif

