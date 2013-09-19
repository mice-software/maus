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
#include "DataStructure/Data.hh"
#include "DataStructure/MCEvent.hh"
#include "DataStructure/Primary.hh"
#include "DataStructure/ReconEvent.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"

namespace MAUS {

class PolynomialOpticsModel;
class ReconEvent;

// Object for use with process functions
template <typename Type>
class ProcessableObject {
  public:
  explicit ProcessableObject(Type * object)
      : object_(object), error_string_(NULL) { }
  explicit ProcessableObject(const std::string error_string)
      : object_(NULL), error_string_(new std::string(error_string)) { }
  ~ProcessableObject() {
    if (!object_) delete object_;
    if (!error_string_) delete error_string_;
  }
  Type * object() {
    return object_;
  }
  std::string * error_string() {
    return error_string_;
  }
  private:
  Type * object_;
  std::string * error_string_;
};

/** @class MapCppPolynomialMapResiduals
 *  Reconstruct tracks at the desired longitudinal spacing using the desired
 *  track fitting method.
 */
class MapCppPolynomialMapResiduals {
 public:
  // typedef int (MapCppPolynomialMapResiduals::*Method)();

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
  PolynomialOpticsModel * optics_model_;

  static const std::string kClassname;

  // ****************************
  //  Spill Processing Functions
  // ****************************

  ProcessableObject<Data> DeserializeRun(
      Json::Value & run) const;
  std::string ProcessRun(
      ProcessableObject<Data> & run_data) const;
  std::string SerializeRun(
      const Data * run,
      ProcessableObject<bool> result) const;
  ProcessableObject<bool> ProcessSpill(
      const Spill * spill) const;
  ProcessableObject<bool> GenerateResiduals(
      const MCEventPArray * mc_events,
      const ReconEventPArray * recon_events) const;
  std::vector<std::vector<PhaseSpaceVector> > ExtractMonteCarloTracks(
      const MCEventPArray * mc_events) const;
  std::vector<Primary *> ExtractBeamPrimaries(
      const MCEventPArray * mc_events) const;
  std::vector<std::vector<PhaseSpaceVector> > CalculateResiduals(
      const std::vector<std::vector<PhaseSpaceVector> > & mapped_tracks,
      const std::vector<std::vector<PhaseSpaceVector> > & mc_tracks) const;
  std::vector<PhaseSpaceVector> CalculateResiduals(
      const std::vector<PhaseSpaceVector> & mapped_hits,
      const std::vector<PhaseSpaceVector> & mc_hits) const;
  std::vector<std::vector<PhaseSpaceVector> > TransportBeamPrimaries(
      PolynomialOpticsModel const * const optics_model,
      std::vector<int64_t> z_positions,
      std::vector<Primary *> primaries) const;
  PhaseSpaceVector VirtualHit2PhaseSpaceVector(
      const VirtualHit & hit) const;
  PhaseSpaceVector Primary2PhaseSpaceVector(
      const Primary & primary) const;
  ProcessableObject<bool> WriteResiduals(
      const ReconEventPArray & recon_events,
      const std::vector<int64_t> z_positions,
      const std::vector<std::vector<PhaseSpaceVector> > & residuals) const;
};

}  // namespace MAUS

#endif  // _SRC_MAP_MapCppPolynomialMapResiduals_HH_
