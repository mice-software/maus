/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#include "Utils/TH2EMR.hh"

namespace MAUS {

TH2EMR::TH2EMR()
  : _h(NULL), _v() {
}

TH2EMR::TH2EMR(const TH2EMR& th2emr)
  : _h(NULL), _v() {

  *this = th2emr;
}

TH2EMR& TH2EMR::operator=(const TH2EMR& th2emr) {
  if (this == &th2emr)
    return *this;

  if ( _h )
    delete _h;
  if ( th2emr._h ) {
    _h = static_cast<TH2Poly*>(th2emr._h->Clone());
  } else {
    _h = NULL;
  }

  if ( !_v.size() )
    _v.resize(0);
  if ( !th2emr._v.size() )
    _v = th2emr._v;

  return *this;
}

TH2EMR::~TH2EMR() {

  if ( _h )
    delete _h;
  if ( !_v.size() )
    _v.resize(0);
}

TH2EMR::TH2EMR(const char* name, const char* title) {

  gStyle->SetOptStat(0);

  _h = new TH2Poly(name, title, 0, 48, 0, 60);
  _h->GetXaxis()->SetTitle("Plane ID");
  _h->GetYaxis()->SetTitle("Bar ID");
}

void TH2EMR::Fill(int i, int j) {

  // Skip the test channel
  if (j == 0) return;

  // If the hit is in the sea (bin = -5), add a bin
  int bin = _h->FindBin(i+.5, j);
  if (bin < 0) {
    double I = static_cast<double>(i);
    double J = static_cast<double>(j);
    double px[4] = {I+j%2, I+1-j%2, I+j%2, I+j%2};
    double py[4] = {J-1, J, J+1, J-1};
    bin = _h->AddBin(4, px, py);
  }

  // Fill the bin
  double wi = _h->GetBinContent(bin);
  _h->SetBinContent(bin, wi + 1);

  // Save the values to determine the range of the z axis
  _v.push_back(wi + 1);
}

void TH2EMR::Fill(int i, int j, double w) {

  // Skip the test channel
  if (j == 0) return;

  // If the hit is in the sea (bin = -5), add a bar
  int bin = _h->FindBin(i+.5, j);
  if (bin < 0) {
    double I = static_cast<double>(i);
    double J = static_cast<double>(j);
    double px[4] = {I+j%2, I+1-j%2, I+j%2, I+j%2};
    double py[4] = {J-1, J, J+1, J-1};
    bin = _h->AddBin(4, px, py);
  }

  // Fill the bar
  double wi = _h->GetBinContent(bin);
  _h->SetBinContent(bin, wi + w);

  // Save the values to determine the range of the z axis
  _v.push_back(wi + w);
}

void TH2EMR::FillPlane(int i, double w) {

  // If the hit is in the sea (bin = -5), add a plane
  int bin = _h->FindBin(i+.5, 30);
  if (bin < 0) {
    double I = static_cast<double>(i);
    double px[5] = {I+1, I, I, I+1, I+1};
    double py[5] = {0., 1., 59., 60., 0.};
    bin = _h->AddBin(5, px, py);
  }

  // Fill the plane
  double wi = _h->GetBinContent(bin);
  _h->SetBinContent(bin, wi+w);

  // Save the values to determine the range of the z axis
  _v.push_back(wi+w);
}

void TH2EMR::Draw() {

  if ( !_v.size() )
    return;

  // Identify the optimal limits of the z axis and set them
  std::sort(_v.begin(), _v.end());
  _h->SetAxisRange(_v.front() - 1, _v.back(), "Z");
  _h->SetAxisRange(_v.front() - 1, _v.back(), "Z");

  // Draw with COLZ
  _h->Draw("COLZ L");
}

double TH2EMR::GetBinContent(int i, int j) {

  int bin = _h->FindBin(i+.5, j);
  return _h->GetBinContent(bin);
}

void TH2EMR::SetBinContent(int i, int j, double w) {

  // If the hit is in the sea (bin = -5), add a bar
  int bin = _h->FindBin(i+.5, j);
  if (bin < 0) {
    double I = static_cast<double>(i);
    double J = static_cast<double>(j);
    double px[4] = {I+j%2, I+1-j%2, I+j%2, I+j%2};
    double py[4] = {J-1, J, J+1, J-1};
    bin = _h->AddBin(4, px, py);
  } else {
    _v.erase(std::remove(_v.begin(), _v.end(), _h->GetBinContent(bin)), _v.end());
  }

  // Set content of the bar
  _h->SetBinContent(bin, w);

  // Save the values to determine the range of the z axis
  _v.push_back(w);
}
} // namespace MAUS
