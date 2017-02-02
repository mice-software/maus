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

// C headers
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <json/json.h>

// C++ headers
// #include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

// External libs headers
#include "TROOT.h"
#include "TFile.h"
#include "TH1D.h"
#include "TMatrixD.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"


namespace MAUS {


// Four predicate functions used by the stl sort algorithm to sort spacepoints in vectors
bool compare_spoints_ascending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() < sp2->get_position().z());
}

bool compare_spoints_descending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() > sp2->get_position().z());
}

bool compare_spoints_ascending_station(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_station() < sp2->get_station());
}

bool compare_spoints_descending_station(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_station() > sp2->get_station());
}

PatternRecognition::PatternRecognition(): _debug(false),
                                          _up_straight_pr_on(true),
                                          _down_straight_pr_on(true),
                                          _up_helical_pr_on(true),
                                          _down_helical_pr_on(true),
                                          _verb(0),
                                          _n_trackers(2),
                                          _n_stations(5),
                                          _bz_t1(4.0),
                                          _bz_t2(4.0),
                                          _sd_1to4(0.3844),
                                          _sd_5(0.4298),
                                          _sd_phi_1to4(1.0),
                                          _sd_phi_5(1.0),
                                          _res_cut(7.0),
                                          _straight_chisq_cut(50.0),
                                          _R_res_cut(150.0),
                                          _circle_chisq_cut(15.0),
                                          _n_turns_cut(0.75),
                                          _sz_chisq_cut(24.0),
                                          _Pt_max(180.0),
                                          _Pz_min(50.0),
                                          _rfile(NULL),
                                          _hx(NULL),
                                          _hy(NULL),
                                          _hxchisq(NULL),
                                          _hychisq(NULL),
                                          _hxychisq(NULL),
                                          _hszchisq(NULL) {
  // Do nothing
}

void PatternRecognition::set_parameters_to_default() {
  _debug = false;
  _up_straight_pr_on = true;
  _down_straight_pr_on = true;
  _up_helical_pr_on = true;
  _down_helical_pr_on = true;
  _verb = 0;
  _n_trackers = 2;
  _n_stations = 5;
  _bz_t1 = 4.0;
  _bz_t2 = 4.0;
  _sd_1to4 = 0.3844;
  _sd_5 = 0.4298;
  _sd_phi_1to4 = 1.0;
  _sd_phi_5 = 1.0;
  _res_cut = 7.0;
  _straight_chisq_cut = 50.0;
  _R_res_cut = 150.0;
  _circle_chisq_cut = 15.0;
  _n_turns_cut = 0.75;
  _sz_chisq_cut = 24.0;
  _Pt_max = 180.0;
  _Pz_min = 50.0;
}

PatternRecognition::~PatternRecognition() {
  if (_debug) {
    if (_rfile) _rfile->cd();
    if (_hx) _hx->Write();
    if (_hy) _hy->Write();
    if (_hxchisq) _hxchisq->Write();
    if (_hychisq) _hychisq->Write();
    if (_hxychisq) _hxychisq->Write();
    if (_hszchisq) _hszchisq->Write();
  }
}

void PatternRecognition::setup_debug() {
  _debug = true;
  _rfile = new TFile("pattern_recongition_debug.root", "RECREATE");
  _hx = new TH1D("hx", "Pattern Recognition Road (mm) X Residuals All Candidates", 200, -150, 150);
  _hy = new TH1D("hy", "Pattern Recognition Road (mm) Y Residuals All Candidates", 500, -150, 150);
  _hxchisq =
    new TH1D("hxchisq", "Pattern Recognition Straight Fit x ChiSq/DoF All Candidates", 200, 0, 500);
  _hychisq =
    new TH1D("hychisq", "Pattern Recognition Straight Fit y ChiSq/DoF All Candidates", 200, 0, 500);
  _hxychisq =
    new TH1D("hxychisq", "Pattern Recognition Circle Fit ChiSq/DoF All Candidates", 200, 0, 10);
  _hszchisq =
    new TH1D("hszchisq", "Pattern Recognition s-z Fit ChiSq/DoF All Candidates", 200, 0, 400);
  _hx->Write();
  _hy->Write();
  _hxchisq->Write();
  _hychisq->Write();
  _hxychisq->Write();
  _hszchisq->Write();
}

bool PatternRecognition::LoadGlobals() {
  if (Globals::HasInstance()) {
    Json::Value *json = Globals::GetConfigurationCards();
    _verb = (*json)["SciFiPatRecVerbosity"].asInt();
    _n_trackers = (*json)["SciFinTrackers"].asInt();
    _n_stations = (*json)["SciFinStations"].asInt();
    _sd_1to4 = (*json)["SciFi_sigma_triplet"].asDouble();
    _sd_5 = (*json)["SciFi_sigma_tracker0_station5"].asDouble();
    _sd_phi_1to4 = (*json)["SciFi_sigma_phi_1to4"].asDouble();
    _sd_phi_5 = (*json)["SciFi_sigma_phi_5"].asDouble();
    _res_cut = (*json)["SciFiStraightRoadCut"].asDouble();
    _straight_chisq_cut = (*json)["SciFiStraightChi2Cut"].asDouble();
    _R_res_cut = (*json)["SciFiRadiusResCut"].asDouble();
    _circle_chisq_cut = (*json)["SciFiPatRecCircleChi2Cut"].asDouble();
    _n_turns_cut = (*json)["SciFiNTurnsCut"].asDouble();
    _sz_chisq_cut = (*json)["SciFiPatRecSZChi2Cut"].asDouble();
    _Pt_max = (*json)["SciFiMaxPt"].asDouble();
    _Pz_min = (*json)["SciFiMinPz"].asDouble();
    return true;
  } else {
    return false;
  }
}

