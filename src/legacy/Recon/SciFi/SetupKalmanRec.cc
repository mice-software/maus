// SetupKalmanRec.cc
//
// M.Ellis 20/10/20003
//
// Code to setup the Kalman package for the reconstruction of the MICE SciFi tracker


#include <algorithm>


#include "SetupKalmanRec.hh"


#include "Interface/dataCards.hh"
#include "Interface/MICERun.hh"
#include "Config/MiceModule.hh"
#include "Interface/MiceMaterials.hh"
#include "BeamTools/BTField.hh"
#include "BeamTools/BTFieldConstructor.hh"
#include "SciFiFieldMap.hh"


#include "recpack/string_tools.h"
#include "recpack/stc_tools.h"
#include "recpack/ERandom.h"
#include "recpack/HelixEquation.h"
#include "recpack/RecpackManager.h"
#include "recpack/Setup.h"
#include "recpack/Tube.h"
#include "recpack/Ring.h"
#include "recpack/EAlgebra.h"
#include "recpack/string_tools.h"


#include <vector>
#include <map>
#include <string>
#include <iostream>


static const double scale = 2.; // ME - scale factor applied to transvers dimensions


using namespace Recpack;


Recpack::RecpackManager _man;
Recpack::Setup _setup;


static std::vector<EVector*>    _BFields;
static std::vector<double*>      _X0s;
static std::vector<double*>      _dEdxs;


double getdEdx( double );


static bool useMcs = true;
static bool usedEdx = true;


std::vector<Surface*> _surfaces;


static std::vector<MiceModule*> _kalModules;


static SciFiFieldMap* _fieldMap = NULL;
static double       _step = 1e12 * mm;


std::string findCorrectMotherVolume( MiceModule* mod );


void setupSciFiKalman( MICERun* run )
{
  setupSciFiKalman( run, false );
}


void setupSciFiKalman( MICERun* run, bool verbose )
{
  if( verbose )
  {
    std::cout << "Checking the MiceModule Geometry for overlapping volumes..." << std::endl;
    bool ok = run->miceModule->checkGeometry();
    std::cout << "Check returned " << ok << std::endl;
    std::cout << "============================================================" << std::endl;
  }


  // ME - for now never include MCS model until it is debugged!
  //if( run->DataCards->fetchValueString( "MultipleScatteringModel" ) == "none" )
    useMcs = false;


  // ME - for now never include dEdx model until it is debugged!
  //if( run->DataCards->fetchValueString( "EnergyLossModel" ) == "none" )
    usedEdx = false;


  if( run->DataCards->fetchValueInt( "KalmanUseFieldMap" ) != 0 )
  {
    _fieldMap = new SciFiFieldMap( run->btFieldConstructor );
    _step = run->DataCards->fetchValueDouble( "KalmanStepSize" );
  }


  // select the fitter
  _man.fitting_svc().select_fitter( "kalman" );


  //create the experimental setup
  create_setup( run );


  // set the zero length

  _man.geometry_svc().set_zero_length( 0.1 * mm );
  _man.geometry_svc().set_zero_transverse_component( 1e-8 );

  if( verbose )
    std::cout << _setup << std::endl;


  //select verbosity levels (3 is max)
  // fitting, navigation, model, matching, simulation
  set_verbosity( run );
}


