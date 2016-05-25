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

/** @class TrackMatching
 *  @author Jan Greis, University of Warwick
 *  Creates global tracks from a global event.
 *  @date 2015/11/26
 */

#ifndef _SRC_COMMON_CPP_RECON_TRACKMATCHING_HH_
#define _SRC_COMMON_CPP_RECON_TRACKMATCHING_HH_

// C++ headers
#include <map>
#include <string>
#include <utility>
#include <vector>

// ROOT headers
#include "TLorentzVector.h"

// gtest include for friend-testing
#include "gtest/gtest_prod.h"

// MAUS headers
#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

class BTFieldConstructor;

namespace MAUS {
namespace recon {
namespace global {

  class TrackMatching {
  public:

    /// Constructor
    TrackMatching(GlobalEvent* global_event,
                  std::string mapper_name,
                  std::string pid_hypothesis_string,
                  std::map<std::string, std::pair<double, double> >
                  matching_tolerances, double max_step_size,
                  bool energy_loss = true);

    /// Destructor
    ~TrackMatching() {}

    /**
     * @brief Create an upstream track by matching the other upstream detectors
     * to the upstream tracker
     */
    void USTrack();

    /**
     * @brief Create a downstream track by matching the other downstream
     * detectors to the downstream tracker
     */
    void DSTrack();

    /**
     * @brief Matches previously created upstream and downstream tracks via the
     * TOF1/2 time difference (thus can only match tracks that contain TOF1/2
     * hits respectively. Typically only used for no-field commissioning data.
     */
    void throughTrack();

  private:

    /// Disallow copy constructor as unnecessary
    TrackMatching(const TrackMatching&);

    /// Disallow operator= as unnecessary
    void operator=(const TrackMatching);

    /**
     * @brief Returns an array of Tracks in the global event corresponding to
     * the chosen detector (either tracker or the EMR, other detectors don't
     * produce tracks). If EMR is chosen, the primary track is returned
     * (selected by requiring the secondary range of the object to be ~0)
     *
     * @param detector Chosen detector (kTracker0, kTracker1, or kEMR)
     *
     * @see GetDetectorTrackPoints()
     */
    MAUS::DataStructure::Global::TrackPArray* GetDetectorTrackArray(
        MAUS::DataStructure::Global::DetectorPoint detector);

    /**
     * @brief Returns a vector of TrackPoints in the global event corresponding
     * to the chosen detector (TOFs or KL).
     *
     * As the recon import for these detectors outputs SpacePoints only, these
     * are first converted into TrackPoints by this function
     *
     * @param detector Chosen detector (kTOF0, kTOF1, kTOF2, or kCalorimeter)
     * @param mapper_name The mapper name to assign to the new TrackPoints
     *
     * @see GetDetectorTrackArray()
     */
    std::vector<DataStructure::Global::SpacePoint*> GetDetectorSpacePoints(
        DataStructure::Global::DetectorPoint detector, std::string mapper_name);

    /**
     * @brief Creates a vector of PIDs that designate the PID hypotheses over
     * which track matching should run.
     *
     * If the configuration setting track_matching_pid_hypothesis is set to a
     * specific PID, the vector will only contain that one, otherwise, depending
     * on the charge hypothesis supplied, the vector will contain either
     * kEPlus, kMuPlus, kPiPlus (chargy hypothesis 1),
     * kEMinus, kMuMinus, kPiMinus (charge hypothesis -1),
     * or all six (charge hypothesis 0)
     *
     * @param charge_hypothesis the charge hypothesis provided by the tracker,
     * can be 1, -1, 0
     * @param pid_hypothesis_string the value of track_matching_pid_hypothesis
     * in the configuration (as long as the TrackMatching constructor sets it
     * as such)
     */
    std::vector<DataStructure::Global::PID> PIDHypotheses(
        int charge_hypothesis, std::string pid_hypothesis_string);

    /**
     * @brief Matches hits in TOFs or KL to a tracker track using a wrapper for
     * the GSL 4th order Runge-Kutta integration to propagate from a tracker
     * TrackPoint to the detector and checking the agreement with TrackPoints
     * in that detector.
     *
     * @param position The position of the tracker TrackPoint to which the
     * detector TrackPoint is matched
     * @param momentum Momentum of the tracker TrackPoint
     * @param trackpoints A vector of TrackPoints in the target detector to
     * which matching is attempted
     * @param pid The PID setting for the propagation
     * @param field magnetic fields for the geometry
     * @param detector_name name of the detector (TOF0, TOF1, TOF2, KL) to which
     * matching is attempted. Used to pull the matching tolerances from the
     * configuration
     * @param hypothesis_track The track that collects the matched points
     *
     * @see MatchEMRTrack()
     */
    void MatchTrackPoint(
        const TLorentzVector &position, const TLorentzVector &momentum,
        const std::vector<DataStructure::Global::SpacePoint*> &spacepoints,
        DataStructure::Global::PID pid, BTFieldConstructor* field,
        std::string detector_name,
        DataStructure::Global::Track* hypothesis_track);

    void MatchTOF0(
        const TLorentzVector &position, const TLorentzVector &momentum,
        double tof1_z, double tof1_t,
        const std::vector<DataStructure::Global::SpacePoint*> &spacepoints,
        DataStructure::Global::PID pid, BTFieldConstructor* field,
        DataStructure::Global::Track* hypothesis_track);