void PatternRecognition::process(SciFiEvent &evt) const {

  if ( evt.spacepoints().size() > 0 ) {
    if ( _verb > 0 )
      std::cerr << "Number of spoints in event: " << evt.spacepoints().size() << std::endl;

    // Some setup
    evt.set_spacepoints_used_flag(false);
    SpacePoint2dPArray spnts_by_tracker(_n_trackers);
    spnts_by_tracker = SciFiTools::sort_by_tracker(evt.spacepoints());
    if (_verb > 0) {
      std::cerr << "Number of spoints in event in T1: " << spnts_by_tracker[0].size() << std::endl;
      SciFiTools::print_spacepoint_xyz(spnts_by_tracker[0]);
      std::cerr << "Number of spoints in event in T2: " << spnts_by_tracker[1].size() << std::endl;
      SciFiTools::print_spacepoint_xyz(spnts_by_tracker[1]);
    }

    int trker_no = 0;
    // Split spacepoints according to which station they occured in
    SpacePoint2dPArray spnts_by_station_tku(_n_stations);
    SciFiTools::sort_by_station(spnts_by_tracker[trker_no], spnts_by_station_tku);

    // Make the helical and straight tracks, depending on flags
    if ( _up_helical_pr_on ) {
      bool track_type = 1;
      make_all_tracks(track_type, trker_no, spnts_by_station_tku, evt);
    }
    if ( _up_straight_pr_on ) {
      bool track_type = 0;
      make_all_tracks(track_type, trker_no, spnts_by_station_tku, evt);
    }

    trker_no = 1;
    // Split spacepoints according to which station they occured in
    SpacePoint2dPArray spnts_by_station_tkd(_n_stations);
    SciFiTools::sort_by_station(spnts_by_tracker[trker_no], spnts_by_station_tkd);

    // Make the helical and straight tracks, depending on flags
    if ( _down_helical_pr_on ) {
      bool track_type = 1;
      make_all_tracks(track_type, trker_no, spnts_by_station_tkd, evt);
    }
    if ( _down_straight_pr_on ) {
      bool track_type = 0;
      make_all_tracks(track_type, trker_no, spnts_by_station_tkd, evt);
    }
  } else {
    if (_verb > 0) std::cerr << "No spacepoints in event" << std::endl;
  }
};

void PatternRecognition::make_all_tracks(const bool track_type, const int trker_no,
                                     SpacePoint2dPArray &spnts_by_station, SciFiEvent &evt) const {

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);

  // Make the tracks
  if (num_stations_hit == 5) {
    std::vector<SciFiStraightPRTrack*> strks;
    std::vector<SciFiHelicalPRTrack*> htrks;
    make_5tracks(track_type, trker_no, spnts_by_station, strks, htrks);
    std::vector<SciFiStraightPRTrack*> accepted_strks = select_tracks(strks);
    std::vector<SciFiHelicalPRTrack*> accepted_htrks = select_tracks(htrks);
    track_processing(trker_no, 5, strks, htrks);
    add_tracks(accepted_strks, accepted_htrks, evt);
  }
  num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);
  if (num_stations_hit > 3) {
    std::vector<SciFiStraightPRTrack*> strks;
    std::vector<SciFiHelicalPRTrack*> htrks;
    make_4tracks(track_type, trker_no, spnts_by_station, strks, htrks);
    std::vector<SciFiStraightPRTrack*> accepted_strks = select_tracks(strks);
    std::vector<SciFiHelicalPRTrack*> accepted_htrks = select_tracks(htrks);
    track_processing(trker_no, 4, strks, htrks);
    add_tracks(accepted_strks, accepted_htrks, evt);
  }
  num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);
  if (num_stations_hit > 2  && track_type != 1) { // No 3pt tracks for helical
    std::vector<SciFiStraightPRTrack*> strks;
    std::vector<SciFiHelicalPRTrack*> htrks;
    make_3tracks(trker_no, spnts_by_station, strks);
    std::vector<SciFiStraightPRTrack*> accepted_strks = select_tracks(strks);
    std::vector<SciFiHelicalPRTrack*> accepted_htrks = select_tracks(htrks);
    track_processing(trker_no, 3, strks, htrks);
    add_tracks(accepted_strks, accepted_htrks, evt);
  }
}

void PatternRecognition::add_tracks(std::vector<SciFiStraightPRTrack*> &strks,
      std::vector<SciFiHelicalPRTrack*> &htrks, SciFiEvent &evt) const {
  for (auto trk : strks) {
    evt.add_straightprtrack(trk);
  }
  for (auto trk : htrks) {
    evt.add_helicalprtrack(trk);
  }
}

void PatternRecognition::track_processing(const int trker_no, const int n_points,
                                          std::vector<SciFiStraightPRTrack*> &strks,
                                          std::vector<SciFiHelicalPRTrack*> &htrks) const {
  set_tracker_number(trker_no, strks, htrks);
}

void PatternRecognition::set_tracker_number(const int trker_no,
                                            std::vector<SciFiStraightPRTrack*> &strks,
                                            std::vector<SciFiHelicalPRTrack*> &htrks) const {
  for ( int i = 0; i < static_cast<int>(strks.size()); ++i ) {
    strks[i]->set_tracker(trker_no);
  }
  for ( int i = 0; i < static_cast<int>(htrks.size()); ++i ) {
    htrks[i]->set_tracker(trker_no);
  }
}

void PatternRecognition::make_5tracks(const bool track_type, const int trker_no,
                                      SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack*> &strks,
                                      std::vector<SciFiHelicalPRTrack*> &htrks) const {
  if ( _verb > 0 ) std::cout << "INFO: PatternRecognition: Making 5 point tracks\n";
  int n_points = 5;
  std::vector<int> ignore_stations; // A zero size vector sets that all stations are to be used
  if ( track_type == 0 )
    make_straight_tracks(n_points, trker_no, ignore_stations, spnts_by_station, strks);
  if ( track_type == 1 ) {
    std::vector<SciFiSpacePoint*> current_spnts;
    make_helix(n_points, 0, ignore_stations, current_spnts, spnts_by_station, htrks);
  }
  if ( _verb > 0 ) std::cerr << "INFO: PatternRecognition: Finished making 5 pt tracks\n";
} // ~make_spr_5pt(...)

void PatternRecognition::make_4tracks(const bool track_type, const int trker_no,
                                      SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack*> &strks,
                                      std::vector<SciFiHelicalPRTrack*> &htrks) const {
  int n_points = 4;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    if ( _verb > 0 ) std::cerr <<
      "INFO: PatternRecognition: Making 4 point track: 5 stations with unused spacepoints\n";

    for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
      // Recount how many stations have at least one unused spacepoint
      num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);
      // If there are enough occupied stations left to make a 4 point track, keep making tracks
      if ( num_stations_hit  >= n_points ) {
        std::vector<int> ignore_stations(1, i);
        if ( track_type == 0 )
          make_straight_tracks(n_points, trker_no, ignore_stations, spnts_by_station, strks);
        if ( track_type == 1 ) {
          std::vector<SciFiSpacePoint*> current_spnts;
          make_helix(n_points, 0, ignore_stations, current_spnts, spnts_by_station, htrks);
        }
      } else {
        break;
      }
    } // ~Loop of stations, ignoring each one in turn
  } else if ( num_stations_hit == 4 ) {

    if ( _verb > 0 ) std::cout <<
      "INFO: PatternRecognition: Making 4 point track: 4 stations with unused spacepoints\n";

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    SciFiTools::stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      if ( track_type == 0 )
        make_straight_tracks(n_points, trker_no, stations_not_hit, spnts_by_station, strks);
      if ( track_type == 1 ) {
        std::vector<SciFiSpacePoint*> current_spnts;
        make_helix(n_points, 0, stations_not_hit, current_spnts, spnts_by_station, htrks);
      }
    } else {
      if ( _verb > 0 ) {
        std::cerr << "WARNING: PatternRecognition: Wrong number of stations without spacepoints, ";
        std::cerr << "aborting 4 pt track." << std::endl;
      }
    }
  } else if ( num_stations_hit < 4 ) {
    if ( _verb > 0 ) std::cout <<
      "INFO: PatternRecognition: Not enough unused spacepoints, quiting 4 point track\n";
  } else if ( num_stations_hit > 6 ) {
    if ( _verb > 0 ) {
      std::cerr << "WARNING: PatternRecognition: Wrong number of stations without spacepoints, ";
      std::cerr << "aborting 4 pt track." << std::endl;
    }
  }
  if ( _verb > 0 ) std::cout << "INFO: PatternRecognition:Finished making 4 point tracks\n";
} // ~make_straight_4tracks(...)

