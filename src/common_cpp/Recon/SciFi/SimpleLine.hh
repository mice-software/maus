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

/** @class SimpleLine
 *
 *
 */

#ifndef  SIMPLELINE_HH
#define SIMPLELINE_HH

// namespace MAUS {

class SimpleLine {
  public:
    SimpleLine();   // Default constructor
    ~SimpleLine();  // Default destructor

    // Getters
    double get_c() const { return _c; }
    double get_c_err() const { return _c_err; }
    double get_m() const { return _m; }
    double get_m_err() const { return _m_err; }
    double get_chisq() const { return _chisq; }
    double get_chisq_dof() const { return _chisq_dof; }

    // Setters
    void set_c(double c) { _c = c; }
    void set_c_err(double c_err) { _c_err = c_err; }
    void set_m(double m) { _m = m; }
    void set_m_err(double m_err) { _m_err = m_err; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_chisq_dof(double chisq_dof) { _chisq_dof = chisq_dof; }
    void set_parameters(double c, double c_err, double m, double m_err,
                        double chisq, double chisq_dof);

  private:
    double _c;
    double _c_err;
    double _m;
    double _m_err;
    double _chisq;
    double _chisq_dof;
};
// } // ~namespace MAUS

#endif
