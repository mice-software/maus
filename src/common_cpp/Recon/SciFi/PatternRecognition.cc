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

// C++ headers
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

// External libs headers
#include "TROOT.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"


namespace MAUS {

// Two predicate functions used by the stl sort algorithm to sort spacepoints in vectors
bool compare_spoints_ascending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() < sp2->get_position().z());
}

bool compare_spoints_descending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() > sp2->get_position().z());
}

PatternRecognition::PatternRecognition(): _lsq(_sd_1to4, _sd_5, _R_res_cut) {
  if (_debug == 2) {
    _f_res = new ofstream();
    _f_res->open("residuals.dat", std::ios::app);

    _f_res_good = new ofstream();
    _f_res_good->open("residuals_good.dat", std::ios::app);

    _f_res_chosen = new ofstream();
    _f_res_chosen->open("residuals_chosen.dat", std::ios::app);

    _f_trks = new ofstream();
    _f_trks->open("tracks.dat", std::ios::app);
  }
};

PatternRecognition::~PatternRecognition() {
  if (_debug == 2) {
    if ( _f_res ) {
      _f_res->close();
      delete _f_res;
      _f_res = NULL;
    }

    if ( _f_res_good ) {
      _f_res_good->close();
      delete _f_res_good;
      _f_res_good = NULL;
    }

    if ( _f_res_chosen ) {
      _f_res_chosen->close();
      delete  _f_res_chosen;
      _f_res_chosen = NULL;
    }

    if ( _f_trks ) {
      _f_trks->close();
      delete _f_trks;
      _f_trks = NULL;
    }
  }
};

void PatternRecognition::process(const bool helical_pr_on, const bool straight_pr_on,
                                 SciFiEvent &evt) {

  set_helical_pr_on(helical_pr_on);
  set_straight_pr_on(straight_pr_on);

  if ( evt.spacepoints().size() > 0 ) {

    // Some setup
    evt.set_spacepoints_used_flag(false);
    SpacePoint2dPArray spnts_by_tracker(_n_trackers);
    spnts_by_tracker = sort_by_tracker(evt.spacepoints());

    // Loop over trackers
    for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
      // Split spacepoints according to which station they occured in
      SpacePoint2dPArray spnts_by_station(_n_stations);
      sort_by_station(spnts_by_tracker[trker_no], spnts_by_station);

      // Make the helical and straight tracks, depending on flags
      if ( _helical_pr_on ) {
        bool track_type = 1;
        make_all_tracks(track_type, trker_no, spnts_by_station, evt);
      }
      if ( _straight_pr_on ) {
        bool track_type = 0;
        make_all_tracks(track_type, trker_no, spnts_by_station, evt);
      }
    }// ~Loop over trackers
    std::cout << "Number of straight tracks found: " << evt.straightprtracks().size() << "\n\n";
    std::cout << "Number of helical tracks found: " << evt.helicalprtracks().size() << "\n\n";
  } else {
    std::cout << "No spacepoints in event" << std::endl;
  }
};

void PatternRecognition::make_all_tracks(const bool track_type, const int trker_no,
                                         SpacePoint2dPArray &spnts_by_station, SciFiEvent &evt) {

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  // Make the tracks
  if (num_stations_hit == 5) {
    std::vector<SciFiStraightPRTrack*> strks;
    std::vector<SciFiHelicalPRTrack*> htrks;
    make_5tracks(track_type, trker_no, spnts_by_station, strks, htrks);
    add_tracks(trker_no, strks, htrks, evt);
  }
  num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
  if (num_stations_hit > 3) {
    std::vector<SciFiStraightPRTrack*> strks;
    std::vector<SciFiHelicalPRTrack*> htrks;
    make_4tracks(track_type, trker_no, spnts_by_station, strks, htrks);
    add_tracks(trker_no, strks, htrks, evt);
  }
  num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
  if (num_stations_hit > 2  && track_type != 1) { // No 3pt tracks for helical
    std::vector<SciFiStraightPRTrack*> strks;
    std::vector<SciFiHelicalPRTrack*> htrks;
    make_3tracks(trker_no, spnts_by_station, strks);
    add_tracks(trker_no, strks, htrks, evt);
  }
}

