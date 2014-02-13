/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <sstream>
#include <fstream>

// Geant4 bureaucracy
#include "Geant4/globals.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4GeometryManager.hh"
// Geant4 physical model
#include "Geant4/G4VSolid.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4Element.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4PVPlacement.hh"
// fields and transport
#include "Geant4/G4ChordFinder.hh"
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4EquationOfMotion.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4UniformMagField.hh"
#include "Geant4/G4EqMagElectricField.hh"
#include "Geant4/G4Mag_UsualEqRhs.hh"
// Electromagnetic steppers
#include "Geant4/G4ClassicalRK4.hh"
#include "Geant4/G4SimpleHeum.hh"
#include "Geant4/G4ImplicitEuler.hh"
#include "Geant4/G4SimpleRunge.hh"
#include "Geant4/G4ExplicitEuler.hh"
#include "Geant4/G4CashKarpRKF45.hh"
// Magnetic only steppers:
#include "Geant4/G4HelixImplicitEuler.hh"
#include "Geant4/G4HelixExplicitEuler.hh"
#include "Geant4/G4HelixSimpleRunge.hh"
#include "Geant4/G4HelixHeum.hh"
<<<<<<< TREE
#include "Geant4/G4Mag_SpinEqRhs.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4EqEMFieldWithSpin.hh"
//#ifdef G4VIS_USE
=======
// Visualisation
>>>>>>> MERGE-SOURCE
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4Colour.hh"

// legacy
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include "Interface/MiceMaterials.hh"
#include "Simulation/FillMaterials.hh"
#include "Interface/dataCards.hh"
#include "EngModel/Polycone.hh"
#include "EngModel/MultipoleAperture.hh"
#include "EngModel/MiceModToG4Solid.hh"

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

// non-legacy
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/Simulation/DetectorConstruction.hh"

