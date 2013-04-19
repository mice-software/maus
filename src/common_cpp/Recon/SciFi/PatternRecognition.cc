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
#include "src/common_cpp/DataStructure/SimpleLine.hh"
#include "src/common_cpp/DataStructure/SimpleCircle.hh"
#include "src/common_cpp/DataStructure/SimpleHelix.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"


namespace MAUS {

// Two predicate functions used by the stl sort algorithm to sort spacepoints in vectors
bool compare_spoints_ascending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() < sp2->get_position().z());
}

bool compare_spoints_descending_z(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2) {
  return (sp1->get_position().z() > sp2->get_position().z());
}

PatternRecognition::PatternRecognition() {
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
  // if (num_stations_hit > 2  && track_type != 1 ) { // No 3pt tracks for helical
  if ( num_stations_hit > 2 ) {
    std::vector<SciFiStraightPRTrack*> strks;
    std::vector<SciFiHelicalPRTrack*> htrks;
    make_3tracks(track_type, trker_no, spnts_by_station, strks, htrks);
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
  if ( track_type == 1 )
    make_helix(n_points, trker_no, ignore_stations, spnts_by_station, htrks);
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
        if ( track_type == 1 )
          make_helix(n_points, trker_no, ignore_stations, spnts_by_station, htrks);
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
      if ( track_type == 1 )
        make_helix(n_points, trker_no, stations_not_hit, spnts_by_station, htrks);
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

void PatternRecognition::make_3tracks(const bool track_type, const int trker_no,
                                      SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack*> &strks,
                                      std::vector<SciFiHelicalPRTrack*> &htrks) {
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
              if ( track_type == 0 )
                make_straight_tracks(n_points, trker_no, ignore_stations,
                                     spnts_by_station, strks);
              if ( track_type == 1 )
                make_helix(n_points, trker_no, ignore_stations, spnts_by_station, htrks);
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
            if ( track_type == 0 )
              make_straight_tracks(n_points, trker_no, ignore_stations, spnts_by_station, strks);
            if ( track_type == 1 )
              make_helix(n_points, trker_no, ignore_stations, spnts_by_station, htrks);
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
      if ( track_type == 0 )
        make_straight_tracks(n_points, trker_no, stations_not_hit, spnts_by_station, strks);
      if ( track_type == 1 )
        make_helix(n_points, trker_no, stations_not_hit, spnts_by_station, htrks);
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
        linear_fit(z, x, x_err, line_x);
        linear_fit(z, y, y_err, line_y);

        // Check track passes chisq test, then create SciFiStraightPRTrack
        if ( ( line_x.get_chisq() / ( n_points - 2 ) < _chisq_cut ) &&
            ( line_y.get_chisq() / ( n_points - 2 ) < _chisq_cut ) ) {

          if ( _debug > 0 )
            std::cout << "** chisq test passed, adding " << n_points << "pt track **\n";
          SciFiStraightPRTrack* track =
              new SciFiStraightPRTrack(-1, n_points, line_x, line_y);
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

void PatternRecognition::linear_fit(const std::vector<double> &_x, const std::vector<double> &_y,
                                    const std::vector<double> &_y_err, SimpleLine &line) {

  int n_points = static_cast<int>(_x.size());

  CLHEP::HepMatrix A(n_points, 2); // rows, columns
  CLHEP::HepMatrix V(n_points, n_points); // error matrix
  CLHEP::HepMatrix Y(n_points, 1); // measurements

  for ( int i = 0; i < static_cast<int>(_x.size()); ++i ) {
    // std::cout <<"( " << _x[i] << "," << _y[i] << " )" << std::endl;
    A[i][0] = 1;
    A[i][1] = _x[i];
    V[i][i] = ( _y_err[i] * _y_err[i] );
    Y[i][0] = _y[i];
  }

  CLHEP::HepMatrix At, tmpy, yparams;

  int ierr;
  V.invert(ierr);
  At = A.T();

  tmpy = At * V * A;
  tmpy.invert(ierr);
  yparams = tmpy * At * V * Y;

  line.set_c(yparams[0][0]);
  line.set_m(yparams[1][0]);
  line.set_c_err(sqrt(tmpy[0][0]));
  line.set_m_err(sqrt(tmpy[1][1]));

  CLHEP::HepMatrix C, result;

  C = Y - (A * yparams);
  result = C.T() * V * C;
  line.set_chisq(result[0][0]);
  line.set_chisq_dof(result[0][0] / n_points);
} // ~linear_fit(...)

void PatternRecognition::make_helix(const int n_points, const int trker_no,
                                    const std::vector<int> ignore_stations,
                                    SpacePoint2dPArray &spnts_by_station,
                                    std::vector<SciFiHelicalPRTrack*> &htrks) {
  // Set inner and outer stations
  int o_st_num = -1, i_st_num = -1, mid_st_num = -1;
  bool success = set_seed_stations(ignore_stations, o_st_num, i_st_num, mid_st_num);
  if ( !success ) return;

  // Set variables to hold which stations are to be ignored
  int ignore_st_1 = -1, ignore_st_2 = -1;
  set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

  // Loop over spacepoints in outer station
  for ( size_t outer_sp = 0; outer_sp < spnts_by_station[o_st_num].size(); ++outer_sp ) {
    // Loop over spacepoints in inner station
    for ( size_t inner_sp = 0; inner_sp < spnts_by_station[i_st_num].size(); ++inner_sp ) {
      // Loop over spacepoints in middle station
      for ( size_t mid_sp = 0; mid_sp < spnts_by_station[mid_st_num].size(); ++mid_sp ) {

        // Form an initial circle
        SciFiSpacePoint *sp1 = spnts_by_station[o_st_num][outer_sp];
        SciFiSpacePoint *sp2 = spnts_by_station[i_st_num][inner_sp];
        SciFiSpacePoint *sp3 = spnts_by_station[mid_st_num][mid_sp];
        SimpleCircle c_init = make_3pt_circle(sp1, sp2, sp3);

        // Map to keep track for each intermediate station of which spoints pass the roadcut
        std::map<int, int> best_sp;

        // Vector to hold the spacpoints currently being considered
        std::vector<SciFiSpacePoint*> good_spnts;

        // Loop over other intermediate stations (IS)
        for ( int st_num = i_st_num + 1; st_num < o_st_num; ++st_num ) {
          // Move to the next IS if we are on an ignore station or the middle seed station
          if (st_num == mid_st_num || st_num == ignore_st_1 || st_num == ignore_st_2) continue;

          double best_res = _circ_res_cut + 1.0;
          int best_res_num = -1;

          // Loop over spacepoints in the current station
          for (size_t sp_no = 0; sp_no < spnts_by_station[st_num].size(); ++sp_no) {
            // Calculate the residual of this sp against the initial circle
            double res = calc_circle_residual(spnts_by_station[st_num][sp_no], c_init);
            // Apply roadcut and find sp for this station which best matches the initial circle
            if ( res < _circ_res_cut && res < best_res ) {
              best_res = res;
              best_res_num = sp_no;
            }
          } // ~Loop over spacepoints in the current station

          // If at least one sp was found which passed the roadcut
          if ( best_res_num > -1 ) {
            best_sp[st_num] = best_res_num;
            good_spnts.push_back(spnts_by_station[st_num][best_res_num]);
          }
        } // ~Loop over intermediate stations

        // Check we have enough stations with sp that have passed the road cuts to make the track
        if ( static_cast<int>(best_sp.size()) != (n_points - 3) ) {
          if ( _debug > 0 ) std::cerr << "Not enough spoints passed road cuts" << std::endl;
          continue;
        } else {
          if ( _debug > 0 ) std::cerr << "Found " << best_sp.size() << " sp which passed rc,";
          if ( _debug > 0 ) std::cerr << " n_points is " << n_points << std::endl;
        }

        // Add the other spacepoints currently being considerd to the good_spnts vector
        if ( good_spnts.size() < 1 )
          good_spnts.push_back(spnts_by_station[mid_st_num][mid_sp]);
        else if ( good_spnts[0]->get_station() > mid_st_num+1 )
          good_spnts.insert(good_spnts.begin(), spnts_by_station[mid_st_num][mid_sp]);
        else if ( good_spnts[0]->get_station() < mid_st_num+1 )
          good_spnts.insert(good_spnts.begin() + 1, spnts_by_station[mid_st_num][mid_sp]);
        else
          good_spnts.push_back(spnts_by_station[mid_st_num][mid_sp]);

        good_spnts.insert(good_spnts.begin(), spnts_by_station[i_st_num][inner_sp]);
        good_spnts.push_back(spnts_by_station[o_st_num][outer_sp]);

        // Perform a circle fit now that we have found a set of spacepoints
        SimpleCircle c_trial;
        bool good_radius = circle_fit(good_spnts, c_trial);

        // If the radius calculated is too large or chisq fails, force loop to iterate from here
        if ( !good_radius || !( c_trial.get_chisq() / ( n_points - 2 ) < _chisq_cut ) ) {
          if ( _debug > 0 ) std::cerr << "Failed circle cut" << std::endl;
          continue;
        }

        SimpleLine line_sz;
        std::vector<double> phi_i; // to hold change between turning angles wrt first spacepoint
        double phi_0; // initial turning angle

        // Perform s-z fit
        calculate_dipangle(good_spnts, c_trial, phi_i, line_sz, phi_0);
        phi_i.insert(phi_i.begin(), phi_0);
        for ( int i = 1; i < phi_i.size(); ++i ) {
          phi_i[i] = phi_i[i] + phi_0;
        }
        /*
        bool good_dsdz = find_dsdz(n_points, good_spnts, c_trial, phi_i, line_sz);
        if (!good_dsdz) {
          std::cerr << "dsdz fit failed, looping..." << std::endl;
          continue;
        }
        */

        // Form the helical track
        double psi_0 = phi_0 + (CLHEP::pi / 2);
        double x0 = c_trial.get_x0() + c_trial.get_R()*cos(phi_0);
        double y0 = c_trial.get_y0() + c_trial.get_R()*sin(phi_0);
        CLHEP::Hep3Vector pos_0(x0, y0, -1);
        SciFiHelicalPRTrack *track;
        track = new SciFiHelicalPRTrack(-1, n_points, pos_0, phi_0, psi_0, c_trial, line_sz);
        track->set_phi(phi_i);

        // Set all the good sp to used
        for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i )
          good_spnts[i]->set_used(true);
        // Populate the sp of the track and then push the track back into trks vector
        track->set_spacepoints(good_spnts);
        htrks.push_back(track);
        // Remove the accepted sp from spnts_by_station
        for ( std::map<int, int>::iterator ii = best_sp.begin(); ii != best_sp.end(); ++ii ) {
          spnts_by_station[(*ii).first].erase(spnts_by_station[(*ii).first].begin()+(*ii).second);
        }
      } // ~Loop over middle station spacepoints
    } // ~Loop over inner station spacepoints
  } // ~Loop over outer station spacepoints
} // ~make_helix(...)

bool PatternRecognition::circle_fit(const std::vector<SciFiSpacePoint*> &spnts,
                                    SimpleCircle &circle) {

  int n_points = static_cast<int>(spnts.size());
  CLHEP::HepMatrix A(n_points, 3); // rows, columns
  CLHEP::HepMatrix V(n_points, n_points); // error matrix
  CLHEP::HepMatrix K(n_points, 1);

  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    // This part will change once I figure out proper errors
    double sd = -1.0;
    if ( spnts[i]->get_station() == 5 )
      sd = _sd_5;
    else
      sd = _sd_1to4;

    double x_i = spnts[i]->get_position().x();
    double y_i = spnts[i]->get_position().y();

    A[i][0] = ( x_i * x_i ) + ( y_i * y_i );
    A[i][1] = x_i;
    A[i][2] = y_i;

    V[i][i] = ( sd * sd );
    K[i][0] = 1.;
  }

  CLHEP::HepMatrix At, tmpx, tmp_params;
  int ierr;
  V.invert(ierr);
  At = A.T();
  tmpx = At * V * A;
  tmpx.invert(ierr);
  tmp_params = tmpx * At * V * K;

  // These values will be used for delta_R calculation
  double alpha, beta, gamma;
  alpha = tmp_params[0][0];
  beta = tmp_params[1][0];
  gamma = tmp_params[2][0];

  // Convert the linear parameters into the circle center and radius
  double x0, y0, R;
  x0 = (-1*beta) / (2 * alpha);
  y0 = (-1*gamma) / (2 * alpha);
  if ( ((4 * alpha) + (beta * beta) + (gamma * gamma)) < 0 )
    R = 0;
  else
    R = sqrt((4 * alpha) + (beta * beta) + (gamma * gamma)) / (2 * alpha);

  if ( R < 0. )
    std::cout << "R was < 0 geometrically but taking abs_val for physical correctness\n";
  R = fabs(R);

  if (R > _R_res_cut) return false; // Cannot be larger than 150mm or the track is not contained

  circle.set_x0(x0);
  circle.set_y0(y0);
  circle.set_R(R);
  circle.set_alpha(alpha);
  circle.set_beta(beta);
  circle.set_gamma(gamma);

  CLHEP::HepMatrix C, result;

  C = K - (A * tmp_params);
  result = C.T() * V * C;
  double chi2 = result[0][0];
  circle.set_chisq(chi2); // should I leave this un-reduced?
  return true;
} // ~circle_fit(...)

bool PatternRecognition::find_dsdz(int n_points, std::vector<SciFiSpacePoint*> &spnts,
                                   const SimpleCircle &circle, std::vector<double> &phi_i,
                                   SimpleLine &line_sz) {

  // Sort spacepoints in order seen by the beam (descending z for T1, ascending z for T2)
  if (spnts[0]->get_tracker() == 0)
    std::sort(spnts.begin(), spnts.end(), compare_spoints_descending_z);
  else if (spnts[0]->get_tracker() == 1)
    std::sort(spnts.begin(), spnts.end(), compare_spoints_ascending_z);

  // Find each z_i and phi_i value for each spacepoint relative to the first spacepoint
  std::vector<double> x_i;        // Vector of the x coord of each successive spacepoint
  std::vector<double> y_i;        // Vector of the y coord of each successive spacepoint
  std::vector<double> z_i;        // Vector of the z coord of each successive spacepoint
  std::vector<double> dz;         // The distance in z from the first spacepoint
  std::vector<double> true_dphi;  // dphi corrected for any extra 2*n*pi rotations
  std::vector<double> phi_err;    // The errors on the phi_i
  std::vector<double> dphi;       // The distance in phi from the first spacepoint

  // Find the phi value of the first spacepoint
  double phi_1 = calc_phi((*(spnts.begin()))->get_position().x(),
                          (*(spnts.begin()))->get_position().y(), circle);

  // Loop over the spacepoints
  for (std::vector<SciFiSpacePoint*>::const_iterator it = spnts.begin(); it != spnts.end(); ++it) {
    x_i.push_back((*it)->get_position().x());
    y_i.push_back((*it)->get_position().y());
    z_i.push_back((*it)->get_position().z());
    phi_i.push_back(calc_phi((*it)->get_position().x(), (*it)->get_position().y(), circle));

    /*
    std::cerr << "r is " << circle.get_R() << ", X0 is " << circle.get_x0() << ", Y0 is ";
    std::cerr << circle.get_y0() << "\n";
    for ( size_t i = 0; i < z_i.size(); ++i ) {
      std::cerr << "x[" << i << "] is " << x_i[i] << ", y_i[" << i << "] is " << y_i[i];
      std::cerr << " z[" << i << "] is " << z_i[i] << ", phi_i[" << i << "] is " << phi_i[i];
      std::cerr << std::endl;
    }
    */

    // Calculate each dz_ji and dphi_ji value (separation in z and phi between spacepoints)
    if ( it != spnts.begin() ) {
      dz.push_back(fabs((*it)->get_position().z() - (*(spnts.begin()))->get_position().z()));
      // dphi is negative means later spacepoint has gone over 2pi so we add to 2pi to compensate
      if ( (*(phi_i.end()-1) - phi_1) > 0 ) {
        dphi.push_back(*(phi_i.end()-1) - phi_1);
      } else {
        dphi.push_back((*(phi_i.end()-1)) + 2*CLHEP::pi - phi_1);
      }
    }

    // Set the phi_i errors
    double sd_phi = -1.0;
    if ( ((*it)->get_station() == 5) )
      sd_phi = _sd_phi_5;
    else
      sd_phi = _sd_phi_1to4;
    phi_err.push_back(sd_phi);
  }

  // Calculate any 2*n*pi correction to the phi_i
  bool success = find_n_turns(dz, dphi, true_dphi);
  if (!success) {
    std::cerr << "Failed to find turns between stations" << std::endl;
    return false;
  }

  // Set phi_i to the corrected values
  for ( size_t i = 0; i < true_dphi.size(); ++i ) {
    phi_i[i+1] = true_dphi[i] + phi_1;  // As first phi_i is just phi_1
    std::cerr << "dz[" << i << "] is " << dz[i] << ", dphi[" << i << "] is " << dphi[i];
    std::cerr << ", true_dphi[" << i << "] is " << true_dphi[i] << std::endl;
  }

  // Using the circle radius and the true_dphi, calc the s_i (distance in x-y between sp)
  std::vector<double> ds;
  dphi_to_ds(circle.get_R(), true_dphi, ds);

  // Fit ds and dz to a straight line, to get the gradient, which equals ds/dz
  linear_fit(dz, true_dphi, phi_err, line_sz);

  // Check linear fit passes chisq test
  if ( !(line_sz.get_chisq() / ( n_points - 2 ) < _sz_chisq_cut ) ) {
    if ( _debug > 0 ) std::cerr << "Failed s-z chisq cut" << std::endl;
    return false;
  } else {
    if ( _debug > 0 ) std::cerr << "Passed s-z cut, ds/dz is " << line_sz.get_m() << "\n";
    return true;
  }
}

bool PatternRecognition::find_n_turns(const std::vector<double> &dz,
                                      const std::vector<double> &dphi,
                                      std::vector<double> &true_dphi) {
  true_dphi = dphi;

  for (size_t i = 0; i < (dz.size() - 1); ++i) { // Loop over the separation between stations
    bool found = false;
    int true_n = -1;
    int true_m = -1;
    for (int n = 0; n < _n_limit; ++n) {
      if ( found ) break;
      for (int m = 0; m < _m_limit; ++m) {
        // Remainder should be ~0 if correct n_ji and m are found
        double remainder = fabs((((true_dphi[i+1] + 2*m*CLHEP::pi) /
                                  (true_dphi[i] + 2*n*CLHEP::pi)) -
                                  (dz[i+1] / dz[i])) / (dz[i+1] / dz[i]));
        // double remainder = evaluate_nm(n, m, dz[i], dz[i+1], dphi[i], dphi[i+1]);
        if ( remainder < _AB_cut ) {
          found = true;
          true_n = n;
          true_m = m;
          break;
        }
      }
    }
    if ( found ) { // If we suceeding in finding corrections which pass the cut
      true_dphi[i] = dphi[i] + 2*true_n*CLHEP::pi;
      true_dphi[i+1] = dphi[i+1] + 2*true_m*CLHEP::pi;
    } else { // Abort finding dsdz if we fail to find any single n turns correction
      return false;
    }
  }
  return true;
}

double PatternRecognition::evaluate_nm(const int n, const int m,
                                      const double dz_ji, const double dz_kj,
                                      const double dphi_ji, const double dphi_kj) {
  double gamma = ((2*CLHEP::pi)/dz_kj) * (m - (dz_kj/dz_ji)*n);
  double lambda = (dphi_ji/dz_ji) - (dphi_kj/dz_kj);
  return fabs(lambda - gamma);
}

void PatternRecognition::calculate_dipangle(const std::vector<SciFiSpacePoint*> &spnts,
                                            const SimpleCircle &circle, std::vector<double> &dphi,
                                            SimpleLine &line_sz, double &phi_0) {
  // For the linear fit in s-z, we care about the change in z vs change in s
  // So here we calculate the values dz_i and phi_i (because ds_i = R * phi_i)

  std::vector<double> dz;
  std::vector<double> dphi_err;
  std::vector<SciFiSpacePoint*> spnts_by_zed;

  // TODO replace this with a proper stl sort function
  int spnts_size = spnts.size();
  if (spnts[0]->get_tracker() == 0) {
    for (int i = 0; i < spnts_size; i++) {
      int swith = spnts_size - i - 1;
      spnts_by_zed.push_back(spnts[swith]);
    }
  } else { spnts_by_zed = spnts; }

  for ( size_t i = 0; i < spnts.size(); ++i ) {
    std::cerr << "SP" << i << ": Tracker = " << spnts[i]->get_tracker() << " z = ";
    std::cerr << spnts[i]->get_position().z() << " t  = " << spnts[i]->get_time() << std::endl;
  }

  // Calculate phi_0, the rotation when moving from x to x'
  // TODO approximates 1st sp to x0, y0 which is not quite true at best,
  // and completely false if there is no spacepoint in the first station
  ThreeVector pos0 = spnts_by_zed[0]->get_position();
  phi_0 = old_calc_phi(pos0.x(), pos0.y(), circle);

  // Loop over spacepoints
  for ( int i = 1; i < static_cast<int>(spnts_by_zed.size()); ++i ) {

    // TODO Why are we removing the offset from sp0?
    if (spnts[0]->get_tracker() == 0) {
      dz.push_back(spnts_by_zed[0]->get_position().z() - spnts_by_zed[i]->get_position().z());
    } else {
      dz.push_back(spnts_by_zed[i]->get_position().z() - spnts_by_zed[0]->get_position().z());
    }

    // theta_i is defined as phi_i + phi_0 i.e. the turning angle wrt the x (not x') axis
    double theta_i = old_calc_phi(spnts_by_zed[i]->get_position().x(),
                              spnts_by_zed[i]->get_position().y(), circle);

    // phi_i is defined as the turning angle wrt the x' axis, given by theta_i - phi_0
    dphi.push_back(theta_i - phi_0);
    // std::cerr << "Pushed " << theta_i - phi_0 << " into dphi, size is " << dphi.size() << "\n";

    // Set the error on phi
    double sd_phi = -1.0;
    if ( spnts_by_zed[i]->get_station() == 5 )
      sd_phi = _sd_phi_5;
    else
      sd_phi = _sd_phi_1to4;
    dphi_err.push_back(sd_phi);
  } // ~Loop over spacepoints

  // Correct dphi for 2pi rotations
  bool dphi_ok = turns_between_stations(dz, dphi);

  // Use the dphi to calculate the ds by multiplying each element of dphi by R
  if ( dphi_ok ) {
    std::vector<double> ds;
    dphi_to_ds(circle.get_R(), dphi, ds);
    // Peform a linear fit in s - z
    linear_fit(dz, ds, dphi_err, line_sz); // Take ds_err to be dphi_err (is this true??)
  }
} // ~calculate_dipangle(...)

double PatternRecognition::calc_phi(double xpos, double ypos, const SimpleCircle &circle) {
  // Note this function returns phi_i + phi_0, unless using x0, y0 in which case it returns phi_0
  double angle = 0.0;

  std::cerr << "calc_phi: x is " << xpos << ", y is " << ypos << ", R is " << circle.get_R();
  std::cerr << ", X0 is " << circle.get_x0() << ", Y0 is " << circle.get_y0();

  // As we use tan to find the angle we need to be careful which of the 4 trig quadrants we are in
  if ( (xpos > circle.get_x0()) && (ypos > circle.get_y0()) ) {        // Angle in first quadrant
    angle = fabs(atan((ypos - circle.get_y0())/(xpos - circle.get_x0())));
  } else if ( (xpos < circle.get_x0()) && (ypos > circle.get_y0()) ) { // Angle in second quadrant
    angle = (CLHEP::pi) - fabs(atan((ypos - circle.get_y0())/(xpos - circle.get_x0())));
  } else if ( (xpos < circle.get_x0()) && (ypos < circle.get_y0()) ) { // Angle in third quadrant
    angle = (CLHEP::pi) + fabs(atan((ypos - circle.get_y0())/(xpos - circle.get_x0())));
  } else if ( (xpos > circle.get_x0()) && (ypos < circle.get_y0()) ) { // Angle in fourth quadrant
    angle = (2*CLHEP::pi) - fabs(atan((ypos - circle.get_y0())/(xpos - circle.get_x0())));
  }
  std::cerr << ", phi is " << angle << "\n";
  return angle;
} // ~calculate_phi(...)

double PatternRecognition::old_calc_phi(double xpos, double ypos, const SimpleCircle &circle) {
    // Note this function returns phi_i + phi_0, unless using x0, y0 in which case it returns phi_0
    double angle = atan2(ypos - circle.get_y0(), xpos - circle.get_x0());
    if ( angle < 0. ) angle += 2. * CLHEP::pi; // TODO is this ok if have different sign particles?
    return angle;
} // ~calculate_phi(...)

bool PatternRecognition::turns_between_stations(const std::vector<double> &dz,
                                                std::vector<double> &dphi) {
  // Make sure that you have enough points to make a line (2)
  if ( dz.size() < 2 || dphi.size() < 2 )
    return false;
  for ( int k = 0; k < _k_limit; ++k ) {  // TODO what is this obscure outer loop doing?
    for ( int i = 0; i < static_cast<int>(dphi.size()) - 1; ++i ) {
      int j = i + 1;
      // TODO Is this ok in the case of having both positive and negative particles?
      if ( dphi[i] < 0 )
        dphi[i] += 2. * CLHEP::pi;

      if ( dphi[j] < 0 )
        dphi[j] += 2. * CLHEP::pi;

      if ( dphi[j] < dphi[i] )
        dphi[j] += 2. * CLHEP::pi;

      double z_ratio = dz[j] / dz[i];
      double phi_ratio = dphi[j] / dphi[i];

      if ( fabs(phi_ratio - z_ratio) /  z_ratio > _AB_cut ) {
        // try
        bool passed_cut = AB_ratio(dphi[i], dphi[j], dz[i], dz[j]);
        if ( !passed_cut )
          return false;
      }
    }
  }
  return true;
} // ~turns_between_stations(...)

bool PatternRecognition::AB_ratio(double &dphi_ji, double &dphi_kj, double dz_ji, double dz_kj) {
  for ( int n = 0; n < _n_limit; ++n ) {
    for ( int m = 0; m < _m_limit; ++m ) {
      double A, B;
      A = ( dphi_kj + ( 2 * n * CLHEP::pi ) ) / ( dphi_ji + ( 2 * m * CLHEP::pi ) ); // phi_ratio
      B = dz_kj / dz_ji; // z_ratio
      if ( fabs(A - B) / B < _AB_cut ) {
        dphi_kj += 2 * n * CLHEP::pi;
        dphi_ji += 2 * m * CLHEP::pi;
        return true;
      }
    } // end m loop
  } // end n loop
  return false; // Return false if _ABcut is never satisfied
} // ~AB_ratio(...)

void PatternRecognition::dphi_to_ds(double R, const std::vector<double> &dphi,
                                    std::vector<double> &ds) {
  // This function performs scaler multiplication on the input vector.
  for ( int i = 0; i < static_cast<int>(dphi.size()); ++i ) {
    double ds_ji = dphi[i] * R;
    ds.push_back(ds_ji);
  }
} // ~dphi_to_ds(...)

bool PatternRecognition::check_time_consistency(const std::vector<SciFiSpacePoint*> good_spnts) {

  double dT_first, dT_last;
  /* TODO Waiting for Spacepoints to have time added ****
  double dT_first = good_spnts.front()->get_time();
  double dT_last  = good_spnts.back()->get_time();
  */
  double dZ = fabs(good_spnts.back()->get_position().z() - good_spnts.front()->get_position().z());

  double dS;
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
    if (_debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
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
