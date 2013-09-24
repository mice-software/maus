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

#ifndef JointPDF_HH
#define JointPDF_HH

// C headers
#include <assert.h>

// C++ headers
#include <vector>
#include <string>
#include "TH2D.h"
#include "TF1.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TRandom.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

#include "src/common_cpp/Recon/Bayes/PDF.hh"

namespace MAUS {

/** @class JointPDF
 *
 *  @brief A Probability Distribution Function.
 *
 */
class JointPDF {
 public:
  JointPDF();

  JointPDF(std::string name, double bin_width, double bin_min, double bin_max);

  ~JointPDF();

  JointPDF(const JointPDF &site);

  JointPDF& operator=(const JointPDF& pdf);

  void Build(std::string model, double sigma, int number_of_tosses);

  TH1D GetLikelihood(double param);

  TH2D *GetJointPDF()  const { return _joint;     }

  std::string name()  const { return _name;      }

  int n_bins()        const { return _n_bins;    }

  double bin_width()  const { return _bin_width; }

  double min()        const { return _min;       }

  double max()        const { return _max;       }

 private:
  TH2D *_joint;

  std::string _name;

  int _n_bins;

  double _bin_width;

  double _min;

  double _max;
};

} // ~namespace MAUS

#endif