void PatternRecognition::add_tracks(const int trker_no, std::vector<SciFiStraightPRTrack*> &strks,
                                    std::vector<SciFiHelicalPRTrack*> &htrks, SciFiEvent &evt ) {
  for ( int i = 0; i < static_cast<int>(strks.size()); ++i ) {
    strks[i]->set_tracker(trker_no);
    evt.add_straightprtrack(strks[i]);
  }
  for ( int i = 0; i < static_cast<int>(htrks.size()); ++i ) {
    htrks[i]->set_tracker(trker_no);
    evt.add_helicalprtrack(htrks[i]);
  }
}

void PatternRecognition::make_5tracks(const bool track_type, const int trker_no,
                                      SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack*> &strks,
                                      std::vector<SciFiHelicalPRTrack*> &htrks) {
  if ( _debug > 0 ) std::cout << "Making 5 point tracks" << std::endl;
  int n_points = 5;
  std::vector<int> ignore_stations; // A zero size vector sets that all stations are to be used
  if ( track_type == 0 )
    make_straight_tracks(n_points, trker_no, ignore_stations, spnts_by_station, strks);
  if ( track_type == 1 ) {
    std::vector<SciFiSpacePoint*> current_spnts;
    make_helix(n_points, 0, ignore_stations, current_spnts, spnts_by_station, htrks);
  }
  if ( _debug > 0 ) std::cout << "Finished making 5 pt tracks" << std::endl;
} // ~make_spr_5pt(...)

void PatternRecognition::make_4tracks(const bool track_type, const int trker_no,
                                      SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack*> &strks,
                                      std::vector<SciFiHelicalPRTrack*> &htrks) {
  if ( _debug > 0 ) std::cout << "Making 4 point tracks" << std::endl;

  int n_points = 4;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    if ( _debug > 0 ) std::cout << "4pt track: 5 stations with unused spacepoints" << std::endl;

    for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
      // Recount how many stations have at least one unused spacepoint
      num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
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

    if ( _debug > 0 ) std::cout << "4pt track: 4 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      if ( track_type == 0 )
        make_straight_tracks(n_points, trker_no, stations_not_hit, spnts_by_station, strks);
      if ( track_type == 1 ) {
        std::vector<SciFiSpacePoint*> current_spnts;
        make_helix(n_points, 0, stations_not_hit, current_spnts, spnts_by_station, htrks);
      }
    } else {
      if ( _debug > 0 )
        std::cerr << "Wrong number of stations without spacepoints aborting 4 pt track.\n";
    }
  } else if ( num_stations_hit < 4 ) {
    if ( _debug > 0 )
      std::cout << "Not enough unused spacepoints, quiting 4 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
    if ( _debug > 0 )
      std::cerr << "Wrong number of stations with spacepoints, aborting 4 pt track.\n";
  }
  if ( _debug > 0 ) std::cout << "Finished making 4 pt tracks" << std::endl;
} // ~make_straight_4tracks(...)