// create setup
void create_setup( MICERun* run )
{
/////////////////////////////////////////////////////////////////////


  // the axes for the definition of volumes and surfaces
  EVector xaxis(3,0);
  EVector yaxis(3,0);
  EVector zaxis(3,0);
  xaxis[0] =1.; yaxis[1] =1.; zaxis[2] =1.;


///////////////////////////////////////////////////////////////////////


  // the magnetic field


  double EMField[6];
  double bpos[4];


  bpos[0] = run->miceModule->globalPosition().x();
  bpos[1] = run->miceModule->globalPosition().y();
  bpos[2] = run->miceModule->globalPosition().z();
  bpos[3] = 0.;


  run->btFieldConstructor->GetMagneticField()->GetFieldValue( bpos, EMField );


  double mag = sqrt( EMField[0] * EMField[0] + EMField[1] * EMField[1] + EMField[2] * EMField[2] ) / tesla;


  if( mag < 1e-4 )
  {
    EMField[0] = 0.;
    EMField[1] = 0.;
    EMField[2] = 1e-6;
  }


  EVector* _BField = new EVector( 3, 0 );
  (*_BField)[0] = EMField[0];
  (*_BField)[1] = EMField[1];
  (*_BField)[2] = EMField[2];


  _BFields.push_back( _BField );


////////////////////////////////////////////////////////////////////////


  // radiation length
  double* _X0 = new double;
  *_X0 = run->miceMaterials->radiationLength( run->miceModule->propertyString( "Material" )  );


  _X0s.push_back(_X0);


  // energy loss 


  double* _dEdx = new double;
  *_dEdx = run->miceMaterials->dEdxLength( run->miceModule->propertyString( "Material" )  );


  _dEdxs.push_back( _dEdx );


////////////////////////////////////////////////////////////////////////


  // name of setup
  _setup.set_name("MICE");


////////////////////////////////////////////////////////////////////////


  // Create a mandatory mother volume
  EVector pos(3,0);
  pos[0] = run->miceModule->globalPosition().x();
  pos[1] = run->miceModule->globalPosition().y();
  pos[2] = run->miceModule->globalPosition().z();
  double xSize = run->miceModule->dimensions().x() / 2. * scale;
  double ySize = run->miceModule->dimensions().y() / 2. * scale;
  double zSize = run->miceModule->dimensions().z() / 2.;


  Hep3Vector xax( xaxis[0], xaxis[1], xaxis[2] );
  Hep3Vector xaxrot = run->miceModule->globalRotation() * xax;
  EVector xaxisRot(3,0);
  xaxisRot[0] = xaxrot.x();
  xaxisRot[1] = xaxrot.y();
  xaxisRot[2] = xaxrot.z();


  Hep3Vector yax( yaxis[0], yaxis[1], yaxis[2] );
  Hep3Vector yaxrot = run->miceModule->globalRotation() * yax;
  EVector yaxisRot(3,0);
  yaxisRot[0] = yaxrot.x();
  yaxisRot[1] = yaxrot.y();
  yaxisRot[2] = yaxrot.z();


  Volume* mother = new Box(pos,xaxisRot,yaxisRot,xSize,ySize,zSize);
  _setup.set_mother(mother);


////////////////////////////////////////////////////////////////////////


  if( useMcs && *_X0 < 1. * m )
    _setup.set_volume_property( "mother","X0", *_X0 );


  if( usedEdx && *_X0 < 1. * m )
    _setup.set_volume_property( "mother", "eloss", *_dEdx );


  if( mag > 1e-4 )
  {
    if( ! _fieldMap )
      _setup.set_volume_property("mother","BField",*_BField);
    else
    {
      _setup.set_volume_property("mother","BFieldMap", *_fieldMap );
      _setup.set_volume_property("mother","StepSize", _step );
    }
  }


////////////////////////////////////////////////////////////////////////


  // create several volumes and surfaces


  for( int i = 0; i < run->miceModule->daughters(); ++i )
    addVolumeToKalman( run, run->miceModule->daughter(i) );


  // add the setup to the geometry service
  _man.geometry_svc().add_setup("main",_setup);


////////////////////////////////////////////////////////////////////////


  // select the setup to be used by the geometry service
  _man.geometry_svc().select_setup("main");
}


