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

#ifndef _SRC_MAP_MapCppPolynomialMapResiduals_HH_
#define _SRC_MAP_MapCppPolynomialMapResiduals_HH_

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

/** @class MapCppPolynomialMapResiduals
 *  Reconstruct tracks at the desired longitudinal spacing using the desired
 *  track fitting method.
 */
class MapCppPolynomialMapResiduals {
 public:
  typedef int (A::*Method)();

  /** @brief Allocates the global-scope Minuit instance used for minimization.
   */
  MapCppPolynomialMapResiduals();

  /** @brief Destroys the *global* scope Minuit object
   *
   *  This takes no arguments.
   */
  ~MapCppPolynomialMapResiduals();

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
  MAUS::recon::global::DetectorMap detectors_;
  PolynomialOpticsModel * optics_model_;
  int beam_polarity_;

  static const std::string kClassname;

  // Object for use with process functions
  template <typename Type>
  class ProcessableObject {
   public:
    ProcessableObject(const Type * object)
        : object_(object, error_string_(NULL) { }
    ProcessableObject(const std::string error_string)
        : object_(NULL), error_string_(new std::string(error_string)) { }
    Type * object() { return object; };
    const std::string * error_string() { return error_string_; }
   private:
    Type * object_;
    std::string * error_string_;
  }

  // ****************************
  //  Spill Processing Functions
  // ****************************

  ProcessableObject<MAUS::Data> DeserializeRun(
      Json::Value & run) const;
  std::string ProcessRun(
      ProcessableObject<MAUS::Data> & run_data) const;
  std::string SerializeRun(
      const MAUS::Data * run,
      ProcessableObject<Bool> result) const;
  ProcessableObject<Bool> ProcessSpill(
      const MAUS::Spill * spill) const;
  ProcessableObject<Bool> GenerateResiduals(
      const MCEventPArray * mc_events,
      const MAUS::ReconEventPArray * recon_events) const;
  std::vector<std::vector<PhaseSpaceVector> > ExtractMonteCarloTracks(
      const MCEventPArray * mc_events) const;
  std::vector<PhaseSpaceVector> ExtractBeamPrimaries(
      const MCEventPArray * mc_events) const;
  std::vector<std::vector<PhaseSpaceVector> > CalculateResiduals(
      std::vector<std::vector<PhaseSpaceVector> > & mapped_tracks,
      std::vector<std::vector<PhaseSpaceVector> > & mc_tracks) const;
  std::vector<PhaseSpaceVector> CalculateResiduals(
      std::vector<PhaseSpaceVector> & mapped_hits,
      std::vector<PhaseSpaceVector> & mc_hits) const;
  std::vector<std::vector<PhaseSpaceVector> > TransportBeamPrimaries(
      const PolynomialOpticsModel & optics_model,
      std::vector<long> & z_positions,
      std::vector<Primary> & primaries) const;
  PhaseSpaceVector VirtualHit2PhaseSpaceVector(
      Primary hit) const;
  PhaseSpaceVector Primary2PhaseSpaceVector(
      VirtualHit primary) const;
  ProcessableObject<Bool> WriteResiduals(
      const std::vector<std::vector<PhaseSpaceVector> > & residuals,
      const ReconEventPArray & recon_events) const;
};

}  // namespace MAUS

#endif  // _SRC_MAP_MapCppPolynomialMapResiduals_HH_
