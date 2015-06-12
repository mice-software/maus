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

/** @class SciFiStraightPRTrack Pattern Recognition straight track class */

#ifndef  _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISTRAIGHTPRTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISTRAIGHTPRTRACK_HH_

// C++ headers
#include <vector>

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"

namespace MAUS {

class SciFiStraightPRTrack : public SciFiBasePRTrack {
  public:
    /** Default constructor */
    SciFiStraightPRTrack();

    /** Constructor using explicit parameters */
    SciFiStraightPRTrack(int tracker, int num_points, double x0, double mx, double x_chisq,
                         double y0, double my, double y_chisq,
                         const DoubleArray& covariance);

    /** Constructor using SimpleLines */
    SciFiStraightPRTrack(int tracker, int num_points, SimpleLine line_x, SimpleLine line_y,
                         const DoubleArray& covariance);

    /** Copy constructor */
    SciFiStraightPRTrack(const SciFiStraightPRTrack &_strk);

    /** Default destructor */
    virtual ~SciFiStraightPRTrack();

    /** Equality operator - any pointers are deep copied */
    SciFiStraightPRTrack& operator=(const SciFiStraightPRTrack &_strk);


    /** Get the x position of the track at z=0 */
    double get_x0() const { return _x0; }

    /** Set the x position of the track at z=0 */
    void set_x0(double x0) { _x0 = x0; }

    /** Get the gradient of the line in x-z */
    double get_mx() const { return _mx; }

    /** Set the gradient of the line in x-z */
    void set_mx(double mx) { _mx = mx; }

    /** Get the chi_2 of the line fit in x-z */
    double get_x_chisq() const { return _x_chisq; }

    /** Set the chi_2 of the line fit in x-z */
    void set_x_chisq(double x_chisq) { _x_chisq = x_chisq; }

    /** Get the y position of the track at z=0 */
    double get_y0() const { return _y0; }

    /** Set the y position of the track at z=0 */
    void set_y0(double y0) { _y0 = y0; }

    /** Get the gradient of the line in y-z */
    double get_my() const { return _my; }

    /** Set the gradient of the line in y-z */
    void set_my(double my) { _my = my; }

    /** Get the chi_2 of the line fit in y-z */
    double get_y_chisq() const { return _y_chisq; }

    /** Set the chi_2 of the line fit in y-z */
    void set_y_chisq(double y_chisq) { _y_chisq = y_chisq; }

    /** Get the tracker number */
    int get_tracker() const { return _tracker; }

    /** Set the tracker number */
    void set_tracker(int tracker) { _tracker = tracker; }

    /** Get the number of points in a track (5, 4, or 3) */
    int get_num_points() const { return _num_points; }

    /** Set the number of points in a track (5, 4, or 3) */
    void set_num_points(int num_points) { _num_points = num_points; }

    /** Get the track type, for straight, 1 for helical */
    int get_type() const { return _type; }


  private:
    int _tracker;
    int _num_points;
    static const int _type = 0; // 0 for straight, 1 for helical

    double _x0;
    double _mx;
    double _x_chisq;
    double _y0;
    double _my;
    double _y_chisq;

    MAUS_VERSIONED_CLASS_DEF(SciFiStraightPRTrack)
};

typedef std::vector<SciFiStraightPRTrack*> SciFiStraightPRTrackPArray;
typedef std::vector<SciFiStraightPRTrack> SciFiStraightPRTrackArray;

} // ~namespace MAUS

#endif
