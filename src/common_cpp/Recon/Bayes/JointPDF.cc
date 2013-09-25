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

#include "src/common_cpp/Recon/Bayes/JointPDF.hh"

namespace MAUS {

JointPDF::JointPDF(): _joint(NULL),
                      _name(""),
                      _n_bins(0),
                      _bin_width(0.),
                      _min(0.),
                      _max(0.) {}

JointPDF::JointPDF(std::string name,
                       double bin_width,
                       double min,
                       double max) : _joint(NULL),
                                     _bin_width(bin_width) {
  _min = min - _bin_width/2.;
  _max = max + _bin_width/2.;

  const char *c_name = name.c_str();
  _n_bins = static_cast<int> ( ((max-min)/_bin_width)+1 );
  _joint = new TH2D(c_name, c_name,
                    _n_bins, _min, _max,
                    _n_bins, _min, _max);
}

JointPDF::~JointPDF() {
  delete _joint;
}

JointPDF& JointPDF::operator=(const JointPDF &rhs) {
  if ( this == &rhs ) {
    return *this;
  }
  _name = rhs._name;
  const char *c_name = _name.c_str();

  //  _joint = TH2D(rhs._joint);
  _joint = static_cast<TH2D*>(rhs._joint->Clone(c_name));

  _n_bins    = rhs._n_bins;
  _bin_width = rhs._bin_width;
  _min       = rhs._min;
  _max       = rhs._max;

  return *this;
}

JointPDF::JointPDF(const JointPDF &joint) {
  _name = joint._name;
  const char *c_name = _name.c_str();

  //  _joint = TH2D(joint._joint);
  _joint = static_cast<TH2D*>(joint._joint->Clone(c_name));

  _n_bins    = joint._n_bins;
  _bin_width = joint._bin_width;
  _min       = joint._min;
  _max       = joint._max;
}

void JointPDF::Build(std::string model, double sigma, int number_of_tosses) {
  if ( model != "gaussian" ) {
    std::cerr << "Model not implemented. Aborting" << std::endl;
  }

  TF1 *gaussian = new TF1("gaussian","gaus(0)", _min*2., _max*2.);
  gaussian->SetParameters(1, 0., sigma);

  TH1F *h = new TH1F("temp", "temp", _n_bins*2, _min*2., _max*2.);
  h->FillRandom("gaussian", number_of_tosses);

  int centroid_shift = -1;
  for ( int param_bin = 1; param_bin <= _n_bins; param_bin++ ) {
    centroid_shift++;
    for ( int D_bin = 1; D_bin <= _n_bins; D_bin++ ) {
      int corresponding_bin = D_bin+_n_bins-centroid_shift;
      double probability = h->GetBinContent(corresponding_bin);
      _joint->SetBinContent(param_bin, D_bin, probability);
    }
  }

  delete h;
}

// Returns P(Data|parameter)
TH1D JointPDF::GetLikelihood(double data) {
  // This is the histogram to be returned.
  TH1D likelihood("", "", _n_bins, _min, _max);
  // The value observed (the data) corresponds to some
  // bin number in the Y axis of the TH2D.
  // FIX THIS
  int data_bin = static_cast<int> ( (data+_max)*(_n_bins/(_max-_min)) + 1);
  // Now, for this Y-bin, we are going to swipe all possible values
  // in the paramenter axis (the x-axis) and fill our JointPDF histogram.
  for ( int param_bin = 1; param_bin <= _n_bins; param_bin++ ) {
    // The probability in a particular bin.
    double p = _joint->GetBinContent(param_bin, data_bin);
    // The parameter value the x-bin corresponds to.
    // double parameter_value = _joint.GetXaxis()->GetBinCenter(param_bin);
    // And fill the histogram.
    likelihood.SetBinContent(param_bin, p);
  }
  return likelihood;
}

} // ~namespace MAUS