void PatternRecognition::make_3tracks(const int trker_no, SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack*> &strks) const {
  int n_points = 3;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);

  bool sufficient_hit_stations = true;

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    if ( _verb > 0 ) std::cout <<
      "INFO: PatternRecognition: Making 3 point track: 5 stations with unused spacepoints\n";

    for (int i = 0; i < 4; ++i) { // Loop of first station to ignore
      if ( sufficient_hit_stations ) {
        for (int j = i + 1; j < 5; ++j) { // Loop of second station to ignore
          if ( sufficient_hit_stations ) {
            // Recount how many stations have at least one unused spacepoint
            num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);
            // If there are enough occupied stations left to make a 3pt track, keep making tracks
            if ( num_stations_hit  >= n_points ) {
              std::vector<int> ignore_stations;
              ignore_stations.push_back(i);
              ignore_stations.push_back(j);
              make_straight_tracks(n_points, trker_no, ignore_stations, spnts_by_station, strks);
            } else {
                sufficient_hit_stations = false;
            }
          } // ~if ( sufficient_hit_stations )
        } // ~Loop of second station to ignore
      } // ~if ( sufficient_hit_stations )
    } // ~Loop of first station to ignore
  } else if ( num_stations_hit == 4 ) {

    if ( _verb > 0 ) std::cout <<
      "INFO: PatternRecognition: Making 3 point track: 4 stations with unused spacepoints\n";

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    SciFiTools::stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);
    std::vector<int> ignore_stations;

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
        // Recount how many stations have at least one unused spacepoint
        num_stations_hit = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);
        // If there are enough occupied stations left to make a 4 point track, keep making tracks
        if ( num_stations_hit  >= n_points ) {
          ignore_stations.clear();
          if ( stations_not_hit[0] != i ) { // Don't send the same 2 ignore stations
            ignore_stations.push_back(stations_not_hit[0]);
            ignore_stations.push_back(i);
            make_straight_tracks(n_points, trker_no, ignore_stations, spnts_by_station, strks);
          }
        } else {
          break;
        }
      }
    }
  } else if ( num_stations_hit == 3 ) {

    if ( _verb > 0 ) std::cout <<
      "INFO: PatternRecognition: Making 3 point track: 3 stations with unused spacepoints\n";

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    SciFiTools::stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 2 ) {
      make_straight_tracks(n_points, trker_no, stations_not_hit, spnts_by_station, strks);
    } else {
      if ( _verb > 0 ) {
        std::cerr << "WARNING: PatternRecognition: Wrong number of stations without spacepoints, ";
        std::cerr << "aborting 3 pt track." << std::endl;
      }
    }
  } else if ( num_stations_hit < 3 ) {
    if ( _verb > 0 )
      std::cout << "INFO: PatternRecognition: Not enough unused spacepoints, quiting 3 pt track\n";
  } else if ( num_stations_hit > 6 ) {
      if ( _verb > 0 ) {
        std::cerr << "WARNING: PatternRecognition: Wrong number of stations without spacepoints, ";
        std::cerr << "aborting 3 pt track." << std::endl;
      }
  }
  if ( _verb > 0 ) std::cout << "INFO: PatternRecognition: Finished making 3 pt tracks\n";
} // ~make_straight_3tracks(...)

