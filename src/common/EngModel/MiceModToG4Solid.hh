#ifndef MiceModToG4Solid_hh
#define MiceModToG4Solid_hh 1

#include "Config/MiceModule.hh"


//! Interface between MiceModules and GEANT4 solids
//!
//! Just a bunch of functions to generate solid volumes that we are interested in 
//! including in G4MICE simulations. Mostly fairly trivial. I try to shield the
//! user from G4 exceptions, which are usually not very intelligible, not giving
//! the volume where the exception occured for example. The boolean volume stuff 
//! is more complicated.
//!
//! Note that when adding new volumes, you also need to update 
//! Config/*/ModuleTextFileIO.*, which also stores a list of valid volumes (gack).
//!
//! buildSolid method builds any solid in the list. If you know the type in
//! advance, you can call the buildThing functions, probably easiest just to use
//! buildSolid
//
// TODO:
// * Should probably just add all GEANT4 volume types...
// * Should move getModule function to micemodule or moduletextfileio
// * Boolean is not tested (needs reworking)

namespace MiceModToG4Solid
{
	//! Build some G4VSolid. throw a Squeal if it isn't supported.
	G4VSolid * buildSolid    ( MiceModule* mod );


  //! Builds a G4Trd that is wedge shaped, taking dimensions().x as width, 
  //! dimensions.z() as height and dimensions().y as length at open end.
	G4VSolid * buildWedge    ( MiceModule* );
  //! Builds a G4Box, taking dimensions as full lengths of each side.
	G4VSolid * buildBox      ( MiceModule* mod );
  //! Builds a G4Tubs, as a filled cylinder with radius dimensions.x() and length 
  //! as dimensions.y()
	G4VSolid * buildCylinder ( MiceModule* mod );
  //! Builds a G4Tubs, as a filled cylinder with inner radius dimensions.x(), outer
  //! radius dimensions.y() and length as dimensions.z()
	G4VSolid * buildTube     ( MiceModule* mod );
  //! Builds a G4Sphere
  //! 
  //! Builds as a sphere with inner radius dimensions.x(), outer radius 
  //! dimensions.y(); start phi angle phi.x(), end phi angle phi.y(); start theta 
  //! angle theta.x() end theta angle theta.y(); where phi and theta are 
  //! PropertyHep3Vectors Phi and Theta respectively.
	G4VSolid * buildSphere   ( MiceModule* mod );
  //! Builds a polycone volume of rotation (see Polycone.hh for details).
	G4VSolid * buildPolycone ( MiceModule* mod );
  //! Builds a torus
  //!
  //! Uses following parameters:
  //!   * TorusInnerRadius
  //!   * TorusOuterRadius
  //!   * TorusRadiusOfCurvature
  //!   * TorusAngleStart
  //!   * TorusOpeningAngle
	G4VSolid * buildTorus    ( MiceModule* mod );
    G4VSolid * buildTrapezoid ( MiceModule* mod);
  //! Build a boolean volume
  //! 
  //! Makes a base volume and then starts hacking bits off and sticking bits on as 
  //! unions, intersections, or subtractions. The base volume is defined by the
  //! propertyString BaseModule. Subsequent modules are defined by including a set 
  //! of properties like BooleanModule1Type, BooleanModule1Pos and Boolean1ModuleRot
  //! to define a type, position and rotation. pos and rot are Hep3Vectors while 
  //! type is a string - either Union, Intersection or Subtraction 
	G4VSolid * buildBoolean  ( MiceModule* mod );
  //! Builds a multipole (see Multipole.hh for details)
	G4VSolid * buildMultipole( MiceModule* mod );
  //! Builds an elliptical cone using dimensions.x()/dimensions.z() and 
  //! dimensions.y()/dimensions.z() as radius in x and y respectively, and 
  //! dimensions.z() as height. Additionally, ZCut property can be used to truncate 
  //! the cone.
	G4VSolid * buildEllipticalCone ( MiceModule* mod );

  //! Get a MiceModule from some file
	MiceModule* getModule      (MiceModule* mod, std::string modName);

  //* throw a Squeal if mod->Dimensions() are <= 0. length is either 2 or 3 (depending on 
  //* how many parameters the dimensions takes
  void       checkDim(MiceModule* mod, size_t length);
}

#endif

