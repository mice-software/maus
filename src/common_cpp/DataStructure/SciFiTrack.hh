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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFITRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFITRACK_HH_

#include <TMatrixD.h>

// C++ headers
#include <vector>

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

namespace MAUS {

class SciFiTrack {
 public:
  /** @brief Constructor.
   */
  SciFiTrack();

  /** @brief Destructor, deletes the track points
   */
  virtual ~SciFiTrack();

  /** @brief Copy constructor, deeps copies the track points
   */
  SciFiTrack(const SciFiTrack &a_track);

  /** @brief Assignment operator, deeps copies the track points
   */
  SciFiTrack& operator=(const SciFiTrack &a_track);

  /** @brief Sets the tracker number.
   */
  void set_tracker(int tracker) { _tracker = tracker; }

  /** @brief Sets the filtered chi2.
   */
  void set_chi2(double chi2) { _chi2 = chi2; }

  /** @brief Sets the ndf for this track.
   */
  void set_ndf(int ndf)      { _ndf = ndf; }

  /** @brief Sets the P-value for the chi2 and ndf found.
   */
  void set_P_value(double p_value) { _P_value = p_value; }

  /** @brief Sets the charge of the fitted particle.
   */
  void set_charge(int charge) { _charge = charge; }

  /** @brief Sets trackpoints vector, shallow copy
   */
  void set_scifitrackpoints(SciFiTrackPointPArray points) { _trackpoints = points; }

  /** @brief  Returns the mother pr_tracks as a TRef*.
   */
  void set_pr_track(TRef* const pr_track) { _pr_track = pr_track; }

  /** @brief  Returns the mother pr_track as a TObject pointer
   */
  void set_pr_track_tobject(TObject* const pr_track) { *_pr_track = pr_track; }

  /** @brief  Returns the mother pr_tracks as an array of pointers.
   */
  void set_pr_track_pointer(SciFiBasePRTrack* const pr_track) { *_pr_track = pr_track; }


  /** @brief Returns the tracker number.
   */
  int tracker() const      { return _tracker; }

  /** @brief Returns the filtered chi2.
   */
  double chi2()    const   { return _chi2; }

  /** @brief Returns the ndf.
   */
  int ndf()     const   { return _ndf; }

  /** @brief Returns the P-value.
   */
  double P_value()    const   { return _P_value; }

  /** @brief Returns the charge of the fitted particle.
   */
  int charge()        const   { return _charge; }

  /** @brief Returns the array of SciFiTrackPoints which belong to this track.
   */
  SciFiTrackPointPArray scifitrackpoints()      const   { return _trackpoints; }

  /** @brief Adds a SciFiTrackPoint to the member array.
   */
  void add_scifitrackpoint(SciFiTrackPoint* trackpoint) { _trackpoints.push_back(trackpoint); }

  /** @brief  Returns the mother pr_track as a TRef*.
   */
  TRef* pr_track()                const { return _pr_track; }

  /** @brief  Returns the mother pr_track as a TObject pointer
   */
  TObject* pr_track_tobject()     const { return _pr_track->GetObject(); }

  /** @brief  Returns the mother pr_track as a pointer
   */
  SciFiBasePRTrack* pr_track_pointer() const
                                 { return static_cast<SciFiBasePRTrack*>(_pr_track->GetObject()); }

  /** @brief  Returns the mother pr_trackas a helical pointer
   */
  SciFiHelicalPRTrack* pr_track_pointer_helical() const
                              { return static_cast<SciFiHelicalPRTrack*>(_pr_track->GetObject()); }

  /** @brief  Returns the mother pr_tracks as a straight pointer
   */
  SciFiStraightPRTrack* pr_track_pointer_straight() const
                             { return static_cast<SciFiStraightPRTrack*>(_pr_track->GetObject()); }

  /** @brief Enumeration of the possible types of SciFi tracks.
   */
  enum AlgorithmUsed { kalman_straight, kalman_helical };

  /** @brief Returns the type of algorithm used to reconstruct this track.
   */
  int GetAlgorithmUsed() const { return static_cast<int> (_algorithm_used); }

  void SetAlgorithmUsed(int algorithm) { _algorithm_used = (AlgorithmUsed) algorithm; }

  void SetSeedPosition(ThreeVector pos) { this->_seed_position = pos; }

  ThreeVector GetSeedPosition() const { return _seed_position; }

  void SetSeedMomentum(ThreeVector mom) { this->_seed_momentum = mom; }

  ThreeVector GetSeedMomentum() const { return _seed_momentum; }

  void SetSeedCovariance(std::vector<double> seed) { this->_seed_covariance_matrix = seed; }
  void SetSeedCovariance(double* cov, unsigned int size);

  std::vector<double> GetSeedCovariance() const { return _seed_covariance_matrix; }

  int GetNumberDataPoints() const;

  /** @brief Returns an integer rating of the track (1:Excellent, 5:Terrible)
   */
  int GetRating() const { return this->_rating; }
  void SetRating(int rating) { this->_rating = rating; }

  /** @brief Returns an boolean value if it's a "good" track
   */
  int IsGood() const { return (this->GetRating() <= 2); }

 private:
  int _tracker;

  double _chi2;

  int _ndf;

  double _P_value;

  int _rating;

  int _charge;

  AlgorithmUsed _algorithm_used;

  SciFiTrackPointPArray _trackpoints;

  ThreeVector _seed_position;
  ThreeVector _seed_momentum;
  std::vector<double> _seed_covariance_matrix;

  // Pointer to the seed track. This does not assume memory
  TRef* _pr_track;

  MAUS_VERSIONED_CLASS_DEF(SciFiTrack)
};

typedef std::vector<SciFiTrack*> SciFiTrackPArray;
} // ~namespace MAUS

#endif
