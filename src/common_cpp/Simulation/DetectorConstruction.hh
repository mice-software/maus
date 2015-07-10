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

#ifndef _SRC_COMMON_CPP_SIMULATION_DETECTORCONSTRUCTION_HH_
#define _SRC_COMMON_CPP_SIMULATION_DETECTORCONSTRUCTION_HH_

#include <string>
#include <vector>

#ifdef TESTS_CPP_UNIT_SIMULATION_DETECTORCONSTRUCTORTEST_CC
  #include "gtest/gtest_prod.h"
#endif

#include "json/json.h"

#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4UniformMagField.hh"
#include "Geant4/G4RotationMatrix.hh"

#include "DetModel/MAUSSD.hh"

#include "src/legacy/Interface/MiceMaterials.hh"
#include "src/legacy/Config/MiceModule.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/legacy/EngModel/MiceMagneticField.hh"
#include "src/legacy/EngModel/MiceElectroMagneticField.hh"

class G4VPhysicalVolume;
class G4PVPlacement;
class G4LogicalVolume;
class G4MagIntegratorStepper;
class G4VSolid;
class G4ChordFinder;
class G4UserLimits;
class G4VisAttributes;
class G4EquationOfMotion;

class SciFiPlane;
class KLFiber;
class KLGlue;
class CkovMirror;
class MICEEvent;
class TofSD;

namespace MAUS {
namespace Simulation {

/** DetectorConstruction handles parsing the MiceModules into Geant4
 *
 *  DetectorConstruction is responsible for reading the MiceModules and
 *  interpreting the corresponding commands for Geant4 to use
 */
class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  /** Constructor - initialises some variables and accepts a pre-generated volume
   *
   *   @param worldvol - description of the geometry. DetectorConstruction makes a
   *          deepcopy of model (caller owns memory)
   *   @param cards (borrowed reference) - the control variables. Caller still
   *          owns memory allocated to cards
   */
  explicit DetectorConstruction(G4VPhysicalVolume* worldvol, const Json::Value& cards);
  /** Constructor - initialises some variables but does not construct
   *
   *   @param model - description of the geometry. DetectorConstruction makes a
   *          deepcopy of model (caller owns memory)
   *   @param cards (borrowed reference) - the control variables. Caller still
   *          owns memory allocated to cards
   */
  explicit DetectorConstruction(const Json::Value& cards);

  /** Destructor */
  ~DetectorConstruction();

  /** Construct a dummy default geometry
   *
   *  Overrides G4VUserDetectorConstruction and is called by Geant4 when it is
   *  time to reconstruct the geometry. This just generates a dummy geometry -
   *  the real work is done in ResetGeometry() and ResetFields() which needs to
   *  be called by user
   */
  G4VPhysicalVolume* Construct();

  /** Get sensitive detector hits from a particular sensitive detector
   *
   *  @param int i; indexes the detector for which hits are requested
   *
   *  Returns the sensitive detector hits or an empty object i.e. {} if the
   *  detector was not hit.
   */
  Json::Value GetSDHits(size_t i);

  /** Clear all sensitive detector hits
   */
  void ClearSDHits();

  /** Get the size of the SD array (should be number of sensitive detectors)
   */
  int GetSDSize() { return _SDs.size(); }

  /** Get the last element of the SD array added. For the purpose of adding daughters
   * to sensitive detectors
   */
  // std::vector<MAUS::MAUSSD*> GetSDList(){ return _SDs; }

  /** Get the field maps
   *
   *  Access the field maps that are seen by Geant4.
   */
  BTFieldConstructor* GetField() {return _btField;}

  /** Set control variables from datacards
   *
   *  @param cards the json datacards; we use the following cards
   *  "check_volume_overlaps", "stepping_algorithm", "delta_one_step", 
   *  "delta_intersection", "epsilon_min", "epsilon_max", "miss_distance"
   */
  void SetDatacardVariables(const Json::Value& cards);

  /** Set the mice modules
   *
   *  Updates Geant4 geometry and MC field maps according to the new geoemtry
   *
   *  Caller owns memory referenced by mods (makes a deep copy)
   */
  void SetMiceModules(const MiceModule& mods);