void PatternRecognition::make_straight_tracks(const int n_points, const int trker_no,
                                     const std::vector<int> ignore_stations,
                                     SpacePoint2dPArray &spnts_by_station,
                                     std::vector<SciFiStraightPRTrack*> &strks) const {

  // Set up a secondary ROOT file to hold non-standard debug data
  if (_rfile && _debug) _rfile->cd();

  // Set inner and outer stations
  int o_st_num = -1, i_st_num = -1;
  set_end_stations(ignore_stations, o_st_num, i_st_num);

  if (static_cast<int>(spnts_by_station.size()) != _n_stations
      || o_st_num < 0 || o_st_num >= _n_stations || i_st_num < 0 || i_st_num >= _n_stations) {
    if ( _verb > 0 ) {
      std::cerr << "WARNING: PatternRecognition:Bad spnts_by_station passed, ";
      std::cerr << "aborting make_straight_tracks.\n";
    }
    return;
  }

  // Loop over spacepoints in outer station
  for ( size_t outer_sp = 0; outer_sp < spnts_by_station[o_st_num].size(); ++outer_sp ) {

    // Check the outer spacepoint is unused and enough stations are left with unused sp
    if ( spnts_by_station[o_st_num][outer_sp]->get_used() ||
          SciFiTools::num_stations_with_unused_spnts(spnts_by_station) < n_points) continue;

    // Loop over spacepoints in inner station
    for ( size_t inner_sp = 0; inner_sp < spnts_by_station[i_st_num].size(); ++inner_sp ) {

      // Check the inner spacepoint is unused and enough stations are left with unused sp
      if ( spnts_by_station[i_st_num][inner_sp]->get_used() ||
           SciFiTools::num_stations_with_unused_spnts(spnts_by_station) < n_points ) continue;

      // Vector to hold the good spacepoints in each station
      std::vector<SciFiSpacePoint*> good_spnts;

      // Set variables to hold which stations are to be ignored
      int ignore_st_1 = -1, ignore_st_2 = -1;
      set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

      // Draw a straight line between spacepoints in outer most and inner most stations
      SimpleLine line_x, line_y;
      SciFiTools::draw_line(spnts_by_station[o_st_num][outer_sp],
                            spnts_by_station[i_st_num][inner_sp], line_x, line_y);

      // Loop over intermediate stations and compare spacepoints with the line
      for ( int st_num = i_st_num + 1; st_num < o_st_num; ++st_num ) {
        if (st_num != ignore_st_1 && st_num != ignore_st_2) {

          // A large number so initial value is set as best first
          double delta_sq = 1000000;
          int best_sp = -1;

          // Loop over spacepoints
          for ( size_t sp_no = 0; sp_no < spnts_by_station[st_num].size(); ++sp_no ) {
            SciFiSpacePoint* sp = spnts_by_station[st_num][sp_no];
            // If the spacepoint has not already been used in a track fit
            if ( !sp->get_used() ) {
              // Apply roadcuts & find spoints with smallest residuals to the line
              double cx = line_x.get_c();
              double mx = line_x.get_m();
              double cy = line_y.get_c();
              double my = line_y.get_m();
              ThreeVector pos = sp->get_position();
              // Vertical residuals
              double dx = (mx * pos.z() + cx) - pos.x();
              double dy = (my * pos.z() + cy) - pos.y();
              // Perpendicular residuals
              // double dx = fabs(pos.x() - (cx + mx*pos.z())) / sqrt(1.0 + mx*mx);
              // double dy = fabs(pos.y() - (cy + my*pos.z())) / sqrt(1.0 + my*my);
              if (_hx && _debug) _hx->Fill(dx);
              if (_hy && _debug) _hy->Fill(dy);
              if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut )  {
                if ( delta_sq > (dx*dx + dy*dy) ) {
                  delta_sq = dx*dx + dy*dy;
                  best_sp = sp_no;
                }
              } // ~If pass roadcuts and beats previous best fit point
            } // ~If spacepoint is unused
          } // ~Loop over spacepoints
          // Push back the best spacepoint found for the current station
          if (best_sp > -1) {
            SciFiSpacePoint * sp = spnts_by_station[st_num][best_sp];
            good_spnts.push_back(sp);
          }// ~if (counter > 0)
        } // ~if (st_num != ignore_station)
      } // ~Loop over intermediate stations

      // Clear the line objects so we can reuse them
      line_x.clear();
      line_y.clear();

      // Check we have at least 1 good spacepoint in each of the intermediate stations
      if ( static_cast<int>(good_spnts.size()) > (n_points - 3) ) {
        good_spnts.insert(good_spnts.begin(), spnts_by_station[i_st_num][inner_sp]);
        good_spnts.push_back(spnts_by_station[o_st_num][outer_sp]);

        // Fit the track, and if it succeeds add to the tracks vector
        SciFiStraightPRTrack* track = fit_straight_track(n_points, good_spnts);
        if (track) strks.push_back(track);
      } else {
        // std::cerr << "DEBUG: PatternRecognition: No good trial track found\n";
      }// ~ if ( good_spnts.size() > 1 )
    } // ~Loop over sp in station 1
  } // ~Loop over sp in station 5
} // ~make_straight_tracks(...)



SciFiStraightPRTrack* PatternRecognition::fit_straight_track(const int n_points,
    std::vector<SciFiSpacePoint*> spnts) const {
  SciFiStraightPRTrack* track = NULL;

  std::vector<double> x, x_err, y, y_err, z;
  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    x.push_back(spnts[i]->get_position().x());
    y.push_back(spnts[i]->get_position().y());
    z.push_back(spnts[i]->get_position().z());

    // The error on the position measurements of sp in a tracker (same in x and y)
    double sd = -1.0;
    if ( spnts[i]->get_station() == 5 )
      sd = _sd_5;
    else
      sd = _sd_1to4;
    x_err.push_back(sd);
    y_err.push_back(sd);
  }

  // Fit track
  SimpleLine line_x, line_y;
  TMatrixD cov_x(2, 2), cov_y(2, 2);
  LeastSquaresFitter::linear_fit(z, x, x_err, line_x, cov_x);
  LeastSquaresFitter::linear_fit(z, y, y_err, line_y, cov_y);

  // Squash the covariances of each fit into one matrix
  cov_x.ResizeTo(4, 4);
  for ( int i = 0; i < 2; ++i ) {
    for ( int j = 0; j < 2; ++j ) {
      cov_x(i+2, j+2) = cov_y(i, j);
    }
  }
  double* a1 = cov_x.GetMatrixArray();
  std::vector<double> covariance(a1, &a1[16]);

  // Check track passes chisq test, then create SciFiStraightPRTrack
  if (_hxchisq && _debug) _hxchisq->Fill(( line_x.get_chisq() / ( n_points - 2 )));
  if (_hychisq && _debug) _hychisq->Fill(( line_y.get_chisq() / ( n_points - 2 )));
  if ( ( line_x.get_chisq() / ( n_points - 2 ) < _straight_chisq_cut ) &&
      ( line_y.get_chisq() / ( n_points - 2 ) < _straight_chisq_cut ) ) {

    if ( _verb > 0 ) {
      std::cout << "INFO: Pattern Recognition: chisq test passed, ";
      std::cout << "adding " << n_points << "pt candidate track\n";
    }
    track = new SciFiStraightPRTrack(-1, line_x, line_y, covariance);

    // Set all the good sp to used
    // for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    //   spnts[i]->set_used(true);
    // }

    // Populate the sp of the track and then push the track back into the strks vector
    track->set_spacepoints_pointers(spnts);
  } else {
    if ( _verb > 0 ) {
      std::cout << "INFO: Pattern Recognition: x_chisq = " << line_x.get_chisq();
      std::cout << "\ty_chisq = " << line_y.get_chisq() << std::endl;
      std::cout << "chisq test failed, " << n_points << "pt track rejected\n";
    }
  } // ~Check track passes chisq test
  return track;
}