    /**
     * @brief As MatchTrackPoint() but matching to the most upstream trackpoint
     * of an EMR track.
     *
     * @param position The position of the tracker TrackPoint to which the
     * EMR TrackPoint is matched
     * @param momentum Momentum of the tracker TrackPoint
     * @param emr_track_array Array of EMR Tracks to which matching is attempted
     * @param pid The PID setting for the propagation
     * @param field magnetic fields for the geometry
     * @param hypothesis_track The track that collects the matched points
     *
     * @see MatchTrackPoint()
     */
    void MatchEMRTrack(
        const TLorentzVector &position, const TLorentzVector &momentum,
        DataStructure::Global::TrackPArray* emr_track_array,
        DataStructure::Global::PID pid, BTFieldConstructor* field,
        DataStructure::Global::Track* hypothesis_track);

    /**
     * @brief Adds all TrackPoints from a tracker Track to a different track and
     * sets their energy in accordance with the provided mass value.
     *
     * Before being added to the Track, the TrackPoints are sorted by ascending
     * z position using std::sort and GlobalTools::TrackPointSort()
     *
     * @param tracker_track The track to be added
     * @param mapper_name The mapper name to tag the TrackPoints with
     * @param mass The mass used to calculate the energy from the 3-momentum
     * @param hypothesis_track The track to which the tracker TrackPoints are
     * added
     */
    void AddTrackerTrackPoints(
        DataStructure::Global::Track* tracker_track, std::string mapper_name,
        double mass, DataStructure::Global::Track* hypothesis_track);

    /**
     * @brief Returns all matched upstream and downstream tracks from the global
     * tracks given in the supplied TrackPArrays.
     *
     * The tracks are selected based on aggreement with the mapper names set in
     * USTrack() and DSTrack() as well as the supplied PIDs.
     *
     * @param global_tracks Tracks from the global event from which to pick out
     * the upstream and downstream tracks.
     * @param pid The PID for which the tracks are picked out
     * @param us_tracks The container for the upstream tracks to be returned
     * @param ds_tracks The container for the downstream tracks to be returned
     */
    void USDSTracks(
        DataStructure::Global::TrackPArray* global_tracks,
        DataStructure::Global::PID pid,
        DataStructure::Global::TrackPArray* us_tracks,
        DataStructure::Global::TrackPArray* ds_tracks);

    /**
     * @brief Produces a through-going track from the supplied upstream and
     * downstream TrackPoints based on a cut in the TOF1/2 travel time
     *
     * @param us_trackpoints Upstream TrackPoints
     * @param ds_trackpoints Downstream TrackPoints
     * @param pid PID to set for the new track
     * @param emr_range_primary EMR primary Track range to pass to the new track
     */
    void MatchUSDS(
        DataStructure::Global::Track* us_track,
        DataStructure::Global::Track* ds_track,
        DataStructure::Global::PID pid, double emr_range_primary);

    /**
     * @brief Returns the time from a TrackPoint in the chosen detector (TOF0,
     * TOF1, TOF2).
     *
     * If several TrackPoints for this detector exist in the supplied
     * TrackPoints, the first one is chosen. Hence it is necessary to first
     * make sure that if several exist, they are consistent with being produced
     * by a single particle
     *
     * @param trackpoints The TrackPoints from which to pick out the selected
     * one
     * @param detector ID of the detector to get the time from (kTOF0, kTOF1,
     * kTOF2)
     */
    double TOFTimeFromTrackPoints(
        DataStructure::Global::TrackPointCPArray trackpoints,
        DataStructure::Global::DetectorPoint detector);

    /**
     * @brief Only add matched TrackPoints, if hits are consistent with each other
     *
     * If only one matche TrackPoint exists, it is added to the track, if multiple
     * exist, they are only added if they are consistent with each other (i.e.
     * max. one slab position difference and for TOFs a cut on the difference in t)
     *
     * @param trackpoints The TrackPoints which should be checked/added
     * @param hypothesis_track The Track to which the TrackPoints should be added
     */
    void AddIfConsistent(
        std::vector<DataStructure::Global::SpacePoint*> spacepoints,
        DataStructure::Global::Track* hypothesis_track);

    /// Mapper name passed by the mapper calling this class
    std::string _mapper_name;

    /// Configuration setting for which PIDs to consider, can be kEPlus,
    /// kEMinus, kMuPlus, kMuMinus, kPiPlus, kPiMinus, or all
    std::string _pid_hypothesis_string;

    /// Matching tolerances for the various detectors that are matched
    std::map<std::string, std::pair<double, double> > _matching_tolerances;

    /// Maximum stepsize for the RK4 propagation
    double _max_step_size;

    /// Should the RK4 include energy loss
    bool _energy_loss;

    /// The global event to be processed
    GlobalEvent* _global_event;

    /// Declarations required for tests to access private member functions
    FRIEND_TEST(TrackMatchingTest, GetDetectorTrackArray);
    FRIEND_TEST(TrackMatchingTest, GetDetectorTrackPoints);
    FRIEND_TEST(TrackMatchingTest, PIDHypotheses);
    FRIEND_TEST(TrackMatchingTest, MatchTrackPoint);
    FRIEND_TEST(TrackMatchingTest, MatchTOF0);
    FRIEND_TEST(TrackMatchingTest, MatchEMRTrack);
    FRIEND_TEST(TrackMatchingTest, AddTrackerTrackPoints);
    FRIEND_TEST(TrackMatchingTest, USDSTracks);
    FRIEND_TEST(TrackMatchingTest, MatchUSDS);
    FRIEND_TEST(TrackMatchingTest, TOFTimeFromTrackPoints);
  }; // ~class TrackMatching
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
