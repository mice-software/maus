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

#include "src/common_cpp/Recon/Bayes/PDF.hh"
#include <iostream>

namespace MAUS {
PDF::PDF() : _probability(),
             _name(""),
             _n_bins(0),
             _bin_width(0.),
             _min(0.),
             _max(0.) {}

PDF::PDF(std::string name, double bin_width, double min, double max)
                                                : //_probability(NULL),
                                                  _name(name),
                                                  _bin_width(bin_width) {
  _min = min - _bin_width/2.;
  _max = max + _bin_width/2.;

  const char *c_name = name.c_str();
  _n_bins = static_cast<int> ( ((max-min)/_bin_width)+1 );

  _probability = TH1D(c_name, c_name, _n_bins, _min, _max);

  for ( int bin = 1; bin <= _n_bins; bin++ ) {
    double bin_centre = _probability.GetXaxis()->GetBinCenter(bin);
    _probability.Fill(bin_centre, 1./_n_bins);
  }
}

PDF::~PDF() {
  // delete _probability;
}

PDF& PDF::operator=(const PDF &rhs) {
  if ( this == &rhs ) {
    return *this;
  }

  _name = rhs._name;

  _probability = TH1D(rhs._probability);


  _n_bins    = rhs._n_bins;
  _bin_width = rhs._bin_width;
  _min       = rhs._min;
  _max       = rhs._max;

  return *this;
}

PDF::PDF(const PDF &pdf) {
  _name = pdf._name;

  _probability = TH1D(pdf._probability);

  _n_bins    = pdf._n_bins;
  _bin_width = pdf._bin_width;
  _min       = pdf._min;
  _max       = pdf._max;
}

void PDF::ComputeNewPosterior(TH1D likelihood) {
  // posterior = prior*likelihood
  _probability.Multiply(&likelihood);

  // The likelihood isn't a PDF. Get around this
  // by normalizing the resulting posterior.
  double norm = 1./_probability.Integral();
  _probability.Scale(norm);
}

} // ~namespace MAUS
