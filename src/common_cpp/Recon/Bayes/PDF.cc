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

namespace MAUS {
PDF::PDF() {}

PDF::PDF(std::string name,
         double bins,
         double min,
         double max) : _n_bins(bins), _probability(NULL) {
  const char *c_name = name.c_str();
  _probability = new TH1D(c_name, c_name, bins, min, max);

  for ( int bin = 1; bin <= _n_bins; bin++ ) {
    double bin_centre = _probability->GetXaxis()->GetBinCenter(bin);
    _probability->Fill(bin_centre, 1./_n_bins);
  }
}

PDF::~PDF() {}

PDF& PDF::operator=(const PDF &rhs) {
  if ( this == &rhs ) {
    return *this;
  }

  _probability = reinterpret_cast<TH1D*>(rhs._probability->Clone("clone"));

  _n_bins = rhs._n_bins;

  return *this;
}

PDF::PDF(const PDF &pdf) {
  _probability = reinterpret_cast<TH1D*>(pdf._probability->Clone("clone"));
  _n_bins = pdf._n_bins;
}

void PDF::ComputeNewPosterior(TH1D likelihood) {
  // posterior = prior*likelihood
  _probability->Multiply(&likelihood);

  // The likelihood isn't a PDF. Get around this
  // by normalizing the resulting posterior.
  double norm = 1./_probability->Integral();
  _probability->Scale(norm);
}

} // ~namespace MAUS
