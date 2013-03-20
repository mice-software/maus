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

// TODO: Find a proper way to save
//       misalignments and respective errors.
//

#include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"

namespace MAUS {

KalmanSciFiAlignment::KalmanSciFiAlignment() : file(0),
                                               station1_x(0),
                                               station2_x(0),
                                               station3_x(0),
                                               station4_x(0),
                                               station5_x(0),
                                               station6_x(0),
                                               station7_x(0),
                                               station8_x(0),
                                               station9_x(0),
                                               station10_x(0) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  file = "SciFiMisalignments";
  fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerRecon/"+file;

  for ( int i = 0; i < 30; ++i ) {
    shifts_array[i].     ResizeTo(3, 1);
    covariance_shifts[i].ResizeTo(3, 3);
    shifts_array[i].     Zero();
    covariance_shifts[i].Zero();
  }
  SetRootOutput();
}

KalmanSciFiAlignment::~KalmanSciFiAlignment() {
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
/*
  TMultiGraph *mg = new TMultiGraph("multigraph", "multigraph");
  mg->SetMaximum(1.);
  mg->SetMinimum(-3.);
  station2_x->SetLineColor(kBlack);
  station2_x->SetLineWidth(2);
  station3_x->SetLineColor(kBlue);
  station3_x->SetLineWidth(2);
  station4_x->SetLineColor(kRed);
  station4_x->SetLineWidth(2);
  station2_y->SetLineColor(kBlack);
  station2_y->SetLineWidth(2);
  station2_y->SetLineStyle(9);
  station3_y->SetLineColor(kBlue);
  station3_y->SetLineWidth(2);
  station3_y->SetLineStyle(9);
  station4_y->SetLineColor(kRed);
  station4_y->SetLineWidth(2);
  station4_y->SetLineStyle(9);
  mg->Add(station2_x);
  mg->Add(station3_x);
  mg->Add(station4_x);
  mg->Add(station2_y);
  mg->Add(station3_y);
  mg->Add(station4_y);
  mg->SetMaximum(1.);
  mg->SetMinimum(-3.);
  mg->Write("", TObject::kOverwrite);
*/
  rootfile->Close();
}

bool KalmanSciFiAlignment::LoadMisaligments() {
  std::ifstream inf(fname.c_str());
  if (!inf) {
    throw(Squeal(Squeal::recoverable,
          "Could not load misalignments.",
          "KalmanSciFiAlignment::load_misaligments"));
  }

  std::string line;
  // Titles line.
  double station, xd, yd, zd;
  double s_xx, s_xy, s_xz;
  double s_yx, s_yy, s_yz;
  double s_zx, s_zy, s_zz;
  int size = 11;

  getline(inf, line);
  for ( int line_i = 1; line_i < size; line_i++ ) {
    getline(inf, line);
    std::istringstream ist1(line.c_str());
    ist1 >> station
         >> xd >> s_xx >> s_xy >> s_xz
         >> yd >> s_yx >> s_yy >> s_yz
         >> zd >> s_zx >> s_zy >> s_zz;

    assert(line_i == station && "SciFiMisalignments (Shifts) set up as expected.");
    int site_id = 3*(station-1); // 0, 3, 6, 9, 12

    // Shifts.
    TMatrixD shifts(3, 1);
    shifts(0, 0) = xd;
    shifts(1, 0) = yd;
    shifts(2, 0) = zd;
    shifts_array[site_id]   = shifts;
    shifts_array[site_id+1] = shifts;
    shifts_array[site_id+2] = shifts;
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
    covariance_shifts[site_id]   = cov_s;
    covariance_shifts[site_id+1] = cov_s;
    covariance_shifts[site_id+2] = cov_s;
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
  std::ofstream file_out(fname.c_str());
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
      int site_i = 3*(station)-1; // j==5 || j==8 || j==11
      file_out << station << "\t"
      << shifts_array[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 1) << "\t"
      << covariance_shifts[site_i](0, 2) << "\t"
      << shifts_array[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 1) << "\t"
      << covariance_shifts[site_i](1, 2) << "\t"
      << shifts_array[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 1) << "\t"
      << covariance_shifts[site_i](2, 2) << "\n";
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
      int site_i = 3*(station)-1;
      file_out << station << "\t"
      << shifts_array[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 0) << "\t"
      << covariance_shifts[site_i](0, 1) << "\t"
      << covariance_shifts[site_i](0, 2) << "\t"
      << shifts_array[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 0) << "\t"
      << covariance_shifts[site_i](1, 1) << "\t"
      << covariance_shifts[site_i](1, 2) << "\t"
      << shifts_array[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 0) << "\t"
      << covariance_shifts[site_i](2, 1) << "\t"
      << covariance_shifts[site_i](2, 2) << "\n";
  }
      file_out << 10 << "\t"
      << 0. << "\t"
      << 0.1 << "\t" << 0. << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0.1 << "\t" << 0. << "\t"
      << 0. << "\t"
      << 0. << "\t" << 0. << "\t" << 0.1 << "\n";

  file_out.close();

  SaveToHistogram();
}

