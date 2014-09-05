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

// C++ headers
#include <iostream>
#include <iomanip>
#include <string>

// ROOT Headers
#include "TROOT.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterXYZ.hh"

namespace MAUS {

TrackerDataManager::TrackerDataManager()
  : _print_tracks(false),
    _print_seeds(false) {
  // Do nothing
};

TrackerDataManager::~TrackerDataManager() {
  // Do nothing
};

void TrackerDataManager::process(const Spill *spill) {
  if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
    if (_print_tracks || _print_seeds) std::cout << "TrackerDataManager: Opening spill "
      << spill->GetSpillNumber() << std::endl;
    // Loop over recon events
    for (size_t i = 0; i < spill->GetReconEvents()->size(); ++i) {
      _t1._spill_num = spill->GetSpillNumber();
      _t2._spill_num = spill->GetSpillNumber();
      // Process the SciFi data objects
      SciFiEvent *evt = (*spill->GetReconEvents())[i]->GetSciFiEvent();
      process_digits(spill, evt->digits());
      process_clusters(evt->clusters());
      process_spoints(evt->spacepoints());
      process_strks(evt->straightprtracks());
      process_htrks(evt->helicalprtracks());
    } // ~ loop over recon events
  } // ~ check event is a physics event
};

void TrackerDataManager::process_digits(const Spill *spill, const std::vector<SciFiDigit*> digits) {
  for (size_t i = 0; i < digits.size(); ++i) {
    SciFiDigit *dig = digits[i];
    if ( dig->get_tracker() == 0 ) {
      ++_t1._num_digits;
      _t1._num_events = spill->GetReconEvents()->size();
    } else if ( dig->get_tracker() == 1 ) {
      ++_t2._num_digits;
      _t2._num_events = spill->GetReconEvents()->size();
    }
  }
};

void TrackerDataManager::process_clusters(const std::vector<SciFiCluster*> clusters) {
  for (size_t i = 0; i < clusters.size(); ++i) {
    SciFiCluster *clus = clusters[i];
    if ( clus->get_tracker() == 0 ) {
      ++_t1._num_clusters;
    } else if ( clus->get_tracker() == 1 ) {
      ++_t2._num_clusters;
    }
  }
};

void TrackerDataManager::process_spoints(const std::vector<SciFiSpacePoint*> spoints) {
  for (size_t i = 0; i < spoints.size(); ++i) {
    SciFiSpacePoint *sp = spoints[i];
    ThreeVector pos = sp->get_position();
    if ( sp->get_tracker() == 0 ) {
      ++_t1._num_spoints;
      _t1._spoints_x.push_back(pos.x());
      _t1._spoints_y.push_back(pos.y());
      _t1._spoints_z.push_back(pos.z());
    } else if ( sp->get_tracker() == 1 ) {
      ++_t2._num_spoints;
      _t2._spoints_x.push_back(pos.x());
      _t2._spoints_y.push_back(pos.y());
      _t2._spoints_z.push_back(pos.z());
    }
  }
};

void TrackerDataManager::process_strks(const std::vector<SciFiStraightPRTrack*> strks) {
  for (size_t i = 0; i < strks.size(); ++i) {
    SciFiStraightPRTrack *trk = strks[i];
    if ( trk->get_tracker() == 0 ) {
      if ( trk->get_num_points() == 5 ) ++_t1._num_stracks_5pt;
      if ( trk->get_num_points() == 4 ) ++_t1._num_stracks_4pt;
      if ( trk->get_num_points() == 3 ) ++_t1._num_stracks_3pt;
      _t1._trks_str_xz.push_back(make_str_track(trk->get_x0(), trk->get_mx(), _zmin, _zmax));
      _t1._trks_str_yz.push_back(make_str_track(trk->get_y0(), trk->get_my(), _zmin, _zmax));
    } else if ( trk->get_tracker() == 1 ) {
      if ( trk->get_num_points() == 5 ) ++_t2._num_stracks_5pt;
      if ( trk->get_num_points() == 4 ) ++_t2._num_stracks_4pt;
      if ( trk->get_num_points() == 3 ) ++_t2._num_stracks_3pt;
      _t2._trks_str_xz.push_back(make_str_track(trk->get_x0(), trk->get_mx(), _zmin, _zmax));
      _t2._trks_str_yz.push_back(make_str_track(trk->get_y0(), trk->get_my(), _zmin, _zmax));
    }
  }
};