void PatternRecognition::make_3tracks(const int trker_no, SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack*> &strks) {
  if ( _debug > 0 ) std::cout << "Making 3 point track" << std::endl;

  int n_points = 3;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  bool sufficient_hit_stations = true;

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    if ( _debug > 0 ) std::cout << "3pt track: 5 stations with unused spacepoints" << std::endl;

    for (int i = 0; i < 4; ++i) { // Loop of first station to ignore
      if ( sufficient_hit_stations ) {
        for (int j = i + 1; j < 5; ++j) { // Loop of second station to ignore
          if ( sufficient_hit_stations ) {
            // Recount how many stations have at least one unused spacepoint
            num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
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

    if ( _debug > 0 ) std::cout << "3pt track: 4 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);
    std::vector<int> ignore_stations;

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
        // Recount how many stations have at least one unused spacepoint
        num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
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

    if ( _debug > 0 ) std::cout << "3pt track: 3 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 2 ) {
      make_straight_tracks(n_points, trker_no, stations_not_hit, spnts_by_station, strks);
    } else {
      if ( _debug > 0 ) {
        std::cerr << "Wrong number of stations without spacepoints, ";
        std::cerr << "aborting 3 pt track." << std::endl;
      }
    }
  } else if ( num_stations_hit < 3 ) {
    if ( _debug > 0 )
      std::cout << "Not enough unused spacepoints, quiting 3 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
    if ( _debug > 0 )
      std::cerr << "Wrong number of stations with spacepoints, aborting 3 pt track." << std::endl;
  }
  if ( _debug > 0 ) std::cout << "Finished making 3 pt tracks" << std::endl;
} // ~make_straight_3tracks(...)

void PatternRecognition::make_straight_tracks(const int n_points, const int trker_no,
                                     const std::vector<int> ignore_stations,
                                     SpacePoint2dPArray &spnts_by_station,
                                     std::vector<SciFiStraightPRTrack*> &strks) {
  // Set inner and outer stations
  int o_st_num = -1, i_st_num = -1;
  set_end_stations(ignore_stations, o_st_num, i_st_num);

  if (static_cast<int>(spnts_by_station.size()) != _n_stations
      || o_st_num < 0 || o_st_num >= _n_stations || i_st_num < 0 || i_st_num >= _n_stations) {
    if ( _debug > 0 ) std::cerr << "Bad spnts_by_station passed, aborting make_straight_tracks.\n";
    return;
  }

  // Loop over spacepoints in outer station
  for ( size_t outer_sp = 0; outer_sp < spnts_by_station[o_st_num].size(); ++outer_sp ) {

    // Check the outer spacepoint is unused and enough stations are left with unused sp
    if ( spnts_by_station[o_st_num][outer_sp]->get_used() ||
          num_stations_with_unused_spnts(spnts_by_station) < n_points) continue;

    // Loop over spacepoints in inner station
    for ( size_t inner_sp = 0; inner_sp < spnts_by_station[i_st_num].size(); ++inner_sp ) {

      // Check the inner spacepoint is unused and enough stations are left with unused sp
      if ( spnts_by_station[i_st_num][inner_sp]->get_used() ||
           num_stations_with_unused_spnts(spnts_by_station) < n_points ) continue;

      // Vector to hold the good spacepoints in each station
      std::vector<SciFiSpacePoint*> good_spnts;

      // Set variables to hold which stations are to be ignored
      int ignore_st_1 = -1, ignore_st_2 = -1;
      set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

      // Draw a straight line between spacepoints in outer most and inner most stations
      SimpleLine line_x, line_y;
      draw_line(spnts_by_station[o_st_num][outer_sp],
                spnts_by_station[i_st_num][inner_sp], line_x, line_y);

      // Loop over intermediate stations and compare spacepoints with the line
      for ( int st_num = i_st_num + 1; st_num < o_st_num; ++st_num ) {
        if (st_num != ignore_st_1 && st_num != ignore_st_2) {

          // A large number so initial value is set as best first
          double delta_sq = 1000000;
          int best_sp = -1;

          // Loop over spacepoints
          for ( size_t sp_no = 0; sp_no < spnts_by_station[st_num].size(); ++sp_no ) {

            // If the spacepoint has not already been used in a track fit
            if ( !spnts_by_station[st_num][sp_no]->get_used() ) {
              SciFiSpacePoint *sp = spnts_by_station[st_num][sp_no];
              double dx = 0, dy = 0;
              calc_straight_residual(sp, line_x, line_y, dx, dy);
              if ( _debug > 1 ) {
                *_f_res << trker_no << "\t" << st_num << "\t" << n_points << "\t";
                *_f_res << dx << "\t" << dy << "\n";
              }

              // Apply roadcuts & find the spoints with the smallest residuals for the line
              if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut )  {
                if ( _debug > 1 ) {
                  *_f_res_good << trker_no << "\t" << st_num << "\t" << n_points << "\t";
                  *_f_res_good << dx << "\t" << dy << "\n";
                }
                if ( delta_sq > (dx*dx + dy*dy) )
                  delta_sq = dx*dx + dy*dy;
                  best_sp = sp_no;
                // add_residuals(true, dx, dy, residuals);
              } // ~If pass roadcuts and beats previous best fit point
            } // ~If spacepoint is unused
          } // ~Loop over spacepoints
          // Push back the best spacepoint found for the current station
          if (best_sp > -1) {
            SciFiSpacePoint * sp = spnts_by_station[st_num][best_sp];
            good_spnts.push_back(sp);
            double dx = 0, dy = 0;
            calc_straight_residual(sp, line_x, line_y, dx, dy);
            if ( _debug > 1 ) {
              *_f_res_chosen << trker_no << "\t" << st_num << "\t" << n_points << "\t";
              *_f_res_chosen << dx << "\t" << dy << "\n";
            }
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
        std::vector<double> x, x_err, y, y_err, z;

        for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i ) {
          x.push_back(good_spnts[i]->get_position().x());
          y.push_back(good_spnts[i]->get_position().y());
          z.push_back(good_spnts[i]->get_position().z());

          // The error on the position measurements of sp in a tracker (same in x and y)
          double sd = -1.0;
          if ( good_spnts[i]->get_station() == 5 )
            sd = _sd_5;
          else
            sd = _sd_1to4;
          x_err.push_back(sd);
          y_err.push_back(sd);
        }

        // Fit track
        SimpleLine line_x, line_y;
        _lsq.linear_fit(z, x, x_err, line_x);
        _lsq.linear_fit(z, y, y_err, line_y);

        // Check track passes chisq test, then create SciFiStraightPRTrack
        if ( ( line_x.get_chisq() / ( n_points - 2 ) < _chisq_cut ) &&
            ( line_y.get_chisq() / ( n_points - 2 ) < _chisq_cut ) ) {

          if ( _debug > 0 )
            std::cout << "** chisq test passed, adding " << n_points << "pt track **\n";
          SciFiStraightPRTrack* track = new SciFiStraightPRTrack(-1, n_points, line_x, line_y);
          if ( _debug > 0 ) {
            std::cout << "x0 = " << track->get_x0() << " mx = " << track->get_mx();
            std::cout << " y0 = " << track->get_y0() << " my = " << track->get_my() << "\n";
          }

          // Set all the good sp to used
          for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i ) {
            good_spnts[i]->set_used(true);
          }

          // Populate the sp of the track and then push the track back into the strks vector
          track->set_spacepoints(good_spnts);
          strks.push_back(track);
        } else {
          if ( _debug > 0 ) {
            std::cout << "x_chisq = " << line_x.get_chisq();
            std::cout << "\ty_chisq = " << line_y.get_chisq() << std::endl;
            std::cout << "chisq test failed, " << n_points << "pt track rejected\n";
          }
        } // ~Check track passes chisq test
      } // ~ if ( good_spnts.size() > 1 )
    } // ~Loop over sp in station 1
  } // ~Loop over sp in station 5
} // ~make_straight_tracks(...)

void PatternRecognition::make_helix(const int n_points, const int stat_num,
                                    const std::vector<int> ignore_stations,
                                    std::vector<SciFiSpacePoint*> &current_spnts,
                                    SpacePoint2dPArray &spnts_by_station,
                                    std::vector<SciFiHelicalPRTrack*> &htrks) {

  // Set variables to hold which stations are to be ignored
  int ignore_st_1 = -1, ignore_st_2 = -1;
  set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

  // If we are on an ignore station, move to the next station without doing anything else
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
    if ( spnts_by_station[stat_num][sp_num]->get_used() ) continue;
    // If we are on the last station, attempt to form a track using the spacepoints collected
    if (stat_num == stat_num_max) {
      current_spnts.push_back(spnts_by_station[stat_num][sp_num]);
      SciFiHelicalPRTrack* trk = form_track(n_points, current_spnts);
      // If we found a track, clear current spacepoints to trigger break out to outer most station
      if ( trk != NULL ) {
        htrks.push_back(trk);
        current_spnts.clear();
        current_spnts.resize(0);
        return;
      }
    // If not on the final station, add current spacepoint from this station and move on to next
    } else {
      current_spnts.push_back(spnts_by_station[stat_num][sp_num]);
      make_helix(n_points, stat_num+1, ignore_stations, current_spnts, spnts_by_station, htrks);
      // If we are not on the first station and current_spnts is empty, break out as track was found
      if ( stat_num != stat_num_min && current_spnts.size() == 0 ) return;
    }
  }
  return;
}