void PatternRecognition::make_helix(const int n_points, const int stat_num,
                                    const std::vector<int> ignore_stations,
                                    std::vector<SciFiSpacePoint*> &current_spnts,
                                    SpacePoint2dPArray &spnts_by_station,
                                    std::vector<SciFiHelicalPRTrack*> &htrks) const {
// if (_verb > 0) std::cout << "make_helix: # of current spnts: " << current_spnts.size() << "\n";

  // Set variables to hold which stations are to be ignored
  int ignore_st_1 = -1, ignore_st_2 = -1;
  set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

  // If we are on an ignore station, move to the next station without doing anything else
  // (this is the first possible recursive bit)
  if ( stat_num == ignore_st_1 || stat_num == ignore_st_2 ) {
      make_helix(n_points, stat_num+1, ignore_stations, current_spnts, spnts_by_station, htrks);
      return;
  }

  // Set the minimum station number to 0, unless that is an ignore station
  int stat_num_min = 0;
  if ( ignore_st_1 == 0 || ignore_st_2 == 0 ) stat_num_min = 1;

  // Set the maximum station number to 4, unless that is an ignore station
  int stat_num_max = 4;
  if ( ignore_st_1 == 4 || ignore_st_2 == 4 ) stat_num_max = 3;

  // Loop over spacepoints in current station
  for ( size_t sp_num = 0; sp_num < spnts_by_station[stat_num].size(); ++sp_num ) {
    // If the current sp is used, skip it
    if ( spnts_by_station[stat_num][sp_num]->get_used() ) {
    // if (_verb > 0) std::cout << "Stat: " << stat_num << " SP: " << sp_num << " used, skipin\n";
      continue;
    }

  // Add the current spnt to the list being tried at present
  // if (_verb > 0) std::cout << "Stat: " << stat_num << " SP: " << sp_num  << " unused, adding ";
    current_spnts.push_back(spnts_by_station[stat_num][sp_num]);
  // if (_verb > 0) std::cout << " new number of current spnts: " << current_spnts.size() << "\n";

    // If we are on the last station, attempt to form a track using the spacepoints collected
    if (stat_num == stat_num_max) {
      SciFiHelicalPRTrack* trk = form_track(n_points, current_spnts);

      // If we found a track, clear current spacepoints to trigger break out to outer most station
      if ( trk != NULL ) {
        if (_verb > 0)
          std::cout << "INFO: Pattern Recognition: Found track candidate, adding" << std::endl;
        htrks.push_back(trk);
        // current_spnts.clear();
        // current_spnts.resize(0);
        // return;
      } // else {
        // current_spnts.pop_back();
      // }
      current_spnts.pop_back();

    // If not on the final station, move on to next (the last recursive call)
    } else {
      make_helix(n_points, stat_num+1, ignore_stations, current_spnts, spnts_by_station, htrks);

      // If not on the first station and current_spnts is empty, break out as track was found
      if ( stat_num != stat_num_min && current_spnts.size() == 0 )
        return;
      // If we we have current spnts, remove the last tried, before trying another
      else if ( current_spnts.size() != 0 )
        current_spnts.pop_back();
    }
  }
  return;
}

SciFiHelicalPRTrack* PatternRecognition::form_track(const int n_points,
                                                    std::vector<SciFiSpacePoint*> spnts ) const {
  // SciFiTools::print_spacepoint_xyz(spnts);

  // Perform a circle fit now that we have found a set of spacepoints
  SimpleCircle c_trial;
  TMatrixD cov_circle(3, 3); // The covariance matrix for the circle parameters alpha, beta, gamma
  bool good_radius = LeastSquaresFitter::circle_fit(_sd_1to4, _sd_5, _R_res_cut, spnts, c_trial,
                                                    cov_circle);

  // If the radius calculated is too large or chisq fails, return NULL
  if ( !good_radius || !( c_trial.get_chisq() / ( n_points - 2 ) < _circle_chisq_cut ) ) {
    if ( _verb > 0 ) std::cout << "INFO: Pattern Recognition: Failed circle cut, chisq = "
                               << c_trial.get_chisq() << "\n";
    return NULL;
  }

  // std::cerr << "cov_circle: " << std::endl;
  // cov_circle.Print();

  // Calculate the standard deviations on the values of s found
  std::vector<double> sigma_s;
  for (auto sp : spnts) {
    double sig = sigma_on_s(c_trial, cov_circle, sp);
    sigma_s.push_back(sig);
    // std::cerr << "Adding sigma_s: " << sig << std::endl; 
  }

  // Perform the s - z fit
  SimpleLine line_sz;
  std::vector<double> phi_i;  // The change between turning angles wrt first spacepoint
  int handedness = 0;         // The particle track handedness
  TMatrixD cov_sz(2, 2);      // The covariance matrix of the sz fit paramters c_sz, dsdz
  bool good_dsdz = find_dsdz(n_points, spnts, c_trial, sigma_s, phi_i, line_sz, cov_sz, handedness);
  if (!good_dsdz) {
    if ( _verb > 0 ) std::cout << "INFO: Pattern Recognition: dsdz fit failed, looping...\n";
    return NULL;
  }

  // Squash the covariances of each fit into one vector
  std::vector<double> covariance;
  for ( int i = 0; i < 3; ++i ) {
    for ( int j = 0; j < 3; ++j ) {
      covariance.push_back(cov_circle[i][j]);
    }
  }
  for ( int i = 0; i < 2; ++i ) {
    for ( int j = 0; j < 2; ++j ) {
      covariance.push_back(cov_sz[i][j]);
    }
  }

  // Set all the good sp to used and set the track seeds with them
//   for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
//     spnts[i]->set_used(true);
//   }

  // Set the charge
//  int charge = - handedness;
  int charge;
  if (spnts[0]->get_tracker() == 0) {
    if ( _bz_t1 > 0 ) {
      charge = handedness;
    } else {
      charge = - handedness;
    }
  } else {
    if ( _bz_t2 > 0 ) {
      charge = - handedness;
    } else {
      charge = handedness;
    }
  }

  // Set the remaining track parameters
  double phi_0 = phi_i[0];
  double x0 = c_trial.get_x0() + c_trial.get_R()*cos(phi_0);
  double y0 = c_trial.get_y0() + c_trial.get_R()*sin(phi_0);
  ThreeVector pos_0(x0, y0, -1);

  // Form the track and return it
  SciFiHelicalPRTrack *track = new SciFiHelicalPRTrack(-1, charge, pos_0, phi_0, c_trial, line_sz,
                                                                  -1.0, phi_i, spnts, covariance);
  return track;
}