namespace MAUS {
namespace Simulation {
DetectorConstruction::DetectorConstruction(const Json::Value& cards)
  : _model(), _btField(NULL), _miceMagneticField(NULL),
    _miceElectroMagneticField(NULL), _rootLogicalVolume(NULL),
    _rootPhysicalVolume(NULL), _stepper(NULL), _chordFinder(NULL),
    _rootVisAtts(NULL), _equationM(NULL), _equationE(NULL) {
  _event = new MICEEvent();
  SetDatacardVariables(cards);
  SetBTMagneticField();
  _materials = fillMaterials(NULL);
  if (_materials == NULL)
    throw(MAUS::Exception(MAUS::Exception::recoverable,
                 "Failed to acquire MiceMaterials",
<<<<<<< TREE
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
  if(_checkVolumes) Squeak::setStandardOutputs(0);
  for( int i = 0; i < _model->daughters(); ++i )
    addDaughter( _model->daughter( i ), MICEExpHall );
  // turn cout back to default mode if check volumes is active
  if(_checkVolumes) Squeak::setStandardOutputs(-1);
  return MICEExpHall;
=======
                 "DetectorConstruction::DetectorConstruction()"));
}

DetectorConstruction::~DetectorConstruction() {
    if (_miceElectroMagneticField != NULL) {
      delete _miceElectroMagneticField;
    }
    if (_miceMagneticField != NULL) {
      delete _miceMagneticField;
    }
    if (_btField != NULL) {
      delete _btField;
    }
    if (_rootPhysicalVolume != NULL) {
      delete _rootPhysicalVolume;
    }
    if (_rootLogicalVolume != NULL) {
      if (_rootLogicalVolume->GetVoxelHeader() != NULL) {
          delete _rootLogicalVolume->GetVoxelHeader();
          _rootLogicalVolume->SetVoxelHeader(0);
      }
      delete _rootLogicalVolume;
    }
    if (_materials != NULL) {
      delete _materials;
    }
    if (_event != NULL) {
      delete _event;
    }
    if (_model != NULL) {
      delete _model;
    }
    if (_stepper != NULL) {
      delete _stepper;
    }
    if (_chordFinder != NULL) {
      delete _chordFinder;
    }
    GeometryCleanup();
    if (_rootVisAtts != NULL) {
        delete _rootVisAtts;
    }
    if (_equationM != NULL) {
        delete _equationM;
    }
    if (_equationE != NULL) {
        delete _equationE;
    }
}

void DetectorConstruction::SetDatacardVariables(const Json::Value& cards) {
  if (&cards == NULL)
    throw(MAUS::Exception(MAUS::Exception::recoverable,
                 "Failed to acquire datacards",
                 "DetectorConstruction::DetectorConstruction()"));
  _maxModDepth = JsonWrapper::GetProperty
        (cards, "maximum_module_depth", JsonWrapper::intValue).asUInt();

  _checkVolumes = JsonWrapper::GetProperty
        (cards, "check_volume_overlaps", JsonWrapper::booleanValue).asBool();
  _stepperType = JsonWrapper::GetProperty(cards,
                     "stepping_algorithm", JsonWrapper::stringValue).asString();
  _deltaOneStep = JsonWrapper::GetProperty(cards, "delta_one_step",
                                          JsonWrapper::realValue).asDouble();
  _deltaIntersection = JsonWrapper::GetProperty(cards,
                      "delta_intersection", JsonWrapper::realValue).asDouble();
  _epsilonMin = JsonWrapper::GetProperty(cards,
                              "epsilon_min", JsonWrapper::realValue).asDouble();
  _epsilonMax = JsonWrapper::GetProperty(cards,
                              "epsilon_max", JsonWrapper::realValue).asDouble();
  _missDistance = JsonWrapper::GetProperty(cards,
                            "miss_distance", JsonWrapper::realValue).asDouble();
  _everythingSpecialVirtual = JsonWrapper::GetProperty(cards,
                                         "everything_special_virtual",
                                         JsonWrapper::booleanValue).asBool();
  _physicsProcesses = JsonWrapper::GetProperty(cards,
                     "physics_processes", JsonWrapper::stringValue).asString();
  _keThreshold = JsonWrapper::GetProperty(cards, "kinetic_energy_threshold",
                                             JsonWrapper::realValue).asDouble();
  _trackMax = JsonWrapper::GetProperty(cards, "max_track_length",
                                             JsonWrapper::realValue).asDouble();
  _timeMax = JsonWrapper::GetProperty(cards, "max_track_time",
                                             JsonWrapper::realValue).asDouble();
  _stepMax = JsonWrapper::GetProperty(cards, "max_step_length",
                                             JsonWrapper::realValue).asDouble();
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  // Set up the logical volume
  G4Box* rootBox = new G4Box("Default", 1, 1, 1);
  G4Material* rootMat = _materials->materialByName("Galactic");
  _rootLogicalVolume = new G4LogicalVolume(rootBox, rootMat, "Dummy", 0, 0, 0);
  _rootPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(),
                   "DummyPV", _rootLogicalVolume, 0, false, 0, _checkVolumes);
  // we never visualise the root LV
  _rootVisAtts = new G4VisAttributes(false);
  _rootLogicalVolume->SetVisAttributes(_rootVisAtts);
  return _rootPhysicalVolume;
}

void DetectorConstruction::SetMiceModules(const MiceModule& mods) {
    if (_model != NULL)
        delete _model;
    _model = MiceModule::deepCopy(mods, false);
    ResetGeometry();
    ResetFields();
}

void DetectorConstruction::ResetGeometry() {
  // open the geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  // update root PV
  G4Material* rootMat = _materials->materialByName
                                           (_model->propertyString("Material"));
  // we have to remove daughter volumes before changing rootLV dimensions - in
  // case daughter volumes are outside of the rootLV volume (makes a G4Exc)
  while (_rootLogicalVolume->GetNoDaughters() > 0) {
      G4VPhysicalVolume* vol = _rootLogicalVolume->GetDaughter(0);
      if (vol != NULL && _rootLogicalVolume->IsDaughter(vol)) {
          _rootLogicalVolume->RemoveDaughter(vol);
      }
  }

  // now change the rootBox dimensions (and LV name)
  G4Box* rootBox = reinterpret_cast<G4Box*>(_rootLogicalVolume->GetSolid());
  rootBox->SetXHalfLength(_model->propertyHep3Vector("Dimensions").x());
  rootBox->SetYHalfLength(_model->propertyHep3Vector("Dimensions").y());
  rootBox->SetZHalfLength(_model->propertyHep3Vector("Dimensions").z());
  _rootLogicalVolume->UpdateMaterial(rootMat);
  _rootLogicalVolume->SetName(_model->name());
  // update the voxel header whatever this is (some G4 optimisation routine)
  if (_rootLogicalVolume->GetVoxelHeader() != NULL) {
      delete _rootLogicalVolume->GetVoxelHeader();
      _rootLogicalVolume->SetVoxelHeader(0);
  }
  // clear the G4SDManager
  MAUSSD::ResetSDs();
  // clear the sensitive detector list - we will rebuild this in daughter mods
  _SDs = std::vector<MAUS::MAUSSD*>();
  // user limits (e.g. step size)
  SetUserLimits(_rootLogicalVolume, _model);
  // now add new daughters
  bool cout_alive = Squeak::coutIsActive();
  if (_checkVolumes && !cout_alive)
      Squeak::activateCout(true);
  try {
  for (int i = 0; i < _model->daughters(); ++i)
    AddDaughter(_model->daughter(i), _rootPhysicalVolume);
  } catch (Exception exc) {
    Squeak::activateCout(cout_alive);
    throw exc;
  }
  Squeak::activateCout(cout_alive);
  // close the geometry
  G4GeometryManager::GetInstance()->CloseGeometry();
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
  G4RunManager::GetRunManager()->Initialize();  // makes a G4 segv
  // std::cout << "Dumping new geometry" << std::endl;  // need verbose level 0
  // G4RunManager::GetRunManager()->DumpRegion();
>>>>>>> MERGE-SOURCE
}

void DetectorConstruction::AddDaughter
                                    (MiceModule* mod, G4VPhysicalVolume* moth) {
  CheckModuleDepth(mod);
  G4LogicalVolume* logic = NULL;
  G4PVPlacement* place = NULL;
<<<<<<< TREE

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
=======
  if (mod->propertyExistsThis("G4Detector", "string")) {
    BuildG4DetectorVolume(&place, &logic, moth, mod);
  } else if (mod->volType() == "None") {
    CheckForVolumeInChildren(mod);
  } else {
    BuildNormalVolume(&place, &logic, moth, mod);
  }

  if (logic != NULL) {
      SetUserLimits(logic, mod);
      SetVisAttributes(logic, mod);
      BuildSensitiveDetector(logic, mod);
  }

  for (int i = 0; i < mod->daughters(); ++i)
    AddDaughter(mod->daughter(i), place);
}

void DetectorConstruction::BuildG4DetectorVolume(G4PVPlacement** place,
                                                 G4LogicalVolume** logic,
                                                 G4VPhysicalVolume* moth,
                                                 MiceModule* mod) {
    G4Material* mat =
                    _materials->materialByName(mod->propertyString("Material"));
    std::string detector = mod->propertyString("G4Detector");
    if (detector == "SciFiPlane") {
      _sciFiPlanes.push_back(new SciFiPlane(mod, mat, moth));
      *logic = _sciFiPlanes.back()->logicalCore();
      *place = _sciFiPlanes.back()->placementCore();
    } else if (detector == "KLGlue") {
      _klGlues.push_back(new KLGlue(mod, mat, moth));
      *logic = _klGlues.back()->logicalStrip();
      *place = _klGlues.back()->placementStrip();
    } else if (detector == "KLFiber") {
      _klFibers.push_back(new KLFiber(mod, mat, moth));
      *logic = _klFibers.back()->logicalFiber();
      *place = _klFibers.back()->placementFiber();
    } else if (detector == "CkovMirror") {
      _ckovMirrors.push_back(new CkovMirror(mod, mat, moth));
      *logic = _ckovMirrors.back()->logicalMirror();
      *place = _ckovMirrors.back()->placementMirror();
    } else {
      throw Exception(Exception::nonRecoverable,
                      "Unknown G4Detector type "+detector,
                      "DetectorConstruction::AddDaughter");
    }
}

void DetectorConstruction::BuildNormalVolume(G4PVPlacement** place,
                                             G4LogicalVolume** logic,
                                             G4VPhysicalVolume* moth,
                                             MiceModule* mod) {
    G4Material* mat =
                    _materials->materialByName(mod->propertyString("Material"));
>>>>>>> MERGE-SOURCE
    G4VSolid* solid = MiceModToG4Solid::buildSolid(mod);
<<<<<<< TREE
    logic = new G4LogicalVolume( solid, mat, mod->name() + "Logic", 0, 0, 0 );
    place = new G4PVPlacement( (G4RotationMatrix*) mod->rotationPointer(), mod->position(), mod->name(), logic, moth, false, 0, _checkVolumes);
=======
    *logic = new G4LogicalVolume(solid, mat, mod->name() + "Logic", 0, 0, 0);
    // who owns memory allocated to rot? This is making a bug... not defined in
    // G4 docs
    _rotations.push_back(new G4RotationMatrix(mod->rotation()));
    *place = new G4PVPlacement(_rotations.back(), mod->position(), mod->name(),
                               *logic, moth, false, 0, _checkVolumes);
    // for some reason this doesnt take if we are *rebuilding* - try to force it
    if (moth == _rootPhysicalVolume) {
        _rootLogicalVolume->AddDaughter(*place);
    }
>>>>>>> MERGE-SOURCE
    Squeak::errorLevel my_err = Squeak::debug;
<<<<<<< TREE
    if(mod->mother()->isRoot()) my_err = Squeak::info;
    Squeak::mout(my_err) << "Placing " << mod->name() << " of type " 
                        << mod->volType() << " position: " 
                        << mod->globalPosition() << " rotationVector: " 
                        << mod->globalRotation().getAxis() 
                        << " angle: "  << mod->globalRotation().delta()/degree 
                        << " volume: "
                        << solid->GetCubicVolume()/meter/meter/meter << " m^3 "
                        << " density: " << mat->GetDensity()/gram*centimeter*centimeter*centimeter << " g cm^-3 ";
    if(mod->propertyExistsThis("Material", "string"))
        Squeak::mout(my_err)  << " material: "
                             << mod->propertyStringThis("Material") << std::endl;
=======
    if (mod->mother()->isRoot()) my_err = Squeak::info;
    Squeak::mout(my_err) << "Placing " << mod->name() << " of type "
                        << mod->volType() << " position: "
                        << mod->globalPosition() << " mm, rotationVector: "
                        << mod->globalRotation().getAxis()
                        << " angle: "  << mod->globalRotation().delta()/degree
                        << " degrees, volume (incl daughters): "
                        << solid->GetCubicVolume()/meter/meter/meter << " m^3, ";
    if (mod->propertyExistsThis("Material", "string"))
        Squeak::mout(my_err) << " material: "
                            << mod->propertyStringThis("Material")
                            << " mass (excl daughters): "
                            << (*logic)->GetMass(false, false)/kilogram
                            << " kg" << std::endl;
>>>>>>> MERGE-SOURCE
    else
        Squeak::mout(my_err)  << std::endl;
}

void DetectorConstruction::SetVisAttributes
                                     (G4LogicalVolume* logic, MiceModule* mod) {
  bool vis = true;
  if (mod->propertyExistsThis("Invisible", "bool"))
    vis = !mod->propertyBoolThis("Invisible");
  if (vis) {
    double red = 0.;
    double green = 0.;
    double blue = 0.;
    if (mod->propertyExistsThis("RedColour", "double"))
      red = mod->propertyDoubleThis("RedColour");
    if (mod->propertyExistsThis("GreenColour", "double"))
      green = mod->propertyDoubleThis("GreenColour");
    if (mod->propertyExistsThis( "BlueColour", "double"))
      blue = mod->propertyDoubleThis("BlueColour");
    _visAtts.push_back(new G4VisAttributes(G4Color(red, green, blue)));
  } else {
    _visAtts.push_back(new G4VisAttributes(false));
  }
  logic->SetVisAttributes(_visAtts.back());
}

void DetectorConstruction::BuildSensitiveDetector
                                     (G4LogicalVolume* logic, MiceModule* mod) {
    if (!mod->propertyExistsThis("SensitiveDetector", "string") &&
        !_everythingSpecialVirtual)
        return;
    // there is no way to delete sensitive detectors; all we can do is disable
    // old ones and ensure that we use a unique naming convention so we only
    // add detectors once.
    std::string sdName = mod->propertyStringThis("SensitiveDetector");
    if (sdName == "SpecialVirtual" || _everythingSpecialVirtual) {
      SpecialVirtualSD * specVirtSD = new SpecialVirtualSD(_event, mod);
      logic->SetSensitiveDetector(specVirtSD);
      _SDs.push_back(specVirtSD);
    } else if (sdName == "TOF") {
      TofSD* tofSD = new TofSD(mod);
      logic->SetSensitiveDetector(tofSD);
      _SDs.push_back(tofSD);
<<<<<<< TREE
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

//void MICEDetectorConstruction::AllowSpinTracking()//#############################
//{   

  //  if(Enable_Spin_Traking == True){
    //    G4Mag_EqRhs* fEquation = new G4Mag_SpinEqRhs(magField); //my stuff
  
      //  G4MagIntegratorStepper* pStepper = new G4ClassicalRK4(fEquation,12); //my stuff
   // };
    
//}//#################################################################################



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
=======
    } else if (sdName == "SciFi") {
      SciFiSD* sciFiSD = new SciFiSD(mod);
      logic->SetSensitiveDetector(sciFiSD);
      _SDs.push_back(sciFiSD);
    } else if (sdName == "EMR") {
      EMRSD* emrSD = new EMRSD(mod);
      logic->SetSensitiveDetector(emrSD);
      _SDs.push_back(emrSD);
    } else if (sdName == "KL")  {
      // disabled
    } else if (sdName == "CKOV") {
      // disabled
    } else if (sdName != "Virtual" && sdName != "Envelope") {
      // Virtual and Envelope are special cases
      throw(Exception(Exception::recoverable,
            "Sensitive detector type "+sdName+" not recognised in module "+
            mod->fullName(),
            "DetectorConstruction::AddDaughter(...)"));
    }
}

void DetectorConstruction::SetUserLimits
                                  (G4LogicalVolume* logic, MiceModule* module) {
    double stepMax = _stepMax;
    double trackMax = _trackMax;
    double timeMax = _timeMax;
    double kinMin = _keThreshold;

    if (module->propertyExistsThis("G4StepMax", "double"))
        stepMax  = module->propertyDouble("G4StepMax");
    if (module->propertyExistsThis("G4TrackMax", "double"))
        trackMax = module->propertyDouble("G4TrackMax");
    if (module->propertyExistsThis("G4TimeMax", "double"))
        timeMax  = module->propertyDouble("G4TimeMax");
    if (module->propertyExistsThis("G4KinMin", "double"))
        kinMin   = module->propertyDouble("G4KinMin");
    _userLims.push_back(new G4UserLimits(stepMax, trackMax, timeMax, kinMin));
    logic->SetUserLimits(_userLims.back());
}


void DetectorConstruction::SetBTMagneticField() {
  _btField = new BTFieldConstructor();
>>>>>>> MERGE-SOURCE
  _miceMagneticField = new MiceMagneticField(_btField);
  _miceElectroMagneticField = new MiceElectroMagneticField(_btField);
}

// Set G4 Stepping Algorithm for BTFields
// Choose lower order (simpler) steppers for faster processing in rapidly
// changing fields
void DetectorConstruction::SetSteppingAlgorithm() {
  G4FieldManager*        fieldMgr     =
         G4TransportationManager::GetTransportationManager()->GetFieldManager();

  if (!_btField->HasRF()) { // No rf field, default integration
    fieldMgr->SetDetectorField(_miceMagneticField);
    if (_equationM != NULL)
      delete _equationM;
    _equationM = new G4Mag_UsualEqRhs(_miceMagneticField);
  } else { // Electrical field are present, used full E.M.
    fieldMgr->SetFieldChangesEnergy(true);
    fieldMgr->SetDetectorField(_miceElectroMagneticField);
<<<<<<< TREE
    fEquationE = new G4EqMagElectricField(_miceElectroMagneticField);
  }

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
  else if(stepperType=="SpinTrack")//#################################################
  {
    G4EqEMFieldWithSpin* fEquation = new G4EqEMFieldWithSpin(_miceMagneticField); //my stuff

    pStepper = new G4ClassicalRK4(fEquation,12); //my stuff
  }//#################################################################################
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
/*void DetectorConstruction::SetSteppingAccuracy() {
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
  if (_deltaOneStep > 0)
    fieldMgr->SetDeltaOneStep(_deltaOneStep);
  if (_deltaIntersection > 0)
    fieldMgr->SetDeltaIntersection(_deltaIntersection);
  if (_missDistance > 0)
    fieldMgr->GetChordFinder()->SetDeltaChord(_missDistance);
  if (_epsilonMin > 0)
    fieldMgr->SetMinimumEpsilonStep(_epsilonMin);
  if (_epsilonMax > 0)
    fieldMgr->SetMaximumEpsilonStep(_epsilonMax);
}

Json::Value DetectorConstruction::GetSDHits(size_t i) {
  if (i >= _SDs.size()) {
    throw Exception(Exception::recoverable,
                    "Attempt to get SD for out-of-range detector",
                    "DetectorConstruction::GetSDHits(...)");
  }
  if (_SDs[i] and _SDs[i]->isHit()) {
    return _SDs[i]->GetHits();
  }
  Json::Value empty(Json::objectValue);
  return empty;
}*/
=======
    if (_equationE != NULL)
      delete _equationE;
    _equationE = new G4EqMagElectricField(_miceElectroMagneticField);
  }

