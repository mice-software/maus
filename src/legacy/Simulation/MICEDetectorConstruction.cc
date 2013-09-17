// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: MICEDetectorConstruction.cc,v 1.98 2010-09-03 10:07:22 rogers Exp $  $Name:  $
//
// International Muon Ionization Cooling Experiment setup based on
// Neutrino Factory Feasibility Study II SFOFO channel components
//
// Modified May 2006 - Use MiceModules and MiceMaterials
//

#include "MICEDetectorConstruction.hh"
#include "Interface/dataCards.hh"
#include "EngModel/Polycone.hh"
#include "EngModel/MultipoleAperture.hh"
#include "EngModel/MiceModToG4Solid.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "Geant4/G4VSolid.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4Element.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/globals.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4UniformMagField.hh"
#include "Geant4/G4ChordFinder.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4EquationOfMotion.hh"
#include "Geant4/G4EqMagElectricField.hh"
#include "Geant4/G4Mag_UsualEqRhs.hh"
//Electromagnetic steppers:W
#include "Geant4/G4ClassicalRK4.hh"
#include "Geant4/G4SimpleHeum.hh"
#include "Geant4/G4ImplicitEuler.hh"
#include "Geant4/G4SimpleRunge.hh"
#include "Geant4/G4ExplicitEuler.hh"
#include "Geant4/G4CashKarpRKF45.hh"
//Magnetic only steppers:
#include "Geant4/G4HelixImplicitEuler.hh"
#include "Geant4/G4HelixExplicitEuler.hh"
#include "Geant4/G4HelixSimpleRunge.hh"
#include "Geant4/G4HelixHeum.hh"

#include "Geant4/G4PVPlacement.hh"
//#ifdef G4VIS_USE
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4Colour.hh"
//#endif*/

#include <sstream>
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include "Interface/MiceMaterials.hh"
#include "Simulation/FillMaterials.hh"

#include "DetModel/SciFi/SciFiPlane.hh"
#include "DetModel/KL/KLGlue.hh"
#include "DetModel/KL/KLFiber.hh"
#include "DetModel/Ckov/CkovMirror.hh"

#include "DetModel/TOF/TofSD.hh"
#include "DetModel/SciFi/SciFiSD.hh"
#include "DetModel/Ckov/CKOVSD.hh"
#include "DetModel/EMR/EMRSD.hh"
#include "DetModel/KL/KLSD.hh"
#include "DetModel/Virtual/SpecialVirtualSD.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"

MICEDetectorConstruction::MICEDetectorConstruction(MiceModule* model, Json::Value* cards) : _checkVolumes(false)
{
  _event = new MICEEvent();
  _model = model;
  _cards = cards;
 
  _materials = fillMaterials(NULL);
  _checkVolumes = JsonWrapper::GetProperty
        (*_cards, "check_volume_overlaps", JsonWrapper::booleanValue).asBool();
  if (_cards == NULL)
    throw(MAUS::Exception(MAUS::Exception::recoverable,
                 "Failed to acquire datacards",
                 "MiceDetectorConstruction::MiceDetectorConstruction()"));

  if (_model == NULL)
    throw(MAUS::Exception(MAUS::Exception::recoverable,
                 "Failed to acquire MiceModule",
                 "MiceDetectorConstruction::MiceDetectorConstruction()"));
  if (_materials == NULL)
    throw(MAUS::Exception(MAUS::Exception::recoverable,
                 "Failed to acquire MiceMaterials",
                 "MiceDetectorConstruction::MiceDetectorConstruction()"));
}

MICEDetectorConstruction::~MICEDetectorConstruction() {
}

