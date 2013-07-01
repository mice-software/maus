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

// ROOT Headers
#include "TROOT.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterXYZ.hh"



namespace MAUS {

TrackerDataManager::TrackerDataManager()
  : _t1_sp(0),
    _t2_sp(0),
    _t1_seeds(0),
    _t2_seeds(0),
    _t1_5pt_strks(0),
    _t1_4pt_strks(0),
    _t1_3pt_strks(0),
    _t2_5pt_strks(0),
    _t2_4pt_strks(0),
    _t2_3pt_strks(0),
    _t1_5pt_htrks(0),
    _t1_4pt_htrks(0),
    _t1_3pt_htrks(0),
    _t2_5pt_htrks(0),
    _t2_4pt_htrks(0),
    _t2_3pt_htrks(0) {
  // Do nothing
};

TrackerDataManager::~TrackerDataManager() {
  // Do nothing
};

void TrackerDataManager::process(const Spill *spill) {
  if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
    std::cout << "\nOpening spill " << spill->GetSpillNumber() << std::endl;

    // Loop over recon events
    for (size_t i = 0;  i < spill->GetReconEvents()->size(); ++i) {
      SciFiEvent *evt = (*spill->GetReconEvents())[i]->GetSciFiEvent();

      // Loop over digits
      for (size_t j = 0; j < evt->digits().size(); ++j) {
        SciFiDigit *dig = evt->digits()[j];
        if ( dig->get_tracker() == 0 ) {
          ++_t1._num_digits;
        } else if ( dig->get_tracker() == 1 ) {
          ++_t2._num_digits;
        }
      }

      // Loop over clusters
      for (size_t j = 0; j < evt->clusters().size(); ++j) {
        SciFiCluster *clus = evt->clusters()[j];
        if ( clus->get_tracker() == 0 ) {
          ++_t1._num_clusters;
        } else if ( clus->get_tracker() == 1 ) {
          ++_t2._num_clusters;
        }
      }

      // Loop over spacepoints
      for (size_t j = 0; j < evt->spacepoints().size(); ++j) {
        SciFiSpacePoint *sp = evt->spacepoints()[j];
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

      // Love over straight tracks
      for (size_t j = 0; j < evt->straightprtracks().size(); ++j) {
        SciFiStraightPRTrack *trk = evt->straightprtracks()[j];
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

      // Loop over helical tracks
      for (size_t j = 0; j < evt->helicalprtracks().size(); ++j) {
        SciFiHelicalPRTrack *trk = evt->helicalprtracks()[j];

        // Print track info to screen
        print_track_info(trk, i);

        // Pull out turning angles for each track seed
        std::vector<double> phi_i;
        std::vector<double> s_i;
        for ( size_t j = 0; j < trk->get_phi().size(); ++j ) {
          phi_i.push_back(trk->get_phi()[j]);
          s_i.push_back(trk->get_phi()[j]*trk->get_R());
        }

        double x0 = trk->get_circle_x0();
        double y0 = trk->get_circle_y0();
        double rad = trk->get_R();
        double dsdz = trk->get_dsdz();
        double sz_c = trk->get_line_sz_c();

        if ( trk->get_tracker() == 0 ) {
          if ( trk->get_num_points() == 5 ) ++_t1._num_htracks_5pt;
          if ( trk->get_num_points() == 4 ) ++_t1._num_htracks_4pt;
          if ( trk->get_num_points() == 3 ) ++_t1._num_htracks_3pt;
          _t1._seeds_phi.push_back(phi_i);
          _t1._seeds_s.push_back(s_i);
          _t1._trks_xy.push_back(make_circle(x0, y0, rad));
          dsdz = - dsdz;  // Needed due to the way we plot...
          _t1._trks_xz.push_back(make_xz(x0, rad, dsdz, sz_c, _zmin, _zmax));
          _t1._trks_yz.push_back(make_yz(y0, rad, dsdz, sz_c, _zmin, _zmax));
        } else if ( trk->get_tracker() == 1 ) {
          if ( trk->get_num_points() == 5 ) ++_t2._num_htracks_5pt;
          if ( trk->get_num_points() == 4 ) ++_t2._num_htracks_4pt;
          if ( trk->get_num_points() == 3 ) ++_t2._num_htracks_3pt;
          _t2._seeds_phi.push_back(phi_i);
          _t2._seeds_s.push_back(s_i);
          _t2._trks_xy.push_back(make_circle(x0, y0, rad));
          _t2._trks_xz.push_back(make_xz(x0, rad, dsdz, sz_c, _zmin, _zmax));
          _t2._trks_yz.push_back(make_yz(y0, rad, dsdz, sz_c, _zmin, _zmax));
        }

        // Loop over track seed spacepoints
        std::cout << "x\ty\tz\ttime\t\tphi\tpx_mc\tpy_mc\tpt_mc\tpz_mc\n";
        for ( size_t j = 0; j < trk->get_spacepoints().size(); ++j ) {
          if ( trk->get_tracker() == 0 ) ++_t1._num_seeds;
          if ( trk->get_tracker() == 1 ) ++_t2._num_seeds;
          print_seed_info(trk, j);
        }
      } // ~ loop over helical tracks
    } // ~ loop over recon events

    // Update run totals held by the manager class
    _t1_sp += _t1._num_spoints;
    _t2_sp += _t2._num_spoints;
    _t1_5pt_htrks += _t1._num_stracks_5pt;
    _t1_4pt_htrks += _t1._num_stracks_4pt;
    _t1_3pt_htrks += _t1._num_stracks_3pt;
    _t2_5pt_htrks += _t2._num_stracks_5pt;
    _t2_4pt_htrks += _t2._num_stracks_4pt;
    _t2_3pt_htrks += _t2._num_stracks_3pt;
    _t1_5pt_htrks += _t1._num_htracks_5pt;
    _t1_4pt_htrks += _t1._num_htracks_4pt;
    _t1_3pt_htrks += _t1._num_htracks_3pt;
    _t2_5pt_htrks += _t2._num_htracks_5pt;
    _t2_4pt_htrks += _t2._num_htracks_4pt;
    _t2_3pt_htrks += _t2._num_htracks_3pt;
  } // ~ check event is a physics event
};

void TrackerDataManager::clear_spill() {
  _t1.clear();
  _t2.clear();
};

void TrackerDataManager::clear_run() {
  _t1_sp = 0;
  _t2_sp = 0;
  _t1_seeds = 0;
  _t2_seeds = 0;
  _t1_5pt_strks = 0;
  _t1_4pt_strks = 0;
  _t1_3pt_strks = 0;
  _t2_5pt_strks = 0;
  _t2_4pt_strks = 0;
  _t2_3pt_strks = 0;
  _t1_5pt_htrks = 0;
  _t1_4pt_htrks = 0;
  _t1_3pt_htrks = 0;
  _t2_5pt_htrks = 0;
  _t2_4pt_htrks = 0;
  _t2_3pt_htrks = 0;

  clear_spill();
};

void TrackerDataManager::draw(std::vector<TrackerDataPlotterBase*> plotters) {
  // Loop over all the plotters and draw
  // TCanvas* lCanvas = NULL;
  for ( size_t i = 0; i < plotters.size(); ++i ) {
    TrackerDataPlotterBase * plt = plotters[i];
    if (plt) {
      (*plt)(_t1, _t2);
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

TF1 TrackerDataManager::make_xz(double circle_x0, double rad, double dsdz,
                                double sz_c, double zmin, double zmax) {
    // The x in the cos term is actually representing z (the indep variable)
    TF1 func = TF1("xz_func", "[0]+([1]*cos((1/[1])*([2]*x+[3])))", zmin, zmax);
    func.SetParameter(0, circle_x0);
    func.SetParameter(1, rad);
    func.SetParameter(2, dsdz);
    func.SetParameter(3, sz_c);
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
  std::cout << "X0 = " << std::setprecision(4) << trk->get_circle_x0() << "mm, ";
  std::cout << "Y0 = " <<  std::setprecision(4) << trk->get_circle_y0() << "mm, ";
  std::cout << "dsdz " <<  std::setprecision(4) << trk->get_dsdz() << ", ";
  std::cout << "pt = " <<  std::setprecision(4) << pt << "MeV/c, ";
  std::cout << "pz = " <<  std::setprecision(4) << pz << "MeV/c, ";
  std::cout << "xy_chi2 = " << std::setprecision(4) << trk->get_circle_chisq() << ", ";
  std::cout << "sz_c = " << std::setprecision(4) << trk->get_line_sz_c() << ", ";
  std::cout << "sz_chi2 = " << std::setprecision(4) << trk->get_line_sz_chisq() << "\n";
};

void TrackerDataManager::print_seed_info(const SciFiHelicalPRTrack * const trk, int seed_num) {
  MAUS::ThreeVector pos = trk->get_spacepoints()[seed_num]->get_position();
  MAUS::SciFiCluster *clus = trk->get_spacepoints()[seed_num]->get_channels()[0];
  MAUS::ThreeVector mom = clus->get_true_momentum();
  double t = trk->get_spacepoints()[seed_num]->get_time();
  double pt_mc = sqrt(mom.x()*mom.x()+mom.y()*mom.y());
  std::cout <<  std::setprecision(4) << pos.x() << "\t";
  std::cout <<  std::setprecision(4) << pos.y() << "\t";
  std::cout <<  std::setprecision(4) << pos.z() << "\t";
  std::cout <<  std::setprecision(12) << t << "\t";
  std::cout <<  std::setprecision(4) << trk->get_phi()[seed_num] << "\t";
  std::cout <<  std::setprecision(4) << mom.x() << "\t";
  std::cout <<  std::setprecision(4) << mom.y() << "\t";
  std::cout <<  std::setprecision(4) << pt_mc << "\t";
  std::cout <<  std::setprecision(4) << mom.z() << "\n";
};

void TrackerDataManager::print_run_info() {
  std::cout << "T1 spacepoints: " <<  _t1_sp << std::endl;
  std::cout << "T2 spacepoints: " <<  _t2_sp << std::endl;
  std::cout << "T1 seeds: " <<  _t1_seeds << std::endl;
  std::cout << "T2 seeds: " <<  _t2_seeds << std::endl;
  std::cout << "T1 5 point straight tracks: " <<  _t2_5pt_strks << std::endl;
  std::cout << "T1 4 point straight tracks: " <<  _t2_4pt_strks << std::endl;
  std::cout << "T1 3 point straight tracks: " <<  _t2_3pt_strks << std::endl;
  std::cout << "T2 5 point straight tracks: " <<  _t2_5pt_strks << std::endl;
  std::cout << "T2 4 point straight tracks: " <<  _t2_4pt_strks << std::endl;
  std::cout << "T2 3 point straight tracks: " <<  _t2_3pt_strks << std::endl;
  std::cout << "T1 5 point helical tracks: " <<  _t2_5pt_htrks << std::endl;
  std::cout << "T1 4 point helical tracks: " <<  _t2_4pt_htrks << std::endl;
  std::cout << "T1 3 point helical tracks: " <<  _t2_3pt_htrks << std::endl;
  std::cout << "T2 5 point helical tracks: " <<  _t2_5pt_htrks << std::endl;
  std::cout << "T2 4 point helical tracks: " <<  _t2_4pt_htrks << std::endl;
  std::cout << "T2 3 point helical tracks: " <<  _t2_3pt_htrks << std::endl;
};

} // ~namespace MAUS