bool PatternRecognition::find_dsdz(int n_points, std::vector<SciFiSpacePoint*> &spnts,
                                   const SimpleCircle& circle, const std::vector<double>& sigma_s,
                                   std::vector<double>& phi_i, SimpleLine& line_sz,
                                   TMatrixD& cov_sz, int& handedness) const {

  // Sort spacepoints in order seen by the beam (descending z for T1, ascending z for T2)
//  if (spnts[0]->get_tracker() == 0)
//    std::sort(spnts.begin(), spnts.end(), compare_spoints_descending_z);
//  else if (spnts[0]->get_tracker() == 1)
    std::sort(spnts.begin(), spnts.end(), compare_spoints_ascending_z);
//    std::sort(spnts.begin(), spnts.end(), compare_spoints_ascending_z);

  // Find each z_i and phi_i value for each spacepoint relative to the first spacepoint
  std::vector<double> z_i;         // Vector of the z coord of each successive spacepoint

  // Loop over the spacepoints
  for (std::vector<SciFiSpacePoint*>::const_iterator it = spnts.begin(); it != spnts.end(); ++it) {
    z_i.push_back((*it)->get_position().z());
    phi_i.push_back(SciFiTools::calc_phi((*it)->get_position().x(),
                    (*it)->get_position().y(), circle));
  }

  // Find the track handedness and the number of turns made between tracker stations
  std::vector<double> true_phi_i;  // phi corrected for any extra 2*n*pi rotations
  bool success = find_n_turns(z_i, phi_i, true_phi_i, handedness);
  if (success) {
    phi_i = true_phi_i;
  } else {
    if ( _verb > 0 ) std::cout << "INFO: Pattern Recognition: Failed to find n turns\n";
    return false;
  }

  // Using the circle radius and the true_dphi, calc the s_i (distance in x-y between sp)
  std::vector<double> s_i = SciFiTools::phi_to_s(circle.get_R(), true_phi_i);

  if ( _verb > 0 ) {
    for (size_t i = 0; i < s_i.size(); ++i) {
      std::cout << "(" << z_i[i] << ", " << s_i[i] << ")  ";
    }
    std::cout << std::endl;
  }

  // Fit ds and dz to a straight line, to get the gradient, which equals ds/dz
  LeastSquaresFitter::linear_fit(z_i, s_i, sigma_s, line_sz, cov_sz);

  // Check linear fit passes chisq test
  if ( !(line_sz.get_chisq() / ( n_points - 2 ) < _sz_chisq_cut ) ) {
    if ( _verb > 0 ) {
      std::cout << "INFO: Pattern Recognition: Failed s-z cut, ds/dz = " << line_sz.get_m() << ", "
                << "intercept = " << line_sz.get_c() << ", "
                << "chisq = " << line_sz.get_chisq() << std::endl;
    }
    return false;
  } else {
    if ( _verb > 0 ) {
      std::cout << "INFO: Pattern Recognition: Passed s-z cut, ds/dz is " << line_sz.get_m()
                << ", intercept = " << line_sz.get_c() << ", "
                << "chisq = " << line_sz.get_chisq() << std::endl;
    }
    return true;
  }
}

bool PatternRecognition::find_n_turns(const std::vector<double> &z, const std::vector<double> &phi,
                                      std::vector<double> &true_phi, int &handedness) const {
  // Sanity checks
  if ( (z.size() != phi.size()) || (z.size() < 3) || (z.size() > 5) ) {
    std::cerr << "WARNING: Pattern Recognition:find_n_turns: bad arguments supplied, aborting\n";
    return false;
  }

  if (_verb > 0) {
    std::cout << "INFO: Pattern Recognition: n_turns_cut: " << _n_turns_cut << ", and using phi: ";
    for (size_t i = 0; i < phi.size(); ++i) {
      std::cout << phi[i] << " ";
    }
    std::cout << std::endl;
  }

  true_phi.clear();
  true_phi.resize(phi.size());
  // Separations are calculated in z and phi between each station and the first station seen by
  // the beam (station 5 for T1, station 1 for T2).
  std::vector<double> dz;
  std::vector<double> dphi;
  // 'Close' refers to a calculation of the corrected angle based on a value on our current value
  // of n for the last angle sep, then inferring the other angle seps using the station z separation
  std::vector<double> close_dphi(phi.size(), 0.0);
  std::vector<double> close_phi(phi.size(), 0.0);

  // Calculate the separations in phi and z between stations
//  std::cout << "i\tphi\tdphi\tz\tdz" << std::endl;
  bool add_2pi = false;
  for (size_t i = 0; i < phi.size(); ++i) {
    dz.push_back(z[i] - z[0]);
    if (add_2pi) dphi[i] = dphi[i] + 2*CLHEP::pi;
    dphi.push_back(phi[i] - phi[0]);
    if ( dphi[i] < 0.0 ) {
      dphi[i] = dphi[i] + 2*CLHEP::pi;
      add_2pi = true;
    }
//    std::cout << i << "\t" << phi[i] << "\t" << dphi[i] << "\t" << z[i] << "\t" << dz[i] << "\n";
  }

  // Setup a vector holding the values of n to try
  int myints[] = {0, 1, -1, 2, -2, 3, -3};
  std::vector<int> n_values(myints, myints + sizeof(myints) / sizeof(int));

  // Loop over n_values, the number of extra turns completed by the particle by the *final* station
  bool found = false;
  int true_n = 0;
  // std::cout << "n\tj\tdphi\tclose_dphi\trem\tres" << std::endl;
  for (size_t i = 0; i < n_values.size(); ++i) {
    // Calc a value of the last turning angle sep based on the current value of n
    close_dphi.back() = dphi.back() + 2*n_values[i]*CLHEP::pi;

    // Loop over the other turning angles
    bool pass = true;
    for (size_t j = 1; j < close_dphi.size(); ++j) {
      // Use this last angle sep and the separation between stations in z to calc the
      // other corrected turning angle separations
      close_dphi[j] = close_dphi.back() * ( dz[j] / dz.back());

      // If a sufficiently small residual is produced, accept current n as correct, for this angle
      double remainder = SciFiTools::my_mod(close_dphi[j], 2*CLHEP::pi);
      double residual = fabs(remainder) - fabs(dphi[j]);
      if ( fabs(residual) > _n_turns_cut ) pass = false;
//       std::cout << n_values[i] << "\t" << j << "\t" << dphi[j] << "\t" << close_dphi[j] << "\t"
//                 << remainder << "\t" << residual << std::endl;
    }

    // If n was accepted for all the turning angles
    if (pass) {
      found = true;
      true_n = n_values[i];
      if ( _verb > 0 ) std::cout << "INFO: Pattern Recognition: Found n = " << true_n << std::endl;
      break;
    }
    if ( _verb > 0 ) std::cout << std::endl;
  } // ~Loop over n_values

  // If we have found a value of n which was accepted, calc the true turning angles
  if (found) {
    if ( true_n < 0 ) {
      handedness = -1;
    } else {
      handedness = 1;
    }
    // std::cout << "Found particle track with handedness " << handedness << std::endl;

    // Transform dphi to phi
    for (size_t i = 0; i < close_dphi.size(); ++i) {
      close_phi[i] = close_dphi[i] + phi[0];
    }
    // Correct close phi to true phi, by working the number of turns that need to be added to each
    // angle, then using this to correct the angles directly without relying on the z ratios
    for (size_t i = 0; i < close_phi.size(); ++i) {
      double diff = fabs(phi[i] - close_phi[i]);
      if ( diff < CLHEP::pi ) {
        true_phi[i] = phi[i];
      } else if ( diff < 3*CLHEP::pi ) {
        true_phi[i] = phi[i] + 2*handedness*CLHEP::pi;
      } else if ( diff < 5*CLHEP::pi ) {
        true_phi[i] = phi[i] + 4*handedness*CLHEP::pi;
      } else if ( diff < 7*CLHEP::pi ) {
        true_phi[i] = phi[i] + 6*handedness*CLHEP::pi;
      }
    }
    return true;
  } else {
    return false;
  }
};

