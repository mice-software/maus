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


/** @class Reducer used to plot to tracker data */

#ifndef _REDUCECPPGLOBALRECONEFFICIENCY_H
#define _REDUCECPPGLOBALRECONEFFICIENCY_H

// C++ includes
#include <string>
#include <vector>
#include <map>

#include "json/json.h"

// MAUS includes
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterBase.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterInfoBox.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterXYZ.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {

class ReduceCppGlobalReconEfficiency {

 public:

  /** @brief Sets up the worker
   *  @param argJsonConfigDocument a JSON document with the configuration.
   */
  bool birth(std::string aJsonConfigDocument);

  /** @brief Shutdowns the worker, does nothing */
  bool death();

  /** @brief Process JSON document */
  std::string process(std::string document);

  /** @brief Return the spill object */
  MAUS::Spill* get_spill() { return _spill; }

  //~ /** @brief Takes json data and returns a Spill
  //~ *   @param json_data a string holding spill's worth of data in json format
  //~ */
  //~ bool read_in_json(std::string aJsonData);

 private:

  /**
   * @brief Checks whether the given TrackPoint belongs to a detector given by
   * the range of enums between min and max
   * 
   * @param track_point The TrackPoint to be checked
   * @param min the lower end of the range of enum values to check against
   * @param max the higher end of the range of enum values to check against
   */
  bool checkDetector(const MAUS::DataStructure::Global::TrackPoint* track_point,
                     int min, int max);

  /**
   * @brief Checks whether the given SpacePoint belongs to a detector given by
   * the range of enums between min and max
   * 
   * @param track_point The TrackPoint to be checked
   * @param min the lower end of the range of enum values to check against
   * @param max the higher end of the range of enum values to check against
   */
  bool checkDetector(const MAUS::DataStructure::Global::SpacePoint* space_point,
                     int min, int max);

  /**
   * @brief Compares the position of a TOF Recon hit with that of an MC hit to
   * check whether they agree
   * 
   * @param recon_position The position of the TOF recon hit
   * @param tof_hit The MC hit
   * @param primary_time The timestamp of the primary particle to convert
   *        between MC and recon time
   * @param dX_max the maximum allowed disagreement in x for a match
   * @param dY_max the maximum allowed disagreement in y for a match
   * @param dT_max the maximum allowed disagreement in t for a match
   */
  bool TOFReconMatchesMC(TLorentzVector recon_position, TOFHit tof_hit,
      double primary_time, double dX_max, double dY_max, double dT_max);

  /**
   * @brief Compares the position of a KL Recon hit with that of an MC hit to
   * check whether they agree
   * 
   * @param recon_position The position of the KL Recon hit
   * @param kl_hit The MC hit
   * @param dY_max the maximum allowed disagreement in y for a match
   */
  bool KLReconMatchesMC(TLorentzVector recon_position, KLHit kl_hit,
      double dY_max);

  /**
   * @brief Compares the position of an EMR Recon hit with that of an MC hit to
   * check whether they agree
   * 
   * @param recon_position The position of the EMR Recon hit
   * @param emr_hit The MC hit
   * @param dX_max the maximum allowed disagreement in x for a match
   * @param dY_max the maximum allowed disagreement in y for a match
   */
  bool EMRReconMatchesMC(TLorentzVector recon_position, EMRHit emr_hit,
      double dX_max, double dY_max);

  /**
   * @brief Determines the number of expected matches, correct matches, false
   * matches, and hits where local reconstruction failed for TOF hits in a
   * reconstructed track
   *
   * @param detector_number integer denoting position in the counter arrays,
   * 0-4 for TOF0, TOF1, TOF2, KL, EMR
   * @param detector DetectorPoint enum for the selected detector
   * @param track the matched reconstructed track
   * @param mc_event MC event to compare to
   * @param p_time The timestamp of the primary particle to convert between MC
   *        and recon time
   * @param dX_max the maximum allowed disagreement in x for a match
   * @param dY_max the maximum allowed disagreement in y for a match
   * @param dT_max the maximum allowed disagreement in t for a match
   */
  void TOFEfficiency(size_t detector_number,
      MAUS::DataStructure::Global::DetectorPoint detector,
      MAUS::DataStructure::Global::Track* track, MAUS::MCEvent* mc_event,
      double p_time, double dX_max, double dY_max, double dT_max);

  /**
   * @brief Determines the number of expected matches, correct matches, false
   * matches, and hits where local reconstruction failed for KL hits in a
   * reconstructed track
   *
   * @param track the matched reconstructed track
   * @param mc_event MC event to compare to
   * @param dY_max the maximum allowed disagreement in y for a match
   */
  void KLEfficiency(MAUS::DataStructure::Global::Track* track,
      MAUS::MCEvent* mc_event, double dY_max);

  /**
   * @brief Determines the number of expected matches, correct matches, false
   * matches, and hits where local reconstruction failed for EMR hits in a
   * reconstructed track
   *
   * @param track the matched reconstructed track
   * @param mc_event MC event to compare to
   * @param dX_max the maximum allowed disagreement in x for a match
   * @param dY_max the maximum allowed disagreement in y for a match
   */
  void EMREfficiency(MAUS::DataStructure::Global::Track* track,
      MAUS::MCEvent* mc_event);


  void throughEfficiency(MAUS::DataStructure::Global::Track* track,
      MAUS::MCEvent* mc_event, double p_time, double dX_max1, double dX_max2,
      double dY_max1, double dY_max2, double dT_max1, double dT_max2);

  /**
   * @brief Pairs tracks with mc events in a spill based on a matching between
   * tracker mc hits and reconstruction
   *
   * @param mapper_name The mapper name by which to select tracks
   * @param pid The PID by which to select tracks
   *
   * @return A vector of Track/MCEvent pairs
   */
  std::vector<std::pair<MAUS::DataStructure::Global::Track*, MAUS::MCEvent*> >
      matchTrackerReconWithMC(std::string mapper_name,
                              MAUS::DataStructure::Global::PID pid);

  size_t _detector_matches[9];
  size_t _detector_matches_expected[9];
  size_t _detector_false_matches[9];
  size_t _detector_lr_failed[9];
  std::string mClassname;
  //~ Json::Value mRoot;
  Spill* _spill;
};
      
} // ~namespace MAUS

#endif
