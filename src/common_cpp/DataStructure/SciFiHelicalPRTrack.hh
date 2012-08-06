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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIHELICALPRTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIHELICALPRTRACK_HH_

// C++ headers
#include <vector>

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SimpleHelix.hh"
#include "src/common_cpp/DataStructure/SimpleLine.hh"
#include "src/common_cpp/DataStructure/SimpleCircle.hh"

namespace MAUS {

class SciFiHelicalPRTrack {
  public:

    /** Default constructor */
    SciFiHelicalPRTrack();

    /** Constructor from parameters */
    SciFiHelicalPRTrack(int tracker, int num_points, double x0, double y0, double z0, double phi0,
                        double psi0, double dsdz, double R, double chisq);

    /** Constructor from SimpleCircle and SimpleLine */
    SciFiHelicalPRTrack(int tracker, int num_points, CLHEP::Hep3Vector pos0, SimpleCircle circle,
                        SimpleLine line_sz);

    /** Constructor from SimpleHelix */
    SciFiHelicalPRTrack(int tracker, int num_points, CLHEP::Hep3Vector pos0, SimpleHelix helix);

    /** Copy constructor - any pointers are deep copied */
    SciFiHelicalPRTrack(const SciFiHelicalPRTrack &_htrk);

    /** Destructors */
    virtual ~SciFiHelicalPRTrack();

    /** Assignment operator - any pointers are deep copied */
    SciFiHelicalPRTrack& operator=(const SciFiHelicalPRTrack &_htrk);

    // Getters
    SciFiSpacePointArray get_spacepoints() const { return _spoints; }

    int get_tracker() const { return _tracker; }
    int get_num_points() const { return _num_points; }

    double get_x0() const { return _x0; }
    double get_y0() const { return _y0; }
    double get_z0() const { return _z0; }
    double get_phi0() const { return _phi0; }
    double get_psi0() const { return _psi0; }
    double get_dsdz() const { return _dsdz; }
    double get_R() const { return _R; }
    double get_chisq() const { return _chisq; }
    double get_chisq_dof() const { return _chisq_dof; }

    // Setters
    void set_tracker(int tracker) { _tracker = tracker; }
    void set_spacepoints(SciFiSpacePointArray spoints) { _spoints = spoints; }
    void set_num_points(int num_points) { _num_points = num_points; }
    void set_x0(double x0) { _x0 = x0; }
    void set_y0(double y0) { _y0 = y0; }
    void set_z0(double z0) { _z0 = z0; }
    void set_phi0(double phi0) { _phi0 = phi0; }
    void set_psi0(double psi0) { _psi0 = psi0; }
    void set_dsdz(double dsdz) { _dsdz = dsdz; }
    void set_R(double R) { _R = R; }

    void set_chisq(double chisq) { _chisq = chisq; }
    void set_chisq_dof(double chisq_dof) { _chisq_dof = chisq_dof; }

    /** Print track parameters */
    void print_params();

  private:

    double _x0;
    double _y0;
    double _z0;
    double _phi0;
    double _psi0;
    double _dsdz;
    double _R;
    double _chisq;
    double _chisq_dof;

    int _tracker;
    int _num_points;

    SciFiSpacePointArray _spoints;

    ClassDef(SciFiHelicalPRTrack, 1)
};

typedef std::vector<SciFiHelicalPRTrack*> SciFiHelicalPRTrackPArray;
typedef std::vector<SciFiHelicalPRTrack> SciFiHelicalPRTrackArray;

} // ~namespace MAUS

#endif
