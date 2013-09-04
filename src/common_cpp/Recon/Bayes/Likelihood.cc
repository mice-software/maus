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

#include "src/common_cpp/Recon/Bayes/Likelihood.hh"

namespace MAUS {

Likelihood::Likelihood(std::string name,
                       double n_bins,
                       double min,
                       double max) : _n_bins(n_bins),
                                     _min(min),
                                     _max(max),
                                     _joint(NULL) {
  const char *c_name = name.c_str();
  _joint = new TH2D(c_name, c_name, n_bins, min, max, n_bins, min, max);
}

Likelihood::~Likelihood() {}

Likelihood& Likelihood::operator=(const Likelihood &rhs) {
  if ( this == &rhs ) {
    return *this;
  }

  return *this;
}

Likelihood::Likelihood(const Likelihood &pdf) {}

void Likelihood::Build(std::string model, double sigma, double number_of_tosses) {
  if ( model != "gaussian" ) {
    std::cerr << "Model not implemented. Aborting" << std::endl;
  }
  TRandom rand;

  for ( int param_bin = 0; param_bin < _n_bins; param_bin++ ) {
    for ( int toss = 0; toss < number_of_tosses; toss++ ) {
      double param = _joint->GetXaxis()->GetBinCenter(param_bin);
      double data_value = rand.Gaus(param, sigma);
      _joint->Fill(param, data_value);
    }
  }
}

// Returns P(Data|parameter)
TH1D Likelihood::GetLikelihood(double data) {
  // This is the histogram to be returned.
  TH1D likelihood("", "", _n_bins, _min, _max);
  // The value observed (the data) corresponds to some
  // bin number in the Y axis of the TH2D.
  double data_bin = (data+_max)*(_n_bins/(_max-_min));
  // Now, for this Y-bin, we are going to swipe all possible values
  // in the paramenter axis (the x-axis) and fill our likelihood histogram.
  for ( int param_bin = 1; param_bin <= _n_bins; param_bin++ ) {
    // The likelihood in a particular bin.
    double l = _joint->GetBinContent(param_bin, data_bin);
    // The parameter value the x-bin corresponds to.
    double parameter_value = _joint->GetXaxis()->GetBinCenter(param_bin);
    // And fill the histogram.
    likelihood.Fill(parameter_value, l);
  }
  return likelihood;
}

} // ~namespace MAUS