  /** Get the MAUS internal list of regions
   *
   *  MAUS needs to keep this internal list so that MAUS defaults can be 
   *  applied to regions that are not listed in the datacards. It would be nice
   *  if G4RegionStore let me access it's list of regions but not possible...
   */
  std::vector<std::string> GetRegions() {return _regions;}
  std::vector<G4UserLimits*> GetUserLimits() {return _userLims;}

  void BuildSensitiveDetector(G4LogicalVolume* logic, MiceModule* module);
  void SetUserLimits(G4LogicalVolume* logic, MiceModule* module);
  void SetVisAttributes(G4LogicalVolume* logic, MiceModule* mod);
  // Add to a G4Region if required
  void AddToRegion(G4LogicalVolume* logic, MiceModule* mod);
  /** Get the world volume pointer.
   *
   *  Allows access to the geometry outside of Geant4.
   */
  G4VPhysicalVolume* GetWorldVolume() const {return _rootPhysicalVolume;}

 private:
  void ResetGeometry();
  void ResetFields();


  std::vector<MAUS::MAUSSD*> _SDs; // G4 owns the memory - this is borrowed

  void BuildG4DetectorVolume(G4PVPlacement** place,
                             G4LogicalVolume** logic,
                             G4VPhysicalVolume* moth,
                             MiceModule* mod);
  void BuildNormalVolume(G4PVPlacement** place,
                             G4LogicalVolume** logic,
                             G4VPhysicalVolume* moth,
                             MiceModule* mod);

  void SetMagneticField(G4LogicalVolume* logic, MiceModule* module);

  void SetBTMagneticField();

  void AddDaughter(MiceModule*, G4VPhysicalVolume*);

  void GeometryCleanup();

  // Set G4 Stepping Accuracy parameters
  void SetSteppingAccuracy();
  // Set G4 Stepping Algorithm
  void SetSteppingAlgorithm();
  // Throw an exception if Volume of all children != None
  void CheckForVolumeInChildren(MiceModule* mod, MiceModule* recurse = NULL);
  // Throw an exception if module is more than _maxModDepth deep
  void CheckModuleDepth(MiceModule* moduel);

  // Build a Q35 using Q35.hh methods
  G4LogicalVolume* BuildQ35(MiceModule * mod);

  MICEEvent* _event;
  MiceModule* _model;
  MiceMaterials* _materials;
  BTFieldConstructor* _btField;
  MiceMagneticField* _miceMagneticField;
  MiceElectroMagneticField* _miceElectroMagneticField;

  // Geant4 sloppy memory usage - I keep pointers to G4 stuff so I can
  // delete it when required; valgrind should not report any leakage...
  G4LogicalVolume* _rootLogicalVolume;
  G4VPhysicalVolume* _rootPhysicalVolume;
  G4MagIntegratorStepper* _stepper;
  G4ChordFinder* _chordFinder;
  std::vector<G4RotationMatrix*> _rotations;
  std::vector<G4VisAttributes*> _visAtts;
  std::vector<G4UserLimits*> _userLims;
  std::vector<SciFiPlane*> _sciFiPlanes;
  std::vector<KLGlue*> _klGlues;
  std::vector<KLFiber*> _klFibers;
  std::vector<CkovMirror*> _ckovMirrors;
  G4VisAttributes* _rootVisAtts;
  G4EquationOfMotion* _equation;

  size_t _maxModDepth;
  std::string _stepperType;
  std::string _physicsProcesses;
  bool _checkVolumes;
  bool _everythingSpecialVirtual;
  bool _polarisedTracking;
  bool _useGDML;
  G4double _deltaOneStep;
  G4double _deltaIntersection;
  G4double _epsilonMin;
  G4double _epsilonMax;
  G4double _missDistance;
  G4double _keThreshold;
  G4double _trackMax;
  G4double _timeMax;
  G4double _stepMax;

  std::vector<std::string> _regions;

  #ifdef TESTS_CPP_UNIT_SIMULATION_DETECTORCONSTRUCTORTEST_CC
    FRIEND_TEST(DetectorConstructionTest, SetDatacardVariablesTest);
    FRIEND_TEST(DetectorConstructionTest, SetSteppingAlgorithmTest);
    FRIEND_TEST(DetectorConstructionTest, SetSteppingAccuracyTest);
  #endif
};
} // Simulation
} // MAUS
#endif  // _SRC_COMMON_CPP_SIMULATION_DETECTORCONSTRUCTION_HH_