G4VPhysicalVolume* MICEDetectorConstruction::Construct()
{
  G4Box* MICEExpHallBox = new G4Box( _model->name(), _model->dimensions().x(),
                           _model->dimensions().y(), _model->dimensions().z() );
  G4Material* hallMat = _materials->materialByName
                                       ( _model->propertyString( "Material" ) );
  MICEExpHallLog = new G4LogicalVolume
                           ( MICEExpHallBox, hallMat, _model->name(), 0, 0, 0 );

  G4PVPlacement* MICEExpHall = new G4PVPlacement( 0, G4ThreeVector(),
                   _model->name(), MICEExpHallLog, 0, false, 0, _checkVolumes);

  G4VisAttributes* vis = new G4VisAttributes( false );

  MICEExpHallLog->SetVisAttributes( vis );

  setUserLimits( MICEExpHallLog, _model );

  setBTMagneticField( _model );
  // turn on cout if check volumes is active
  bool cout_alive = Squeak::coutIsActive();
  if(_checkVolumes && !cout_alive)
      Squeak::activateCout(true);
  for( int i = 0; i < _model->daughters(); ++i )
    addDaughter( _model->daughter( i ), MICEExpHall );
  // turn cout back to default mode if check volumes is active
  Squeak::activateCout(cout_alive);
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
    // who owns memory allocated to rot? This is making a bug... not defined in
    // G4 docs
    G4RotationMatrix * rot = new G4RotationMatrix(mod->rotation());
    place = new G4PVPlacement(rot, mod->position(), mod->name(), logic, moth, false, 0, _checkVolumes);
    Squeak::errorLevel my_err = Squeak::debug;
    if(mod->mother()->isRoot()) my_err = Squeak::info;
    Squeak::mout(my_err) << "Placing " << mod->name() << " of type " 
                        << mod->volType() << " position: " 
                        << mod->globalPosition() << " rotationVector: " 
                        << mod->globalRotation().getAxis() 
                        << " angle: "  << mod->globalRotation().delta()/degree 
                        << " volume: "
                        << solid->GetCubicVolume()/meter/meter/meter << " m^3 ";
    if(mod->propertyExistsThis("Material", "string"))
        Squeak::mout(my_err)  << " material: "
                             << mod->propertyStringThis("Material") << std::endl;
    else
        Squeak::mout(my_err)  << std::endl;
  }

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

  if( JsonWrapper::GetProperty(*_cards, "everything_special_virtual",
                                         JsonWrapper::booleanValue).asBool() ) {
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
    std::string physics_model = JsonWrapper::GetProperty(*_cards,
                     "physics_processes", JsonWrapper::stringValue).asString();
    if(physics_model == "none" )
      cut = false;
    if( sdName == "TOF" )
    {
      TofSD* tofSD = new TofSD( mod );
      MICESDMan->AddNewDetector( tofSD );
      logic->SetSensitiveDetector( tofSD );
      _SDs.push_back(tofSD);
    }
    else if( sdName == "SciFi" )
    {
      SciFiSD* sciFiSD = new SciFiSD( mod );
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
      EMRSD* emrSD = new EMRSD( mod );
      MICESDMan->AddNewDetector( emrSD );
      logic->SetSensitiveDetector( emrSD );
      _SDs.push_back( emrSD );
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
      _SDs.push_back( specVirtSD );
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

    if(module->propertyExistsThis("G4StepMax", "double")) 
        stepMax  = module->propertyDouble( "G4StepMax" );
    if(module->propertyExistsThis("G4TrackMax", "double")) 
        trackMax = module->propertyDouble( "G4TrackMax" );
    if(module->propertyExistsThis("G4TimeMax", "double")) 
        timeMax  = module->propertyDouble( "G4TimeMax" );
    if(module->propertyExistsThis("G4KinMin", "double")) 
        kinMin   = module->propertyDouble( "G4KinMin" );

    logic->SetUserLimits(  new G4UserLimits( stepMax, trackMax, timeMax, kinMin ) );
}


void MICEDetectorConstruction::setBTMagneticField(MiceModule* rootModule)
{
  _btField = new BTFieldConstructor(rootModule);
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
  std::string stepperType = JsonWrapper::GetProperty(*_cards,
                     "stepping_algorithm", JsonWrapper::stringValue).asString();

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
  std::string mag_only = "stepping_algorithm '"+stepperType+
      "' for magnets only - called when RF was present";


  //Scan through the list of steppers
  if(stepperType == "Classic" || stepperType=="ClassicalRK4")
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
  else throw(MAUS::Exception(MAUS::Exception::recoverable,
                    "stepping_algorithm '"+stepperType+"' not found",
                    "MICEDetectorConstruction::setSteppingAlgorithm()"));

  pChordFinder =  new G4ChordFinder(_miceMagneticField, 0.1*mm, pStepper);
  fieldMgr->SetChordFinder(pChordFinder);
}

//Set G4 Stepping Accuracy parameters
void MICEDetectorConstruction::setSteppingAccuracy()
{
  G4FieldManager* fieldMgr
    = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  G4double deltaOneStep = JsonWrapper::GetProperty(*_cards, "delta_one_step",
                                          JsonWrapper::realValue).asDouble();
  G4double deltaIntersection = JsonWrapper::GetProperty(*_cards,
                      "delta_intersection", JsonWrapper::realValue).asDouble();
  G4double epsilonMin = JsonWrapper::GetProperty(*_cards,
                              "epsilon_min", JsonWrapper::realValue).asDouble();
  G4double epsilonMax = JsonWrapper::GetProperty(*_cards,
                              "epsilon_max", JsonWrapper::realValue).asDouble();
  G4double missDistance = JsonWrapper::GetProperty(*_cards,
                            "miss_distance", JsonWrapper::realValue).asDouble();

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

Json::Value MICEDetectorConstruction::GetSDHits(int i){
  if (i >= 0 and ((size_t) i) < _SDs.size() and _SDs[i]) {
    if (_SDs[i]->isHit()) {
      return _SDs[i]->GetHits();
    }
  }
  Json::Value empty(Json::objectValue);
  return empty;
}

void MICEDetectorConstruction::ClearSDHits(){
  for ( int i = 0; ((size_t) i) < _SDs.size(); i++ ) {
    if (_SDs[i]) _SDs[i]->ClearHits();
  }
}
      
