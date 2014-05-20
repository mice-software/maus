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

/** @class MapCppGlobalPID
 *  Perform PID on incoming global tracks.
 *
 */

#ifndef _SRC_MAP_MAPCPPGLOBALPID_H_
#define _SRC_MAP_MAPCPPGLOBALPID_H_
// Python / C API
#include <Python.h>

// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// Other headers
#include "Interface/Squeak.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

#include "DataStructure/Global/Track.hh"

#include "src/common_cpp/Recon/Global/PIDVarA.hh"
#include "src/common_cpp/Recon/Global/PIDVarB.hh"


namespace MAUS {

  class MapCppGlobalPID {
  public:
    /** Constructor, setting the internal variable #_classname */
    MapCppGlobalPID();

    /** @brief Sets up the worker
     *
     *  @param argJsonConfigDocument a JSON document with
     *         the configuration.
     */
    bool birth(std::string argJsonConfigDocument);

    /** @brief Shutdowns the worker
     *
     *  This takes no arguments and does nothing
     */
    bool death();

    /** @brief process JSON document
     *
     *  Receive a document with global tracks, perform PID functons
     *  and return tracks with PID set
     * @param document a line/spill from the JSON input
     */
    std::string process(std::string) const;


  private:
    bool _configCheck;
    /// This will contain the configuration
    Json::Value _configJSON;

    ///  JsonCpp setup
    Json::Reader _reader;

    /// Mapper name, useful for tracking results...
    std::string _classname;

    /// PID variables to perform
    std::vector<MAUS::recon::global::PIDBase*> _pid_vars;

    /// Hypotheses to test PID variables against
    std::vector<std::string> _hypotheses;

    /// File that holds PDFs
    TFile* _histFile;

    /// The current spill
    Spill* _spill;

    /// File containing PDFs for use in PID
    std::string PDF_file;
  }; // Don't forget this trailing colon!!!!
} // ~MAUS

#endif
