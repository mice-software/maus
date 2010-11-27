// @(#) $Id: MICEDetectorConstruction.cc,v 1.98 2010-09-03 10:07:22 rogers Exp $  $Name:  $
//
// International Muon Ionization Cooling Experiment setup based on
// Neutrino Factory Feasibility Study II SFOFO channel components
//
// Yagmur Torun
//
// Modified May 2006 - Use MiceModules and MiceMaterials
//
// Malcolm Ellis

#include "MICEDetectorConstruction.hh"
#include "Interface/dataCards.hh"
#include "Config/CoolingChannelGeom.hh"
#include "EngModel/Q35.hh"
#include "EngModel/Polycone.hh"
#include "EngModel/MultipoleAperture.hh"
#include "EngModel/MiceModToG4Solid.hh"
#include "VirtualPlanes.hh"

#include "G4VSolid.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4ChordFinder.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4UserLimits.hh"
#include "G4EquationOfMotion.hh"
#include "G4EqMagElectricField.hh"
#include "G4Mag_UsualEqRhs.hh"
//Electromagnetic steppers:W
#include "G4ClassicalRK4.hh"
#include "G4SimpleHeum.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4ExplicitEuler.hh"
#include "G4CashKarpRKF45.hh"
//Magnetic only steppers:
#include "G4HelixImplicitEuler.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4HelixHeum.hh"

#include "G4PVPlacement.hh"
//#ifdef G4VIS_USE
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
//#endif*/

#include <sstream>
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "Config/MiceModule.hh"
#include "Interface/MiceMaterials.hh"

#include "DetModel/SciFi/SciFiPlane.hh"
#include "DetModel/KL/KLGlue.hh"
#include "DetModel/KL/KLFiber.hh"
#include "DetModel/EMCal/EmCalKLGlue.hh"
#include "DetModel/EMCal/EmCalKLFiber.hh"
#include "DetModel/Ckov/CkovMirror.hh"

#include "DetModel/TOF/TofSD.hh"
#include "DetModel/SciFi/SciFiSD.hh"
#include "DetModel/Ckov/CKOVSD.hh"
#include "DetModel/EMR/EMRSD.hh"
#include "DetModel/KL/KLSD.hh"
#include "DetModel/Virtual/SpecialVirtualSD.hh"
#include "DetModel/EMCal/EmCalSD.hh"
#include "BeamTools/BTPhaser.hh"

const double MICEDetectorConstruction::_pillBoxSpecialVirtualLength = 1.e-6*mm;

MICEDetectorConstruction::MICEDetectorConstruction( MICERun& run, MICEEvent* event ) : _simRun(*MICERun::getInstance()), _checkVolumes(false)
{
  _event = event;
  _model = run.miceModule;
  _materials = run.miceMaterials;
  _checkVolumes = (_simRun.DataCards->fetchValueInt("CheckVolumeOverlaps") == 1);

  magField = new G4UniformMagField( 0., 0., 0. );
}

MICEDetectorConstruction::~MICEDetectorConstruction()
{
}