void addVolumeToKalman( MICERun* run, MiceModule* module )
{


/////////////////////////////////////////////////////////////////////


  // the axes for the definition of volumes and surfaces
  EVector xaxis(3,0);
  EVector yaxis(3,0);
  EVector zaxis(3,0);
  xaxis[0] =1.; yaxis[1] =1.; zaxis[2] =1.;


///////////////////////////////////////////////////////////////////////


  // the magnetic field


  double EMField[6];
  double bpos[4];


  bpos[0] = module->globalPosition().x();
  bpos[1] = module->globalPosition().y();
  bpos[2] = module->globalPosition().z();
  bpos[3] = 0.;


  run->btFieldConstructor->GetMagneticField()->GetFieldValue( bpos, EMField );


  double mag = sqrt( EMField[0] * EMField[0] + EMField[1] * EMField[1] + EMField[2] * EMField[2] ) / tesla;


  if( mag < 1e-4 )
  {
    EMField[0] = 0.;
    EMField[1] = 0.;
    EMField[2] = 1e-6;
  }


  EVector* _BField = new EVector( 3, 0 );
  (*_BField)[0] = EMField[0];
  (*_BField)[1] = EMField[1];
  (*_BField)[2] = EMField[2];


 _BFields.push_back( _BField );
 




////////////////////////////////////////////////////////////////////////


  // radiation length
  double* _X0 = new double;
  *_X0 = run->miceMaterials->radiationLength( module->propertyString( "Material" )  );
  _X0s.push_back( _X0 );


  // energy loss 
  


  double* _dEdx = new double;
  *_dEdx = run->miceMaterials->dEdxLength( module->propertyString( "Material" )  );
  


  _dEdxs.push_back( _dEdx );


////////////////////////////////////////////////////////////////////////


  std::string mother_name = "mother";


  if( module->mother() != module->rootModule() )
    mother_name = findCorrectMotherVolume( module );


  EVector posZero(3,0);
  posZero[0] = module->globalPosition().x();
  posZero[1] = module->globalPosition().y();
  posZero[2] = module->globalPosition().z();


  bool made = true;


  bool stat = false;
  if( module->propertyExistsThis( "Station", "int" ) || module->propertyExistsThis( "KalmanSolenoid", "int" ) )
    stat = true;


  if( stat && module->volType() == "Box" )
  {
    EVector pos(3,0);
    pos[0] = module->globalPosition().x();
    pos[1] = module->globalPosition().y();
    pos[2] = module->globalPosition().z();
    double xSize = module->dimensions().x() / 2. * scale;
    double ySize = module->dimensions().y() / 2. * scale;
    double zSize = module->dimensions().z() / 2.;


    Hep3Vector xax( xaxis[0], xaxis[1], xaxis[2] );
    Hep3Vector xaxrot = module->globalRotation() * xax;
    EVector xaxisRot(3,0);
    xaxisRot[0] = xaxrot.x();
    xaxisRot[1] = xaxrot.y();
    xaxisRot[2] = xaxrot.z();


    Hep3Vector yax( yaxis[0], yaxis[1], yaxis[2] );
    Hep3Vector yaxrot = module->globalRotation() * yax;
    EVector yaxisRot(3,0);
    yaxisRot[0] = yaxrot.x();
    yaxisRot[1] = yaxrot.y();
    yaxisRot[2] = yaxrot.z();


    Hep3Vector zax( zaxis[0], zaxis[1], zaxis[2] );
    Hep3Vector zaxrot = module->globalRotation() * zax;
    EVector zaxisRot(3,0);
    zaxisRot[0] = zaxrot.x();
    zaxisRot[1] = zaxrot.y();
    zaxisRot[2] = zaxrot.z();


    Volume* vol = new Box(pos,xaxisRot,yaxisRot,xSize,ySize,zSize);
    _setup.add_volume(mother_name,module->fullName(),vol);
    if( useMcs && *_X0 < 1. * m )
      _setup.set_volume_property(module->fullName(),"X0", *_X0 );
    if( usedEdx && *_X0 < 1. * m )
      _setup.set_volume_property(module->fullName(),"eloss", *_dEdx );


    if( mag > 1e-4 )
    {
      if( ! _fieldMap )
        _setup.set_volume_property(module->fullName(),"BField",*_BField);
      else
      {
        _setup.set_volume_property(module->fullName(),"BFieldMap",*_fieldMap );
        _setup.set_volume_property(module->fullName(),"StepSize", _step );
      }
    }


    addSurfacesDaughters( module );
  }
  else if( stat && module->volType() == "Cylinder" )
  {
    EVector pos(3,0);
    pos[0] = module->globalPosition().x();
    pos[1] = module->globalPosition().y();
    pos[2] = module->globalPosition().z();
    double rSize = module->dimensions().x() * scale;
    double lSize = module->dimensions().y() / 2. - module->depth() * 0.001 * mm;


    Hep3Vector xax( xaxis[0], xaxis[1], xaxis[2] );
    Hep3Vector xaxrot = module->globalRotation() * xax;
    EVector xaxisRot(3,0);
    xaxisRot[0] = xaxrot.x();
    xaxisRot[1] = xaxrot.y();
    xaxisRot[2] = xaxrot.z();


    Hep3Vector zax( zaxis[0], zaxis[1], zaxis[2] );
    Hep3Vector zaxrot = module->globalRotation() * zax;
    EVector zaxisRot(3,0);
    zaxisRot[0] = zaxrot.x();
    zaxisRot[1] = zaxrot.y();
    zaxisRot[2] = zaxrot.z();


    Volume* vol = new Tube(pos,zaxisRot,lSize,rSize);
    _setup.add_volume(mother_name,module->fullName(),vol);
    if( useMcs && *_X0 < 1. * m )
      _setup.set_volume_property(module->fullName(),"X0", *_X0 );
    if( usedEdx && *_X0 < 1. * m )
      _setup.set_volume_property(module->fullName(),"eloss", *_dEdx );


    if( mag > 1e-4 )
    {
      if( ! _fieldMap )
        _setup.set_volume_property(module->fullName(),"BField",*_BField);
      else
      {
        _setup.set_volume_property(module->fullName(),"BFieldMap",*_fieldMap );
        _setup.set_volume_property(module->fullName(),"StepSize", _step );
      }
    }


    addSurfacesDaughters( module );
  }
  else if( stat && module->volType() == "Tube" )
  {
    EVector pos(3,0);
    pos[0] = module->globalPosition().x();
    pos[1] = module->globalPosition().y();
    pos[2] = module->globalPosition().z();
    double r1Size = module->dimensions().x() - module->depth() * 0.001 * mm;
    double r2Size = module->dimensions().y() - module->depth() * 0.001 * mm;
    double lSize = module->dimensions().z() / 2. - module->depth() * 0.001 * mm;


    Hep3Vector xax( xaxis[0], xaxis[1], xaxis[2] );
    Hep3Vector xaxrot = module->globalRotation() * xax;
    EVector xaxisRot(3,0);
    xaxisRot[0] = xaxrot.x();
    xaxisRot[1] = xaxrot.y();
    xaxisRot[2] = xaxrot.z();


    Hep3Vector zax( zaxis[0], zaxis[1], zaxis[2] );
    Hep3Vector zaxrot = module->globalRotation() * zax;
    EVector zaxisRot(3,0);
    zaxisRot[0] = zaxrot.x();
    zaxisRot[1] = zaxrot.y();
    zaxisRot[2] = zaxrot.z();


    Volume* vol = new Tube(pos,zaxisRot,lSize,r2Size,r1Size);
    _setup.add_volume(mother_name,module->fullName(),vol);
    if( useMcs && *_X0 < 1. * m )
      _setup.set_volume_property(module->fullName(),"X0", *_X0 );
    if( usedEdx && *_X0 < 1. * m )
      _setup.set_volume_property(module->fullName(),"eloss", *_dEdx );


    if( mag > 1e-4 )
    {
      if( ! _fieldMap )
        _setup.set_volume_property(module->fullName(),"BField",*_BField);
      else
      {
        _setup.set_volume_property(module->fullName(),"BFieldMap",*_fieldMap );
        _setup.set_volume_property(module->fullName(),"StepSize", _step );
      }
    }


    addSurfacesDaughters( module );
  }
  else
    made = false;


  if( made ) // add this module to the list of modules that Kalman knows about
    _kalModules.push_back( module );


    for( int i = 0; i < module->daughters(); ++i )
      addVolumeToKalman( run, module->daughter(i) );
}




