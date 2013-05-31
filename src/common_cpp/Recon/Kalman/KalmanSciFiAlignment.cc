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

// TODO: Find a better way to save
//       misalignments and respective errors?
//       The current implementation creates a set of TGraphs,
//       where each TGraph stores the misalignment evaluation (x & y) at each
//       iteration.
//

#include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"

namespace MAUS {

KalmanSciFiAlignment::KalmanSciFiAlignment()
      : _file("SciFiMisalignments"),
        _rootfile(0),
        _graphs_tracker0(0),
        _graphs_tracker1(0) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  _fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerRecon/"+_file;
}

KalmanSciFiAlignment::~KalmanSciFiAlignment() {}

TMatrixD KalmanSciFiAlignment::get_shifts(int site_id) {
  std::map<int, TMatrixD>::iterator it;
  it = _shifts_map.find(site_id);
  return (*it).second;
}

TMatrixD KalmanSciFiAlignment::get_cov_shifts(int site_id) {
  std::map<int, TMatrixD>::iterator it;
  it = _covariance_map.find(site_id);
  return (*it).second;
}

bool KalmanSciFiAlignment::LoadMisaligments() {
  std::ifstream inf(_fname.c_str());
  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load misalignments.",
          "KalmanSciFiAlignment::load_misaligments"));
  }

  std::string line;
  // Titles line.
  int plane;
  double xd, yd, zd;
  double s_xx, s_xy, s_xz;
  double s_yx, s_yy, s_yz;
  double s_zx, s_zy, s_zz;
  int size = 11;

  getline(inf, line);
  for ( int line_i = 1; line_i < size; ++line_i ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());
    ist1 >> plane
         >> xd >> s_xx >> s_xy >> s_xz
         >> yd >> s_yx >> s_yy >> s_yz
         >> zd >> s_zx >> s_zy >> s_zz;

    // Store shifts.
    TMatrixD shifts(3, 1);
    shifts(0, 0) = xd;
    shifts(1, 0) = yd;
    shifts(2, 0) = zd;
    _shifts_map.insert(std::make_pair(plane,   shifts));
    _shifts_map.insert(std::make_pair(plane+1, shifts));
    _shifts_map.insert(std::make_pair(plane+2, shifts));

    // Store their covariance.
    TMatrixD cov_s(3, 3);
    cov_s(0, 0) = s_xx;
    cov_s(0, 1) = s_xy;
    cov_s(0, 2) = s_xz;
    cov_s(1, 0) = s_yx;
    cov_s(1, 1) = s_yy;
    cov_s(1, 2) = s_yz;
    cov_s(2, 0) = s_zx;
    cov_s(2, 1) = s_zy;
    cov_s(2, 2) = s_zz;
    _covariance_map.insert(std::make_pair(plane,   cov_s));
    _covariance_map.insert(std::make_pair(plane+1, cov_s));
    _covariance_map.insert(std::make_pair(plane+2, cov_s));
  }

  inf.close();
  return true;
}

void KalmanSciFiAlignment::Update(const KalmanSite &site) {
  int id = site.id();
  set_shifts(site.shift(), id);
  set_cov_shifts(site.shift_covariance(), id);
}

