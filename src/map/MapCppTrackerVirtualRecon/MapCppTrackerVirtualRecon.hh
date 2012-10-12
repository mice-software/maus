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

/** @class MapCppTrackerRecon 
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPTrackerVirtualRecon_H_
#define _SRC_MAP_MAPCPPTrackerVirtualRecon_H_

// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Units/PhysicalConstants.h>

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

#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpill.hh"
#include "src/common_cpp/Recon/SciFi/RealDataDigitization.hh"
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
// #include "src/common_cpp/Recon/SciFi/SeedFinder.hh"

namespace MAUS {

  class MapCppTrackerVirtualRecon {
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
     * \param document a line/spill from the JSON input
     */
    std::string process(std::string document);

    /** fills digits from MC digitization
     *
     *  \param digits the MC digits
     *  \param a_spill the SciFiSpill we are processing
     */
    void fill_digits_vector(Json::Value &digits, MAUS::SciFiSpill &a_spill);

    /** performs the cluster reconstruction
     *
     *  \param evt the current SciFiEvent
     */
    void fill_virtualhits_vector(Json::Value &v_hits, MAUS::SciFiSpill &spill);
    void cluster_recon(MAUS::SciFiEvent &evt);

    /** performs the spacepoint reconstruction
     *
     *  \param evt the current SciFiEvent
     */
    void spacepoint_recon(MAUS::SciFiEvent &evt);

    void pattern_recognition(const bool helical_pr_on, const bool straight_pr_on,
                             MAUS::SciFiEvent &evt);

    // void make_seed_and_fit(MAUS::SciFiEvent &event);

    void track_fit(MAUS::SciFiEvent &evt);

    void save_to_json_recon(MAUS::SciFiEvent &evt, int event_i);

    void print_event_info_recon(MAUS::SciFiEvent &event);


    void fit(std::vector<MAUS::SciFiSpacePoint*> spacepoints,
	     double &x_const, double &x_slope,
	     double &y_const, double &y_slope);

    void save_to_json_mc(MAUS::SciFiEvent &evt, int event_i); // NEEDS TO BE WRITTEN
    void print_event_info_mc(MAUS::SciFiEvent &event); // written
    // Json::Value ConvertToJson(std::string jsonString); // written
    // std::string JsonToString(Json::Value json_in); // writen
    void find_tracker_station(MAUS::SciFiSpacePoint& spt, double z_val);
  private:
    /// This should be the classname
    std::string _classname;
    /// This will contain the configuration
    Json::Value _configJSON;
    /// This will contain the root value after parsing
    Json::Value root;
    ///  JsonCpp setup
    Json::Reader reader;
    ///  Cut value for npe.
    double minPE;
    /// Value above which reconstruction is aborted.
    int ClustException;
    /// Pattern recognition flags
    bool _helical_pr_on;
    bool _straight_pr_on;
    ///  Vector with the MICE SciFi Modules.
    std::vector<const MiceModule*> modules;

    int SciFiRunRecon;
    int num_event_spill2;
  }; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