G4VPhysicalVolume* MICEDetectorConstruction::Construct()
{
  G4Box* MICEExpHallBox = new G4Box( _model->name(), _model->dimensions().x(), _model->dimensions().y(), _model->dimensions().z() );

  G4Material* hallMat = _materials->materialByName( _model->propertyString( "Material" ) );

  MICEExpHallLog = new G4LogicalVolume( MICEExpHallBox, hallMat, _model->name(), 0, 0, 0 );

  G4PVPlacement* MICEExpHall = new G4PVPlacement( 0, G4ThreeVector(), _model->name(), MICEExpHallLog, 0, false, 0, _checkVolumes);

  G4VisAttributes* vis = new G4VisAttributes( false );

  MICEExpHallLog->SetVisAttributes( vis );

  setUserLimits( MICEExpHallLog, _model );

  G4FieldManager* globalFieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  Hep3Vector field( 0., 0., 0. );

  _hasBTFields = BTFieldConstructor::HasBTFields();

  Squeak::mout(Squeak::info) << "Building fields" << std::endl;
  if(_hasBTFields)
    setBTMagneticField( _model );
  else
  {
    if( _model->propertyExistsThis( "MagneticField", "Hep3Vector" ) )
      field = _model->propertyHep3Vector( "MagneticField" );
    G4UniformMagField* magField = new G4UniformMagField( field );
    globalFieldMgr->SetDetectorField ( magField );
    globalFieldMgr->CreateChordFinder( magField );
  }
  Squeak::mout(Squeak::info) << "Building physical geometry" << std::endl;
  if(_checkVolumes) Squeak::setStandardOutputs(0); //turn on cout if check volumes is active
  for( int i = 0; i < _model->daughters(); ++i )
    addDaughter( _model->daughter( i ), MICEExpHall );
  if(_checkVolumes) Squeak::setStandardOutputs(-1); //turn cout back to default mode if check volumes is active
  VirtualPlaneManager::ConstructVirtualPlanes(_miceElectroMagneticField->GetField(), _model);

  return MICEExpHall;
}

