/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#ifndef _SRC_MAP_MAPCPPGLOBALRESIDUALS_HH_
#define _SRC_MAP_MAPCPPGLOBALRESIDUALS_HH_

// C++ headers
#include <string>
#include <vector>
#include <map>

// external libraries
#include "TMinuit.h"
#include "json/json.h"

// G4MICE from legacy
#include "Config/MiceModule.hh"
#include "Interface/Squeak.hh"

// MAUS
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"
#include "Recon/Global/Track.hh"
#include "Recon/Global/TrackPoint.hh"

namespace MAUS {

class CovarianceMatrix;

namespace recon {
namespace global {

}  // namespace global
}  // namespace recon

/** @class MapCppGlobalResiduals
 *  Reconstruct tracks at the desired longitudinal spacing using the desired
 *  track fitting method.
 */
class MapCppGlobalResiduals {
 public:
  /** @brief Allocates the global-scope Minuit instance used for minimization.
   */
  MapCppGlobalResiduals();

  /** @brief Destroys the *global* scope Minuit object
   *
   *  This takes no arguments.
   */
  ~MapCppGlobalResiduals();

  /** @brief Begin the startup procedure for TrackReconstructor
   *
   *
   *  @param config a JSON document with the configuration.
   */
  bool birth(std::string json_configuration);

  /** @brief Shuts down the reconstructor
   *
   *  This takes no arguments
   */
  bool death();

  /** @brief Calculate residuals and statistics.
   *
   *  Calculate residuals and related statistics of raw and reconstructed tracks
   *
   * @param document a JSON document for a spill
   */
  std::string process(std::string json_run_data);

 private:
  Json::Value configuration_;
  Json::Value run_data_;
  std::map<MAUS::recon::global::Detector::ID, MAUS::recon::global::Detector>
    detectors_;
  std::vector<MAUS::recon::global::Track> raw_tracks_;
  std::vector<MAUS::recon::global::Track> tracks_;
  std::vector<MAUS::recon::global::TrackPoint> residuals_;

  static const std::string kClassname;
  void GenerateGlobalResiduals();
  int FindMatchingTrack(
    const MAUS::recon::global::TrackPoint& raw_track_point,
    const MAUS::recon::global::Track& track);
/*
  void GenerateStatistics();
*/
};

}  // namespace MAUS

#endif  // _SRC_MAP_MAPCPPGLOBALRESIDUALS_HH_
