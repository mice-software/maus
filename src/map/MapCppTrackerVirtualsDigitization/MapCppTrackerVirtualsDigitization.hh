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

/** @class MapCppTrackerVirtualsDigitization
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPTRACKERVIRTUALSDIGITIZATION_H_
#define _COMPONENTS_MAP_MAPCPPTRACKERVIRTUALSDIGITIZATION_H_

// C headers
#include <json/json.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <cmath>
#include <vector>
#include <string>

#include "Config/MiceModule.hh"
#include "Utils/Squeak.hh"
#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/API/MapBase.hh"

#include "src/common_cpp/Recon/Kalman/MAUSSciFiMeasurements.hh"

namespace MAUS {

  class MapCppTrackerVirtualsDigitization : public MapBase<Data> {
   public:
    /** Constructor - initialises pointers to NULL */
    MapCppTrackerVirtualsDigitization();

    /** Constructor - deletes any allocated memory */
    ~MapCppTrackerVirtualsDigitization();

   private:
    /** Sets up the worker
     *
     *  \param argJsonConfigDocument a JSON document with
     *         the configuration.
     */
    void _birth(const std::string& argJsonConfigDocument);

    /** @brief Shutdowns the worker
     *
     *  This takes no arguments and does nothing
     */
    void _death();

    /** @brief process MAUS Data object
     *
     *  Receive a object with MC hits and return
     *  a object with digits
     */
    void _process(MAUS::Data* data) const;

    /** @brief Simulate data for straight tracks
     */
    void _process_straight(MAUS::SciFiEvent* event, int spill_num, int event_num) const;

    /** @brief Simulate data for helical tracks
     */
    void _process_helix(MAUS::SciFiEvent* event, int spill_num, int event_num) const;

    /** @brief builds digits
     */
    void construct_virtual_digits(MAUS::VirtualHitArray* hits,
                          int spill_num,
                          int event_num,
                          MAUS::SciFiDigitPArray& digits) const;

    /** @brief builds clusters
     */
    void construct_virtual_clusters(MAUS::VirtualHitArray* hits,
                            int spill_num,
                            int event_num,
                            MAUS::SciFiClusterPArray& clusters) const;

    /** @brief builds spacepoints
     */
    void construct_virtual_spacepoints(MAUS::VirtualHitArray* hits,
                               int spill_num,
                               int event_num,
                               MAUS::SciFiSpacePointPArray& spoints) const;


    /** @brief computes alpha from geomeery and virtual hit
     */
//    int _compute_channel_number(VirtualHit* ahit, SciFiPlaneGeometry, int, int, int ) const;
//    double _compute_alpha(VirtualHit* vhit, SciFiPlaneGeometry& geom, int tracker_num ) const;
    double _compute_alpha(ThreeVector& pos, SciFiPlaneGeometry& geom) const;

    // Store Geometry Information
    SciFiGeometryHelper _geometry_helper;
    SciFiTrackerMap _geometry_map;

    /// The ratio of deposited eV to NPE
    double _default_npe;
    /// Std dev of simulated measurement error.
    double _smear_value;

    /// Tolerance on assigning virtual planes to tracker planes
    double _assignment_tolerance;

    /// Type of testing to perform
    int _test_method;

    double _straight_rms_pos;
    double _straight_rms_ang;

    double _helix_rms_pos;
    double _helix_rms_pt;
    double _helix_pz;

    bool _make_digits;
    bool _make_clusters;
    bool _make_perfect_clusters;
    bool _make_spacepoints;
    bool _make_perfect_spacepoints;

    int _spacepoint_plane_num;

    SciFiHelicalMeasurements* _helix_measure;
  };

} // ~namespace MAUS

#endif