void    MICEDetectorConstruction::addDaughter( MiceModule* mod, G4VPhysicalVolume* moth )
{
  G4LogicalVolume* logic = NULL;
  G4PVPlacement* place = NULL;

  G4Material* mat = _materials->materialByName( mod->propertyString( "Material" ) );

  // if we have to use a special G4 description of the shape of this component

  if( mod->propertyExistsThis( "G4Detector", "string" ) )
  {
    std::string detector = mod->propertyString( "G4Detector" );

    if( detector == "SciFiPlane" )
    {
      SciFiPlane* plane = new SciFiPlane( mod, mat, moth );
      logic = plane->logicalCore();
      place = plane->placementCore();
    }
    else if ( detector == "EmCalKLGlue" )
    {
      EmCalKLGlue* glue = new EmCalKLGlue( mod, mat, moth);
      logic = glue->logicalStrip();
      place = glue->placementStrip();
    }
    else if ( detector == "EmCalKLFiber" )
    {
      EmCalKLFiber* fiber = new EmCalKLFiber( mod, mat, moth);
      logic = fiber->logicalFiber();
      place = fiber->placementFiber();
    }
    else if ( detector == "KLGlue" )
    {
      KLGlue* glue = new KLGlue( mod, mat, moth);
      logic = glue->logicalStrip();
      place = glue->placementStrip();
    }
    else if ( detector == "KLFiber" )
    {
      KLFiber* fiber = new KLFiber( mod, mat, moth);
      logic = fiber->logicalFiber();
      place = fiber->placementFiber();
    }
    else if ( detector == "CkovMirror" )
    {
      CkovMirror* mirror = new CkovMirror( mod, mat, moth);
      logic = mirror->logicalMirror();
      place = mirror->placementMirror();
    }
    else
      std::cerr << "ERROR unknown G4Detector type " << detector << " crashing shortly..." << std::endl;
  }
  else if(mod->volType() == "Quadrupole")
  {
    logic = BuildQ35(mod); //Care about memory management - buildQ35 creates a new LogicalVolume
    place = new G4PVPlacement( (G4RotationMatrix*) mod->rotationPointer(), mod->position(),
                                  mod->name(), logic, moth, false, 0, _checkVolumes);
  }
  else if( mod->volType() == "None" )
  {
    if(mod->daughters() > 0)
      std::cerr << "Module with Volume None cannot have daughters. They will not be placed." << std::endl;
    return;
  }
  else // standard shapes such as box, tube, etc...
  {
    G4VSolid* solid = MiceModToG4Solid::buildSolid(mod);
    logic = new G4LogicalVolume( solid, mat, mod->name() + "Logic", 0, 0, 0 );
    place = new G4PVPlacement( (G4RotationMatrix*) mod->rotationPointer(), mod->position(), mod->name(), logic, moth, false, 0, _checkVolumes);
    Squeak::errorLevel myerr;
    if(mod->mother()->isRoot()) myerr = Squeak::info;
    else              myerr = Squeak::debug;
    Squeak::mout(myerr) << "Placing " << mod->name() << " of type " << mod->volType() << " position: " << mod->globalPosition() << " rotationVector: " << mod->globalRotation().getAxis() 
                        << " angle: "  << mod->globalRotation().delta()/degree << " volume: " << solid->GetCubicVolume()/meter/meter/meter << " m^3" << std::endl;

  }

  //Set a volume in cavity centre for phasing
  SetPhasingVolume(mod, place);

  if(!_hasBTFields)
    setMagneticField( logic, mod );
  setUserLimits( logic, mod ); //CARE!

  bool vis = true;
  if( mod->propertyExistsThis( "Invisible", "bool" ) )
    vis = ! mod->propertyBoolThis( "Invisible" );
  if( vis )
  {
    double red = 0.;
    double green = 0.;
    double blue = 0.;
    if( mod->propertyExistsThis( "RedColour", "double" ) )
      red = mod->propertyDoubleThis( "RedColour" );
    if( mod->propertyExistsThis( "GreenColour", "double" ) )
      green = mod->propertyDoubleThis( "GreenColour" );
    if( mod->propertyExistsThis( "BlueColour", "double" ) )
      blue = mod->propertyDoubleThis( "BlueColour" );
    logic->SetVisAttributes( new G4VisAttributes( G4Color( red, green, blue ) ) );
  }
  else
    logic->SetVisAttributes( new G4VisAttributes( false ) );

  if( _simRun.DataCards->fetchValueString("EverythingSpecialVirtual") == "true" )
  {
    G4SDManager *MICESDMan = G4SDManager::GetSDMpointer();
    SpecialVirtualSD * specVirtSD = new SpecialVirtualSD(_event, mod);
    MICESDMan->AddNewDetector  ( specVirtSD );
    logic->SetSensitiveDetector( specVirtSD );
  }   
  else if( mod->propertyExistsThis( "SensitiveDetector", "string" ) )
  {
    std::string sdName = mod->propertyStringThis( "SensitiveDetector" );
    G4SDManager *MICESDMan = G4SDManager::GetSDMpointer();
    bool cut = true;
    if( _simRun.DataCards->fetchValueString( "EnergyLossModel" ) == "none" )
      cut = false;
    if( sdName == "TOF" )
    {
      TofSD* tofSD = new TofSD( mod, cut );
      MICESDMan->AddNewDetector( tofSD );
      logic->SetSensitiveDetector( tofSD );
      _SDs.push_back(tofSD);
    }
    else if( sdName == "SciFi" )
    {
      SciFiSD* sciFiSD = new SciFiSD( mod, cut );
      MICESDMan->AddNewDetector( sciFiSD );
      logic->SetSensitiveDetector( sciFiSD );
      _SDs.push_back(sciFiSD); 
    }
    else if( sdName == "CKOV" )
    {
      CkovSD* ckovSD = new CkovSD( _event, mod, cut );
      MICESDMan->AddNewDetector( ckovSD );
      logic->SetSensitiveDetector( ckovSD );
    }
    else if( sdName == "EMR" )
    {
      EMRSD* emrSD = new EMRSD( _event, mod );
      MICESDMan->AddNewDetector( emrSD );
      logic->SetSensitiveDetector( emrSD );
    }
    else if( sdName == "KL" )
    {
      KLSD* klSD = new KLSD( _event, mod );
      MICESDMan->AddNewDetector( klSD );
      logic->SetSensitiveDetector( klSD );
    }
    else if( sdName == "SpecialVirtual" )
    {
      SpecialVirtualSD * specVirtSD = new SpecialVirtualSD(_event, mod);
      MICESDMan->AddNewDetector( specVirtSD );
      logic->SetSensitiveDetector( specVirtSD );
    }
    else if ( sdName == "EMCAL" ) 
    {
      EmCalSD* emCalSD = new EmCalSD(_event, mod);
      MICESDMan->AddNewDetector( emCalSD );
      logic->SetSensitiveDetector( emCalSD );
    }
    else if( sdName != "Virtual" && sdName != "Envelope") //note these are special cases
      std::cerr << "Error in volume " << mod->fullName() << " do not have an SD definition for " << sdName << std::  endl;
  }
  for( int i = 0; i < mod->daughters(); ++i )
    addDaughter( mod->daughter( i ), place );
}

