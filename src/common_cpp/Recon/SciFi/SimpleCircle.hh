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

/** @class SimpleCircle
 *
 *
 */

#ifndef  SIMPLECIRCLE_HH
#define SIMPLECIRCLE_HH

// namespace MAUS {

class SimpleCircle {
  public:
    SimpleCircle();   // Default constructor
    ~SimpleCircle();  // Default destructor

    // Getters
    double get_x0() const { return _x0; }
    double get_x0_err() const { return _x0_err; }
    double get_y0() const { return _y0; }
    double get_y0_err() const { return _y0_err; }
    double get_R() const { return _R; }
    double get_R_err() const { return _R_err; }
    double get_chisq() const { return _chisq; }

    // Setters
    void set_x0(double x0) { _x0 = x0; }
    void set_x0_err(double x0_err) { _x0_err = x0_err; }
    void set_y0(double y0) { _y0 = y0; }
    void set_y0_err(double y0_err) { _y0_err = y0_err; }
    void set_R(double R) { _R = R; }
    void set_R_err(double R_err) { _R_err = R_err; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_parameters(double x0, double x0_err, double y0, double y0_err,
                        double R, double R_err, double chisq);

  private:
    double _x0;
    double _x0_err;
    double _y0;
    double _y0_err;
    double _R;
    double _R_err;
    double _chisq;
};
// } // ~namespace MAUS

#endif
