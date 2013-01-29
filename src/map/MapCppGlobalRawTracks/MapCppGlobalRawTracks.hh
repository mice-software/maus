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

#ifndef _SRC_MAP_MAPCPPGLOBALRAWTRACKS_HH_
#define _SRC_MAP_MAPCPPGLOBALRAWTRACKS_HH_

// C++ headers
#include <string>
#include <vector>
#include <map>

// external libraries
#include "TMinuit.h"

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

/** @class MapCppGlobalRawTracks
 *  Reconstruct tracks at the desired longitudinal spacing using the desired
 *  track fitting method.
 */
class MapCppGlobalRawTracks {
 public:
  /** @brief Allocates the global-scope Minuit instance used for minimization.
   */
  MapCppGlobalRawTracks();

  /** @brief Destroys the *global* scope Minuit object
   *
   *  This takes no arguments.
   */
  ~MapCppGlobalRawTracks();

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
  int beam_polarity_;

  Json::Value run_data_;
  std::vector<MAUS::recon::global::Track> tracks_;

  static const std::string kClassname;

  void LoadRandomData();
  void LoadTestingData();
  void LoadSimulationData();
  void LoadSmearedData();
  void LoadLiveData();

  void LoadSimulationData(const std::string mc_branch_name);

  void LoadTOFTracks(
      std::map<MAUS::recon::global::Detector::ID,
               MAUS::recon::global::Detector>& detectors,
      Json::Value& recon_event,
      std::vector<MAUS::recon::global::Track>& tof_tracks);
  void LoadSciFiTracks(
      std::map<MAUS::recon::global::Detector::ID,
               MAUS::recon::global::Detector>& detectors,
      Json::Value& recon_event,
      std::vector<MAUS::recon::global::Track>& sci_fi_tracks);
  void LoadDetectorConfiguration(
    std::map<MAUS::recon::global::Detector::ID,
    MAUS::recon::global::Detector> & detectors);
  void LoadMonteCarloData(
    const std::string branch_name,
    const std::map<MAUS::recon::global::Detector::ID,
                   MAUS::recon::global::Detector> & detectors);
  recon::global::Particle::ID IdentifyParticle(const double beta);
  double Beta(recon::global::Particle::ID pid, const double momentum);

  static CovarianceMatrix const GetJsonCovarianceMatrix(
      Json::Value const & value);

  void CorrelateTrackPoints(
      std::vector<MAUS::recon::global::Track> & tracks);

  static Json::Value TrackToJson(
      const MAUS::recon::global::Track & track);
  static Json::Value TrackPointToJson(
      const MAUS::recon::global::TrackPoint & track_point);
};

}  // namespace MAUS

#endif  // _SRC_MAP_MapCppGlobalRawTracks_HH_