SciFiHelicalPRTrack* PatternRecognition::form_track(const int n_points,
                                                    std::vector<SciFiSpacePoint*> spnts ) {

  // Perform a circle fit now that we have found a set of spacepoints
  SimpleCircle c_trial;
  bool good_radius = _lsq.circle_fit(spnts, c_trial);

  // If the radius calculated is too large or chisq fails, return NULL
  if ( !good_radius || !( c_trial.get_chisq() / ( n_points - 2 ) < _chisq_cut ) ) {
    if ( _debug > 0 ) std::cerr << "Failed circle cut, chisq = " << c_trial.get_chisq() << "\n";
    return NULL;
  }

  // Perform the s - z fit
  SimpleLine line_sz;
  std::vector<double> phi_i; // to hold change between turning angles wrt first spacepoint
  int charge = 0;            // the particle track charge
  bool good_dsdz = find_dsdz(n_points, spnts, c_trial, phi_i, line_sz, charge);
  if (!good_dsdz) {
    if ( _debug > 0 ) std::cerr << "dsdz fit failed, looping..." << std::endl;
    return NULL;
  }

  // Form the helical track
  double phi_0 = phi_i[0];
  double psi_0 = phi_0 + (CLHEP::pi / 2);
  double x0 = c_trial.get_x0() + c_trial.get_R()*cos(phi_0);
  double y0 = c_trial.get_y0() + c_trial.get_R()*sin(phi_0);
  ThreeVector pos_0(x0, y0, -1);
  SciFiHelicalPRTrack *track = NULL;
  track = new SciFiHelicalPRTrack(-1, n_points, charge, pos_0, phi_0, psi_0, c_trial, line_sz);
  track->set_phi(phi_i);
  // if (spnts[0]->get_tracker() == 0) track->set_dsdz(-track->get_dsdz());  // sign flip for t1

  // Set all the good sp to used and set the track seeds with them
  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i )
    spnts[i]->set_used(true);
  track->set_spacepoints(spnts);

  // Return the completed track
  return track;
}

