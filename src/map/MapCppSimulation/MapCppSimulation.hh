/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

/** @class MapCppSimulation
 *  Simulate the JSON input and return JSON input
 *
 */

// TODO(Rogers): Error is that we use wrong geant4 devices for this stuff.
//               Really we should use EventAction to control per spill stuff and
//               stick multiple tracks from the spill into the same primary


#ifndef _SRC_MAP_MAPCPPSIMULATION_HH_
#define _SRC_MAP_MAPCPPSIMULATION_HH_

// C headers
#include <stdlib.h>

// C++ headers
#include <string>
#include <sstream>
#include <vector>

// external libraries
#include "json/json.h"

//#include "globals.hh" // temp

//  MAUS code
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

// G4MICE from commonCpp
#include "BeamTools/BTPhaser.hh"
#include "Config/MiceModule.hh"
#include "DetModel/MAUSSD.hh"  //  non-G4MICE addition
#include "Interface/MICEEvent.hh"
#include "Interface/MiceEventManager.hh"
#include "Interface/MiceMaterials.hh"
#include "Interface/MICERun.hh"
#include "Interface/SpecialHit.hh"  // needed by persist
#include "Interface/Squeak.hh"

class MapCppSimulation {
 public:
  /** @brief Sets up the worker
   */
  MapCppSimulation() :_g4manager(NULL) {
    _classname = "MapCppSimulation";
    _geometry = "Stage6.dat";
    _storeTracks = true;
  }

  /** @brief Begin the startup procedure for Simulation
   *
   *  This takes one argument.  This constructs the geometry
   *  and prepares geant4 for being able to run beamOn(1).
   *  This process also builds the fields and can take a
   *  while.  Be sure that you do not run birth() after
   *  death due to Geant4 slopiness.
   *
   *  @param config a JSON document with the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** @brief Shuts down the Simulation by closing files
   *
   *  This takes no arguments
   */
  bool death();

  /** @brief Simulate JSON input and return new document
   *
   *  This function will simulate a single spill defined
   *  in JSON format.
   *
   * @param document a JSON document for a spill
   */
  std::string process(std::string document);

  /** @brief Store tracking information in the particle
   *
   *  stores:
   *    sensitive detector hits registered in the detectors (from 
   *      MICEDetectorConstruction::GetSDHits())
   *    tracks (list of step points) if _storeTracks is set (from
   *      MAUSSteppingAction::GetTrack())
   *  
   *  @param particle Json value where the information is stored
   */
  Json::Value StoreTracking(Json::Value particle);

  /** @brief Store tracking information in the particle
   *
   *  stores:
   *    sensitive detector hits registered in the detectors (from 
   *      MICEDetectorConstruction::GetSDHits())
   *    tracks (list of step points) if _storeTracks is set (from
   *      MAUSSteppingAction::GetTrack())
   *  
   *  @param particle Json value where the information is stored
   */
  void SetNextParticle(Json::Value particle);

  /** @brief Set up configuration information on the MICERun
   *  
   *  Sets the datacards, json configuration, Squeak standard outputs,
   *  and MiceModules
   */
  void SetConfiguration(std::string config);

 private:
  MAUSGeant4Manager* _g4manager;
  std::string _jsonConfigDocument;
  std::string _classname;
  std::string _geometry;
  bool _storeTracks;
};  // Don't forget this trailing colon!!!!



#endif  // _SRC_MAP_MAPCPPSIMULATION_HH_
