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

#ifndef Likelihood_HH
#define Likelihood_HH

// C headers
#include <assert.h>

// C++ headers
#include <vector>
#include <string>
#include "TH2D.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TRandom.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

#include "src/common_cpp/Recon/Bayes/PDF.hh"

namespace MAUS {

/** @class Likelihood
 *
 *  @brief A Probability Distribution Function.
 *
 */
class Likelihood {
 public:
  Likelihood();

  Likelihood(std::string name, double n_bins, double bin_min, double bin_max);

  ~Likelihood();

  Likelihood(const Likelihood &site);

  Likelihood& operator=(const Likelihood& pdf);

  void Build(std::string model, double sigma, double number_of_tosses);

  TH1D GetLikelihoodOfData(double param);

  TH2D* GetHistogram() const { return _likelihood; }

 private:
  TH2D *_likelihood;

  int _n_bins;

  double _min;

  double _max;
};

} // ~namespace MAUS

#endif
