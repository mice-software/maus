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


/** @class MapCppTOFReconstruction
 *  Reconstruct events by running over TOF real data.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPTOFRECON_H_
#define _COMPONENTS_MAP_MAPCPPTOFRECON_H_
// C headers
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>  //  from old file?
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// G4MICE from commonCpp
#include "Config/MiceModule.hh"  //  from old file?


#include "Utils/TOFChannelMap.hh"
#include "Utils/DAQChannelMap.hh"
using std::string;

class MapCppTOFReconstruction {
 public:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(string argJsonConfigDocument);

  /** Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool death();

  /** process JSON document
   *
   *  Receive a document with MC hits and return
   *  a document with digits
   */
  string process(string document);

 private:

  bool level1_recon(Json::Value xDocDetectorData, Json::Value &xDocLevel1Recon);
	bool level2_recon(Json::Value xDocLevel1Recon, Json::Value &xDocLevel2Recon);
	bool level3_recon(Json::Value xDocLevel2Recon, Json::Value &xDocLevel3Recon);
	bool level4_recon(Json::Value xDocLevel3Recon, Json::Value &xDocLevel4Recon);

  /// This should be the classname
  string _classname;

/** The TOF channel map object.
* It is used to associate the DAQ notations of the channels
* with the TOF notations of the PMTs.*/
  TOFChannelMap map;


  MiceModule* _module;
};  // Don't forget this trailing colon!!!!

#endif  //  _COMPONENTS_MAP_MAPCPPTOFRECON_H_