void TrackerDataManager::process_htrks(const std::vector<SciFiHelicalPRTrack*> htrks) {
  for (size_t i = 0; i < htrks.size(); ++i) {
    SciFiHelicalPRTrack *trk = htrks[i];

    // Print track info to screen
    if (_print_tracks) print_track_info(trk, i);

    // Pull out turning angles for each track seed
    std::vector<double> phi_i;
    std::vector<double> s_i;
    for ( size_t i = 0; i < trk->get_phi().size(); ++i ) {
      phi_i.push_back(trk->get_phi()[i]);
      s_i.push_back(trk->get_phi()[i]*trk->get_R());
    }

    // Pull out the z coord for each track seed
    std::vector<double> z_i;
    for ( int i = 0; i < (trk->get_spacepoints()->GetLast()+1); ++i ) {
      z_i.push_back(
        static_cast<SciFiSpacePoint*>(trk->get_spacepoints()->At(i))->get_position().z());
    }

    double x0 = trk->get_circle_x0();
    double y0 = trk->get_circle_y0();
    double rad = trk->get_R();
    double dsdz = trk->get_dsdz();
    double sz_c = trk->get_line_sz_c();
    int handness = -1;

    if ( trk->get_tracker() == 0 ) {
      if ( trk->get_num_points() == 5 ) ++_t1._num_htracks_5pt;
      if ( trk->get_num_points() == 4 ) ++_t1._num_htracks_4pt;
      if ( trk->get_num_points() == 3 ) ++_t1._num_htracks_3pt;
      if ( trk->get_charge() == 1 ) ++_t1._num_pos_tracks;
      if ( trk->get_charge() == -1 ) ++_t1._num_neg_tracks;
      _t1._seeds_z.push_back(z_i);
      _t1._seeds_phi.push_back(phi_i);
      _t1._seeds_s.push_back(s_i);
      _t1._trks_xy.push_back(make_circle(x0, y0, rad));
      // dsdz = - dsdz;  // Needed due to the way we plot...
      _t1._trks_xz.push_back(make_xz(handness, x0, rad, dsdz, sz_c, _zmin, _zmax));
      _t1._trks_yz.push_back(make_yz(y0, rad, dsdz, sz_c, _zmin, _zmax));
      _t1._trks_sz.push_back(make_str_track(sz_c, dsdz, _zmin, _zmax));
    } else if ( trk->get_tracker() == 1 ) {
      if ( trk->get_num_points() == 5 ) ++_t2._num_htracks_5pt;
      if ( trk->get_num_points() == 4 ) ++_t2._num_htracks_4pt;
      if ( trk->get_num_points() == 3 ) ++_t2._num_htracks_3pt;
      if ( trk->get_charge() == 1 ) ++_t2._num_pos_tracks;
      if ( trk->get_charge() == -1 ) ++_t2._num_neg_tracks;
      _t2._seeds_z.push_back(z_i);
      _t2._seeds_phi.push_back(phi_i);
      _t2._seeds_s.push_back(s_i);
      _t2._trks_xy.push_back(make_circle(x0, y0, rad));
      _t2._trks_xz.push_back(make_xz(handness, x0, rad, dsdz, sz_c, _zmin, _zmax));
      _t2._trks_yz.push_back(make_yz(y0, rad, dsdz, sz_c, _zmin, _zmax));
      _t2._trks_sz.push_back(make_str_track(sz_c, dsdz, _zmin, _zmax));
    }

    // Loop over track seed spacepoints
//     if (_print_seeds) {
//       std::cout << "x\ty\tz\ttime\t\tphi\tpx_mc\tpy_mc\tpt_mc\tpz_mc\t";
//       std::cout << "px_mc1\tpx_mc2\tpx_mc3\tpy_mc1\tpy_mc2\tpy_mc3\tpz_mc1\tpz_mc2\tpz_mc3\n";
//     }

    for ( int j = 0; j < (trk->get_spacepoints()->GetLast()+1); ++j ) {
      if ( trk->get_tracker() == 0 ) ++_t1._num_seeds;
      if ( trk->get_tracker() == 1 ) ++_t2._num_seeds;
      // if (_print_seeds) print_seed_info(trk, j);
    }
  }  // ~loop over helical tracks
};