bool PatternRecognition::check_time_consistency(const std::vector<SciFiSpacePoint*> good_spnts,
                                                                            int tracker_id) const {

  bool helical_flag = (tracker_id == 0 ? _up_helical_pr_on : _down_helical_pr_on);
  bool straight_flag = (tracker_id == 0 ? _up_straight_pr_on : _down_straight_pr_on);
  double dT_first = 0.0;
  double dT_last = 0.0;
  /* TODO Waiting for Spacepoints to have time added ****
  double dT_first = good_spnts.front()->get_time();
  double dT_last  = good_spnts.back()->get_time();
  */
  double dZ = fabs(good_spnts.back()->get_position().z() - good_spnts.front()->get_position().z());

  double dS = 0.0;
  if ( straight_flag && !helical_flag ) // If you are ONLY looking at straight tracks
    dS = dZ;
  else if ( helical_flag ) // if you are trying to reconstruc EITHER straight OR helical tracks
    dS = dZ * _Pt_max / _Pz_min; // TODO _Pz_min is a guess right now. (both defined in header)

  double longest_allowed_time = dS / CLHEP::c_light;

  if ( fabs(dT_last - dT_first) > longest_allowed_time )
    return false;
  else
    return true;
}

void PatternRecognition::get_cuts(double& res_cut, double& straight_chisq_cut, double& R_res_cut,
       double& circle_chisq_cut, double& n_turns_cut, double& sz_chisq_cut) {
  res_cut = _res_cut;
  straight_chisq_cut = _straight_chisq_cut;
  R_res_cut = _R_res_cut;
  circle_chisq_cut = _circle_chisq_cut;
  n_turns_cut = _n_turns_cut;
  sz_chisq_cut = _sz_chisq_cut;
}

void PatternRecognition::set_cuts(double res_cut, double straight_chisq_cut, double R_res_cut,
       double circle_chisq_cut, double n_turns_cut, double sz_chisq_cut) {
  _res_cut = res_cut;
  _straight_chisq_cut = straight_chisq_cut;
  _R_res_cut = R_res_cut;
  _circle_chisq_cut = circle_chisq_cut;
  _n_turns_cut = n_turns_cut;
  _sz_chisq_cut = sz_chisq_cut;
}

// For linear Pattern Recognition use
bool PatternRecognition::set_end_stations(const std::vector<int> ignore_stations,
                                          int &o_st_num, int &i_st_num) const {

  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    o_st_num = 4;
    i_st_num = 0;
  } else if ( static_cast<int>(ignore_stations.size()) == 1 ) { // 4pt track case
      // Check a valid station number has been supplied
      bool ok = false;
      for ( int i = 0; i < 5; ++i ) {
        if ( ignore_stations[0] == i ) ok = true;
      }
      if ( !ok ) {
        if ( _verb > 0 )
          std::cerr << "WARNING: Pattern Recognition: Invalid ignore station argument\n";
        return false;
      }
      // Set outer station number
      if ( ignore_stations[0] != 4 )
        o_st_num = 4;
      else
        o_st_num = 3;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        i_st_num = 0;
      else
        i_st_num = 1;
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      // Check valid station numbers have been supplied
      bool ok0 = false;
      bool ok1 = false;
      for ( int i = 0; i < 5; ++i ) {
        if ( ignore_stations[0] == i ) ok0 = true;
        if ( ignore_stations[1] == i ) ok1 = true;
      }
      if ( ignore_stations[0] == ignore_stations[1] ) ok0 = false;
      if ( !ok0 || !ok1 ) {
        if ( _verb > 0 )
          std::cerr << "WARNING: Pattern Recognition: Invalid ignore station argument\n";
        return false;
      }
      // Set outer station number
      if ( ignore_stations[0] != 4 && ignore_stations[1] != 4 )
        o_st_num = 4;
      else if ( ignore_stations[0] != 3 && ignore_stations[1] != 3)
        o_st_num = 3;
      else
        o_st_num = 2;
      // Set inner station number
      if ( ignore_stations[0] != 0 && ignore_stations[1] != 0 )
        i_st_num = 0;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 1 )
        i_st_num = 1;
      else
        i_st_num = 2;
  } else {
      if ( _verb > 0 )
        std::cerr << "WARNING: Pattern Recognition: Invalid ignore station argument\n";
    return false;
  }
  return true;
} // ~set_end_stations(...)

