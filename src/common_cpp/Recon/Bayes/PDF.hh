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
#include <iostream>
#include <string>
#include "TH1D.h"
#include "TMath.h"
#include "TMatrixD.h"

#include "Interface/Squeal.hh"

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

  double GetMean()     const { return _probability->GetMean(); }

  double GetRMS()      const { return _probability->GetRMS();  }

  TH1D* probability()  const { return _probability; }

  std::string name()   const { return _name;        }

  int n_bins()         const { return _n_bins;      }

  double bin_width()   const { return _bin_width;   }

  double min()         const { return _min;         }

  double max()         const { return _max;         }

 private:
  TH1D *_probability;

  std::string _name;

  int _n_bins;

  double _bin_width;

  double _min;

  double _max;
};

} // ~namespace MAUS

#endif
