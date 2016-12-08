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

/** @class SimpleCircle */

#ifndef _SRC_REDUCE_REDUCECPPTILTEDHELIX_SIMPLECIRCLE_HH_
#define _SRC_REDUCE_REDUCECPPTILTEDHELIX_SIMPLECIRCLE_HH_

#include <vector>

namespace MAUS {
namespace ReduceCppTiltedHelix_NS {

class SimpleCircle {
  public:
    /** Default constructor */
    SimpleCircle();

    /** First parameter constructor */
    SimpleCircle(double x0, double y0, double R);

    /** Second parameter constructor */
    SimpleCircle(double x0, double x0_err, double y0, double y0_err, double R, double R_err,
                 double kappa, double kappa_err, double delta_R, double chisq,
                 std::vector<double> fit_parameters, std::vector<double> fit_parameter_errors);

    /** Destructor */
    ~SimpleCircle();  // Default destructor

    // Getters
    double get_x0() const { return _x0; }
    double get_x0_err() const { return _x0_err; }
    double get_y0() const { return _y0; }
    double get_y0_err() const { return _y0_err; }
    double get_R() const { return _R; }
    double get_R_err() const { return _R_err; }
    double get_kappa() const { return _kappa; }
    double get_kappa_err() const { return _kappa_err; }
    double get_theta_x() const { return _theta_x; }
    double get_theta_y() const { return _theta_y; }

    std::vector<double> get_fit_parameters() const { return _fit_parameters; }
    std::vector<double> get_fit_parameter_errors() const { return _fit_parameter_errors; }

    double get_chisq() const { return _chisq; }
    double get_delta_R() const { return _delta_R; }

    // Setters
    void clear();
    void set_x0(double x0) { _x0 = x0; }
    void set_x0_err(double x0_err) { _x0_err = x0_err; }
    void set_y0(double y0) { _y0 = y0; }
    void set_y0_err(double y0_err) { _y0_err = y0_err; }
    void set_R(double R) { _R = R; }
    void set_R_err(double R_err) { _R_err = R_err; }
    void set_kappa(double kappa) { _kappa = kappa; }
    void set_kappa_err(double kappa_err) { _kappa_err = kappa_err; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_delta_R(double delta_R) { _delta_R = delta_R; }

    void set_fit_parameters(std::vector<double> fit_pars) {_fit_parameters = fit_pars; }
    void set_fit_parameter_errors(std::vector<double> fit_par_errs) { _fit_parameter_errors = fit_par_errs;}

    void set_parameters(double x0, double x0_err, double y0, double y0_err,
                        double R, double R_err, double kappa, double kappa_err,
                        double delta_R, double chisq,
                        std::vector<double> fit_parameters,
                        std::vector<double> fit_parameter_errors);

  private:
    double _x0;
    double _x0_err;
    double _y0;
    double _y0_err;
    double _R;
    double _R_err;
    double _theta_x;
    double _theta_y;
    std::vector<double> _fit_parameters;
    std::vector<double> _fit_parameter_errors;
    double _kappa;
    double _kappa_err;
    double _delta_R;
    double _chisq;
};
}
} // ~namespace MAUS

#endif
