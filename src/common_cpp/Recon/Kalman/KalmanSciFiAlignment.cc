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
//       iteration. Obviously there are other ways of recording this.
//       The one chosen makes it easy to save and browse, but requires a lot
//       of repetitive coding, hence the legth of this file.
//

#include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"

namespace MAUS {

KalmanSciFiAlignment::KalmanSciFiAlignment()
  : _file("SciFiMisalignments"),
    _rootfile(0),
    station1_x(0), station1_y(0),
    station2_x(0), station2_y(0),
    station3_x(0), station3_y(0),
    station4_x(0), station4_y(0),
    station5_x(0), station5_y(0),
    station6_x(0), station6_y(0),
    station7_x(0), station7_y(0),
    station8_x(0), station8_y(0),
    station9_x(0), station9_y(0),
    station10_x(0), station10_y(0) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  _fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerRecon/"+_file;

  for ( int i = 0; i < 30; ++i ) {
    _shifts_array[i].     ResizeTo(3, 1);
    _covariance_shifts[i].ResizeTo(3, 3);
    _shifts_array[i].     Zero();
    _covariance_shifts[i].Zero();
  }
}

KalmanSciFiAlignment::~KalmanSciFiAlignment() {}

bool KalmanSciFiAlignment::LoadMisaligments() {
  std::ifstream inf(_fname.c_str());
  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load misalignments.",
          "KalmanSciFiAlignment::load_misaligments"));
  }

  std::string line;
  // Titles line.
  int station;
  double xd, yd, zd;
  double s_xx, s_xy, s_xz;
  double s_yx, s_yy, s_yz;
  double s_zx, s_zy, s_zz;
  int size = 11;

  getline(inf, line);
  for ( int line_i = 1; line_i < size; ++line_i ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());
    ist1 >> station
         >> xd >> s_xx >> s_xy >> s_xz
         >> yd >> s_yx >> s_yy >> s_yz
         >> zd >> s_zx >> s_zy >> s_zz;

    assert(line_i == station && "SciFiMisalignments (Shifts) set up as expected.");
    int site_id = 3*(station)-2; // 1, 4, 7, 10, 13, ...

    // Shifts.
    TMatrixD shifts(3, 1);
    shifts(0, 0) = xd;
    shifts(1, 0) = yd;
    shifts(2, 0) = zd;
    _shifts_array[site_id]   = shifts;
    _shifts_array[site_id+1] = shifts;
    _shifts_array[site_id+2] = shifts;
    // Their covariance.
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
    _covariance_shifts[site_id]   = cov_s;
    _covariance_shifts[site_id+1] = cov_s;
    _covariance_shifts[site_id+2] = cov_s;
  }

  inf.close();
  return true;
}

void KalmanSciFiAlignment::Update(KalmanSite site) {
  int id = site.id();
  set_shifts(site.shift(), id);
  set_cov_shifts(site.shift_covariance(), id);
}