void    MICEDetectorConstruction::setUserLimits( G4LogicalVolume* logic, MiceModule* module )
{
  double stepMax = 100.*mm;
  double trackMax = 1.*parsec;
  double timeMax = 1000.*second;
  double kinMin = 0.;

  try{stepMax  = module->propertyDouble( "G4StepMax" ); } catch(Squeal squee) {}
  try{trackMax = module->propertyDouble( "G4TrackMax" );} catch(Squeal squee) {}
  try{timeMax  = module->propertyDouble( "G4TimeMax" ); } catch(Squeal squee) {}
  try{kinMin   = module->propertyDouble( "G4KinMin" );  } catch(Squeal squee) {}

  logic->SetUserLimits(  new G4UserLimits( stepMax, trackMax, timeMax, kinMin ) );
}

void    MICEDetectorConstruction::setMagneticField( G4LogicalVolume* logic, MiceModule* module )
{
  if( ! module->propertyExistsThis( "MagneticField", "Hep3Vector" ) )
    return;

  G4UniformMagField* magField = new G4UniformMagField( module->propertyHep3Vector( "MagneticField" ) );

  G4FieldManager* localFieldMgr = new G4FieldManager( magField );

  logic->SetFieldManager( localFieldMgr, true );
}


void MICEDetectorConstruction::setBTMagneticField(MiceModule* rootModule)
{
  _btField = new BTFieldConstructor(rootModule);
  if(_btField->GetNumberOfFields()==0)
  {
    return;
  }

  _miceMagneticField = new MiceMagneticField(_btField);
  _miceElectroMagneticField = new MiceElectroMagneticField(_btField);

  setSteppingAlgorithm();
  setSteppingAccuracy();

}

