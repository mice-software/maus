/** @class MapCppTOFReconstruction
 *  Reconstruct events by running over EMR real data.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPEMEMONITOR_H_
#define _COMPONENTS_MAP_MAPCPPEMEMONITOR_H_
// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>  //  from old file?
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class MapCppEMRMonitor{
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

  /// This should be the classname
  std::string _classname;

/** The TOF channel map object.
* It is used to associate the DAQ notations of the channels
* with the TOF notations of the PMTs.*/
//  TOFChannelMap map;

  //  JsonCpp setup
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  Json::FastWriter writer;

};  // Don't forget this trailing colon!!!!

#endif  //  _COMPONENTS_MAP_MAPCPPEMEMONITOR_H_