bool PatternRecognition::find_dsdz(int n_points, std::vector<SciFiSpacePoint*> &spnts,
                                   const SimpleCircle &circle, std::vector<double> &phi_i,
                                   SimpleLine &line_sz, int &charge) {

  // Sort spacepoints in order seen by the beam (descending z for T1, ascending z for T2)
  if (spnts[0]->get_tracker() == 0)
    std::sort(spnts.begin(), spnts.end(), compare_spoints_descending_z);
  else if (spnts[0]->get_tracker() == 1)
    std::sort(spnts.begin(), spnts.end(), compare_spoints_ascending_z);

  // Find each z_i and phi_i value for each spacepoint relative to the first spacepoint
  std::vector<double> z_i;         // Vector of the z coord of each successive spacepoint
  std::vector<double> phi_err;     // The errors on the phi_i

  // Loop over the spacepoints
  for (std::vector<SciFiSpacePoint*>::const_iterator it = spnts.begin(); it != spnts.end(); ++it) {
    z_i.push_back((*it)->get_position().z());
    phi_i.push_back(calc_phi((*it)->get_position().x(), (*it)->get_position().y(), circle));


    // Set the phi_i errors
    double sd_phi = -1.0;
    if ( ((*it)->get_station() == 5) )
      sd_phi = _sd_phi_5;
    else
      sd_phi = _sd_phi_1to4;
    phi_err.push_back(sd_phi);
  }

  // Find the track charge and the number of turns made between tracker stations
  std::vector<double> true_phi_i;  // phi corrected for any extra 2*n*pi rotations
  bool success = find_n_turns(z_i, phi_i, true_phi_i, charge);
  if (success) {
    phi_i = true_phi_i;
  } else {
    std::cerr << "Failed to find n turns" << std::endl;
    return false;
  }

  // Using the circle radius and the true_dphi, calc the s_i (distance in x-y between sp)
  std::vector<double> s_i = phi_to_s(circle.get_R(), true_phi_i);

  // Fit ds and dz to a straight line, to get the gradient, which equals ds/dz
  _lsq.linear_fit(z_i, s_i, phi_err, line_sz);

  // Check linear fit passes chisq test
  if ( !(line_sz.get_chisq() / ( n_points - 2 ) < _sz_chisq_cut ) ) {
    if ( _debug > 0 ) std::cerr << "Failed s-z cut, chisq = " << line_sz.get_chisq() << std::endl;
    return false;
  } else {
    if ( _debug > 0 ) std::cerr << "Passed s-z cut, ds/dz is " << line_sz.get_m() << "\n";
    phi_i = true_phi_i;
    return true;
  }
}

