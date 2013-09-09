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

#ifndef PDF_HH
#define PDF_HH

// C headers
#include <assert.h>

// C++ headers
#include <vector>
#include <string>
#include "TH1D.h"
#include "TMath.h"
#include "TMatrixD.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class pdf
 *
 *  @brief A Probability Distribution Function.
 *
 */
class PDF {
 public:
  PDF();

  PDF(std::string name, double bins, double min, double max);

  ~PDF();

  PDF(const PDF &site);

  PDF& operator=(const PDF& pdf);

  void ComputeNewPosterior(TH1D likelihood);

  TH1D* GetHistogram() const { return _probability; }

  double GetMean() const { return _probability->GetMean(); }
  double GetRMS()  const { return _probability->GetRMS(); }

 private:
  TH1D *_probability;

  int _n_bins;

  double _bin_width;
};

} // ~namespace MAUS

#endif
