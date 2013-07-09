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

/** @class SimpleHelix */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SIMPLEHELIX_HH_
#define _SRC_COMMON_CPP_RECON_SCIFI_SIMPLEHELIX_HH_

namespace MAUS {

class SimpleHelix {
  public:
    /** Default constructor */
    SimpleHelix();

    /** Parameter constructor */
    SimpleHelix(double Phi_0, double Phi_0_err, double R, double R_err, double tan_lambda,
                double tan_lambda_err, double Psi_0, double Psi0_err, double chisq,
                double chisq_dof);

    /** Destructor */
    ~SimpleHelix();

    // Getters
    double get_Phi_0() const { return _Phi_0; }
    double get_Phi_0_err() const { return _Phi_0_err; }
    double get_R() const { return _R; }
    double get_R_err() const { return _R_err; }
    double get_tan_lambda() const { return _tan_lambda; }
    double get_tan_lambda_err() const { return _tan_lambda_err;}
    double get_dsdz() const { return _dsdz; }
    double get_dsdz_err() const { return _dsdz_err; }
    double get_Psi_0() const { return _Psi_0; }
    double get_Psi_0_err() const { return _Psi_0_err; }
    double get_chisq() const { return _chisq; }
    double get_chisq_dof() const { return _chisq_dof; }

    // Setters
    void clear();
    void set_Phi_0(double Phi_0) { _Phi_0 = Phi_0; }
    void set_Phi_0_err(double Phi_0_err) { _Phi_0_err = Phi_0_err; }
    void set_R(double R) { _R = R; }
    void set_R_err(double R_err) { _R_err = R_err; }
    void set_tan_lambda(double tan_lambda) { _tan_lambda = tan_lambda; }
    void set_tan_lambda_err(double tan_lambda_err) { _tan_lambda_err = tan_lambda_err; }
    void set_dsdz(double dsdz) { _dsdz = dsdz; }
    void set_dsdz_err(double dsdz_err) { _dsdz_err = dsdz_err; }
    void set_Psi_0(double Psi_0) { _Psi_0 = Psi_0; }
    void set_Psi_0_err(double Psi_0_err) { _Psi_0_err = Psi_0_err; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_chisq_dof(double chisq_dof) { _chisq_dof = chisq_dof; }
    void set_parameters(double Phi_0, double Phi_0_err, double R, double R_err, double tan_lambda,
                        double tan_lambda_err, double Psi_0, double Psi_0_err, double chisq,
                        double chisq_dof);

  private:
    double _Phi_0;
    double _Phi_0_err;
    double _R;
    double _R_err;
    double _tan_lambda;
    double _tan_lambda_err;
    double _dsdz;
    double _dsdz_err;
    double _Psi_0;
    double _Psi_0_err;
    double _chisq;
    double _chisq_dof;
};

} // ~namespace MAUS

#endif