bool PatternRecognition::find_n_turns(const std::vector<double> &z, const std::vector<double> &phi,
                                      std::vector<double> &true_phi, int &charge) {
  // Sanity checks
  if ( (z.size() != phi.size()) || (z.size() < 3) || (z.size() > 5) ) {
    std::cerr << "find_n_turns: bad arguments supplied, aborting";
    return false;
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
  // std::cout << "i\tphi\tdphi\tz\tdz" << std::endl;
  for (size_t i = 0; i < phi.size(); ++i) {
    dz.push_back(z[i] - z[0]);
    dphi.push_back(phi[i] - phi[0]);
    if ( dphi[i] < 0.0 )
      dphi[i] = dphi[i] + 2*CLHEP::pi;
    // std::cout << i << "\t" << phi[i] << "\t" << dphi[i] << "\t" << z[i] << "\t" << dz[i] << "\n";
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
      double remainder = my_mod(close_dphi[j], 2*CLHEP::pi);
      double residual = fabs(remainder) - fabs(dphi[j]);
      // std::cout << n_values[i] << "\t" << j << "\t" << dphi[j] << "\t" << close_dphi[j]
      //          << "\t" << remainder << "\t" << residual << std::endl;
      if ( fabs(residual) > _n_turns_cut ) pass = false;
    }

    // If n was accepted for all the turning angles
    if (pass) {
      found = true;
      true_n = n_values[i];
      break;
    }
  } // ~Loop over n_values

  // If we have found a value of n which was accepted, calc the true turning angles
  if (found) {
    if ( true_n < 0 ) {
      charge = -1;
    } else {
      charge = 1;
    }
    std::cout << "Found particle track with charge " << charge << std::endl;

    // Transfor dphi to phi
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
        true_phi[i] = phi[i] + 2*charge*CLHEP::pi;
      } else if ( diff < 5*CLHEP::pi ) {
        true_phi[i] = phi[i] + 4*charge*CLHEP::pi;
      } else if ( diff < 7*CLHEP::pi ) {
        true_phi[i] = phi[i] + 6*charge*CLHEP::pi;
      }
    }
    return true;
  } else {
    return false;
  }
};

double PatternRecognition::my_mod(const double num, const double denom) {
  return num - (denom * floor(num / denom));
};


double PatternRecognition::calc_phi(double xpos, double ypos, const SimpleCircle &circle) {
    // Note this function returns phi_i + phi_0, unless using x0, y0 in which case it returns phi_0
    double angle = atan2(ypos - circle.get_y0(), xpos - circle.get_x0());
    if ( angle < 0. ) angle += 2. * CLHEP::pi; // TODO is this ok if have different sign particles?

    // std::cerr << "calc_phi: x is " << xpos << ", y is " << ypos << ", R is " << circle.get_R();
    // std::cerr << ", X0 is " << circle.get_x0() << ", Y0 is " << circle.get_y0();
    // std::cerr << ", phi is " << angle << "\n";

    return angle;
} // ~calculate_phi(...)

std::vector<double> PatternRecognition::phi_to_s(const double R, const std::vector<double> &phi) {
  std::vector<double> s_i;
  for ( int i = 0; i < static_cast<int>(phi.size()); ++i ) {
    s_i.push_back(phi[i] * R);
  }
  return s_i;
} // ~dphi_to_ds(...)

bool PatternRecognition::check_time_consistency(const std::vector<SciFiSpacePoint*> good_spnts) {

  double dT_first = 0.0;
  double dT_last = 0.0;
  /* TODO Waiting for Spacepoints to have time added ****
  double dT_first = good_spnts.front()->get_time();
  double dT_last  = good_spnts.back()->get_time();
  */
  double dZ = fabs(good_spnts.back()->get_position().z() - good_spnts.front()->get_position().z());

  double dS = 0.0;
  if ( _straight_pr_on && !_helical_pr_on ) // If you are ONLY looking at straight tracks
    dS = dZ;
  else if ( _helical_pr_on ) // if you are trying to reconstruc EITHER straight OR helical tracks
    dS = dZ * _Pt_max / _Pz_min; // TODO _Pz_min is a guess right now. (both defined in header)

  double longest_allowed_time = dS / CLHEP::c_light;

  if ( fabs(dT_last - dT_first) > longest_allowed_time )
    return false;
  else
    return true;
}