////////////////////////////////////////////////////////////////////////////////


void set_verbosity( MICERun* run )
{
  int fitterVerb = run->DataCards->fetchValueInt( "FitterVerbosity" ); 
  int navigatorVerb = run->DataCards->fetchValueInt( "NavigatorVerbosity" );
  int modelVerb = run->DataCards->fetchValueInt( "ModelVerbosity" );
  int matchingVerb = run->DataCards->fetchValueInt( "MatchingVerbosity" );
 


  Messenger::Level level0 = Recpack::Messenger::MUTE;
  Messenger::Level level1 = Recpack::Messenger::MUTE;
  Messenger::Level level2 = Recpack::Messenger::MUTE;
  Messenger::Level level3 = Recpack::Messenger::MUTE;


  if( fitterVerb == 0 ) level0 = Recpack::Messenger::MUTE;
  else if( fitterVerb == 1 ) level0 = Recpack::Messenger::ERROR;
  else if( fitterVerb == 2 ) level0 = Recpack::Messenger::WARNING;
  else if( fitterVerb == 3 ) level0 = Recpack::Messenger::INFO;
  else if( fitterVerb == 4 ) level0 = Recpack::Messenger::NORMAL;
  else if( fitterVerb == 5 ) level0 = Recpack::Messenger::DETAILED;
  else if( fitterVerb == 6 ) level0 = Recpack::Messenger::VERBOSE;
  else if( fitterVerb == 7 ) level0 = Recpack::Messenger::VVERBOSE;


  if( navigatorVerb == 0 ) level1 = Recpack::Messenger::MUTE;
  else if( navigatorVerb == 1 ) level1 = Recpack::Messenger::ERROR;
  else if( navigatorVerb == 2 ) level1 = Recpack::Messenger::WARNING;
  else if( navigatorVerb == 3 ) level1 = Recpack::Messenger::INFO;
  else if( navigatorVerb == 4 ) level1 = Recpack::Messenger::NORMAL;
  else if( navigatorVerb == 5 ) level1 = Recpack::Messenger::DETAILED;
  else if( navigatorVerb == 6 ) level1 = Recpack::Messenger::VERBOSE;
  else if( navigatorVerb == 7 ) level1 = Recpack::Messenger::VVERBOSE;


  if( modelVerb == 0 ) level2 = Recpack::Messenger::MUTE;
  else if( modelVerb == 1 ) level2 = Recpack::Messenger::ERROR;
  else if( modelVerb == 2 ) level2 = Recpack::Messenger::WARNING;
  else if( modelVerb == 3 ) level2 = Recpack::Messenger::INFO;
  else if( modelVerb == 4 ) level2 = Recpack::Messenger::NORMAL;
  else if( modelVerb == 5 ) level2 = Recpack::Messenger::DETAILED;
  else if( modelVerb == 6 ) level2 = Recpack::Messenger::VERBOSE;
  else if( modelVerb == 7 ) level2 = Recpack::Messenger::VVERBOSE;


  if( matchingVerb == 0 ) level3 = Recpack::Messenger::MUTE;
  else if( matchingVerb == 1 ) level3 = Recpack::Messenger::ERROR;
  else if( matchingVerb == 2 ) level3 = Recpack::Messenger::WARNING;
  else if( matchingVerb == 3 ) level3 = Recpack::Messenger::INFO;
  else if( matchingVerb == 4 ) level3 = Recpack::Messenger::NORMAL;
  else if( matchingVerb == 5 ) level3 = Recpack::Messenger::DETAILED;
  else if( matchingVerb == 6 ) level3 = Recpack::Messenger::VERBOSE;
  else if( matchingVerb == 7 ) level3 = Recpack::Messenger::VVERBOSE;


  // verbosity levels related with fitting
  std::string _model = "particle/helix";


  _man.fitting_svc().fitter(_model).set_verbosity(level0);


  // verbosity levels related with navigation
  _man.navigation_svc().set_verbosity(level1);
  _man.navigation_svc().navigator(_model).set_verbosity(level1);
  _man.navigation_svc().inspector("ms").set_verbosity(level1);
  _man.navigation_svc().navigator(_model).master_inspector().set_verbosity(level1);
  _man.navigation_svc().inspector("BField").set_verbosity(level1);  
  _man.navigation_svc().inspector("BFieldMap").set_verbosity(level1);  
  _man.navigation_svc().inspector("eloss").set_verbosity(level1);     
  _man.model_svc().model(_model).intersector("plane").set_verbosity(level1);


  // verbosity levels related with model operation (soft intersection)
  _man.model_svc().model(_model).equation().set_verbosity(level2);
  _man.model_svc().model(_model).propagator().set_verbosity(level2);
  _man.model_svc().model(_model).noiser().set_verbosity(level2);
  _man.model_svc().model(_model).tool("correction/eloss").set_verbosity(level2);
  _man.model_svc().model(_model).tool("noiser/ms").set_verbosity(level2);


  // verbosity levels related with matching
  _man.matching_svc().set_verbosity(level3);

  // finally - intersector verbosity for debugging info for Anselmo

 /*
  Recpack::Messenger::Level intLev = Recpack::Messenger::VVERBOSE;
  std::string intModel( "particle/helix" );

  _man.model_svc().model( intModel ).intersector("plane").set_verbosity( intLev );
  _man.model_svc().model( intModel ).intersector("numerical").set_verbosity( intLev );
*/
}