  if (_chordFinder != NULL) {
    delete _chordFinder;  // owns _stepper memory
    _chordFinder = NULL;
  }
  if (_stepper != NULL) {
    delete _stepper;
    _stepper = NULL;
  }

  // Scan through the list of steppers
  if (_stepperType == "Classic" || _stepperType == "ClassicalRK4") {
    if (!_btField->HasRF()) _stepper = new G4ClassicalRK4(_equationM);
    else                    _stepper = new G4ClassicalRK4(_equationE, 8);
  } else if (_stepperType == "SimpleHeum") {
    if (!_btField->HasRF()) _stepper = new G4SimpleHeum(_equationM);
    else                    _stepper = new G4SimpleHeum(_equationE, 8);
  } else if (_stepperType == "ImplicitEuler") {
    if (!_btField->HasRF()) _stepper = new G4ImplicitEuler(_equationM);
    else                    _stepper = new G4ImplicitEuler(_equationE, 8);
  } else if (_stepperType == "SimpleRunge") {
    if (!_btField->HasRF()) _stepper = new G4SimpleRunge(_equationM);
    else                    _stepper = new G4SimpleRunge(_equationE, 8);
  } else if (_stepperType == "ExplicitEuler") {
    if (!_btField->HasRF()) _stepper = new G4ExplicitEuler(_equationM);
    else                    _stepper = new G4ExplicitEuler(_equationE, 8);
  } else if (_stepperType == "CashKarpRKF45") {
    if (!_btField->HasRF()) _stepper = new G4CashKarpRKF45(_equationM);
    else                    _stepper = new G4CashKarpRKF45(_equationE, 8);
  } else {
    throw(MAUS::Exception(MAUS::Exception::recoverable,
                "stepping_algorithm '"+_stepperType+"' not found",
                "DetectorConstruction::SetSteppingAlgorithm()"));
  }
  _chordFinder =  new G4ChordFinder(_miceMagneticField, 0.1*mm, _stepper);
  fieldMgr->SetChordFinder(_chordFinder);
}
>>>>>>> MERGE-SOURCE

