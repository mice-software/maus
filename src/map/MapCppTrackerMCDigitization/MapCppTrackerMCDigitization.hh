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

/** @class MapCppTrackerMCDigitization
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPTRACKERMCDIGITIZATION_H_
#define _COMPONENTS_MAP_MAPCPPTRACKERMCDIGITIZATION_H_
// C headers
#include <assert.h>
#include <json/json.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <cmath>
#include <vector>
#include <string>

#include "Config/MiceModule.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Recon/SciFiHit.hh"
#include "src/common_cpp/Recon/TrackerSpill.hh"

class MapCppTrackerMCDigitization {
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
  bool check_sanity_mc(Json::Value mc);

  void json_to_cpp(Json::Value json_event, TrackerSpill &spill);

  void construct_digits(TrackerEvent &event);

  double compute_npe(double edep);

  int compute_chan_no(SciFiHit *ahit);

  int compute_tdc_counts(double time);

  int compute_adc_counts(double numb_pe);

  bool check_param(SciFiHit *hit1, SciFiHit *hit2);

  void save_to_json(TrackerEvent &evt);

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

  double SciFiNPECut;

  /// an array contaning all MiceModules
  std::vector<const MiceModule*> modules;
};  // Don't forget this trailing colon!!!!

#endif
