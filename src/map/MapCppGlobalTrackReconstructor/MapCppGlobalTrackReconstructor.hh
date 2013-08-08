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

#ifndef _SRC_MAP_MAPCPPGLOBALTRACKRECONSTRUCTOR_HH_
#define _SRC_MAP_MAPCPPGLOBALTRACKRECONSTRUCTOR_HH_

// C++ headers
#include <string>
#include <vector>
#include <map>

// external libraries
#include "TMinuit.h"

// G4MICE from legacy
#include "BeamTools/BTField.hh"
#include "Config/MiceModule.hh"
#include "Interface/Squeak.hh"

// MAUS
#include "DataStructure/GlobalEvent.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "DataStructure/Global/Track.hh"
#include "Recon/Global/Detector.hh"

namespace MAUS {

class OpticsModel;
class CovarianceMatrix;

namespace recon {
namespace global {

class TrackFitter;

}  // namespace global
}  // namespace recon

/** @class MapCppGlobalTrackReconstructor
 *  Reconstruct tracks at the desired longitudinal spacing using the desired
 *  track fitting method.
 */
class MapCppGlobalTrackReconstructor {
 public:
  /** @brief Allocates the global-scope Minuit instance used for minimization.
   */
  MapCppGlobalTrackReconstructor();

  /** @brief Destroys the *global* scope Minuit object
   *
   *  This takes no arguments.
   */
  ~MapCppGlobalTrackReconstructor();

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
   *  on the desired method of recon.
   *
   * @param document a JSON document for a spill
   */
  std::string process(std::string document);

 private:
  Json::Value configuration_;
  OpticsModel * optics_model_;
  MAUS::recon::global::TrackFitter * track_fitter_;

  MAUS::recon::global::DetectorMap detectors_;

  static const std::string kClassname;
  BTField * electromagnetic_field_;

  void SetupOpticsModel();
  void SetupTrackFitter();
  void LoadRawTracks(
      GlobalEvent const * const global_event,
      MAUS::DataStructure::Global::TrackPArray & tracks) const;
  void InsertIntermediateTrackPoints(
      MAUS::DataStructure::Global::Track * track) const;
};

}  // namespace MAUS

#endif  // _SRC_MAP_MapCppGlobalTrackReconstructor_HH_
