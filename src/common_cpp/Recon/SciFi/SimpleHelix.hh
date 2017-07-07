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

#include <vector>

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
    SimpleHelix(const std::vector<double>& params, const std::vector<double>& errors,
                const std::vector<int>& ndfs, const std::vector<double>& chisqs,
                double pvalue, TMatrixD& cov);

    /** Destructor */
    ~SimpleHelix();

    // Getters
    double get_xc() const { return _params[0]; }
    double get_xc_err() const { return _errors[0]; }
    double get_yc() const { return _params[1]; }
    double get_yc_err() const { return _errors[1]; }
    double get_R() const { return _params[2]; }
    double get_R_err() const { return _errors[2]; }
    double get_dsdz() const { return _params[3]; }
    double get_dsdz_err() const { return _errors[3]; }
    double get_s0() const { return _params[4]; }
    double get_s0_err() const { return _errors[4]; }
    double get_transverse_chisq() const { return _chisqs[0]; }
    double get_longitudinal_chisq() const { return _chisqs[1]; }
    double get_chisq() const { return _chisqs[2]; }
    double get_pvalue() const { return _pvalue; }
    TMatrixD get_cov() const { return _cov; }

    // Setters
    void clear();
    void set_xc(double xc) { _params[0] = xc; }
    void set_xc_err(double xc_err) { _errors[0] = xc_err; }
    void set_yc(double yc) { _params[1] = yc; }
    void set_yc_err(double yc_err) { _errors[1] = yc_err; }
    void set_R(double R) { _params[2] = R; }
    void set_R_err(double R_err) { _errors[2] = R_err; }
    void set_dsdz(double dsdz) { _params[3] = dsdz; }
    void set_dsdz_err(double dsdz_err) { _errors[3] = dsdz_err; }
    void set_s0(double s0) { _params[4] = s0; }
    void set_s0_err(double s0_err) { _errors[4] = s0_err; }
    void set_transverse_chisq(double transverse_chisq) { _chisqs[0] = transverse_chisq; }
    void set_longitudinal_chisq(double longitudinal_chisq) { _chisqs[1] = longitudinal_chisq; }
    void set_chisq(double chisq) { _chisqs[2] = chisq; }
    void set_pvalue(double pvalue) { _pvalue = pvalue; }
    void set_cov(TMatrixD cov) { _cov = cov; }
    void set_parameters(const std::vector<double>& params, const std::vector<double>& _errors,
                        const std::vector<int>& ndfs, const std::vector<double>& _chisqs,
                        double pvalue, TMatrixD& cov);

  private:
    std::vector<double> _params;   // xc, yc, rad, dsdz, s0
    std::vector<double> _errors;   // xc, yc, rad, dsdz, s0
    std::vector<int> _ndfs;        // tranverse ndf, longitudinal ndf, overall ndf
    std::vector<double> _chisqs;   // tranverse chisq, longitudinal chisq, overall chisq
    double _pvalue;
    TMatrixD _cov;
};

} // ~namespace MAUS

#endif
