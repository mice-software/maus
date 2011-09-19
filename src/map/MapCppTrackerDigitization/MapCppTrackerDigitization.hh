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

/** @class MapCppTrackerDigitization 
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPTRACKERDIGITIZATION_H_
#define _COMPONENTS_MAP_MAPCPPTRACKERDIGITIZATION_H_
// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// from old file
#include "Config/MiceModule.hh"
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

class MapCppTrackerDigitization
{
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

  /** sanity check
   *
   *  Checks the sanity of the JSON input,
   *  in particular, of the MC branch
   */
  bool check_sanity_mc(std::string document);

  /** computes tdc counts
   *
   *  Computes tdc counts for a particular hit
   */ 
  int getTDCcounts(Json::Value ahit);

  /** computes npe from a hit
   *
   *  Takes the energy deposition of a hit
   *  and calculates the corresponding 
   *  number of photoelectrons
   */
  double getNPE(double edep);

  /** computes the channel where the hit occured
   *
   *  the ChanNo is computed from the fiber number
   *  of the hit
   */
  int getChanNo(Json::Value ahit);

  /** a list of temporary "digits"
   *
   *  this is a vector where we store the
   *  hypotetical digit each hit would create.
   *  Because we usually have more than one hit
   *  in the same channel in a particular event,
   *  we will have to iterate over this list
   *  in order to make the bundle of 7 fibers and
   *  construct the corresponding digit.
   */
  std::vector<Json::Value> make_all_digits(Json::Value hits);

  /** makes the bundle of 7 fibers
   *
   *  after all energy deposition in one particular channel
   *  is summed, the adc count is computed. 
   */
  Json::Value make_bundle(std::vector<Json::Value> _alldigits);

  /** computes the adc count 
   */
  int compute_adcCounts(double nPE);

  /** tests if hits belong to the same channel
   */
  bool Check_param(Json::Value* hit1, Json::Value* hit2);

  /** a wrapper function that converts a string to a Json Value
   * Introduced for testing purposes.
  */
  Json::Value ConvertToJson(std::string jsonString);

  /** a wrapper function that converts a Json value to a string
   * Introduced for testing purposes.
  */
  std::string JsonToString(Json::Value json_in);

 private:
  /// This is the Mice Module
  MiceModule*      _module;
  /// This should be the classname
  std::string _classname;
  /// This will contain the configuration
  Json::Value _configJSON;
  /// This will contain the root value after parsing
  Json::Value root;
  ///  JsonCpp setup
  Json::Reader reader;
  /// mc branch of the JSON file
  Json::Value mc;
  /// a digit to be appended to the JSON file
  Json::Value digit;
  /// an array contaning all MiceModules
  std::vector<const MiceModule*> modules;
};  // Don't forget this trailing colon!!!!

#endif  //  _COMPONENTS_MAP_MAPCPPTRACKERDIGITIZATION_H_