void KalmanSciFiAlignment::SaveToHistogram() {
  for ( int id = 0; id < 30; id+=3 ) {
    if ( id == 0 ) {
      double xd = shifts_array[id](0, 0);
      double yd = shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station1_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station1_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 3 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station2_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station2_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 6 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station3_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station3_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 9 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station4_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station4_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 12 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station5_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station5_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 15 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station6_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station6_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 18 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station7_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station7_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 21 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station8_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station8_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 24 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station9_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station9_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
    if ( id == 27 ) {
      double xd = shifts_array[id](0, 0);
      double yd =shifts_array[id](1, 0);
      double n = station1_x->GetN();
      station10_x->SetPoint(static_cast<Int_t> (n), n, xd);
      station10_y->SetPoint(static_cast<Int_t> (n), n, yd);
    }
  }
}

void KalmanSciFiAlignment::SetRootOutput() {
  rootfile = new TFile("misalignments.root", "UPDATE");
  station1_x      = reinterpret_cast<TGraph*> (rootfile->Get("xd_station1"));
  if ( station1_x ) {
    station2_x      = reinterpret_cast<TGraph*> (rootfile->Get("xd_station2"));
    station3_x      = reinterpret_cast<TGraph*> (rootfile->Get("xd_station3"));
    station4_x            = reinterpret_cast<TGraph*> (rootfile->Get("xd_station4"));
    station5_x            = reinterpret_cast<TGraph*> (rootfile->Get("xd_station5"));
    station6_x            = reinterpret_cast<TGraph*> (rootfile->Get("xd_station6"));
    station7_x            = reinterpret_cast<TGraph*> (rootfile->Get("xd_station7"));
    station8_x            = reinterpret_cast<TGraph*> (rootfile->Get("xd_station8"));
    station9_x            = reinterpret_cast<TGraph*> (rootfile->Get("xd_station9"));
    station10_x           = reinterpret_cast<TGraph*> (rootfile->Get("xd_station10"));
    station1_y      = reinterpret_cast<TGraph*> (rootfile->Get("yd_station1"));
    station2_y      = reinterpret_cast<TGraph*> (rootfile->Get("yd_station2"));
    station3_y      = reinterpret_cast<TGraph*> (rootfile->Get("yd_station3"));
    station4_y            = reinterpret_cast<TGraph*> (rootfile->Get("yd_station4"));
    station5_y            = reinterpret_cast<TGraph*> (rootfile->Get("yd_station5"));
    station6_y            = reinterpret_cast<TGraph*> (rootfile->Get("yd_station6"));
    station7_y            = reinterpret_cast<TGraph*> (rootfile->Get("yd_station7"));
    station8_y            = reinterpret_cast<TGraph*> (rootfile->Get("yd_station8"));
    station9_y            = reinterpret_cast<TGraph*> (rootfile->Get("yd_station9"));
    station10_y           = reinterpret_cast<TGraph*> (rootfile->Get("yd_station10"));
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
