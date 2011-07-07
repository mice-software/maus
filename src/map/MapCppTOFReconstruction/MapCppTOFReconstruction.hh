/** @class MapCppTOFReconstruction
 *  Reconstruct events by running over TOF real data.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPTOFRECON_H_
#define _COMPONENTS_MAP_MAPCPPTOFRECON_H_
// C headers
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

using namespace std;

class MapCppTOFReconstruction {
 public:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

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
  std::string process(std::string document);

 private:

  bool level1_recon(Json::Value xDocDetectorData, Json::Value &xDocLevel1Recon);
	bool level2_recon(Json::Value xDocLevel1Recon, Json::Value &xDocLevel2Recon);
	bool level3_recon(Json::Value xDocLevel2Recon, Json::Value &xDocLevel3Recon);
	bool level4_recon(Json::Value xDocLevel3Recon, Json::Value &xDocLevel4Recon);

  bool get_next_ParticleEvent();

  /// This should be the classname
  std::string _classname;

/** The TOF channel map object.
* It is used to associate the DAQ notations of the channels
* with the TOF notations of the PMTs.*/
  TOFChannelMap map;

  //  JsonCpp setup
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  Json::FastWriter writer;

  MiceModule* _module;
};  // Don't forget this trailing colon!!!!

#endif  //  _COMPONENTS_MAP_MAPCPPTOFRECON_H_