// Set G4 Stepping Accuracy parameters
void DetectorConstruction::SetSteppingAccuracy() {
  G4FieldManager* fieldMgr
       = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  if (_deltaOneStep > 0)
    fieldMgr->SetDeltaOneStep(_deltaOneStep);
  if (_deltaIntersection > 0)
    fieldMgr->SetDeltaIntersection(_deltaIntersection);
  if (_missDistance > 0)
    fieldMgr->GetChordFinder()->SetDeltaChord(_missDistance);
  if (_epsilonMin > 0)
    fieldMgr->SetMinimumEpsilonStep(_epsilonMin);
  if (_epsilonMax > 0)
    fieldMgr->SetMaximumEpsilonStep(_epsilonMax);
}

Json::Value DetectorConstruction::GetSDHits(size_t i) {
  if (i >= _SDs.size()) {
    throw Exception(Exception::recoverable,
                    "Attempt to get SD for out-of-range detector",
                    "DetectorConstruction::GetSDHits(...)");
  }
  if (_SDs[i] and _SDs[i]->isHit()) {
    return _SDs[i]->GetHits();
  }
  Json::Value empty(Json::objectValue);
  return empty;
}

void DetectorConstruction::ClearSDHits() {
  for ( size_t i = 0; i < _SDs.size(); ++i ) {
    if (_SDs[i]) _SDs[i]->ClearHits();
  }
}

