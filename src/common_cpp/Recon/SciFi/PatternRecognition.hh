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

/** @class PatternRecognition
 *
 * Pattern Recognition algorithms encapsulated in a class
 *
 */

#ifndef PATTERNRECOGNITION_HH
#define PATTERNRECOGNITION_HH

// C++ headers
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

// ROOT headers
#include "TFile.h"
#include "TH1D.h"
#include "TMatrixD.h"

// Third party library headers
#include "gtest/gtest_prod.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/LeastSquaresFitter.hh"
#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

namespace MAUS {

class PatternRecognition {
  public:

    /** Macro to allow friendship with the gtests */
    FRIEND_TEST(PatternRecognitionTest, test_constructor);
    FRIEND_TEST(PatternRecognitionTest, test_set_parameters_to_default);
    FRIEND_TEST(PatternRecognitionTest, test_setup_debug);

    /** @brief Default constructor. Initialise variables,
     *         using globals if available, otherwise local defaults 
     */
    PatternRecognition();

    /** @brief Default destructor */
    ~PatternRecognition();

    /** @brief Set the member variables using the Global singleton class */
    bool LoadGlobals();

    /** @brief Top level function to begin Pattern Recognition
      * @param evt - The SciFi event
      */
    void process(SciFiEvent &evt) const;

     /** @brief Small function to add trks to a SciFiEvent & to set the tracker number for them
      *  @param strks - The straight tracks vector
      *  @param htrks - The helical tracks vector
      *  @param trker_no - The tracker number
      *  @param evt - The SciFi event
      */
    void add_tracks(const int trker_no, std::vector<SciFiStraightPRTrack*> &strks,
                    std::vector<SciFiHelicalPRTrack*> &htrks, SciFiEvent &evt) const;

    template<typename T>
    std::vector<T*> select_tracks(std::vector<T*> &trks) const;


    void make_all_tracks(const bool track_type, const int trker_no,
                         SpacePoint2dPArray &spnts_by_station, SciFiEvent &evt) const;


