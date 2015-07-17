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
  : _h(), _v() {
}

TH2EMR::TH2EMR(const TH2EMR& _th2emr)
  : _h(), _v() {

  *this = _th2emr;
}

TH2EMR& TH2EMR::operator=(const TH2EMR& _th2emr) {
  if (this == &_th2emr) {
        return *this;
  }

  this->_h = _th2emr._h;
  this->_v = _th2emr._v;

  return *this;
}

TH2EMR::~TH2EMR() {

  delete _h;
  _v.resize(0);
}

TH2EMR::TH2EMR(const char* name, const char* title) {

  gStyle->SetOptStat(0);

  _h = new TH2Poly(name, title, 0, 48, 0, 60);
}

void TH2EMR::Fill(int i, int j) {

  // Skip the test channel
  if (j == 0) return;

  // If the hit is in the sea (bin = -5), add a bin
  int bin = _h->FindBin(i+0.5, j);
  if (bin < 0) {
    double px[4] = {i+j%2, i+1-j%2, i+j%2, i+j%2};
    double py[4] = {j-1, j, j+1, j-1};
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
  int bin = _h->FindBin(i+0.5, j);
  if (bin < 0) {
    double px[4] = {i+j%2, i+1-j%2, i+j%2, i+j%2};
    double py[4] = {j-1, j, j+1, j-1};
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
  int bin = _h->FindBin(i+0.5, 30);
  if (bin < 0) {
    double px[5] = {i+1, i, i, i+1, i+1};
    double py[5] = {0, 1, 59, 60, 0};
    bin = _h->AddBin(5, px, py);
  }

  // Fill the plane
  double wi = _h->GetBinContent(bin);
  _h->SetBinContent(bin, wi+w);

  // Save the values to determine the range of the z axis
  _v.push_back(wi+w);
}

void TH2EMR::Draw() {

  // Identify the optimal limits of the z axis and set them
  std::sort(_v.begin(), _v.end());

  _h->SetAxisRange(_v.front() - 1, _v.back(), "Z");
  _h->SetAxisRange(_v.front() - 1, _v.back(), "Z");

  // Draw with COLZ
  _h->Draw("COLZ");
}

double TH2EMR::GetBinContent(int i, int j) {

  int bin = _h->FindBin(i+0.5, j);
  return _h->GetBinContent(bin);
}

void TH2EMR::SetBinContent(int i, int j, double w) {

  // If the hit is in the sea (bin = -5), add a bar
  int bin = _h->FindBin(i+0.5, j);
  if (bin < 0) {
    double px[4] = {i+j%2, i+1-j%2, i+j%2, i+j%2};
    double py[4] = {j-1, j, j+1, j-1};
    bin = _h->AddBin(4, px, py);
  }

  // Set content of the bar
  _h->SetBinContent(bin, w);

  // Save the values to determine the range of the z axis
  _v.push_back(w);
}

void TH2EMR::Write() {

  _h->Write();
}

} // namespace MAUS