// For linear Pattern Recognition use
bool PatternRecognition::set_end_stations(const std::vector<int> ignore_stations,
                                          int &o_st_num, int &i_st_num) {

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
        if ( _debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
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
        if ( _debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
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
    if ( _debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
    return false;
  }
  return true;
} // ~set_end_stations(...)

// For helical Pattern Recognition use
bool PatternRecognition::set_seed_stations(const std::vector<int> ignore_stations,
                      int &o_st_num, int &i_st_num, int &mid_st_num) {
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
        if ( _debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
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
        if ( _debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
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
    if (_debug > 0) std::cerr << "Error: Invalid ignore station argument." << std::endl;
    return false;
  }
  return true;
} // ~set_seed_stations(...)

void PatternRecognition::sort_by_station(const std::vector<SciFiSpacePoint*> &spnts,
                                         SpacePoint2dPArray &spnts_by_station) {
  for ( int st_num = 0; st_num < static_cast<int>(spnts_by_station.size()); ++st_num ) {
    for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
      if ( spnts[i]->get_station() == st_num + 1 ) {
        spnts_by_station[st_num].push_back(spnts[i]);
      }
    }
  }
} // ~sort_by_station(...)

SpacePoint2dPArray PatternRecognition::sort_by_tracker(const SciFiSpacePointPArray &spnts) {
  SpacePoint2dPArray spnts_by_tracker(_n_trackers);
  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
    for ( size_t i = 0; i < spnts.size(); ++i ) {
      if ( spnts[i]->get_tracker() == trker_no ) {
        spnts_by_tracker[trker_no].push_back(spnts[i]);
      }
    }
  }
  return spnts_by_tracker;
}

int PatternRecognition::num_stations_with_unused_spnts(
                                                    const SpacePoint2dPArray &spnts_by_station) {

  int stations_hit = 0;

  for ( int i = 0; i < static_cast<int>(spnts_by_station.size()); ++i ) {
    int unused_sp = 0;
    for ( int j = 0; j < static_cast<int>(spnts_by_station[i].size()); ++j ) {
      if ( !spnts_by_station[i][j]->get_used() ) {
        ++unused_sp;
      }
    }
    if ( unused_sp > 0 ) {
      ++stations_hit;
    }
  }
  return stations_hit;
} // ~num_stations_with_unused_spnts(...)

void PatternRecognition::stations_with_unused_spnts(const SpacePoint2dPArray &spnts_by_station,
                                                    std::vector<int> &stations_hit,
                                                    std::vector<int> &stations_not_hit) {
  stations_hit.clear();
  stations_not_hit.clear();

  for ( int i = 0; i < static_cast<int>(spnts_by_station.size()); ++i ) {
    int unused_sp = 0;
    for ( int j = 0; j < static_cast<int>(spnts_by_station[i].size()); ++j ) {
      if ( !spnts_by_station[i][j]->get_used() ) {
        ++unused_sp;
      }
    }
    if ( unused_sp > 0 ) {
      stations_hit.push_back(i);
    } else if ( unused_sp == 0 ) {
      stations_not_hit.push_back(i);
    }
  }
} // ~stations_with_unused_spnts(...)

bool PatternRecognition::set_ignore_stations(const std::vector<int> &ignore_stations,
                                             int &ignore_st_1, int &ignore_st_2) {
  ignore_st_1 = -1, ignore_st_2 = -1;
  if ( ignore_stations.size() == 0 ) {
    // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_st_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_st_1 = ignore_stations[0];
    ignore_st_2 = ignore_stations[1];
  } else {
    std::cerr << "Error: Invalid ignore station argument." << std::endl;
    return false;
  }
  return true;
} // ~set_ignore_stations(...)

void PatternRecognition::draw_line(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2,
                                   SimpleLine &line_x, SimpleLine &line_y) {

  ThreeVector pos_outer = sp1->get_position();
  ThreeVector pos_inner = sp2->get_position();

  line_x.set_m(( pos_inner.x() - pos_outer.x()) / (pos_inner.z() - pos_outer.z() ));
  line_x.set_c(pos_outer.x() - ( pos_outer.z() * line_x.get_m()) );
  line_y.set_m(( pos_inner.y() - pos_outer.y()) / (pos_inner.z() - pos_outer.z() ));
  line_y.set_c(pos_outer.y() - ( pos_outer.z() *  line_y.get_m() ));
} // ~draw_line(...)

void PatternRecognition::calc_straight_residual(const SciFiSpacePoint *sp, const SimpleLine &line_x,
                                       const SimpleLine &line_y, double &dx, double &dy) {

    ThreeVector pos = sp->get_position();
    dx = pos.x() - ( line_x.get_c() + ( pos.z() * line_x.get_m() ) );
    dy = pos.y() - ( line_y.get_c() + ( pos.z() * line_y.get_m() ) );
} // ~calc_straight_residual(...)

double PatternRecognition::calc_circle_residual(const SciFiSpacePoint *sp, const SimpleCircle &c) {
    ThreeVector pos = sp->get_position();
    double delta = sqrt(((pos.x()-c.get_x0()) * (pos.x()-c.get_x0())) +
                        ((pos.y()-c.get_y0()) * (pos.y()-c.get_y0()))) - c.get_R();
    return delta;
} // ~calc_circle_residual(...)

SimpleCircle PatternRecognition::make_3pt_circle(const SciFiSpacePoint *sp1,
                                                 const SciFiSpacePoint *sp2,
                                                 const SciFiSpacePoint *sp3) {
  SimpleCircle c;
  ThreeVector pos1 = sp1->get_position();
  ThreeVector pos2 = sp2->get_position();
  ThreeVector pos3 = sp3->get_position();

  // Calculate alpha
  Double_t arr_a[] = {pos1.x(), pos1.y(), 1.0,
                      pos2.x(), pos2.y(), 1.0,
                      pos3.x(), pos3.y(), 1.0};
  TMatrixD mat_a(3, 3, arr_a);
  double alpha = det3by3(mat_a);
  c.set_alpha(alpha);

  // Calculate beta
  Double_t arr_b[] = {(pos1.x()*pos1.x())+(pos1.y()*pos1.y()), pos1.y(), 1.0,
                      (pos2.x()*pos2.x())+(pos2.y()*pos2.y()), pos2.y(), 1.0,
                      (pos3.x()*pos3.x())+(pos3.y()*pos3.y()), pos3.y(), 1.0};
  TMatrixD mat_b(3, 3, arr_b);
  double beta = - det3by3(mat_b);
  c.set_beta(beta);

  // Calculate gamma
  Double_t arr_g[] = {(pos1.x()*pos1.x())+(pos1.y()*pos1.y()), pos1.x(), 1.0,
                      (pos2.x()*pos2.x())+(pos2.y()*pos2.y()), pos2.x(), 1.0,
                      (pos3.x()*pos3.x())+(pos3.y()*pos3.y()), pos3.x(), 1.0};
  TMatrixD mat_g(3, 3, arr_g);
  double gamma = det3by3(mat_g);
  c.set_gamma(gamma);

  // Calculate kappa
  Double_t arr_k[] = {(pos1.x()*pos1.x())+(pos1.y()*pos1.y()), pos1.x(), pos1.y(),
                      (pos2.x()*pos2.x())+(pos2.y()*pos2.y()), pos2.x(), pos2.y(),
                      (pos3.x()*pos3.x())+(pos3.y()*pos3.y()), pos3.x(), pos3.y()};
  TMatrixD mat_k(3, 3, arr_k);
  double kappa = - det3by3(mat_k);
  c.set_kappa(kappa);

  // Calculate the dependent parameters
  double x0 = - beta / (2 * alpha);
  c.set_x0(x0);

  double y0 = - gamma / (2 * alpha);
  c.set_y0(y0);

  double R = sqrt(((beta*beta+gamma*gamma)/(4*alpha*alpha))-(kappa/alpha));
  c.set_R(R);

  return c;
} // ~make_3pt_circle

double PatternRecognition::det3by3(const TMatrixD &m) {
  if ( m.GetNrows() == 3 && m.GetNcols() == 3 ) {
    double det = m(0, 0)*(m(1, 1)*m(2, 2)-m(1, 2)*m(2, 1)) +
                 m(0, 1)*(m(1, 2)*m(2, 0)-m(1, 0)*m(2, 2)) +
                 m(0, 2)*(m(1, 0)*m(2, 1)-m(1, 1)*m(2, 0));
    return det;
  } else {
    std::cerr << "Error: PatternRecognition::det3by3 says: Bad matrix given" << std::endl;
    return 0;
  }
}

} // ~namespace MAUS