void TrackerDataManager::clear_spill() {
  _t1.clear();
  _t2.clear();
};

void TrackerDataManager::draw(std::vector<TrackerDataPlotterBase*> plotters) {
  // Loop over all the plotters and draw
  TCanvas* lCanvas = NULL;
  for ( size_t i = 0; i < plotters.size(); ++i ) {
    TrackerDataPlotterBase * plt = plotters[i];
    if (plt) {
      lCanvas = (*plt)(_t1, _t2);
      if (plt->GetSaveOutput()) {
        std::string fName = plt->GetOutputName();
        lCanvas->SaveAs(fName.c_str());
      }
    } else {
      std::cerr << "Error: Empty plotter pointer passed\n";
    }
  }
};

TArc TrackerDataManager::make_circle(double x0, double y0, double rad) {
  TArc arc = TArc(x0, y0, rad);
  arc.SetFillStyle(0); // 0 - Transparent
  arc.SetLineColor(kBlue);
  return arc;
};

TF1 TrackerDataManager::make_str_track(double c, double m, double zmin, double zmax) {
  // Note: in the function expression, x is just the independent variable, which
  // in this case is the z coordinate in the tracker coordinate system
  TF1 trk = TF1("trk", "[0]+([1]*x)", zmin, zmax);
  trk.SetParameters(c, m);
  trk.SetLineColor(kRed);
  return trk;
};

TF1 TrackerDataManager::make_xz(int handness, double circle_x0, double rad, double dsdz,
                                double sz_c, double zmin, double zmax) {
    // The x in the cos term is actually representing z (the indep variable)
    TF1 func = TF1("xz_func", "[0]-[4]*([1]*cos((1/[1])*([2]*x+[3])))", zmin, zmax);
    func.SetParameter(0, circle_x0);
    func.SetParameter(1, rad);
    func.SetParameter(2, dsdz);
    func.SetParameter(3, sz_c);
    func.SetParameter(4, handness);
    func.SetLineColor(kBlue);
    return func;
};

TF1 TrackerDataManager::make_yz(double circle_y0, double rad, double dsdz,
                                double sz_c, double zmin, double zmax) {
    // The x in the cos term is actually representing z (the indep variable)
    TF1 func = TF1("yz_func", "[0]+([1]*sin((1/[1])*([2]*x+[3])))", zmin, zmax);
    func.SetParameter(0, circle_y0);
    func.SetParameter(1, rad);
    func.SetParameter(2, dsdz);
    func.SetParameter(3, sz_c);
    func.SetLineColor(kBlue);
    return func;
};

void TrackerDataManager::print_track_info(const SciFiHelicalPRTrack * const trk, int trk_num) {
  double rad = trk->get_R();
  double pt = rad * _RtoPt;
  double pz = pt / trk->get_dsdz();
  std::cout << "Tracker " << trk->get_tracker() << + ", ";
  std::cout << "Track " << trk_num << ", ";
  std::cout << "Num points " << trk->get_num_points() << ", ";
  std::cout << "Charge " << trk->get_charge() << ", ";
  std::cout << "R = " << std::setprecision(4) << trk->get_R() << "mm, ";
  std::cout << "X0 = " << std::setprecision(4) << trk->get_circle_x0() << "mm, ";
  std::cout << "Y0 = " <<  std::setprecision(4) << trk->get_circle_y0() << "mm,\n";
  std::cout << "dsdz " <<  std::setprecision(4) << trk->get_dsdz() << ", ";
  std::cout << "pt = " <<  std::setprecision(4) << pt << "MeV/c, ";
  std::cout << "pz = " <<  std::setprecision(4) << pz << "MeV/c, ";
  std::cout << "xy_chi2 = " << std::setprecision(4) << trk->get_circle_chisq() << ", ";
  std::cout << "sz_c = " << std::setprecision(4) << trk->get_line_sz_c() << ", ";
  std::cout << "sz_chi2 = " << std::setprecision(4) << trk->get_line_sz_chisq() << "\n";
};

