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

/** @class SciFiStraightPRTrack */

#ifndef  _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISTRAIGHTPRTRACK_HH
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISTRAIGHTPRTRACK_HH

// C++ headers
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SimpleLine.hh"

namespace MAUS {

class SciFiStraightPRTrack {
  public:
    /** Default constructor */
    SciFiStraightPRTrack();

    /** Constructor using explicit parameters */
    SciFiStraightPRTrack(int tracker, int num_points,
                         double x0, double mx, double x_chisq,
                         double y0, double my, double y_chisq);

    /** Constructor using SimpleLines */
    SciFiStraightPRTrack(int tracker, int num_points, SimpleLine line_x, SimpleLine line_y);

    /** Copy constructor */
    SciFiStraightPRTrack(const SciFiStraightPRTrack &_strk);

    /** Default destructor */
    virtual ~SciFiStraightPRTrack();

    /** Equality operator - any pointers are deep copied */
    SciFiStraightPRTrack& operator=(const SciFiStraightPRTrack &_strk);

    // Getters
    SciFiSpacePointArray get_spacepoints() const { return _spoints; }

    double get_x0() const { return _x0; }
    double get_y0() const { return _y0; }
    double get_mx() const { return _mx; }
    double get_my() const { return _my; }
    double get_x_chisq() const { return _x_chisq; }
    double get_y_chisq() const { return _y_chisq; }

    int get_tracker() const { return _tracker; }
    int get_num_points() const { return _num_points; }

    // Setters
    void set_spacepoints(SciFiSpacePointArray spoints) { _spoints = spoints; }

    void set_x0(double x0) { _x0 = x0; }
    void set_y0(double y0) { _y0 = y0; }
    void set_mx(double mx) { _mx = mx; }
    void set_my(double my) { _my = my; }
    void set_x_chisq(double x_chisq) { _x_chisq = x_chisq; }
    void set_y_chisq(double y_chisq) { _y_chisq = y_chisq; }

    void set_tracker(int tracker) { _tracker = tracker; }
    void set_num_points(int num_points) { _num_points = num_points; }
    // Print track parameters
    void print_params();

  private:
    SciFiSpacePointArray _spoints;

    int _tracker;
    int _num_points;

    double _x0;
    double _y0;
    double _mx;
    double _my;
    double _x_chisq;
    double _y_chisq;

    ClassDef(SciFiStraightPRTrack, 1)
};

typedef std::vector<SciFiStraightPRTrack*> SciFiStraightPRTrackPArray;
typedef std::vector<SciFiStraightPRTrack> SciFiStraightPRTrackArray;

} // ~namespace MAUS

#endif