void KalmanSciFiAlignment::Save() {
  std::ofstream file_out(_fname.c_str());
  // Write shifts.
  file_out << "# station" << "\t" << "xd" << "\t"
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
      << _shifts_array[site_i](0, 0) << "\t"
      << _covariance_shifts[site_i](0, 0) << "\t"
      << _covariance_shifts[site_i](0, 1) << "\t"
      << _covariance_shifts[site_i](0, 2) << "\t"
      << _shifts_array[site_i](1, 0) << "\t"
      << _covariance_shifts[site_i](1, 0) << "\t"
      << _covariance_shifts[site_i](1, 1) << "\t"
      << _covariance_shifts[site_i](1, 2) << "\t"
      << _shifts_array[site_i](2, 0) << "\t"
      << _covariance_shifts[site_i](2, 0) << "\t"
      << _covariance_shifts[site_i](2, 1) << "\t"
      << _covariance_shifts[site_i](2, 2) << "\n";
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
      << _shifts_array[site_i](0, 0) << "\t"
      << _covariance_shifts[site_i](0, 0) << "\t"
      << _covariance_shifts[site_i](0, 1) << "\t"
      << _covariance_shifts[site_i](0, 2) << "\t"
      << _shifts_array[site_i](1, 0) << "\t"
      << _covariance_shifts[site_i](1, 0) << "\t"
      << _covariance_shifts[site_i](1, 1) << "\t"
      << _covariance_shifts[site_i](1, 2) << "\t"
      << _shifts_array[site_i](2, 0) << "\t"
      << _covariance_shifts[site_i](2, 0) << "\t"
      << _covariance_shifts[site_i](2, 1) << "\t"
      << _covariance_shifts[site_i](2, 2) << "\n";
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
  for ( int id = 1; id < 31; id+=3 ) {
    if ( id == 3 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station1_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station1_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 6 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station2_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station2_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 9 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station3_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station3_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 12 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station4_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station4_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 15 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station5_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station5_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 18 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station6_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station6_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 21 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station7_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station7_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 24 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station8_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station8_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 27 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station9_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station9_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 30 ) {
      double xd = _shifts_array[id](0, 0);
      double yd = _shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station10_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station10_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
  }
}

void KalmanSciFiAlignment::CloseRootFile() {
  station1_x->Write("", TObject::kOverwrite);
  station2_x->Write("", TObject::kOverwrite);
  station3_x->Write("", TObject::kOverwrite);
  station4_x->Write("", TObject::kOverwrite);
  station5_x->Write("", TObject::kOverwrite);
  station6_x->Write("", TObject::kOverwrite);
  station7_x->Write("", TObject::kOverwrite);
  station8_x->Write("", TObject::kOverwrite);
  station9_x->Write("", TObject::kOverwrite);
  station10_x->Write("", TObject::kOverwrite);
  station1_y->Write("", TObject::kOverwrite);
  station2_y->Write("", TObject::kOverwrite);
  station3_y->Write("", TObject::kOverwrite);
  station4_y->Write("", TObject::kOverwrite);
  station5_y->Write("", TObject::kOverwrite);
  station6_y->Write("", TObject::kOverwrite);
  station7_y->Write("", TObject::kOverwrite);
  station8_y->Write("", TObject::kOverwrite);
  station9_y->Write("", TObject::kOverwrite);
  station10_y->Write("", TObject::kOverwrite);

  _rootfile->Close();
}

void KalmanSciFiAlignment::SetUpRootOutput() {
  //
  // Sets up a root file containing a TGraphs which monitor
  // the misalignment search. If an outfile already exists,
  // we will append to it.
  //
  _rootfile = new TFile("misalignments.root", "UPDATE");
  station1_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station1"));
  if ( station1_x ) {
    station2_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station2"));
    station3_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station3"));
    station4_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station4"));
    station5_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station5"));
    station6_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station6"));
    station7_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station7"));
    station8_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station8"));
    station9_x      = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station9"));
    station10_x     = reinterpret_cast<TGraph*> (_rootfile->Get("xd_station10"));
    station1_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station1"));
    station2_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station2"));
    station3_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station3"));
    station4_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station4"));
    station5_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station5"));
    station6_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station6"));
    station7_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station7"));
    station8_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station8"));
    station9_y      = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station9"));
    station10_y     = reinterpret_cast<TGraph*> (_rootfile->Get("yd_station10"));
  } else {
    station1_x = new TGraph();
    station1_x->SetName("xd_station1");
    station2_x = new TGraph();
    station2_x->SetName("xd_station2");
    station3_x = new TGraph();
    station3_x->SetName("xd_station3");
    station4_x = new TGraph();
    station4_x->SetName("xd_station4");
    station5_x = new TGraph();
    station5_x->SetName("xd_station5");
    station6_x = new TGraph();
    station6_x->SetName("xd_station6");
    station7_x = new TGraph();
    station7_x->SetName("xd_station7");
    station8_x = new TGraph();
    station8_x->SetName("xd_station8");
    station9_x = new TGraph();
    station9_x->SetName("xd_station9");
    station10_x = new TGraph();
    station10_x->SetName("xd_station10");
    station1_y = new TGraph();
    station1_y->SetName("yd_station1");
    station2_y = new TGraph();
    station2_y->SetName("yd_station2");
    station3_y = new TGraph();
    station3_y->SetName("yd_station3");
    station4_y = new TGraph();
    station4_y->SetName("yd_station4");
    station5_y = new TGraph();
    station5_y->SetName("yd_station5");
    station6_y = new TGraph();
    station6_y->SetName("yd_station6");
    station7_y = new TGraph();
    station7_y->SetName("yd_station7");
    station8_y = new TGraph();
    station8_y->SetName("yd_station8");
    station9_y = new TGraph();
    station9_y->SetName("yd_station9");
    station10_y = new TGraph();
    station10_y->SetName("yd_station10");
  }
}

} // ~namespace MAUS
