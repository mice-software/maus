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

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SIMPLECIRCLE_HH_
#define _SRC_COMMON_CPP_RECON_SCIFI_SIMPLECIRCLE_HH_

#include <vector>

namespace MAUS {

class SimpleCircle {
  public:
    /** Default constructor */
    SimpleCircle();

    /** First parameter constructor */
    SimpleCircle(double x0, double y0, double R);

    /** Second parameter constructor */
    SimpleCircle(double x0, double x0_err, double y0, double y0_err, double R, double R_err,
                 double alpha, double alpha_err, double beta, double beta_err, double gamma,
                 double gamma_err, double kappa, double kappa_err, double delta_R, double chisq);

    /** Destructor */
    ~SimpleCircle();  // Default destructor

    // Getters
    double get_x0() const { return _x0; }
    double get_x0_err() const { return _x0_err; }
    double get_y0() const { return _y0; }
    double get_y0_err() const { return _y0_err; }
    double get_R() const { return _R; }
    double get_R_err() const { return _R_err; }
    double get_alpha() const { return _alpha; }
    double get_alpha_err() const { return _alpha_err; }
    double get_beta() const { return _beta; }
    double get_beta_err() const { return _beta_err; }
    double get_gamma() const { return _gamma; }
    double get_gamma_err() const { return _gamma_err; }
    double get_kappa() const { return _kappa; }
    double get_kappa_err() const { return _kappa_err; }

    double get_chisq() const { return _chisq; }
    double get_pvalue() const { return _pvalue; }
    double get_delta_R() const { return _delta_R; }

    // Setters
    void clear();
    void set_x0(double x0) { _x0 = x0; }
    void set_x0_err(double x0_err) { _x0_err = x0_err; }
    void set_y0(double y0) { _y0 = y0; }
    void set_y0_err(double y0_err) { _y0_err = y0_err; }
    void set_R(double R) { _R = R; }
    void set_R_err(double R_err) { _R_err = R_err; }
    void set_alpha(double alpha) { _alpha = alpha; }
    void set_alpha_err(double alpha_err) { _alpha_err = alpha_err; }
    void set_beta(double beta) { _beta = beta; }
    void set_beta_err(double beta_err) { _beta_err = beta_err; }
    void set_gamma(double gamma) { _gamma = gamma; }
    void set_gamma_err(double gamma_err) { _gamma_err = gamma_err; }
    void set_kappa(double kappa) { _kappa = kappa; }
    void set_kappa_err(double kappa_err) { _kappa_err = kappa_err; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_pvalue(double pvalue) { _pvalue = pvalue; }
    void set_delta_R(double delta_R) { _delta_R = delta_R; }

    void set_parameters(double x0, double x0_err, double y0, double y0_err,
                        double R, double R_err, double alpha, double alpha_err,
                        double beta, double beta_err, double gamma, double gamma_err,
                        double kappa, double kappa_err, double delta_R,
                        double chisq, double pvalue);

  private:
    double _x0;
    double _x0_err;
    double _y0;
    double _y0_err;
    double _R;
    double _R_err;
    double _alpha;
    double _alpha_err;
    double _beta;
    double _beta_err;
    double _gamma;
    double _gamma_err;
    double _kappa;
    double _kappa_err;
    double _delta_R;
    double _chisq;
    double _pvalue;
};

} // ~namespace MAUS

#endif
