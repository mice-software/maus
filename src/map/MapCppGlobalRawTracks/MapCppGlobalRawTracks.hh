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
#include "DataStructure/Global/Track.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"

namespace MAUS {

class CovarianceMatrix;
class GlobalEvent;
class ReconEvent;
class TOFSpacePoint;
class SciFiSpacePoint;

namespace DataStructure {
namespace Global {
  class TrackPoint;
}
}

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
  MAUS::recon::global::DetectorMap detectors_;
  int beam_polarity_;

  static const std::string kClassname;

  // ************************
  //  Data Loading Functions
  // ************************

  void AssembleRawTracks(MAUS::ReconEvent * recon_event,
                         MAUS::GlobalEvent * global_event);

  MAUS::recon::global::DetectorMap LoadDetectorConfiguration();

  void LoadTOFTrack(
      MAUS::ReconEvent const * const recon_event,
      MAUS::DataStructure::Global::TrackPArray & tof_tracks);

  void PopulateTOFTrackPoint(
      const MAUS::recon::global::Detector & detector,
      const std::vector<TOFSpacePoint>::const_iterator & tof_space_point,
      const double slab_width,
      const size_t number_of_slabs,
      MAUS::DataStructure::Global::TrackPoint * track_point);

  double FindEnergy(const double mass,
                    const double delta_z,
                    const double delta_t) const;

  double TOFSlabEnergyLoss(const double beta, const double mass) const;
  double TOFMeanStoppingPower(const double beta, const double mass) const;

  void LoadSciFiTrack(
      MAUS::ReconEvent const * const recon_event,
      MAUS::DataStructure::Global::TrackPArray & sci_fi_tracks);

  void PopulateSciFiTrackPoint(
      const MAUS::recon::global::Detector & detector,
      const std::vector<SciFiSpacePoint *>::const_iterator & scifi_space_point,
      MAUS::DataStructure::Global::TrackPoint * track_point);


  // ***********************************
  //  Particle Identification Functions
  // ***********************************

  MAUS::DataStructure::Global::PID IdentifyParticle(const double beta);

  double Beta(MAUS::DataStructure::Global::PID pid, const double momentum);
};

}  // namespace MAUS

#endif  // _SRC_MAP_MapCppGlobalRawTracks_HH_
