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

/** @class MapCppSimulation
 *  Simulation out the JSON input and return JSON input
 *
 *  This class is meant to be a debugging and simple example class
 *  to illustrate using JSON from within C++.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPSIMULATION_H_
#define _COMPONENTS_MAP_MAPCPPSIMULATION_H_

// C headers
#include <stdlib.h>
#include <json/json.h>
#include <G4RunManager.hh>
#include <G4SDManager.hh>

#include "globals.hh" // temp

// C++ headers
#include <string>
#include <sstream>
#include <vector>

//  MAUS code
#include "MAUSEventAction.h"
#include "MAUSPrimaryGeneratorAction.h"
#include "MAUSTrackingAction.h"

// G4MICE from commonCpp
#include "BeamTools/BTPhaser.hh"
#include "Config/MiceModule.hh"
#include "DetModel/MAUSSD.h"  //  non-G4MICE addition
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MiceEventManager.hh"
#include "Interface/MiceMaterials.hh"
#include "Interface/MICERun.hh"
#include "Interface/SpecialHit.hh"  // needed by persist
#include "Interface/Squeak.hh"
#include "Interface/ZustandVektor.hh"  // needed by persist
#include "Simulation/FillMaterials.hh"
#include "Simulation/MICEDetectorConstruction.hh"
#include "Simulation/MICEPhysicsList.hh"
#include "Simulation/MICERunAction.hh"
#include "Simulation/MICESteppingAction.hh"

class MapCppSimulation {
public:
  /** Sets up the worker
   */
  MapCppSimulation();
  
  /** Begin the startup procedure for Simulation
   *
   *  This takes one arument.  This constructs the geometry
   *  and prepares geant4 for being able to run beamOn(1).
   *  This process also builds the fields and can take a
   *  while.  Be sure that you do not run birth() after
   *  death due to Geant4 slopiness.
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** Shutdowns the Simulation by closing files
   *
   *  This takes no arguments
   */
  bool death();

  /** Simulate JSON input and return new document
   *
   *  This function will simulate a single spill defined
   *  in JSON format.
   *
   * \param document a JSON document for a spill
   */
  std::string process(std::string document);

  std::string GetGeometryFilename() { return _geometry; }
  void SetGeometryFilename(std::string argGeometry) { _geometry = argGeometry; }

  void DisableStoredTracks() { _storeTracks = false; }
  void EnableStoredTracks() { _storeTracks= true; }

 private:
  std::string _jsonConfigDocument;
  std::string _classname;
  std::string _geometry;
  bool _storeTracks;

  G4RunManager* _runManager;
  MICEPhysicsList* _physList;
  MAUSPrimaryGeneratorAction* _primary;
  MAUSSteppingAction*          _stepAct;
  MAUSEventAction*             _eventAct;
  MAUSTrackingAction*          _trackAct;
  MICEDetectorConstruction* _detector;
};  // Don't forget this trailing colon!!!!

#endif  // _COMPONENTS_MAP_MAPCPPSIMULATION_H_
