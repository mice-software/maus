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

#include "Interface/Memory.hh" 

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
    void		printTree( int ) const;
    MiceModule*	mother() const		{ return _mother; };
    MiceModule*	mother()		{ return _mother; };
 
    int			daughters() const	{ return _daughters.size(); };
    MiceModule*	daughter( int ) const;
    std::vector<MiceModule*> allDaughters() const 	{ return _daughters; };

    // Return the type of volume this module is describing
    std::string	volType() const		{ return propertyString("Volume"); };
    Hep3Vector	dimensions() const	{ return propertyHep3Vector("Dimensions"); };
    bool        isInside( const MiceModule* ) const;

    // Return the position of this module with respect to the parent module
    Hep3Vector		position() const	{ return propertyHep3Vector("Position"); };
    // Return the position of this module with respect to another module that it is inside of
    Hep3Vector		relativePosition( const MiceModule* ) const;
    // Return the position of this module with respect to the global coordinate system
    Hep3Vector		globalPosition() const;
    // Return the rotation of this module with respect to the module that it is inside
    HepRotation		rotation() const	{ return Hep3VecToRotation(propertyHep3Vector("Rotation")); };
    // Return the rotation of this module with respect to another module that it is inside of
    const HepRotation*	rotationPointer() { _rotation = Hep3VecToRotation(propertyHep3Vector("Rotation")); return &_rotation; };
    HepRotation		relativeRotation( const MiceModule* ) const;

    // Return the rotation of this module with respect to the global coordinate system
    HepRotation		globalRotation() const;

    // Do the relative transformation
    HepGeom::Transform3D relativeTransform( const MiceModule* ) const;

    // Return the scale factor
    double scaleFactor() const {return propertyDouble("ScaleFactor");}

    // Return the product of this scale factor and all parents' scale factors
    double globalScaleFactor() const;

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

    //Return the named int property
    int			propertyIntThis( std::string propertyName ) const;
    //Return propertyInt in the closest ancestor, including this one, that has the property
    //throw a Squeal if ancestor tree does not have the property
    int			propertyInt( std::string propertyName ) const;

    unsigned int	numPropertyInt() const 							{ return _ints.size(); };
    void		ithInt( int i, std::string& name, int& val ) const;
    void		addPropertyInt( std::string name, int val );

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

    //print all daughters
    void print( std::ostream& ) const;
    //print only this class
    void printThis(std::ostream& ) const;
    const MiceModule*	rootModule() const;
    int			depth() const;
    bool		checkGeometry() const;
    bool		checkGeometry( MiceModule* daught ) const;
    bool		isInside( const Hep3Vector&, const MiceModule*,  bool quiet = false ) const;

	//setup methods
    void        addDaughter(MiceModule* daughter)     {_daughters.push_back(daughter); daughter->_mother=this;}
    void        removeDaughter(MiceModule* daughter);
    void        setDimensions(Hep3Vector dim)         {addPropertyHep3Vector( "Dimensions", dim );}
    void        setPosition(Hep3Vector position)      {addPropertyHep3Vector( "Position", position );}
    void        setRotation(HepRotation rotation);
    void        setScaleFactor(double scaleFactor)    {addPropertyDouble    ( "ScaleFactor", scaleFactor );}
    void        setVolumeType(std::string volumeType) {addPropertyString    ( "Volume", volumeType );}
    void        setName(std::string name)             {_name        = name;}
    void        setMother(MiceModule* mother)         {_mother      = mother;}
    //convert from HepRotation to string "xTheta yTheta zTheta"
    static std::string        rotationString(HepRotation rot);
    static CLHEP::HepRotation Hep3VecToRotation(CLHEP::Hep3Vector  vec);
    static CLHEP::Hep3Vector  HepRotationTo3Vec(CLHEP::HepRotation rot);

    MiceModule* copyDisplaced(Hep3Vector translation, HepRotation rotation, double scaleFactor);

    void        addParameter(std::string key, double value);


  private :
   void			printProperties( std::string ) const;

   void			checkNames();

   bool					_isroot;
   std::string				_name;
   MiceModule*				_mother;
   std::vector<MiceModule*>		_daughters;
   HepRotation				_rotation;
//   std::string			_volType; CTR - Is now a property
//   Hep3Vector				_dimensions;
//   Hep3Vector				_position;
//   double                   		_scaleFactor; //scale factor for fields defined in the module

   std::map<std::string,bool>		_bools;
   std::map<std::string,int>		_ints;
   std::map<std::string,std::string> 	_strings;
   //I store doubles as strings so that I can use evaluator on them...
   //It would be nice if I could always use evaluator at ModuleTextFileIO time, but that's not always possible
   //In particular, when doing repeats, I need to parse the propertyInt number of repeats, then run the
   //evaluator against the $$RepeatNumber, so I have to run the evaluator after doing IO
   std::map<std::string,std::string> 	_doubles; 
   std::map<std::string,std::string>	_hep3vectors;
   //parameters to be passed to the evaluator. Note that $$ is aliased to MI_ as evaluator can only handle alphanumerics
   std::map<std::string,double>		_parameters;
};



#endif