    /** @brief Make Pattern Recognition tracks with 5 spacepoints
     *  @param track_type - boolean, 0 for straight tracks, 1 for helical tracks
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param strks - A vector of the output Pattern Recognition straight tracks
     *  @param htrks - A vector of the output Pattern Recognition helical tracks
     */
    void make_5tracks(const bool track_type, const int trker_no,
                      SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack*> &strks,
                      std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Make Pattern Recognition tracks with 4 spacepoints
     *  @param track_type - boolean, 0 for straight tracks, 1 for helical tracks
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param strks - A vector of the output Pattern Recognition straight tracks
     *  @param htrks - A vector of the output Pattern Recognition helical tracks
     */
    void make_4tracks(const bool track_type, const int trker_no,
                      SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack*> &strks,
                      std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Make Pattern Recognition tracks with 3 spacepoints (straight only)
     *  @param trker_no - the tracker number (0 or 1)
     *  @param spnts - A vector of all the input spacepoints
     *  @param strks - A vector of the output Pattern Recognition straight tracks
     */
    void make_3tracks(const int trker_no, SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack*> &strks) const;

    /** @brief Fits a straight track for a given set of stations
     * 
     *  @param ignore_stations int vector specifying which stations are not to be used for
     *                         the track fit. 0 - 4 represent stations 1 - 5 respectively,
     *                         while -1 means use *all* the stations (ignore none of them). 
     *                         The size of the vector should be 0 for a 5pt track,
     *                         1 for a 4pt track, and 2 for a 3pt track.
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_straight_tracks(const int num_points, const int trker_no,
                              const std::vector<int> ignore_stations,
                              SpacePoint2dPArray &spnts_by_station,
                              std::vector<SciFiStraightPRTrack*> &strks) const;

    /** @brief Make a helical track from spacepoints
     *
     *  Recursive function holding the looping structure for making helical tracks from spacepoints.
     *  Once looping has identified candidate spacepoints, calls form_track which performs the 
     *  circle fit in x-y projection and then the line fit in the s-z projection.
     *
     *  @param num_points - the number of points in the track (4 or 5)
     *  @param stat_num - the current station number
     *  @param ignore_stations - int vector specifying which stations are not to be used for
     *                           the track fit. 0 - 4 represent stations 1 - 5 respectively,
     *                           while -1 means use *all* the stations (ignore none of them).
     *                           The size of the vector should be 0 for a 5pt track or
     *                           1 for a 4pt track
     *  @param current_spnts - the spacepoints assembled so far for the trial track
     *  @param spnts_by_station - 2D vector of spacepoints, sorted by station
     *  @param htrks - vectors of tracks holding the initial helix parameters and spacepoints used
     *
     */
    void make_helix(const int n_points, const int stat_num, const std::vector<int> ignore_stations,
             std::vector<SciFiSpacePoint*> &current_spnts,
             SpacePoint2dPArray &spnts_by_station, std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Attempt to fit a helical track to given spacepoints
     * 
     *  Attempt to fit a helical track to given spacepoints. Two part process: (1) circle fit in the
     *  x-y projection, (2) a line fit in the s-z projection. Returns a pointer to the found 
     *  track if successful, otherwise returns a NULL pointer.
     * 
     *  @param n_points - the number of points in the track (4 or 5)
     *  @param spnts - vector holding pointers to the spacepoints
     * 
     * */
    SciFiHelicalPRTrack* form_track(const int n_points, std::vector<SciFiSpacePoint*> spnts ) const;

    /** @brief Find the ds/dz of a helical track
     *
     * Find the ds/dz of a helical track. Output is the turning angles of the spacepoints
     * and a line of s vs z, the slope of which is dsdz = 1/tan(lambda).
     *
     * @param n_points - Number of points in the current track (used for the chi_sq cut)
     * @param spnts - A vector of all the input spacepoints
     * @param circle - The circle fit of spacepoints from x-y projection
     * @param line_sz - The output fitted line in s-z projection.
     */
    bool find_dsdz(int n_points, std::vector<SciFiSpacePoint*> &spnts, const SimpleCircle &circle,
          std::vector<double> &phi_i, SimpleLine &line_sz, TMatrixD& cov_sz, int &handedness) const;

    /** @brief Find the number of 2pi rotations that occured between each station
     *
     * Find the number of 2pi rotations that occured between each stations. This is
     * necessary in order to later evaluate s, the track path length in x-y, used to find ds/dz.
     *
     * @param z - the z coord of each spacepoint in the order seen by the beam
     * @param phi - the turning angle between successive spacepoints in the order seen by the beam
     * @param true_phi - the corrected turing angles
     */
    bool find_n_turns(const std::vector<double> &z, const std::vector<double> &phi,
                      std::vector<double> &true_phi, int &handedness) const;

    /** @brief Checks that the spacepoints in trial track fall within longest acceptable time range
     *
     *  Tracker timing resolution cable of ~2ns. Longest acceptable time of flight through tracker
     *  was calculated offline for straight and helical tracks
     *
     */
    bool check_time_consistency(const std::vector<SciFiSpacePoint*>,
                                                            int tracker_id) const;

    /** @brief Determine which two stations the initial line should be drawn between
     * 
     *  The initial line is to be drawn between the two outermost stations being used.
     *  This in turn depends on which stations are presently being ignored
     *  e.g. for a 5 pt track, station 5 and station 1 are always  the outer and inner
     *  stations respectively.  This function returns the correct outer and inner
     *  station numbers, given which stations are presently being ignored.
     * 
     *  NB Stations are number 0 - 4 in the code, not 1 - 5 as in the outside world
     *
     *  Returns true if successful, false if fails (due to a bad argument being passed)
     *
     *  @param ignore_stations - Vector of ints, holding which stations should be ignored
     *  @param o_stat_num - The outermost station number used for a given track fit
     *  @param i_stat_num - The innermost station number used for a given track fit
     *
     */
    bool set_end_stations(const std::vector<int> ignore_stations, int &o_stat_num,
                                                                            int &i_stat_num) const;

    /** @brief Determine which three stations the initial circle should be fit to
     *
     *  The initial circle is to be fit between the two outermost stations being used, and a middle
     *  station needs to be picked as well (need three points for a circle fit).
     *  This in turn depends on which stations are presently being ignored
     *  e.g. for a 5 pt track, station 5 and station 1 are always  the outer and inner
     *  stations respectively, and station 3 is the middle station.
     *  This function returns the correct outer, inner, and middle
     *  station numbers, given which stations are presently being ignored.
     *
     *  NB Stations are number 0 - 4 in the code, not 1 - 5 as in the outside world
     *
     *  @param ignore_stations - Vector of ints, holding which stations should be ignored
     *  @param o_stat_num - The outermost station number used for a given track fit
     *  @param i_stat_num - The innermost station number used for a given track fit
     *  @param mid_stat_num - the middle station number used for a given track fit
     *
     */
    bool set_seed_stations(const std::vector<int> ignore_stations, int &o_stat_num,
                           int &i_stat_num, int &mid_stat_num) const;


    bool set_ignore_stations(const std::vector<int> &ignore_stations,
                             int &ignore_station_1, int &ignore_station_2) const;

    /** @brief Return the value for the Bz field in the upstream tracker, in tracker coords */
    double get_bz_t1() const { return _bz_t2; }

    /** @brief Set the value for the Bz field in the upstream tracker, in tracker coords */
    void set_bz_t1(double bz_t1) { _bz_t1 = bz_t1; }

    /** @brief Return the value for the Bz field in the downstream tracker, in tracker coords */
    double get_bz_t2() const { return _bz_t2; }

    /** @brief Set the value for the Bz field in the downstream tracker, in tracker coords */
    void set_bz_t2(double bz_t2) { _bz_t2 = bz_t2; }

    /** @brief Return the whether straight pat rec is on in TkUS */
    bool get_up_straight_pr_on() { return _up_straight_pr_on; }

    /** @brief Set whether or not to use straight pat rec in TkUS */
    void set_up_straight_pr_on(const bool up_straight_pr_on)
      { _up_straight_pr_on = up_straight_pr_on; }

    /** @brief Return the whether straight pat rec is on in TkDS */
    bool get_down_straight_pr_on() { return _down_straight_pr_on; }

    /** @brief Set whether or not to use straight pat rec in TkDS */
    void set_down_straight_pr_on(const bool down_straight_pr_on)
      { _down_straight_pr_on = down_straight_pr_on; }

    /** @brief Return the whether helical pat rec is on in TkUS */
    bool get_up_helical_pr_on() { return _up_helical_pr_on; }

    /** @brief Set whether or not to use helical pat rec in TkUS */
    void set_up_helical_pr_on(const bool up_helical_pr_on) { _up_helical_pr_on = up_helical_pr_on; }

    /** @brief Return the whether helical pat rec is on TkDS */
    bool get_down_helical_pr_on() { return _down_helical_pr_on; }

    /** @brief Set whether or not to use helical pat rec in TkDS */
    void set_down_helical_pr_on(const bool down_helical_pr_on)
      { _down_helical_pr_on = down_helical_pr_on; }

    /** @brief Return the verbosity level */
    bool get_verbosity() { return _verb; }

    /** @brief Set the verbosity level */
    void set_verbosity(const bool verb) { _verb = verb; }

    /** @brief A function to set all the internal parameters to their default values (for tests) */
    void set_parameters_to_default();

    /** @brief Place the different cut value currently being used into the variables supplied */
    void get_cuts(double& res_cut, double& straight_chisq_cut, double& R_res_cut,
       double& circle_chisq_cut, double& n_turns_cut, double& sz_chisq_cut);

    /** @brief Set the various cuts used in Pattern Recognition */
    void set_cuts(double res_cut, double straight_chisq_cut, double R_res_cut,
        double circle_chisq_cut, double n_turns_cut, double sz_chisq_cut);

    /** @brief Activate debug mode (set up the output ROOT file, histos, etc) */
    void setup_debug();

  private:
    bool _debug;                /** Run in debug mode */
    bool _up_straight_pr_on;    /** Upstream straight pattern recogntion on or off */
    bool _down_straight_pr_on;  /** Downstream straight pattern recogntion on or off */
    bool _up_helical_pr_on;     /** Upstream Helical pattern recogntion on or off */
    bool _down_helical_pr_on;   /** Downstream Helical pattern recogntion on or off */
    int _verb;                  /** Verbosity: 0=little, 1=more couts */
    int _n_trackers;            /** Number of trackers */
    int _n_stations;            /** Number of stations per tracker */
    double _bz_t1;              /** Bz field in upstream tracker */
    double _bz_t2;              /** Bz field in downstream tracker */
    double _sd_1to4;            /** Position error associated with stations 1 t0 4 */
    double _sd_5;               /** Position error associated with station 5 */
    double _sd_phi_1to4;        /** Rotation error associated with stations 1 t0 4 */
    double _sd_phi_5;           /** Rotation error associated with station 5 */
    double _res_cut;            /** Road cut for linear fit in mm */
    double _straight_chisq_cut; /** Cut on the chi^2 of the least sqs fit in mm */
    double _R_res_cut;          /** Cut on the radius of the track helix in mm */
    double _circle_chisq_cut;   /** Cut on the chi^2 of the circle least sqs fit in mm */
    double _n_turns_cut;        /** Cut to decide if a given n turns value is good */
    double _sz_chisq_cut;       /** Cut on the sz chi^2 from least sqs fit in mm */
    double _Pt_max;             /** MeV/c max Pt for h tracks (given by R_max = 150mm) */
    double _Pz_min;             /** MeV/c min Pz for helical tracks (this is a guess) */
    // LeastSquaresFitter _lsq;  /** The linear least squares fitting class instance */
    TFile* _rfile;   /** A ROOT file pointer for dumping residuals to in debug mode */
    TH1D* _hx;       /** histo of x residuals taken during straight road cut stage */
    TH1D* _hy;       /** histo of y residuals taken during straight road cut stage */
    TH1D* _hxchisq;  /** histo of chisq of every x-z straight least sq fit tried */
    TH1D* _hychisq;  /** histo of chisq of every y-z straight least sq fit tried */
};

// Two predicate functions used by the stl sort algorithm to sort spacepoints in vectors
bool compare_spoints_ascending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() < sp2->get_position().z());
}

bool compare_spoints_descending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() > sp2->get_position().z());
}