void addSurfacesDaughters( MiceModule* module )
{
  EVector xaxis(3,0);
  EVector yaxis(3,0);
  EVector zaxis(3,0);


  xaxis[0] =1.;
  yaxis[1] =1.;
  zaxis[2] =1.;


  // and add a generic surface for the center of this module
  // if it has a station number


  if( module->propertyExistsThis( "Station", "int" ) )
  {
    if( module->volType() == "Cylinder" )
    {
      EVector posZero(3,0);
      posZero[0] = module->globalPosition().x();
      posZero[1] = module->globalPosition().y();
      posZero[2] = module->globalPosition().z();


      double rSize = module->dimensions().x() * scale;


      Hep3Vector zax( zaxis[0], zaxis[1], zaxis[2] );
      Hep3Vector zaxrot = module->globalRotation() * zax;
      EVector zaxisRot(3,0);
      zaxisRot[0] = zaxrot.x();
      zaxisRot[1] = zaxrot.y();
      zaxisRot[2] = zaxrot.z();


      const dict::Key surf_name = module->fullName() + "_surf";
      Surface* surf = new Ring( posZero, zaxisRot, rSize, 0. );
      _setup.add_surface( module->fullName(), surf_name, surf );
      _setup.set_surface_property( surf_name, "measurement_type", "xy" );
      _surfaces.push_back( surf );
    }
    else if( module->volType() == "Box" )
    {
      EVector posZero(3,0);
      posZero[0] = module->globalPosition().x();
      posZero[1] = module->globalPosition().y();
      posZero[2] = module->globalPosition().z();


      Hep3Vector xax( xaxis[0], xaxis[1], xaxis[2] );
      Hep3Vector xaxrot = module->globalRotation() * xax;
      EVector xaxisRot(3,0);
      xaxisRot[0] = xaxrot.x();
      xaxisRot[1] = xaxrot.y();
      xaxisRot[2] = xaxrot.z();


      Hep3Vector zax( zaxis[0], zaxis[1], zaxis[2] );
      Hep3Vector zaxrot = module->globalRotation() * zax;
      EVector zaxisRot(3,0);
      zaxisRot[0] = zaxrot.x();
      zaxisRot[1] = zaxrot.y();
      zaxisRot[2] = zaxrot.z();


      double xSize =  module->dimensions().x() * scale;
      double ySize =  module->dimensions().y() * scale;


      const dict::Key surf_name = module->fullName() + "_surf";
      Surface* surf = new Rectangle( posZero, zaxisRot, xaxisRot, xSize, ySize );
      _setup.add_surface( module->fullName(), surf_name, surf );
      _setup.set_surface_property( surf_name, "measurement_type", "xy" );
      _surfaces.push_back( surf );
    }
  }
  else if( module->propertyExistsThis( "KalmanSolenoid", "int" ) ) // make surfaces for extrapolation/visualization
  {
    Hep3Vector zax( zaxis[0], zaxis[1], zaxis[2] );
    Hep3Vector zaxrot = module->globalRotation() * zax;
    EVector zaxisRot(3,0);
    zaxisRot[0] = zaxrot.x();
    zaxisRot[1] = zaxrot.y();
    zaxisRot[2] = zaxrot.z();


    if( module->volType() == "Cylinder" )
    {
      Hep3Vector centre = module->globalPosition();
      Hep3Vector dims = module->dimensions();
      double r= dims.x() * scale;
      //double length = dims.y();


      EVector pos(3,0);


      int isurf = 1;


      for( double step = -0.5; step <= +0.5; step += 0.1 )
      {
        // for now, no rotations! This needs to be fixed ASAP


        pos[0] = centre.x();
        pos[1] = centre.y();
        pos[2] = centre.z() + step * dims.y(); // scan along the length of the cylinder


        const dict::Key surf_name = module->fullName() + string_tools::itostring( isurf ) + "_surf";
        Surface* surf = new Ring( pos, zaxisRot, r, 0. );
        _setup.add_surface( module->fullName(), surf_name, surf );
        _setup.set_surface_property( surf_name, "measurement_type", "xy" );
        _surfaces.push_back( surf );
++isurf;
      }
    }
  }
}


std::string findCorrectMotherVolume( MiceModule* mod )
{
  std::string moth = "mother";


  // test this module against those already made, it may be a daughter (or grand-daughter, etc) to one
  // of the other modules, in which case we should use this information


  std::vector<MiceModule*> options;


  for( unsigned int i = 0; i < _kalModules.size(); ++i )
    if( mod->isInside( _kalModules[i] ) )
      options.push_back( _kalModules[i] );


  if( options.size() > 1 )
  {
    // need to work out which of these is the best, we do so by picking the one that is deeper into the tree


    MiceModule* best = NULL;


    for( unsigned int i = 0; i < options.size(); ++i )
      if( ! best || options[i]->depth() > best->depth() )
        best = options[i];


    moth = best->fullName();
  }
  else if( options.size() == 1 )
    moth = options[0]->fullName();


  return moth;
}