void DetectorConstruction::ResetFields() {
    typedef std::vector<BTField*>::iterator field_iter;
    // we don't just delete and new as this would kill existing pointers to the
    // field map (e.g. held by G4 stuff).
    BTFieldGroup* mfield =
       reinterpret_cast<BTFieldGroup*>(_btField->GetMagneticField());
    BTFieldGroup* emfield =
       reinterpret_cast<BTFieldGroup*>(_btField->GetElectroMagneticField());
    // clear fields
    std::vector<BTField*> field_v = mfield->GetFields();
    for (field_iter it = field_v.begin(); it != field_v.end(); it++)
        mfield->Erase((*it), false);
    field_v = emfield->GetFields();
    for (field_iter it = field_v.begin(); it != field_v.end(); it++)
        if (*it != mfield)
            emfield->Erase((*it), false);
    // redo some initialisation stuff
    mfield->Close();
    emfield->Close();
    // now rebuild the fields
    _btField->BuildFields(_model);
    SetSteppingAlgorithm();
    SetSteppingAccuracy();
}

void DetectorConstruction::CheckForVolumeInChildren
                                  (MiceModule* mod, MiceModule* recurse) {
    if (recurse == NULL)
        recurse = mod;
    else
        CheckModuleDepth(recurse);
    if (recurse->volType() != "None") {
        throw(Exception(Exception::recoverable,
          "MiceModule "+mod->name()+" with Volume None has child module "+
          recurse->name()+" with Volume "+recurse->volType()+
          " - but should be None",
          "DetectorConstruction::CheckForVolumeInChildren"));
    } else {
        for (int i = 0; i < recurse->daughters(); ++i) {
            CheckForVolumeInChildren(mod, recurse->daughter(i));
        }
    }
}

