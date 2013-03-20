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
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
// #include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"

namespace MAUS {

class SciFiTrack {
  public:

    SciFiTrack();

    virtual ~SciFiTrack();

    SciFiTrack(const SciFiTrack &a_track);

    explicit SciFiTrack(const KalmanTrack *kalman_track);

    SciFiTrack& operator=(const SciFiTrack &a_track);

    int  tracker() const      { return _tracker; }
    void set_tracker(int tracker) { _tracker = tracker; }

    double f_chi2()    const   { return _f_chi2; }
    void   set_f_chi2(double chi2) { _f_chi2 = chi2; }

    double s_chi2()    const   { return _s_chi2; }
    void   set_s_chi2(double chi2) { _s_chi2 = chi2; }

    int  ndf()     const   { return _ndf; }
    void set_ndf(int ndf)      { _ndf = ndf; }

    double P_value()    const   { return _P_value; }
    void   set_P_value(double p_value) { _P_value = p_value; }

/*
    SciFiTrackPointPArray scifitrackpoints()          const      { return _scifitrackpoints; }
    void set_scifitrackpoints(SciFiTrackPointPArray trackpoints) { _scifitrackpoints = trackpoints; }
    void add_scifitrackpoint(SciFiTrackPoint *trackpoint)        { _scifitrackpoints.push_back(trackpoint); }
*/
  private:
    int _tracker;

    double _f_chi2;

    double _s_chi2;

    int _ndf;

    double _P_value;

    // /** Kalman track points */
    // SciFiTrackPointPArray _scifitrackpoints;

    MAUS_VERSIONED_CLASS_DEF(SciFiTrack)
};

typedef std::vector<SciFiTrack*> SciFiTrackPArray;

} // ~namespace MAUS

#endif