//Set G4 Stepping Algorithm for BTFields
//Choose lower order (simpler) steppers for faster processing in rapidly changing fields
void MICEDetectorConstruction::setSteppingAlgorithm()
{
  G4MagIntegratorStepper* pStepper    = NULL;
  G4ChordFinder*         pChordFinder = NULL;
  G4EqMagElectricField*  fEquationE   = NULL;
  G4Mag_UsualEqRhs*      fEquationM   = NULL;
  G4FieldManager*        fieldMgr     = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  std::string stepperType = CoolingChannelGeom::getInstance()->FieldTrackStepper();

  if (!_btField->HasRF())
  { // No rf field, default integration
    fieldMgr->SetDetectorField(_miceMagneticField);
    fEquationM = new G4Mag_UsualEqRhs(_miceMagneticField);
  }
  else
  { // Electrical field are present, used full E.M.
    fieldMgr->SetFieldChangesEnergy(true);
    fieldMgr->SetDetectorField(_miceElectroMagneticField);
    fEquationE = new G4EqMagElectricField(_miceElectroMagneticField);
  }

  //Scan through the list of steppers
  if(stepperType == "Classic" || stepperType=="ClassicalRK4" || pStepper == NULL)
  {
    if(!_btField->HasRF()) pStepper = new G4ClassicalRK4(fEquationM);
    else                   pStepper = new G4ClassicalRK4(fEquationE, 8);
  }
  else if(stepperType == "SimpleHeum")
  {
    if(!_btField->HasRF()) pStepper = new G4SimpleHeum(fEquationM);
    else                   pStepper = new G4SimpleHeum(fEquationE, 8);
  }
  else if(stepperType == "ImplicitEuler")
  {
    if(!_btField->HasRF()) pStepper = new G4ImplicitEuler(fEquationM);
    else                   pStepper = new G4ImplicitEuler(fEquationE, 8);
  }
  else if(stepperType == "SimpleRunge")
  {
    if(!_btField->HasRF()) pStepper = new G4SimpleRunge(fEquationM);
    else                   pStepper = new G4SimpleRunge(fEquationE, 8);
  }
  else if(stepperType == "ExplicitEuler")
  {
    if(!_btField->HasRF()) pStepper = new G4ExplicitEuler(fEquationM);
    else                   pStepper = new G4ExplicitEuler(fEquationE, 8);
  }
  else if(stepperType == "CashKarpRKF45")
  {
    if(!_btField->HasRF()) pStepper = new G4CashKarpRKF45(fEquationM);
    else                   pStepper = new G4CashKarpRKF45(fEquationE, 8);
  }
  else if(stepperType == "HelixImplicitEuler")
  {
    if(!_btField->HasRF()) pStepper = new G4HelixImplicitEuler(fEquationM);
    else std::cerr << "Attempt to load magnet only stepper when RF present" << std::endl;
  }
  else if(stepperType == "HelixHeum")
  {
    if(!_btField->HasRF()) pStepper = new G4HelixHeum(fEquationM);
    else std::cerr << "Attempt to load magnet only stepper when RF present" << std::endl;
  }
  else if(stepperType == "HelixSimpleRunge")
  {
    if(!_btField->HasRF()) pStepper = new G4HelixSimpleRunge(fEquationM);
    else std::cerr << "Attempt to load magnet only stepper when RF present" << std::endl;
  }
  else if(stepperType == "HelixExplicitEuler")
  {
    if(!_btField->HasRF()) pStepper = new G4HelixExplicitEuler(fEquationM);
    else std::cerr << "Attempt to load magnet only stepper when RF present" << std::endl;
  }

  if(pStepper == NULL)
  {
    std::cerr << "Stepper not found - using ClassicalRK4" << std::endl;
    if(!_btField->HasRF()) pStepper = new G4ClassicalRK4(fEquationM);
    else                   pStepper = new G4ClassicalRK4(fEquationE, 8);
  }

  if(!_btField->HasRF())
    pChordFinder =  new G4ChordFinder(_miceMagneticField, 0.1* mm , pStepper);
  else
    pChordFinder =  new G4ChordFinder(_miceMagneticField, 0.1* mm , pStepper);

  fieldMgr->SetChordFinder(pChordFinder);

}


//Set G4 Stepping Accuracy parameters
void MICEDetectorConstruction::setSteppingAccuracy()
{
  G4FieldManager* fieldMgr
    = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  CoolingChannelGeom* ChanGeom    = CoolingChannelGeom::getInstance();

  //notes on what each thing means in dataCards.cc or G4 docs
  G4double deltaOneStep      = ChanGeom->DeltaOneStep();
  G4double deltaIntersection = ChanGeom->DeltaIntersection();
  G4double epsilonMin        = ChanGeom->EpsilonMin();
  G4double epsilonMax        = ChanGeom->EpsilonMax();
  G4double missDistance      = ChanGeom->MissDistance();

  if(deltaOneStep > 0)
    fieldMgr->SetDeltaOneStep( deltaOneStep );
  if(deltaIntersection > 0)
    fieldMgr->SetDeltaIntersection( deltaIntersection );
  if(missDistance > 0)
    fieldMgr->GetChordFinder()->SetDeltaChord( missDistance );
  if(epsilonMin > 0)
    fieldMgr->SetMinimumEpsilonStep( epsilonMin );
  if(epsilonMax > 0)
    fieldMgr->SetMaximumEpsilonStep( epsilonMax );
}