// Predicate function to sort tracks by their combined fit chisq
template<typename T>
bool compare_tracks_ascending_chisq(const T *trk1,
                                    const T *trk2) {
  return (trk1->get_chi_squared() < trk2->get_chi_squared());
}

template<typename T>
std::vector<T*> PatternRecognition::select_tracks(std::vector<T*> &trks) const {

    // Sort the tracks by combined chisq (cannot do in place due to preserving constness)
  std::vector<T*> sorted_tracks = trks;
  std::sort(sorted_tracks.begin(), sorted_tracks.end(), compare_tracks_ascending_chisq<T>);

  // Now loop over tracks and pull out highest chisq distinct tracks
  std::vector<T*> accepted_tracks;
  for (auto trk : sorted_tracks) {
    std::vector<SciFiSpacePoint*> spoints = trk->get_spacepoints_pointers();
    int n_used = 0;
    for (auto sp : spoints) {
      if (sp->get_used()) ++n_used;
    }
    // Accept the track if it has enough unused spacepoints
    // if (static_cast<size_t>(n_used) < (spoints.size() - 1)) {
    if (static_cast<size_t>(n_used) < 2) {
      // Set the spacepoints to used (they are pointers, so applies to all tracks which hold them)
      for (auto sp : spoints) {
        sp->set_used(true);
      }
      accepted_tracks.push_back(trk);
    }
  }
  return accepted_tracks;
}

} // ~namespace MAUS

#endif