// void TrackerDataManager::print_seed_info(const SciFiHelicalPRTrack * const trk, int seed_num) {
//   SciFiSpacePoint* sp = static_cast<SciFiSpacePoint*>(trk->get_spacepoints()->At(seed_num));
//   MAUS::ThreeVector pos = sp->get_position();
//   std::vector<MAUS::SciFiCluster*> chans = sp->get_channels_pointers();
//   double t = sp->get_time();
//
//   double seed_px_mc = 0.0;
//   double seed_py_mc = 0.0;
//   double seed_pz_mc = 0.0;
//   double px_mc1 = 0.0;
//   double px_mc2 = 0.0;
//   double px_mc3 = 0.0;
//   double py_mc1 = 0.0;
//   double py_mc2 = 0.0;
//   double py_mc3 = 0.0;
//   double pz_mc1 = 0.0;
//   double pz_mc2 = 0.0;
//   double pz_mc3 = 0.0;
//   for (size_t iCl = 0; iCl < chans.size(); ++iCl) {
//     ThreeVector p_mc = chans[iCl]->get_true_momentum();
//     seed_px_mc += p_mc.x();
//     seed_py_mc += p_mc.y();
//     seed_pz_mc += p_mc.z();
//     if (iCl == 0) {
//       px_mc1 = p_mc.x();
//       py_mc1 = p_mc.y();
//       pz_mc1 = p_mc.z();
//     }
//     if (iCl == 1) {
//       px_mc2 = p_mc.x();
//       py_mc2 = p_mc.y();
//       pz_mc2 = p_mc.z();
//     }
//     if (iCl == 2) {
//       px_mc3 = p_mc.x();
//       py_mc3 = p_mc.y();
//       pz_mc3 = p_mc.z();
//     }
//   }
//   seed_px_mc /= chans.size();
//   seed_py_mc /= chans.size();
//   seed_pz_mc /= chans.size();
//   double pt_mc = sqrt(seed_px_mc*seed_px_mc + seed_py_mc*seed_py_mc);
//
//   std::cout << std::setprecision(4) << pos.x() << "\t";
//   std::cout << std::setprecision(4) << pos.y() << "\t";
//   std::cout << std::setprecision(4) << pos.z() << "\t";
//   std::cout << std::setprecision(12) << t << "\t";
//   std::cout << std::setprecision(4) << trk->get_phi()[seed_num] << "\t";
//   std::cout << std::setprecision(4) << seed_px_mc << "\t";
//   std::cout << std::setprecision(4) << seed_py_mc << "\t";
//   std::cout << std::setprecision(4) << pt_mc << "\t";
//   std::cout << std::setprecision(4) << seed_pz_mc << "\t";
//   std::cout << std::setprecision(4) << px_mc1 << "\t";
//   std::cout << std::setprecision(4) << px_mc2 << "\t";
//   std::cout << std::setprecision(4) << px_mc3 << "\t";
//   std::cout << std::setprecision(4) << py_mc1 << "\t";
//   std::cout << std::setprecision(4) << py_mc2 << "\t";
//   std::cout << std::setprecision(4) << py_mc3 << "\t";
//   std::cout << std::setprecision(4) << pz_mc1 << "\t";
//   std::cout << std::setprecision(4) << pz_mc2 << "\t";
//   std::cout << std::setprecision(4) << pz_mc3 << "\n";
// };

} // ~namespace MAUS