void MICEDetectorConstruction::SetPhasingVolume(MiceModule * cavityModule, G4VPhysicalVolume* cavityVolume)
{
  //if I am an Unphased cavity, set up a sensitive detector in the centre
  //BUG what if someone tries to put something in the cavity centre?
  if (!(   cavityModule->propertyExistsThis("CavityMode", "string")
        && cavityModule->propertyExistsThis("FieldType", "string")   ))
    return;

  if(!(   cavityModule->propertyStringThis("CavityMode") == "Unphased"
       && (cavityModule->propertyStringThis("FieldType")  == "PillBox"
       ||  cavityModule->propertyStringThis("FieldType")  == "RFFieldMap")  ))
    return;


  G4Material* PhaseMat = _materials->materialByName( cavityModule->propertyStringThis( "Material" ) );

  G4VSolid* PhaseSolid = NULL;
  G4String name;
  stringstream namestr;
  stringstream namestr2;

  namestr2 << cavityModule->name();
  namestr2 >> name;
  BTPhaser::AddCavityName(name); //I want this to match the cavity volume name. Not unique.

  namestr << cavityModule->fullName();
  namestr >> name;
  name = name+"DetMiddle";
  BTPhaser::AddCavityDetectorName(name); //I want this to be unique... but that means using fullName

  if( cavityModule->volType() == "Box" )
    PhaseSolid = new G4Box( name, cavityModule->dimensions().x() / 2.,
                         cavityModule->dimensions().y() / 2., _pillBoxSpecialVirtualLength / 2. );
  else if( cavityModule->volType() == "Cylinder" )
    PhaseSolid = new G4Tubs( name, 0., cavityModule->dimensions().x(),
                         _pillBoxSpecialVirtualLength / 2., 0. * deg, 360. * deg );
  else
    std::cerr << "ERROR - Unphased cavity can't have Volume " << cavityModule->volType() << std::endl;

  G4LogicalVolume* PhaseLogic = new G4LogicalVolume( PhaseSolid, PhaseMat, name, 0, 0, 0 );
  G4VisAttributes* visAttInv = new G4VisAttributes(false);
  PhaseLogic->SetVisAttributes(visAttInv);
  G4PVPlacement*   PhasePlace = new G4PVPlacement( (G4RotationMatrix*) cavityModule->rotationPointer(),
                                 G4ThreeVector(0,0,0), name, PhaseLogic,
                                 cavityVolume, false, 0, _checkVolumes);

  G4SDManager *MICESDMan = G4SDManager::GetSDMpointer();
  if(cavityModule->propertyExistsThis("PhasingVolume", "string"))
    if(cavityModule->propertyStringThis("PhasingVolume") == "SpecialVirtual")
    { 
      SpecialVirtualSD * specVirtSD = new SpecialVirtualSD(_event, cavityModule);
      MICESDMan->AddNewDetector( specVirtSD );
      PhaseLogic->SetSensitiveDetector( specVirtSD );
    }

  if(PhasePlace);
}

G4LogicalVolume * MICEDetectorConstruction::BuildQ35(MiceModule * mod)
{
  G4Material * BeamlineMaterial = _materials->materialByName( mod->propertyStringThis( "BeamlineMaterial" ) );
  G4Material * QuadMaterial     = _materials->materialByName( mod->propertyStringThis( "QuadMaterial" ) );
  G4double QuadLength     = mod->propertyDoubleThis("PhysicalLength");
  G4double QuadRadius     = mod->propertyDoubleThis("QuadRadius");
  G4double PoleTipRadius  = mod->propertyDoubleThis("PoleTipRadius");
  G4double CoilRadius     = mod->propertyDoubleThis("CoilRadius");
  G4double CoilHalfwidth  = mod->propertyDoubleThis("CoilHalfWidth");

  Q35 Quad(BeamlineMaterial, QuadLength, QuadRadius, -1., PoleTipRadius, CoilRadius, CoilHalfwidth, QuadMaterial);
  return Quad.buildQ35();
}

std::vector<Json::Value> MICEDetectorConstruction::GetSDHits(int i){
  if (i >= 0 and i < _SDs.size() and _SDs[i]){
    if (_SDs[i]->isHit()) {
      return _SDs[i]->GetHits();
    }
  }
  std::vector<Json::Value> empty;
  return empty;
}
      
