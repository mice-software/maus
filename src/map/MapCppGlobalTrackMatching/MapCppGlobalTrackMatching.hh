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

/** @class MapCppGlobalReconImport
 *  @author Jan Greis, University of Warwick
 *  Import detector events from Recon Event into a Global Event and create
 *  global tracks.
 *  @date 2015/11/26
 */

#ifndef _SRC_MAP_MAPCPPGLOBALTRACKMATCHING_H_
#define _SRC_MAP_MAPCPPGLOBALTRACKMATCHING_H_

// C headers
#include <stdlib.h>

// C++ headers
#include <map>
#include <string>
#include <utility>

// external libraries
#include "json/json.h"

//  MAUS code
#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
  class Data;

  class MapCppGlobalTrackMatching : public MapBase<Data> {
  public:
    /// Constructor
    MapCppGlobalTrackMatching();

  private:
    /** Sets up the worker
     *
     *  @param argJsonConfigDocument a JSON document with
     *         the configuration.
     */
    void _birth(const std::string& argJsonConfigDocument);

    /** Shutdowns the worker
     *
     *  This takes no arguments and does nothing
     */
    void _death();

    /** process MAUS Data
     *
     *  Receive a structure with recon events and return a 
     *  structure with populated global events and tracks.
     * @param MAUS data corresponding to a single MICE spill
     */
    void _process(Data* data) const;

  private:
    /// Check that a valid configuration is passed to the process
    bool _configCheck;
    /// This will contain the configuration
    Json::Value _configJSON;
    /// JsonCpp setup
    Json::Reader _reader;
    /// String denoting what PID hypotheses track matching should run with
    std::string _pid_hypothesis_string;
    int _beamline_polarity;
    /// Matching tolerances for the various detectors that are matched
    std::map<std::string, std::pair<double, double> > _matching_tolerances;
    /// Whether matching should not be performed with only one hit per detector
    /// + thresholds for possible noise hits that will trigger matching either way
    std::pair<bool, std::map<std::string, double> > _no_check_settings;
    /// Should the RK4 include energy loss
    bool _energy_loss;

    bool _through_matching;
    /// Mapper name
    std::string _mapper_name;
  }; // Don't forget this trailing colon!!!!
} // ~MAUS

#endif
