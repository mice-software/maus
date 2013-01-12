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

/** @class SciFiHelicalPRTrack
 *
 *
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFITRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFITRACK_HH_

#include <TMatrixD.h>

// C++ headers
#include <vector>

// MAUS headers
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
// #include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
// #include "src/common_cpp/Recon/Global/Detector.hh"
// #include "src/common_cpp/Recon/Global/TrackPoint.hh"

namespace MAUS {

class SciFiTrack {
  public:

    SciFiTrack();

    ~SciFiTrack();

    SciFiTrack(const SciFiTrack &a_track);

    explicit SciFiTrack(const KalmanTrack *kalman_track);

    SciFiTrack& operator=(const SciFiTrack &a_track);

    // void add_track_points(const std::vector<KalmanSite> &sites);

    int  get_tracker() const      { return _tracker; }
    void set_tracker(int tracker) { _tracker = tracker; }

    double get_chi2()    const   { return _chi2; }
    void   set_chi2(double chi2) { _chi2 = chi2; }

    int  get_ndf()     const   { return _ndf; }
    void set_ndf(int ndf)      { _ndf = ndf; }

    double get_P_value()    const   { return _P_value; }
    void   set_P_value(double p_value) { _P_value = p_value; }

  private:
    int _tracker;

    double _chi2;

    int _ndf;

    double _P_value;

    ClassDef(SciFiTrack, 1)
};

typedef std::vector<SciFiTrack*> SciFiTrackPArray;

} // ~namespace MAUS

#endif