void KalmanSciFiAlignment::Save() {
  std::ofstream file_out(_fname.c_str());
  // Write shifts.
  file_out << "# plane" << "\t" << "xd" << "\t"
           << "s_xd_xd" << "\t" << "s_xd_yd" << "\t"
           << "s_xd_zd" << "\t" << "yd" << "\t"
           << "s_yd_xd" << "\t" << "s_yd_yd" << "\t"
           << "s_yd_zd" << "\t" << "zd" << "\t"
           << "s_zd_xd" << "\t" << "s_zd_yd" << "\t"
           << "s_zd_zd \n";
      file_out << 1 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

  // sites 2 to 29; increment 3
  for ( int station = 2; station < 5; station++ ) {
      int site_i = 3*(station); // j==6 || j==9 || j==12
      file_out << station << "\t"
      << _shifts_map[site_i](0, 0) << "\t"
      << _covariance_map[site_i](0, 0) << "\t"
      << _covariance_map[site_i](0, 1) << "\t"
      << _covariance_map[site_i](0, 2) << "\t"
      << _shifts_map[site_i](1, 0) << "\t"
      << _covariance_map[site_i](1, 0) << "\t"
      << _covariance_map[site_i](1, 1) << "\t"
      << _covariance_map[site_i](1, 2) << "\t"
      << _shifts_map[site_i](2, 0) << "\t"
      << _covariance_map[site_i](2, 0) << "\t"
      << _covariance_map[site_i](2, 1) << "\t"
      << _covariance_map[site_i](2, 2) << "\n";
  }
      file_out << 5 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

      file_out << 6 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

  for ( int station = 7; station < 10; station++ ) {
      int site_i = 3*(station);
      file_out << station << "\t"
      << _shifts_map[site_i](0, 0) << "\t"
      << _covariance_map[site_i](0, 0) << "\t"
      << _covariance_map[site_i](0, 1) << "\t"
      << _covariance_map[site_i](0, 2) << "\t"
      << _shifts_map[site_i](1, 0) << "\t"
      << _covariance_map[site_i](1, 0) << "\t"
      << _covariance_map[site_i](1, 1) << "\t"
      << _covariance_map[site_i](1, 2) << "\t"
      << _shifts_map[site_i](2, 0) << "\t"
      << _covariance_map[site_i](2, 0) << "\t"
      << _covariance_map[site_i](2, 1) << "\t"
      << _covariance_map[site_i](2, 2) << "\n";
  }
      file_out << 10 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

  file_out.close();

  SaveToRootFile();
}

void KalmanSciFiAlignment::SaveToRootFile() {
  // Get graphs stored in the multigraph.
  // TRACKER 0:
  int plane = -6;
  TList *tr0_list = static_cast<TList*>(_graphs_tracker0->GetListOfGraphs());
  TIter *hiter0 = new TIter(tr0_list);
  TGraph *graph0 = 0;
  hiter0->Reset();
  while ((graph0 = static_cast<TGraph*> (hiter0->Next())) != NULL) {
    double xd = _shifts_map[plane](0, 0);
    double yd = _shifts_map[plane](1, 0);
    double n = graph0->GetN();
    graph0->SetPoint(static_cast<Int_t> (n), n, xd);
    graph0->SetPoint(static_cast<Int_t> (n), n, yd);
    plane -= 3;
  }
  // TRACKER 1:
  plane = 6;
  TList *tr1_list = static_cast<TList*> (_graphs_tracker1->GetListOfGraphs());
  TIter *hiter1 = new TIter(tr1_list);
  TGraph *graph1 = 0;
  hiter1->Reset();
  while ((graph1 = static_cast<TGraph*>(hiter1->Next())) != NULL) {
    double xd = _shifts_map[plane](0, 0);
    double yd = _shifts_map[plane](1, 0);
    double n = graph1->GetN();
    graph1->SetPoint(static_cast<Int_t> (n), n, xd);
    graph1->SetPoint(static_cast<Int_t> (n), n, yd);
    plane += 3;
  }
}

void KalmanSciFiAlignment::CloseRootFile() {
  _rootfile->Close();
}

void KalmanSciFiAlignment::SetUpRootOutput() {
  //
  // Sets up a root file containing TGraphs which monitor
  // the misalignment search. If an outfile already exists,
  // we will append to it.
  //
  _rootfile = new TFile("misalignments.root", "UPDATE");
  _graphs_tracker0 = reinterpret_cast<TMultiGraph*> (_rootfile->Get("tracker0"));
  if ( _graphs_tracker0 ) {
    _graphs_tracker1= reinterpret_cast<TMultiGraph*> (_rootfile->Get("tracker1"));
  } else {
    _graphs_tracker0 = new TMultiGraph("tracker0", "tracker0");
    _graphs_tracker1 = new TMultiGraph("tracker1", "tracker1");
    for ( int i = 2; i < 5; ++i ) {
      std::stringstream station_number;
      station_number << i;
      std::string name(station_number.str());
      TGraph *graph = new TGraph();
      graph->SetName(name.c_str());
      _graphs_tracker0->Add(graph);
    }
    for ( int i = 2; i < 5; ++i ) {
      std::stringstream station_number;
      station_number << i;
      std::string name(station_number.str());
      TGraph *graph = new TGraph();
      graph->SetName(name.c_str());
      _graphs_tracker1->Add(graph);
    }
  }
}

} // ~namespace MAUS
