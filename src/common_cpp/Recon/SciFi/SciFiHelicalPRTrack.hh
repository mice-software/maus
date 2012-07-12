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

#ifndef  SCIFIHELICALPRTRACK_HH
#define SCIFIHELICALPRTRACK_HH

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"

// namespace MAUS {

class SciFiHelicalPRTrack {
  public:
    // Constructors
    SciFiHelicalPRTrack();
    SciFiHelicalPRTrack(int tracker, double x0, double y0, double z0,
                        double phi0, double psi0, double dzds, double R);
    // SciFiHelicalPRTrack(int tracker, std::vector<double> r0, int station,
    //                     double phi0, double psi0, double dzds, double R);
    SciFiHelicalPRTrack(int tracker, int num_points, CLHEP::Hep3Vector pos0, SimpleHelix helix);

    // Destructors
    ~SciFiHelicalPRTrack();  // Default destructor

    // Getters
    std::vector<SciFiSpacePoint> get_spacepoints() const { return _spoints; }
    // std::vector<double> get_r0() const { return _r0; }

    double get_x0() const { return _x0; }
    double get_y0() const { return _y0; }
    double get_z0() const { return _z0; }
    double get_phi0() const { return _phi0; }
    double get_psi0() const { return _psi0; }
    double get_dzds() const { return _dzds; }
    double get_R() const { return _R; }
    int get_tracker() const { return _tracker; }
    // int get_station() const { return _station; }
    int get_num_points() const { return _num_points; }
    double get_chisq() const { return _chisq; }
    double get_chisq_dof() const { return _chisq_dof; }

    std::vector<double> get_vsl();

    // Setters
    void set_spacepoints(std::vector<SciFiSpacePoint> spoints) { _spoints = spoints; }

    void set_x0(double x0) { _x0 = x0; }
    void set_y0(double y0) { _y0 = y0; }
    void set_z0(double z0) { _z0 = z0; }
    void set_phi0(double phi0) { _phi0 = phi0; }
    void set_psi0(double psi0) { _psi0 = psi0; }
    void set_dzds(double dzds) { _dzds = dzds; }
    void set_R(double R) { _R = R; }
    void set_num_points(int num_points) { _num_points = num_points; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_chisq_dof(double chisq_dof) { _chisq_dof = chisq_dof; }

    void set_tracker(int tracker) { _tracker = tracker; }
    // void set_station(double station) { _station = station; }

    // Print track parameters
    void print_params();

  private:
    std::vector<SciFiSpacePoint> _spoints;

    std::vector<double> _vsl;
    // std::vector<double> r0;

    int _tracker;
    int _num_points;
    // int _station;
    double _x0;
    double _y0;
    double _z0;
    double _phi0;
    double _psi0;
    double _dzds;
    double _R;
    double _chisq;
    double _chisq_dof;
};
// } // ~namespace MAUS

#endif
