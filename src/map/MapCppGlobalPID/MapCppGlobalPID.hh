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
 *  @author Celeste Pidcott, University of Warwick
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
#include <algorithm>

// Other headers
#include "Interface/Squeak.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

#include "DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/PIDLogLPair.hh"

#include "src/common_cpp/Recon/Global/PIDVarA.hh"
#include "src/common_cpp/Recon/Global/PIDVarB.hh"
#include "src/common_cpp/Recon/Global/PIDVarC.hh"
#include "src/common_cpp/Recon/Global/PIDVarD.hh"
#include "src/common_cpp/Recon/Global/PIDVarE.hh"
#include "src/common_cpp/Recon/Global/PIDVarF.hh"
#include "src/common_cpp/Recon/Global/PIDVarG.hh"
#include "src/common_cpp/Recon/Global/PIDVarH.hh"
#include "src/common_cpp/Recon/Global/PIDVarI.hh"
#include "src/common_cpp/Recon/Global/PIDVarJ.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarA.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarB.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarC.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarD.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarE.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarF.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarG.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarH.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarI.hh"


namespace MAUS {

  class MapCppGlobalPID : public MapBase<Data> {
  public:
    /** Constructor, setting the internal variable #_classname */
    MapCppGlobalPID();
    ~MapCppGlobalPID();

  private:
    /** @brief Sets up the worker
     *
     *  @param argJsonConfigDocument a JSON document with
     *         the configuration.
     */
    void _birth(const std::string& argJsonConfigDocument);

    /** @brief Shutdowns the worker
     *
     *  This takes no arguments and does nothing
     */
    void _death();

    /** @brief process MAUS Data
     *
     *  Receive a Spill data object with global tracks, perform PID functons
     *  and return tracks with PID set
     *  @param document a spill data event
     */
    void _process(MAUS::Data* data) const;

    /** @brief perform pid on all US tracks
     *
     *  Perform pid on upstream TrackMatching tracks.
     *  @param 
     */
    void US_PID(std::vector<MAUS::DataStructure::Global::Track*>*
		GlobalTrackArray, MAUS::GlobalEvent* global_event) const;

    /** @brief perform pid on all DS tracks
     *
     *  Perform pid on downstream TrackMatching tracks.
     *  @param 
     */
    void DS_PID(std::vector<MAUS::DataStructure::Global::Track*>*
		GlobalTrackArray, MAUS::GlobalEvent* global_event) const;

    /** @brief perform pid on all through tracks
     *
     *  Perform pid on through TrackMatching tracks.
     *  @param 
     */
    void Through_PID(std::vector<MAUS::DataStructure::Global::Track*>*
		     GlobalTrackArray, MAUS::GlobalEvent* global_event) const;

    /** @brief perform pid on US tracks from a through track
     *
     *  Perform pid on upstream parts of through TrackMatching tracks.
     *  @param 
     */
    void Through_US_PID(std::vector<MAUS::DataStructure::Global::Track*>*
			GlobalTrackArray, MAUS::GlobalEvent* global_event) const;

    /** @brief perform pid on DS tracks from a through track
     *
     *  Perform pid on downstream parts of through TrackMatching tracks.
     *  @param 
     */
    void Through_DS_PID(std::vector<MAUS::DataStructure::Global::Track*>*
			GlobalTrackArray, MAUS::GlobalEvent* global_event) const;

    /** @brief function to perform pid
     *
     *  
     *  @param 
     */
    void perform_pid(std::string output_track_name,
		     std::string function_name,
		     MAUS::DataStructure::Global::Track* track,
		     std::vector<MAUS::recon::global::PIDBase*> _pid_vars,
		     std::string _pid_beamline_polarity,
		     std::string _pid_beam_setting,
		     MAUS::GlobalEvent* global_event) const;

    /** @brief calculate Confidence Level
     *
     *  Calculate the confidence of a track having a given pid from the log likelihoods.
     *  @param LL_x log-likelihood of pid x
     *  @param sum_L sum of likelihoods for all pids
     */
    double ConfidenceLevel(double LL_x, double sum_L) const;

    bool _configCheck;
    /// This will contain the configuration
    Json::Value _configJSON;

    ///  JsonCpp setup
    Json::Reader _reader;

    /// PID variables to perform
    std::vector<MAUS::recon::global::PIDBase*> _pid_vars;

    /// Hypotheses to test PID variables against
    std::vector<std::string> _hypotheses;

    /// File that holds PDFs
    TFile* _histFile;

    /// The current spill
    Spill* _spill;

    /// PID cut limits from datacard
    double _minA;
    double _maxA;
    double _XminB;
    double _XmaxB;
    double _YminB;
    double _YmaxB;
    double _XminC;
    double _XmaxC;
    double _YminC;
    double _YmaxC;
    double _minD;
    double _maxD;
    double _minE;
    double _maxE;
    double _XminF;
    double _XmaxF;
    double _YminF;
    double _YmaxF;
    double _minG;
    double _maxG;
    double _XminH;
    double _XmaxH;
    double _YminH;
    double _YmaxH;
    double _XminI;
    double _XmaxI;
    double _YminI;
    double _YmaxI;
    double _XminJ;
    double _XmaxJ;
    double _YminJ;
    double _YmaxJ;
    double _minComA;
    double _maxComA;
    double _XminComB;
    double _XmaxComB;
    double _YminComB;
    double _YmaxComB;
    double _minComC;
    double _maxComC;
    double _minComD;
    double _maxComD;
    double _XminComE;
    double _XmaxComE;
    double _YminComE;
    double _YmaxComE;
    double _minComF;
    double _maxComF;
    double _XminComG;
    double _XmaxComG;
    double _YminComG;
    double _YmaxComG;
    double _XminComH;
    double _XmaxComH;
    double _YminComH;
    double _YmaxComH;
    double _XminComI;
    double _XmaxComI;
    double _YminComI;
    double _YmaxComI;

    /// MICE configuration for PID
    std::string _pid_config;

    /// PID running mode
    std::string _pid_mode;

    /// list of PID variables to be used when mode is custom
    std::string _custom_pid_set;

    /// PID track selection- set in ConfigurationDefaults to select which
    /// TrackMatching tracks to perform PID on
    std::string _pid_track_selection;

    /// PID beamline polarity- set in ConfigurationDefaults to select 
    /// positive or negative particles
    std::string _pid_beamline_polarity;

    /// PID beam setting- set in ConfigurationDefaults to select 
    /// beam setting, i.e. emittance and momentum
    std::string _pid_beam_setting;

    /// File containing PDFs for use in PID
    std::string PDF_file;

    /// Confidence level
    int _pid_confidence_level;
  }; // Don't forget this trailing colon!!!!
} // ~MAUS

#endif