void DetectorConstruction::GeometryCleanup() {
  for (size_t i = 0; i < _rotations.size(); ++i) {
      delete _rotations[i];
  }
  _rotations = std::vector<G4RotationMatrix*>();
  for (size_t i = 0; i < _visAtts.size(); ++i) {
      delete _visAtts[i];
  }
  _visAtts = std::vector<G4VisAttributes*>();
  for (size_t i = 0; i < _userLims.size(); ++i) {
      delete _userLims[i];
  }
  _userLims = std::vector<G4UserLimits*>();
  /* Note that we never delete memory allocated to G4Detectors right now
  for (size_t i = 0; i < _sciFiPlanes.size(); ++i) {
      delete _sciFiPlanes[i];
  }
  _sciFiPlanes = std::vector<SciFiPlane*>();
  for (size_t i = 0; i < _klGlues.size(); ++i) {
      delete _klGlues[i];
  }
  _klGlues = std::vector<KLGlue*>();
  for (size_t i = 0; i < _klFibers.size(); ++i) {
      delete _klFibers[i];
  }
  _klFibers = std::vector<KLFiber*>();
  for (size_t i = 0; i < _ckovMirrors.size(); ++i) {
      delete _ckovMirrors[i];
  }
  _ckovMirrors = std::vector<CkovMirror*>();
  */
}

void DetectorConstruction::CheckModuleDepth(MiceModule* module) {
  MiceModule* mother = module->mother();
  size_t recursionDepth = 1;
  while (mother != NULL) {
    mother = mother->mother();
    ++recursionDepth;
  }
  if (recursionDepth > _maxModDepth) {
      throw(Exception(Exception::recoverable,
        "MiceModule "+module->fullName()+" is more than "+
        STLUtils::ToString(_maxModDepth)+" levels deep",
        "DetectorConstruction::CheckModuleDepth"));
  }
}
}
}
