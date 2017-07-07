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

#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"

namespace MAUS {

// Constructors
SimpleHelix::SimpleHelix() : _params {0.0, 0.0, 0.0, 0.0, 0.0},
                             _errors {0.0, 0.0, 0.0, 0.0, 0.0},
                             _ndfs {0, 0, 0},
                             _chisqs {0.0, 0.0, 0.0},
                             _pvalue {0.0} {
  _cov.ResizeTo(4, 4);
}

SimpleHelix::SimpleHelix(const std::vector<double>& params,
                         const std::vector<double>& errors,
                         const std::vector<int>& ndfs,
                         const std::vector<double>& chisqs,
                         double pvalue,
                         TMatrixD& cov) : _params {params},
                                          _errors {errors},
                                          _ndfs {ndfs},
                                          _chisqs {chisqs},
                                          _pvalue {pvalue},
                                          _cov {cov} {
  // Do nothing
}

// Destructor
SimpleHelix::~SimpleHelix() {}

void SimpleHelix::clear() {
  _params = {0.0, 0.0, 0.0, 0.0, 0.0},
  _errors = {0.0, 0.0, 0.0, 0.0, 0.0},
  _ndfs = {0, 0, 0},
  _chisqs = {0.0, 0.0, 0.0},
  _pvalue = {0.0},
  _cov = TMatrixD(4, 4);
}

void SimpleHelix::set_parameters(const std::vector<double>& params,
                                 const std::vector<double>& errors,
                                 const std::vector<int>& ndfs,
                                 const std::vector<double>& chisqs,
                                 double pvalue,
                                 TMatrixD& cov) {
  _params = params;
  _errors = errors;
  _ndfs = ndfs;
  _chisqs = chisqs;
  _pvalue = pvalue;
  _cov = cov;
}

} // ~namespace MAUS