// For straight Pattern Recognition use
bool PatternRecognition::set_seed_stations(const std::vector<int> ignore_stations,
                      int &o_st_num, int &i_st_num, int &mid_st_num) const {
  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    o_st_num = 4;
    i_st_num = 0;
    mid_st_num = 2;
  } else if ( static_cast<int>(ignore_stations.size()) == 1 ) { // 4pt track case
      // Check a valid station number has been supplied
      bool ok = false;
      for ( int i = 0; i < 5; ++i ) {
        if ( ignore_stations[0] == i ) ok = true;
      }
      if ( !ok ) {
        if ( _verb > 0 )
          std::cerr << "WARNING: Pattern Recognition: Invalid ignore station argument\n";
        return false;
      }
      // Set outer station number
      if ( ignore_stations[0] != 4  )
        o_st_num = 4;
      else
        o_st_num = 3;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        i_st_num = 0;
      else
        i_st_num = 1;
       // Set middle station number
       if ( ignore_stations[0] != 2 )
         mid_st_num = 2;
       else
         mid_st_num = 1;
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      // Check valid station numbers have been supplied
      bool ok0 = false;
      bool ok1 = false;
      for ( int i = 0; i < 5; ++i ) {
        if ( ignore_stations[0] == i ) ok0 = true;
        if ( ignore_stations[1] == i ) ok1 = true;
      }
      if ( ignore_stations[0] == ignore_stations[1] ) ok0 = false;
      if ( !ok0 || !ok1 ) {
        if ( _verb > 0 )
          std::cerr << "WARNING: Pattern Recognition: Invalid ignore station argument\n";
        return false;
      }

      if ( ignore_stations[0] == 4 || ignore_stations[1] == 4 ) { // 1 of ignore stats is 4
        if ( ignore_stations[0] == 3 || ignore_stations[1] == 3 ) {
          o_st_num = 2;
          mid_st_num = 1;
          i_st_num = 0;
        } else if ( ignore_stations[0] == 2 || ignore_stations[1] == 2 ) {
          o_st_num = 3;
          mid_st_num = 1;
          i_st_num = 0;
        } else if ( ignore_stations[0] == 1 || ignore_stations[1] == 1 ) {
          o_st_num = 3;
          mid_st_num = 2;
          i_st_num = 0;
        } else if ( ignore_stations[0] == 0 || ignore_stations[1] == 0 ) {
          o_st_num = 3;
          mid_st_num = 2;
          i_st_num = 1;
        }
      } else if ( ignore_stations[0] == 3 || ignore_stations[1] == 3 ) {// 1 of ignore stats is 3
        if ( ignore_stations[0] == 2 || ignore_stations[1] == 2 ) {
          o_st_num = 4;
          mid_st_num = 1;
          i_st_num = 0;
        } else if ( ignore_stations[0] == 1 || ignore_stations[1] == 1 ) {
          o_st_num = 4;
          mid_st_num = 2;
          i_st_num = 0;
        } else if ( ignore_stations[0] == 0 || ignore_stations[1] == 0 ) {
          o_st_num = 4;
          mid_st_num = 2;
          i_st_num = 1;
        }
      } else if ( ignore_stations[0] == 2 || ignore_stations[1] == 2 ) {// 1 of ignore stats is 2
        if ( ignore_stations[0] == 1 || ignore_stations[1] == 1 ) {
          o_st_num = 4;
          mid_st_num = 3;
          i_st_num = 0;
        } else if ( ignore_stations[0] == 0 || ignore_stations[1] == 0 ) {
          o_st_num = 4;
          mid_st_num = 3;
          i_st_num = 1;
        }
      } else if ( ignore_stations[0] == 1 || ignore_stations[1] == 1 ) {// 1 of ignore stats is 1
        if ( ignore_stations[0] == 0 || ignore_stations[1] == 0 ) {
        o_st_num = 4;
        mid_st_num = 3;
        i_st_num = 2;
        }
      }
    } else {
      if ( _verb > 0 )
        std::cerr << "WARNING: Pattern Recognition: Invalid ignore station argument\n";
    return false;
  }
  return true;
} // ~set_seed_stations(...)

bool PatternRecognition::set_ignore_stations(const std::vector<int> &ignore_stations,
                                             int &ignore_st_1, int &ignore_st_2) const {
  ignore_st_1 = -1, ignore_st_2 = -1;
  if ( ignore_stations.size() == 0 ) {
    // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_st_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_st_1 = ignore_stations[0];
    ignore_st_2 = ignore_stations[1];
  } else {
      if ( _verb > 0 )
        std::cerr << "WARNING: Pattern Recognition: Invalid ignore station argument\n";
    return false;
  }
  return true;
} // ~set_ignore_stations(...)

double PatternRecognition::sigma_on_s(const SimpleCircle& circ, const TMatrixD& cov_circ,
                                      const SciFiSpacePoint* const spnt) const {
  // Check the spnt pointer
  if (!spnt) {
    std::cerr << "WARNING:PatternRecognition:sigma_on_s: Bad spacepoint pointer" << std::endl;
    return 0.0;
  }
  // Set up the variables needed
  double xc = circ.get_x0();
  double yc = circ.get_y0();
  double rad = circ.get_R();
  double x = spnt->get_position().x();
  double y = spnt->get_position().y();
  double sigma_x = 0.0;
  double sigma_y = 0.0;
  if ( spnt->get_station() == 5 ) {
      sigma_x = _sd_5;
      sigma_y = _sd_5;
  } else {
    sigma_x = _sd_1to4;
    sigma_y = _sd_1to4;
  }

   std::cerr << "Input circle parameters: " << xc << " " << yc << " " << rad << " " << x << " "
             << y << " " << sigma_x << " " << sigma_y << std::endl;

  // Set up the full covariance matrix for circle fit, including station resolutions
  TMatrixD covariance(5,5);

  // Initially all entries are 0.0, set automatically when matrix was intialised
  // Top right 2*2 entries are the covariance matrix of the station resolutions (diagonal)
  covariance[0][0] = sigma_x*sigma_x;
  covariance[1][1] = sigma_y*sigma_y;
  // Bottom left 3*3 entries are the covariance matrix of the circle
  covariance[2][2] = cov_circ[0][0];
  covariance[2][3] = cov_circ[0][1];
  covariance[2][4] = cov_circ[0][2];
  covariance[3][2] = cov_circ[1][0];
  covariance[3][3] = cov_circ[1][1];
  covariance[3][4] = cov_circ[1][2];
  covariance[4][2] = cov_circ[2][0];
  covariance[4][3] = cov_circ[2][1];
  covariance[4][4] = cov_circ[2][2];

   std::cerr << "Full covariance matrix: " << std::endl;
   covariance.Print();

  // Now form delta, a vector holding the partial differentials of s wrt  x, y, r, xc, yc
  double denom = xc*xc - 2*xc*x + yc*yc - 2*yc*y + x*x + y*y;

  double dsx = (rad*(yc-y)) / denom;
  double dsy = (-rad*(xc-x)) / denom;
  double dsr = atan2((y-yc), (x-xc)); // NOTE: Should this be atan, or a mod done too?
  double dsxc = (rad*(yc-y)) / denom;
  double dsyc = (rad*(xc-x)) / denom;

   std::cerr << "denom: " << denom << std::endl;
   std::cerr << "Delta differentials: " << dsx << " " << dsy << " " << dsr << " " << dsxc << " "
             << dsyc << std::endl;

  TMatrixD delta(5,1);
  delta[0] = dsx;
  delta[1] = dsy;
  delta[2] = dsr;
  delta[3] = dsxc;
  delta[4] = dsyc;

   std::cerr << "Delta matrix: " << std::endl;
   delta.Print();

  // Next the transpose
  TMatrixD deltaT(1,5);
  deltaT.Transpose(delta);

   std::cerr << "Delta matrix: " << std::endl;
   delta.Print();
   std::cerr << "DeltaT matrix: " << std::endl;
   deltaT.Print();

  // Now finally calculate the variance of s, and return the sqrt for the standard deviation
  TMatrix var_s = deltaT * (covariance * delta);

   std::cerr << "var_s matrix: " << std::endl;
   var_s.Print();

  return sqrt(var_s[0][0]);
}

} // ~namespace MAUS
