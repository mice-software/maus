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

 // ROOT headers
 #include "TMatrixD.h"

/** @class SimpleHelix */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SIMPLEHELIX_HH_
#define _SRC_COMMON_CPP_RECON_SCIFI_SIMPLEHELIX_HH_

namespace MAUS {

class SimpleHelix {
  public:
    /** Default constructor */
    SimpleHelix();

    /** Parameter constructor */
    SimpleHelix(double xc, double xc_err, double yc, double yc_err, double R, double R_err,
                double lambda, double lambda_err, double s0, double s0_err,
                double chisq, double chisq_dof, double pvalue, TMatrix& cov);

    /** Destructor */
    ~SimpleHelix();

    // Getters
    double get_xc() const { return _xc; }
    double get_xc_err() const { return _xc_err; }
    double get_yc() const { return _xc; }
    double get_yc_err() const { return _xc_err; }
    double get_R() const { return _R; }
    double get_R_err() const { return _R_err; }
    double get_lambda() const { return _lambda; }
    double get_lambda_err() const { return _lambda_err;}
    double get_s0() const { return _s0; }
    double get_s0_err() const { return _s0_err; }
    double get_chisq() const { return _chisq; }
    double get_chisq_dof() const { return _chisq_dof; }
    double get_pvalue() const { return _pvalue; }
    TMatrix get_cov() const { return _cov; }

    // Setters
    void clear();
    void set_xc(double xc) { _xc = xc; }
    void set_xc_err(double xc_err) { _xc_err = xc_err; }
    void set_yc(double yc) { _yc = yc; }
    void set_yc_err(double yc_err) { _yc_err = yc_err; }
    void set_R(double R) { _R = R; }
    void set_R_err(double R_err) { _R_err = R_err; }
    void set_lambda(double lambda) { _lambda = lambda; }
    void set_lambda_err(double lambda_err) { _lambda_err = lambda_err; }
    void set_s0(double s0) { _s0 = s0; }
    void set_s0_err(double s0_err) { _s0_err = s0_err; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_chisq_dof(double chisq_dof) { _chisq_dof = chisq_dof; }
    void set_pvalue(double pvalue) { _pvalue = pvalue; }
    void set_cov(TMatrixD cov) { _cov = cov; }
    void set_parameters(double xc, double xc_err, double yc, double yc_err, double R, double R_err,
                        double lambda, double lambda_err, double s0, double s0_err,
                        double chisq, double chisq_dof, double pvalue, TMatrixD& cov);

  private:
    double _xc;
    double _xc_err;
    double _yc;
    double _yc_err;
    double _R;
    double _R_err;
    double _lambda;
    double _lambda_err;
    double _s0;
    double _s0_err;
    double _chisq;
    double _chisq_dof;
    double _pvalue;
    TMatrixD _cov;
};

} // ~namespace MAUS

#endif
