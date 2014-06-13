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
  void set_f_chi2(double chi2) { _f_chi2 = chi2; }

  /** @brief Sets the smoothed chi2.
   */
  void set_s_chi2(double chi2) { _s_chi2 = chi2; }

  /** @brief Sets the ndf for this track.
   */
  void set_ndf(int ndf)      { _ndf = ndf; }

  /** @brief Sets the P-value for the chi2 and ndf found.
   */
  void set_P_value(double p_value) { _P_value = p_value; }

  /** @brief Sets the charge of the fitted particle.
   */
  void set_charge(int charge) { _charge = charge; }

  /** @brief Sets trackpoints vector, deep copy
   */
  void set_scifitrackpoints(SciFiTrackPointPArray points);

  /** @brief Returns the tracker number.
   */
  int tracker() const      { return _tracker; }

  /** @brief Returns the filtered chi2.
   */
  double f_chi2()    const   { return _f_chi2; }

  /** @brief Returns the smoothed chi2.
   */
  double s_chi2()    const   { return _s_chi2; }

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

  /** @brief Enumeration of the possible types of SciFi tracks.
   */
  enum AlgorithmUsed { kalman_straight, kalman_helical };

  /** @brief Returns the type of algorithm used to reconstruct this track.
   */
  int GetAlgorithmUsed() const { return static_cast<int> (_algorithm_used); }

  void SetAlgorithmUsed(int algorithm) { _algorithm_used = (AlgorithmUsed) algorithm; }

 private:
  int _tracker;

  double _f_chi2;

  double _s_chi2;

  int _ndf;

  double _P_value;

  int _charge;

  AlgorithmUsed _algorithm_used;

  SciFiTrackPointPArray _trackpoints;

  MAUS_VERSIONED_CLASS_DEF(SciFiTrack)
};

typedef std::vector<SciFiTrack*> SciFiTrackPArray;
} // ~namespace MAUS

#endif
