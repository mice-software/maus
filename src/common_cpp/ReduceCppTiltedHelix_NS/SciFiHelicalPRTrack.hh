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

#ifndef _SRC_REDUCE_REDUCECPPTILTEDHELIX_SCIFIHELICALPRTRACK_HH_
#define _SRC_REDUCE_REDUCECPPTILTEDHELIX_SCIFIHELICALPRTRACK_HH_

// C++ headers
#include <vector>

// ROOT headers
#include "TRefArray.h"

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/ReduceCppTiltedHelix_NS/SimpleCircle.hh"

namespace MAUS {
namespace ReduceCppTiltedHelix_NS {

class SciFiHelicalPRTrack : public MAUS::SciFiBasePRTrack {
  public:

    /** Default constructor */
    SciFiHelicalPRTrack();

    /** Constructor from SimpleCircle and SimpleLine */
    SciFiHelicalPRTrack(int tracker, int num_points, int charge, ThreeVector pos0, double phi0,
                        SimpleCircle circle, SimpleLine line_sz, double chisq, double chisq_dof,
                        double point_spread, DoubleArray phi, SciFiSpacePointPArray spoints,
                        const DoubleArray& covariance);

    /** Constructor from SimpleCircle and SimpleLine Without explicit chi squared setting */
    SciFiHelicalPRTrack(int tracker, int num_points, int charge, ThreeVector pos0, double phi0,
                        SimpleCircle circle, SimpleLine line_sz, double point_spread,
                        DoubleArray phi, SciFiSpacePointPArray spoints,
                        const DoubleArray& covariance);

    /** Copy constructor - any pointers are deep copied */
    SciFiHelicalPRTrack(const SciFiHelicalPRTrack &_htrk);

    /** Destructors */
    virtual ~SciFiHelicalPRTrack();

    /** Assignment operator - any pointers are deep copied */
    SciFiHelicalPRTrack& operator=(const SciFiHelicalPRTrack &_htrk);

    /** Get the vector the turning angles of the spacepoints used by the track  */
    DoubleArray get_phi() const { return _phi; }

    /** Set the vector the turning angles of the spacepoints used by the track */
    void set_phi(DoubleArray phi) { _phi = phi; }

    /** Get the tracker number */
    int get_tracker() const { return _tracker; }

    /** Set the tracker number */
    void set_tracker(int tracker) { _tracker = tracker; }

    /** Get the number of points in a track (5, 4, or 3) */
    int get_num_points() const { return _num_points; }

    /** Set the number of points in a track (5, 4, or 3) */
    void set_num_points(int num_points) { _num_points = num_points; }

    /** Get the track charge */
    int get_charge() const { return _charge; }

    /** Set the track charge */
    void set_charge(int charge) { _charge = charge; }

    /** Get the position of the intercept of the track with the tracker ref surface */
    ThreeVector get_pos0() const { return _pos0; }

    /** Set the position of the intercept of the track with the tracker ref surface */
    void set_pos0(ThreeVector pos0) { _pos0 = pos0; }

    /** Get phi0, angle between the helix centre & the rotated frame origin in x-y */
    double get_phi0() const { return _phi0; }

    /** Get phi0, angle between the helix centre & the rotated frame origin in x-y */
    void set_phi0(double phi0) { _phi0 = phi0; }

    /** Get dsdz, the slope in the s-z plane (s is path length) */
    double get_dsdz() const { return _dsdz; }

    /** Set dsdz, the slope in the s-z plane (s is path length) */
    void set_dsdz(double dsdz) { _dsdz = dsdz; }

    /** Get R the radius of the helix in the x-y plane */
    double get_R() const { return _R; }

    /** Set R the radius of the helix in the x-y plane */
    void set_R(double R) { _R = R; }

    /** Get the intercept of the straight line fit in s-z */
    double get_line_sz_c() const { return _line_sz_c; }

    /** Set the intercept of the straight line fit in s-z */
    void set_line_sz_c(double line_sz_c) { _line_sz_c = line_sz_c; }

    /** Get the chi^2 of the straight line fit in s-z */
    double get_line_sz_chisq() const { return _line_sz_chisq; }

    /** Set the chi^2 of the straight line fit in s-z */
    void set_line_sz_chisq(double line_sz_chisq) { _line_sz_chisq = line_sz_chisq; }

    /** Get the x coordinate of the centre of the circle */
    double get_circle_x0() const { return _circle_x0; }

    /** Set the x coordinate of the centre of the circle */
    void set_circle_x0(double circle_x0) { _circle_x0 = circle_x0; }

    /** Get the y coordinate of the centre of the circle */
    double get_circle_y0() const { return _circle_y0; }

    /** Set the y coordinate of the centre of the circle */
    void set_circle_y0(double circle_y0) { _circle_y0 = circle_y0; }

    /** Get the chi^2 of the circle fit in x-y */
    double get_circle_chisq() const { return _circle_chisq; }

    /** Set the chi^2 of the circle fit in x-y */
    void set_circle_chisq(double circle_chisq) { _circle_chisq = circle_chisq; }

    /** Get the chi^2 of the final helix fit, if used */
    double get_chisq() const { return _chisq; }

    /** Set the chi^2 of the final helix fit, if used */
    void set_chisq(double chisq) { _chisq = chisq; }

    /** Get the chi^2 per degree of freedom of the final helix fit, if used */
    double get_chisq_dof() const { return _chisq_dof; }

    /** Set the chi^2 per degree of freedom of the final helix fit, if used */
    void set_chisq_dof(double chisq_dof) { _chisq_dof = chisq_dof; }

    /** Get the point spread (summed separations) in x - y of the seed spacepoints */
    double get_point_spread() const { return _point_spread; }

    /** Set the point spread (summed separations) in x - y of the seed spacepoints */
    void set_point_spread(double point_spread) { _point_spread = point_spread; }

    /** Get the track type, for straight, 1 for helical */
    int get_type() const { return _type; }

    std::vector<double> get_fit_parameters() {return _fit_parameters;}

    void set_fit_parameters(std::vector<double> pars) {_fit_parameters = pars;}

  private:
    int _tracker;
    int _num_points;
    int _charge;
    static const int _type = 1; // 0 for straight, 1 for helical

    double _R;
    double _phi0;
    double _dsdz;
    double _line_sz_c;
    double _line_sz_chisq;
    double _circle_x0;
    double _circle_y0;
    double _circle_chisq;
    double _chisq;
    double _chisq_dof;
    double _point_spread;

    std::vector<double> _fit_parameters;

    ThreeVector _pos0; // Intercept of helix with tracker reference surface, not used at present
    DoubleArray _phi;
};

typedef std::vector<SciFiHelicalPRTrack*> SciFiHelicalPRTrackPArray;
typedef std::vector<SciFiHelicalPRTrack> SciFiHelicalPRTrackArray;
}
} // ~namespace MAUS

#endif
