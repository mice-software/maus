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

#ifndef _SRC_MAP_MAPCPPTRACKRECONSTRUCTOR_HH_
#define _SRC_MAP_MAPCPPTRACKRECONSTRUCTOR_HH_

// C++ headers
#include <string>
#include <vector>

// external libraries
#include "TMinuit.h"

// G4MICE from legacy
#include "BeamTools/BTField.hh"
#include "Config/MiceModule.hh"
#include "Interface/Squeak.hh"

// MAUS
#include "Reconstruction/Track.hh"
#include "Reconstruction/TrackPoint.hh"

namespace MAUS {

class OpticsModel;
class TrackFitter;
class ReconstructionInput;

/** @class MapCppTrackReconstructor
 *  Reconstruct tracks at the desired longitudinal spacing using the desired
 *  track fitting method.
 */
class MapCppTrackReconstructor {
 public:
  /** @brief Allocates the global-scope Minuit instance used for minimization.
   */
  MapCppTrackReconstructor();

  /** @brief Destroys the *global* scope Minuit object
   *
   *  This takes no arguments.
   */
  ~MapCppTrackReconstructor();

  /** @brief Begin the startup procedure for TrackReconstructor
   *
   *
   *  @param config a JSON document with the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** @brief Shuts down the reconstructor
   *
   *  This takes no arguments
   */
  bool death();

  /** @brief Generate a list of reconstructed tracks.
   *
   *  This function takes a single spill and generates a list of tracks based
   *  on the desired method of reconstruction.
   *
   * @param document a JSON document for a spill
   */
  std::string process(std::string document);

 private:
  Json::Value configuration_;
  OpticsModel * optics_model_;
  TrackFitter * track_fitter_;

  Json::Value run_data_;
  ReconstructionInput * reconstruction_input_;
  std::vector<Track> reconstructed_tracks_;
  
  static const std::string classname_;
  BTField * electromagnetic_field_;

  void SetupOpticsModel();
  void SetupTrackFitter();
  void LoadTestingData();
  void LoadSimulationData();
  void LoadLiveData();
  void CorrelateTrackPoints(std::vector<Track> & tracks);

  static Json::Value TrackToJson(const Track & track);
  static Json::Value TrackPointToJson(const TrackPoint & track_point);
};

}  // namespace MAUS

#endif  // _SRC_MAP_MAPCPPTRACKRECONSTRUCTOR_HH_
