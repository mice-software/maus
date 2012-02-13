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
    double get_m() const { return _m; }
    double get_chisq() const { return _chisq; }

    // Setters
    void set_c(double c) { _c = c; }
    void set_m(double m) { _m = m; }
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_parameters(double c, double m, double chisq);

  private:
    double _c;
    double _m;
    double _chisq;
};
// }// ~namespace MAUS

#endif
