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
    enum geometry_algorithm {
        kClassicG4 = 0,
        kAxialLookup = 1,
        kAltG4 = 2
    };

    enum through_track_algorithm {
        kNoThroughMatching,
        kTOF12,
        kPropagate,
        kPropagateRequiringTOF12,
        kPropagateAndTOF12
    };

    /// Constructor
    TrackMatching(GlobalEvent* global_event,
                  std::string mapper_name,
                  std::string pid_hypothesis_string,
                  int beamline_polarity,
                  std::map<std::string, std::pair<double, double> >
                  matching_tolerances, double max_step_size,
                  std::pair<bool, bool> no_check_settings,
                  bool energy_loss = true,
                  bool residuals = false,
                  geometry_algorithm algo = kClassicG4,
                  std::vector<double> extra_z_planes = std::vector<double>(),
                  through_track_algorithm = kTOF12
    );

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
     * @brief Returns a vector of SpacePoints in the global event corresponding
     * to the chosen detector (TOFs or KL).
     *
     * @param detector Chosen detector (kTOF0, kTOF1, kTOF2, or kCalorimeter)
     * @param mapper_name The mapper name to assign to the new TrackPoints
     *
     * @see GetDetectorTrackArray()
     */
    std::vector<DataStructure::Global::SpacePoint*> GetDetectorSpacePoints(
        DataStructure::Global::DetectorPoint detector);

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
     * @brief Matches hits in TOFs (1/2) or KL to a tracker track using a wrapper
     * for the GSL 4th order Runge-Kutta integration to propagate from a tracker
     * TrackPoint to the detector and checking the agreement with TrackPoints
     * in that detector.
     *
     * @param position The position of the tracker TrackPoint to which the
     * detector TrackPoint is matched
     * @param momentum Momentum of the tracker TrackPoint
     * @param spacepoints A vector of SpacePoints in the target detector to
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
        TLorentzVector &position, TLorentzVector &momentum,
        const std::vector<DataStructure::Global::SpacePoint*> &spacepoints,
        DataStructure::Global::PID pid, BTFieldConstructor* field,
        std::string detector_name,
        DataStructure::Global::Track* hypothesis_track);

    /**
     * @brief Matches hits in TOF0 to a TOF1 hit by comparing the delta T to the
     * expected travel time given the particle's energy just before TOF1 (obtained
     * by propagating backwards from a tracker trackpoint)
     *
     * @param position The position of the tracker TrackPoint which is propagated
     * back to TOF1
     * @param momentum Momentum of the tracker TrackPoint
     * @param spacepoints A vector of SpacePoints in TOF0 to which matching is attempted
     * @param pid The PID setting for the propagation
     * @param field magnetic fields for the geometry
     * @param hypothesis_track The track that collects the matched points
     */

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
     *  @brief Adds TrackPoints stored in the virtual_track_points list to the
     *  hypothesis_track
     *
     *  @param hypothesis_track the track to which track points are added
     */
    void AddVirtualTrackPoints(DataStructure::Global::Track* hypothesis_track);

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
     * @brief Only add matched SpacePoints, if hits are consistent with each other
     *
     * If only one matched SpacePoint exists, it is added to the track, if multiple
     * exist, they are only added if they are consistent with each other (i.e.
     * max. one slab position difference and for TOFs a cut on the difference in t)
     *
     * @param spacepoints The SpacePoints which should be checked/added
     * @param hypothesis_track The Track to which the TrackPoints should be added
     */
    void AddIfConsistent(
        std::vector<DataStructure::Global::SpacePoint*> spacepoints,
        DataStructure::Global::Track* hypothesis_track);

    /**
     * @brief Checks whether mutually exclusive through Primary Chains exist in this event
     *
     * If two through chains contain the same daughter chain, they will be marked as
     * kMultipleUS for same US daughters, kMultipleDS for same DS daughters, or kMultipleBoth
     * if both are the same. If no such multiplicity exists, the value is kUnique
     */
    void CheckChainMultiplicity();

    /**
     *  @brief Propagate tracks to given z position, recording extra hits
     *
     *  Figure out the positions of any extra z planes between z and target_z;
     *  propagate through the z planes,  recording any extra track points in
     *  the extra track points vector
     *
     *  @param x_in 8-vector t, x, y, z, energy, px, py, pz for tracking
     *  @param field the field to use for track propagation
     *  @param pid the particle type (PDG number) of particle propagated
     *  @param target_z z-position to propagate to 
     */
    void Propagate(double* x_in,
                   BTFieldConstructor* field,
                   DataStructure::Global::PID pid,
                   double target_z);

    /**
     *  @brief Clear the virtual track points before attempting to match a track
     */
    void ClearVirtualTrackPoints();

    /**
     *  @brief Match through-going track using a simple TOF12 window
     *
     *  If TOF12 is within the range in TOF12dT matching tolerances, assumes a
     *  match for us_track and ds_track.
     *
     *  @param us_track Track with TOF1 data (or matching fails)
     *  @param ds_track Track with TOF2 data (or matching fails)
     *  @param pid PID hypothesis of the through-going track
     *  @param through_track hypothesised through-going track is written here
     *
     *  @returns false if the tracks dont match; true if the tracks match
     */
    bool throughMatchTOF(
                                DataStructure::Global::Track* us_track,
                                DataStructure::Global::Track* ds_track,
                                DataStructure::Global::PID pid,
                                DataStructure::Global::Track* through_track);


    /**
     *  @brief Match through-going track using track propagation
     *
     *  If extrapolated track is within position tolerance, assumes a match for
     *  us_track and ds_track.
     *
     *  @param us_track Track with tku_tp1 data (or matching fails)
     *  @param ds_track Track with tkd_tp1 data (or matching fails)
     *  @param pid PID hypothesis of the through-going track
     *  @param through_track hypothesised through-going track is written here
     *
     *  @returns false if the tracks dont match; true if the tracks match
     */
    bool throughMatchPropagate(
                            DataStructure::Global::Track* us_track,
                            DataStructure::Global::Track* ds_track,
                            DataStructure::Global::PID pid,
                            DataStructure::Global::Track* through_track);

    /**
     *  @brief Add child track and track points to the parent track
     *
     *  Adds the child track to the parent and each TrackPoint on the child
     *  track to the parent.
     *
     *  @param parent The parent track.
     *  @param child The child track.
     *  @param doEmr Set to true to overwrite the parent emr_primary and
     *         emr_density with data from the child.
     */
    void addTrackRecursive(DataStructure::Global::Track* parent,
                           DataStructure::Global::Track* child,
                           bool doEmr);

    /// Mapper name passed by the mapper calling this class
    std::string _mapper_name;

    /// Configuration setting for which PIDs to consider, can be kEPlus,
    /// kEMinus, kMuPlus, kMuMinus, kPiPlus, kPiMinus, or all
    std::string _pid_hypothesis_string;

    /// The beamline polarity, 1 for positive, -1 for negative
    int _beamline_polarity;

    /// Matching tolerances for the various detectors that are matched
    std::map<std::string, std::pair<double, double> > _matching_tolerances;

    /// Whether matching should not be performed with only one hit per detector
    /// (first value for upstream, second for downstream)
    std::pair<bool, bool> _no_check_settings;

    /// Maximum stepsize for the RK4 propagation
    double _max_step_size;

    /// Should the RK4 include energy loss
    bool _energy_loss;

    /// Should residuals be generated during track matching
    bool _residuals;

    /// The global event to be processed
    GlobalEvent* _global_event;

    /// Track matching will report a hit at each of the extra z planes
    std::vector<double> _extra_z_planes;
    std::vector<DataStructure::Global::TrackPoint> _virtual_track_points;

    /// The field for track matching
    BTFieldConstructor* _field;

    /// Controls how the track propagation is done
    geometry_algorithm _geo_algo;

    /// Controls logic of the through track matching
    through_track_algorithm _through_algo = kTOF12;

    /// Declarations required for tests to access private member functions
    FRIEND_TEST(TrackMatchingTest, GetDetectorTrackArray);
    FRIEND_TEST(TrackMatchingTest, GetDetectorSpacePoints);
    FRIEND_TEST(TrackMatchingTest, PIDHypotheses);
    FRIEND_TEST(TrackMatchingTest, MatchTrackPoint);
    FRIEND_TEST(TrackMatchingTest, MatchTOF0);
    FRIEND_TEST(TrackMatchingTest, MatchEMRTrack);
    FRIEND_TEST(TrackMatchingTest, AddTrackerTrackPoints);
    FRIEND_TEST(TrackMatchingTest, USDSTracks);
    FRIEND_TEST(TrackMatchingTest, MatchUSDS);
    FRIEND_TEST(TrackMatchingTest, TOFTimeFromTrackPoints);
    FRIEND_TEST(TrackMatchingTest, AddIfConsistent);
    FRIEND_TEST(TrackMatchingTest, CheckChainMultiplicity);
  }; // ~class TrackMatching
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
