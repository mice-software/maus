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
#include <cmath>
#include <string>
#include <sstream>

// External libs headers
#include "TROOT.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/DataStructure/SimpleLine.hh"
#include "src/common_cpp/DataStructure/SimpleCircle.hh"
#include "src/common_cpp/DataStructure/SimpleHelix.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"


namespace MAUS {

PatternRecognition::PatternRecognition() {
  if (debug == 2) {
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
  if (debug == 2) {
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
    std::vector<SciFiStraightPRTrack> strks;
    std::vector<SciFiHelicalPRTrack> htrks;
    make_5tracks(track_type, spnts_by_station, strks, htrks);
    add_tracks(trker_no, strks, htrks, evt);
  }
  num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
  if (num_stations_hit > 3) {
    std::vector<SciFiStraightPRTrack> strks;
    std::vector<SciFiHelicalPRTrack> htrks;
    make_4tracks(track_type, spnts_by_station, strks, htrks);
    add_tracks(trker_no, strks, htrks, evt);
  }
  num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
  if (num_stations_hit > 2) {
    std::vector<SciFiStraightPRTrack> strks;
    std::vector<SciFiHelicalPRTrack> htrks;
    make_3tracks(track_type, spnts_by_station, strks, htrks);
    add_tracks(trker_no, strks, htrks, evt);
  }
}


void PatternRecognition::add_tracks(const int trker_no, std::vector<SciFiStraightPRTrack> &strks,
                                    std::vector<SciFiHelicalPRTrack> &htrks, SciFiEvent &evt ) {
  for ( int i = 0; i < static_cast<int>(strks.size()); ++i ) {
    strks[i].set_tracker(trker_no);
    evt.add_straightprtrack(strks[i]);
  }
  for ( int i = 0; i < static_cast<int>(htrks.size()); ++i ) {
    htrks[i].set_tracker(trker_no);
    evt.add_helicalprtrack(htrks[i]);
  }
}

void PatternRecognition::make_5tracks(const bool track_type, SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack> &strks,
                                      std::vector<SciFiHelicalPRTrack> &htrks) {
  if ( debug > 0 ) std::cout << "Making 5 point tracks" << std::endl;
  int num_points = 5;
  std::vector<int> ignore_stations; // A zero size vector sets that all stations are to be used
  if ( track_type == 0 )
    make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
  if ( track_type == 1 )
    make_helix(num_points, ignore_stations, spnts_by_station, htrks);
  if ( debug > 0 ) std::cout << "Finished making 5 pt tracks" << std::endl;
} // ~make_spr_5pt(...)

void PatternRecognition::make_4tracks(const bool track_type, SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack> &strks,
                                      std::vector<SciFiHelicalPRTrack> &htrks) {
  if ( debug > 0 ) std::cout << "Making 4 point tracks" << std::endl;

  int num_points = 4;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    if ( debug > 0 ) std::cout << "4pt track: 5 stations with unused spacepoints" << std::endl;

    for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
      // Recount how many stations have at least one unused spacepoint
      num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
      // If there are enough occupied stations left to make a 4 point track, keep making tracks
      if ( num_stations_hit  >= num_points ) {
        std::vector<int> ignore_stations(1, i);
        if ( track_type == 0 )
          make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
        if ( track_type == 1 )
          make_helix(num_points, ignore_stations, spnts_by_station, htrks);
      } else {
        break;
      }
    } // ~Loop of stations, ignoring each one in turn
  } else if ( num_stations_hit == 4 ) {

    if ( debug > 0 ) std::cout << "4pt track: 4 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      if ( track_type == 0 )
        make_straight_tracks(num_points, stations_not_hit, spnts_by_station, strks);
      if ( track_type == 1 )
        make_helix(num_points, stations_not_hit, spnts_by_station, htrks);
    } else {
      if ( debug > 0 ) {
        std::cout << "Wrong number of stations without spacepoints, ";
        std::cout << "aborting 4 pt track." << std::endl;
      }
    }
  } else if ( num_stations_hit < 4 ) {
    if ( debug > 0 )
      std::cout << "Not enough unused spacepoints, quiting 4 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
    if ( debug > 0 )
      std::cout << "Wrong number of stations with spacepoints, aborting 4 pt track." << std::endl;
  }
  if ( debug > 0 ) std::cout << "Finished making 4 pt tracks" << std::endl;
} // ~make_straight_4tracks(...)

void PatternRecognition::make_3tracks(const bool track_type, SpacePoint2dPArray &spnts_by_station,
                                      std::vector<SciFiStraightPRTrack> &strks,
                                      std::vector<SciFiHelicalPRTrack> &htrks) {
  if ( debug > 0 ) std::cout << "Making 3 point track" << std::endl;

  int num_points = 3;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  bool sufficient_hit_stations = true;

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    if ( debug > 0 ) std::cout << "3pt track: 5 stations with unused spacepoints" << std::endl;

    for (int i = 0; i < 4; ++i) { // Loop of first station to ignore
      if ( sufficient_hit_stations ) {
        for (int j = i + 1; j < 5; ++j) { // Loop of second station to ignore
          if ( sufficient_hit_stations ) {
            // Recount how many stations have at least one unused spacepoint
            num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
            // If there are enough occupied stations left to make a 3pt track, keep making tracks
            if ( num_stations_hit  >= num_points ) {
              std::vector<int> ignore_stations;
              ignore_stations.push_back(i);
              ignore_stations.push_back(j);
              if ( track_type == 0 )
                make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
              if ( track_type == 1 )
                make_helix(num_points, ignore_stations, spnts_by_station, htrks);
            } else {
                sufficient_hit_stations = false;
            }
          } // ~if ( sufficient_hit_stations )
        } // ~Loop of second station to ignore
      } // ~if ( sufficient_hit_stations )
    } // ~Loop of first station to ignore
  } else if ( num_stations_hit == 4 ) {

    if ( debug > 0 ) std::cout << "3pt track: 4 stations with unused spacepoints" << std::endl;

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
        if ( num_stations_hit  >= num_points ) {
          ignore_stations.clear();
          ignore_stations.push_back(stations_not_hit[0]);
          ignore_stations.push_back(i);
          if ( track_type == 0 )
            make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
          if ( track_type == 1 )
            make_helix(num_points, ignore_stations, spnts_by_station, htrks);
        } else {
          break;
        }
      }
    }
  } else if ( num_stations_hit == 3 ) {

    if ( debug > 0 ) std::cout << "3pt track: 3 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 2 ) {
      if ( track_type == 0 )
        make_straight_tracks(num_points, stations_not_hit, spnts_by_station, strks);
      if ( track_type == 1 )
        make_helix(num_points, stations_not_hit, spnts_by_station, htrks);
    } else {
      if ( debug > 0 ) {
        std::cout << "Wrong number of stations without spacepoints, ";
        std::cout << "aborting 3 pt track." << std::endl;
      }
    }

  } else if ( num_stations_hit < 3 ) {
    if ( debug > 0 )
      std::cout << "Not enough unused spacepoints, quiting 3 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
    if ( debug > 0 )
      std::cout << "Wrong number of stations with spacepoints, aborting 3 pt track." << std::endl;
  }
  if ( debug > 0 ) std::cout << "Finished making 3 pt tracks" << std::endl;
} // ~make_straight_3tracks(...)

void PatternRecognition::make_straight_tracks(const int num_points,
                                     const std::vector<int> ignore_stations,
                                     SpacePoint2dPArray &spnts_by_station,
                                     std::vector<SciFiStraightPRTrack> &strks) {
  // Set inner and outer stations
  int outer_st_num = -1, inner_st_num = -1;
  set_end_stations(ignore_stations, outer_st_num, inner_st_num);

  if (static_cast<int>(spnts_by_station.size()) == _n_stations
      && outer_st_num > -1 && outer_st_num < _n_stations
      && inner_st_num > -1 && inner_st_num < _n_stations) {

    // Loop over spacepoints in outer station
    for ( unsigned int outer_sp = 0; outer_sp < spnts_by_station[outer_st_num].size();
          ++outer_sp ) {

      // Check the outer spacepoint is unused and enough stations are left with unused sp
      if ( !spnts_by_station[outer_st_num][outer_sp]->get_used() &&
           num_stations_with_unused_spnts(spnts_by_station) >= num_points) {

        // Loop over spacepoints in inner station
        for ( unsigned int inner_sp = 0;
            inner_sp < spnts_by_station[inner_st_num].size(); ++inner_sp ) {

          // Check the inner spacepoint is unused and enough stations are left with unused sp
          if ( !spnts_by_station[inner_st_num][inner_sp]->get_used() &&
              num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

            // Vector to hold the good spacepoints in each station
            std::vector<SciFiSpacePoint*> good_spnts;
            // good_spnts.resize(num_points);

            // Set variables to hold which stations are to be ignored
            int ignore_st_1 = -1, ignore_st_2 = -1;
            set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

            // Draw a straight line between spacepoints in outer most and inner most stations
            SimpleLine line_x, line_y;
            draw_line(spnts_by_station[outer_st_num][outer_sp],
                      spnts_by_station[inner_st_num][inner_sp], line_x, line_y);

            // Loop over intermediate stations and compare spacepoints with the line
            for ( int st_num = inner_st_num + 1; st_num < outer_st_num; ++st_num ) {
              if (st_num != ignore_st_1 && st_num != ignore_st_2) {

                // A large number so initial value is set as best first
                double delta_sq = 1000000;
                int best_sp = -1;

                // Loop over spacepoints
                for ( unsigned int sp_no = 0; sp_no < spnts_by_station[st_num].size(); ++sp_no ) {

                  // If the spacepoint has not already been used in a track fit
                  if ( !spnts_by_station[st_num][sp_no]->get_used() ) {
                    SciFiSpacePoint *sp = spnts_by_station[st_num][sp_no];
                    double dx = 0, dy = 0;
                    calc_residual(sp, line_x, line_y, dx, dy);
                    if ( debug > 1 )
                      *_f_res << st_num << "\t" << num_points << "\t" << dx << "\t" << dy << "\n";

                    // Apply roadcuts & find the spoints with the smallest residuals for the line
                    if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut )  {
                      if ( debug > 1 ) {
                        *_f_res_good << st_num << "\t" << num_points << "\t";
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
                  calc_residual(sp, line_x, line_y, dx, dy);
                  if ( debug > 1 ) {
                    *_f_res_chosen << st_num << "\t" << num_points << "\t";
                    *_f_res_chosen << dx << "\t" << dy << "\n";
                  }
                }// ~if (counter > 0)
              } // ~if (st_num != ignore_station)
            } // ~Loop over intermediate stations

            // Clear the line objects so we can reuse them
            line_x.clear();
            line_y.clear();

            // Check we have at least 1 good spacepoint in each of the intermediate stations
            if ( static_cast<int>(good_spnts.size()) > (num_points - 3) ) {

              good_spnts.insert(good_spnts.begin(), spnts_by_station[inner_st_num][inner_sp]);
              good_spnts.push_back(spnts_by_station[outer_st_num][outer_sp]);
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
              if ( ( line_x.get_chisq() / ( num_points - 2 ) < _chisq_cut ) &&
                  ( line_y.get_chisq() / ( num_points - 2 ) < _chisq_cut ) ) {

                if ( debug > 0 )
                  std::cout << "** chisq test passed, adding " << num_points << "pt track **\n";
                SciFiStraightPRTrack track(-1, num_points, line_x, line_y);
                if ( debug > 0 ) {
                  std::cout << "x0 = " << track.get_x0() << "mx = " << track.get_mx();
                  std::cout << "y0 = " << track.get_y0() << "my = " << track.get_my() << std::endl;
                }
                // *_f_trks << num_points << "\t" << track.get_x0() << "\t" << track.get_mx();
                // *_f_trks << "\t" << track.get_x_chisq() << "\t" << track.get_y0() << "\t";
                // *_f_trks << track.get_my() << "\t" << track.get_y_chisq() << "\t" << 1 << "\n";

                // Set all the good sp to used and convert pointers to variables
                std::vector<SciFiSpacePoint> good_spnts_variables;
                good_spnts_variables.resize(good_spnts.size());

                for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i ) {
                  good_spnts[i]->set_used(true);
                  good_spnts_variables[i] = *good_spnts[i];
                }

                // Populate the sp of the track and then push the track back into the strks vector
                track.set_spacepoints(good_spnts_variables);
                strks.push_back(track);
              } else {
                if ( debug > 0 ) {
                  std::cout << "x_chisq = " << line_x.get_chisq();
                  std::cout << "\ty_chisq = " << line_y.get_chisq() << std::endl;
                  std::cout << "chisq test failed, " << num_points << "pt track rejected\n";
                }

                SciFiStraightPRTrack bad_track(-1, num_points, line_x, line_y);
                // *_f_trks << num_points << "\t" << bad_track.get_x0() << "\t";
                // *_f_trks << bad_track.get_mx() << "\t" << bad_track.get_x_chisq() << "\t";
                // *_f_trks << bad_track.get_y0() << "\t" << bad_track.get_my() << "\t";
                // *_f_trks << bad_track.get_y_chisq() << "\t" << 0 << "\n";
              } // ~Check track passes chisq test
            } // ~ if ( good_spnts.size() > 1 )
          } else {
            // std::cout << "...no" << std::endl;
          }// ~Check the inner spacepoint is unused
        } // ~Loop over sp in station 1
      } else {
        // std::cout << "...no" << std::endl;
      }// ~Check the outer spacepoint is unused
    } // ~Loop over sp in station 5
  } else {
    if ( debug > 0 ) std::cerr << "Bad spnts_by_station passed, aborting make_straight_tracks.\n";
  }
} // ~make_straight_tracks(...)

void PatternRecognition::linear_fit(const std::vector<double> &_x, const std::vector<double> &_y,
                                    const std::vector<double> &_y_err, SimpleLine &line) {

  int num_points = static_cast<int>(_x.size());

  CLHEP::HepMatrix A(num_points, 2); // rows, columns
  CLHEP::HepMatrix V(num_points, num_points); // error matrix
  CLHEP::HepMatrix Y(num_points, 1); // measurements

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
  line.set_chisq_dof(result[0][0] / num_points);
} // ~linear_fit(...)

void PatternRecognition::make_helix(const int num_points, const std::vector<int> ignore_stations,
                                    SpacePoint2dPArray &spnts_by_station,
                                    std::vector<SciFiHelicalPRTrack> &htrks) {

  // Set inner and outer stations
  int outer_st_num = -1, inner_st_num = -1, mid_st_num = -1;
  set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num);

  // ** Beginning nested loops to find unused spacepoints from each station available. **
  // Loop over spacepoints in outer station
  for ( unsigned int outer_sp = 0; outer_sp < spnts_by_station[outer_st_num].size(); ++outer_sp ) {

    // Check the outer spacepoint is unused and enough stations are left with unused sp
    if ( !spnts_by_station[outer_st_num][outer_sp]->get_used() &&
         num_stations_with_unused_spnts(spnts_by_station) >= num_points) {

      // Loop over spacepoints in inner station
      for ( unsigned int inner_sp = 0; inner_sp < spnts_by_station[inner_st_num].size();
           ++inner_sp ) {

        // Check the inner spacepoint is unused and enough stations are left with unused sp
        if ( !spnts_by_station[inner_st_num][inner_sp]->get_used() &&
             !spnts_by_station[outer_st_num][outer_sp]->get_used() &&
             num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

          // Loop over spacepoints in middle station
          for ( unsigned int mid_sp = 0; mid_sp < spnts_by_station[mid_st_num].size(); ++mid_sp ) {

            // Check intermediate spnts are unused and enough stations are left with unused spnts
            if ( !spnts_by_station[mid_st_num][mid_sp]->get_used() &&
                 !spnts_by_station[inner_st_num][inner_sp]->get_used() &&
                 !spnts_by_station[outer_st_num][outer_sp]->get_used() &&
                 num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

              // Vector to hold the good spacepoints in each station
              std::vector<SciFiSpacePoint*> good_spnts;

              // Set variables to hold which stations are to be ignored
              int ignore_st_1 = -1, ignore_st_2 = -1;
              set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

              // Loop over other intermediate stations so that can try adding them to the circle
              for ( int st_num = inner_st_num + 1; st_num < outer_st_num; ++st_num ) {
                if (st_num != mid_st_num && st_num != ignore_st_1 && st_num != ignore_st_2) {

                  for (unsigned int sp_no = 0; sp_no < spnts_by_station[st_num].size(); ++sp_no) {
                    // If the spacepoint has not already been used in a track fit
                    if ( !spnts_by_station[st_num][sp_no]->get_used() ) {
                         good_spnts.push_back(spnts_by_station[st_num][sp_no]);
                    }
                  } // ~loop over intermediate station sp
                } // ~if (st_num != ignore_station)
              } // ~Loop over intermediate stations not used in initial circle_fit

              // Fill the rest of the good_spnts vector
              if ( good_spnts.size() > 0 && good_spnts[0]->get_station() > mid_st_num+1 )
                good_spnts.insert(good_spnts.begin(), spnts_by_station[mid_st_num][mid_sp]);
              else if ( good_spnts.size() > 0 && good_spnts[0]->get_station() < mid_st_num+1 )
                good_spnts.insert(good_spnts.begin() + 1, spnts_by_station[mid_st_num][mid_sp]);
              else
                good_spnts.push_back(spnts_by_station[mid_st_num][mid_sp]);

              // Check we have at least 1 good spacepoint in each of the intermediate stations
              if ( static_cast<int>(good_spnts.size()) >= (num_points - 3) ) {

                good_spnts.insert(good_spnts.begin(), spnts_by_station[inner_st_num][inner_sp]);
                good_spnts.push_back(spnts_by_station[outer_st_num][outer_sp]);

                // Perform a circle fit now that we have found a set of spacepoints
                SimpleCircle circle;
                bool good_radius = circle_fit(good_spnts, circle);

                // If the radius calculated is too large, force loop to iterate from here
                if ( !good_radius ) continue;

                // Check circle chisq test passes, then perform s-z fit
                if ( circle.get_chisq() / ( num_points - 2 ) < _chisq_cut ) {

                  // std::cerr << "Found a good circle using spacepoints:\n";
                  for (unsigned int i = 0; i < good_spnts.size(); ++i) {
                    ThreeVector pos = good_spnts[i]->get_position();
                    std::cerr << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;
                  }
                  /* std::cerr << " circle_x0 is " << circle.get_x0() << std::endl;
                  std::cerr << " circle_y0 is " << circle.get_y0() << std::endl;
                  std::cerr << " circle_R is " << circle.get_R() << std::endl;
                  std::cerr << " circle_chi2 is " << circle.get_chisq() << std::endl; */

                  SimpleLine line_sz;
                  std::vector<double> dphi; // to hold change between turning angles
                  double phi_0; // initial turning angle

                  // Perform s-z fit
                  calculate_dipangle(good_spnts, circle, dphi, line_sz, phi_0);
                  /*
                  std::cerr << "line_sz_c is " << line_sz.get_c() << std::endl;
                  std::cerr << "line_sz_m is " << line_sz.get_m() << std::endl;
                  std::cerr << "line_sz_chisq is " << line_sz.get_chisq() << std::endl;
                  */

                  // Check linear fit passes chisq test, then perform make a helical track
                  if ( line_sz.get_chisq() / ( num_points - 2 ) < _sz_chisq_cut ) {

                    double pt = circle.get_R() * 1.2;
                    double pz = pt / line_sz.get_m();
                    if ( debug > 0 ) std::cerr << "dsdz = " << line_sz.get_m() << std:: endl;

                    double psi_0 = phi_0 + (CLHEP::pi / 2);
                    SciFiHelicalPRTrack track(-1, num_points, good_spnts[0]->get_position(),
                                              phi_0, psi_0, circle, line_sz);

                    // Set all the good sp to used and convert pointers to variables
                    std::vector<SciFiSpacePoint> good_spnts_variables;
                    good_spnts_variables.resize(good_spnts.size());

                    for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i ) {
                      good_spnts[i]->set_used(true);
                      good_spnts_variables[i] = *good_spnts[i];
                    }
                    // Populate the sp of the track and then push the track back into trks vector
                    track.set_spacepoints(good_spnts_variables);
                    htrks.push_back(track);
                  } // ~Check s-z line passes chisq test
                } // ~Check circle passes chisq test
              } // ~if enough spacepoints are found for fit (must be at least 3)
            } // ~if middle station sp unused
          } // ~Loop over middle station spacepoints
        } // ~If inner station spacepoint is unused
      } // ~Loop over inner station spacepoints
    } // ~if outer station spacepoint is unused
  } // ~Loop over outer station spacepoints
} // ~make_helix(...)

bool PatternRecognition::circle_fit(const std::vector<SciFiSpacePoint*> &spnts,
                                    SimpleCircle &circle) {

  int num_points = static_cast<int>(spnts.size());
  CLHEP::HepMatrix A(num_points, 3); // rows, columns
  CLHEP::HepMatrix V(num_points, num_points); // error matrix
  CLHEP::HepMatrix K(num_points, 1);

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

  if ( debug > 0 ) {
    std::cout << "alpha = " << alpha << std::endl;
    std::cout << "beta = " << beta << std::endl;
    std::cout << "gamma = " << gamma << std::endl;
    if ( R < 0. )
      std::cout << "R was < 0 geometrically but taking abs_val for physical correctness\n";
  }

  R = fabs(R);

  if (R > 150)
    return false; // Can not be larger than 150mm or the track is not contained in tracker

  circle.set_x0(x0);
  circle.set_y0(y0);
  circle.set_R(R);
  /*
  circle.set_x0_err(x0_err);
  circle.set_y0_err(y0_err);
  circle.set_R_err(R_err);
  */
  //  Stored to be used for delta_R calculation
  circle.set_alpha(alpha);
  circle.set_beta(beta);
  circle.set_gamma(gamma);
  // circle.set_alpha_err(alpha_err);
  // circle.set_beta_err(beta_err);
  // circle.set_gamma_err(gamma_err);

  CLHEP::HepMatrix C, result;

  C = K - (A * tmp_params);
  result = C.T() * V * C;
  double chi2 = result[0][0];
  circle.set_chisq(chi2); // should I leave this un-reduced?
  return true;
} // ~circle_fit(...)


void PatternRecognition::calculate_dipangle(const std::vector<SciFiSpacePoint*> &spnts,
                                            const SimpleCircle &circle, std::vector<double> &dphi,
                                            SimpleLine &line_sz, double &phi_0) {
  // For the linear fit in s-z, we care about the change in z vs change in s
  // So here we calculate the values dz_i and phi_i (because ds_i = R * phi_i)

  std::vector<double> dz;
  std::vector<double> dphi_err;

  // Calculate phi_0, the rotation when moving from x to x'
  phi_0 = calc_turning_angle(spnts[0]->get_position().x(), spnts[0]->get_position().y(), circle);

  // Loop over spacepoints
  for ( int i = 1; i < static_cast<int>(spnts.size()); ++i ) {

    dz.push_back(spnts[i]->get_position().z() - spnts[0]->get_position().z());

    // theta_i is defined as phi_i + phi_0 i.e. the turning angle wrt the x (not x') axis
    double theta_i = calc_turning_angle(spnts[i]->get_position().x(),
                                        spnts[i]->get_position().y(), circle);

    // phi_i is defined as the turning angle wrt the x' axis, given by theta_i - phi_0
    dphi.push_back(theta_i - phi_0);

    // Set the error on phi
    double sd_phi = -1.0;
    if ( (spnts[i]->get_station() == 5) )
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
    if ( debug > 0 ) {
      for ( unsigned int i = 0; i < ds.size(); i++ ) {
        std::cerr << "ds = " << ds[i] << ", dz = " << dz[i] << std::endl;
      }
    }
    // Peform a linear fit in s - z
    linear_fit(dz, ds, dphi_err, line_sz); // Take ds_err to be dphi_err (is this true??)
  }
} // ~calculate_dipangle(...)

double PatternRecognition::calc_turning_angle(double xpos, double ypos,
                                              const SimpleCircle &circle) {

  // Note this function returns phi_i + phi_0, unless using x0, y0 in which case it returns phi_0
  double angle = atan2(ypos - circle.get_y0(), xpos - circle.get_x0());

  if ( angle < 0. ) angle += 2. * CLHEP::pi;
  if ( debug > 0 ) std::cout << "Turning angle from circle fit = " << angle << std::endl;
  return angle;
} // ~calculate_phi(...)

bool PatternRecognition::turns_between_stations(const std::vector<double> &dz,
                                                std::vector<double> &dphi) {
  // Make sure that you have enough points to make a line (2)
  if ( dz.size() < 2 || dphi.size() < 2 )
    return false;
  for ( int n = 0; n < 2; ++n ) {
    for ( int i = 0; i < static_cast<int>(dphi.size()) - 1; ++i ) {
      int j = i + 1;
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

bool PatternRecognition::AB_ratio(double &dphi_ji, double &dphi_kj, double dz_ji,
                                  double dz_kj) {
  // double A, B;

  for ( int n = 0; n < 10; ++n ) {
    for ( int m = 0; m < 10; ++m ) {
      // std::cerr << "n is " << n << " and m is " << m << std::endl;
      double A, B;
      A = ( dphi_kj + ( 2 * n * CLHEP::pi ) ) / ( dphi_ji + ( 2 * m * CLHEP::pi ) ); // phi_ratio
      B = dz_kj / dz_ji; // z_ratio
      // std::cerr <<  "| A - B | / B = " << fabs(A - B)/B;
      // std::cerr << ", new phi_i = " << dphi_ji + ( 2 * m * CLHEP::pi );
      // std::cerr<< ", new phi_j = " << dphi_kj + ( 2 * n * CLHEP::pi ) << std::endl;
      if ( fabs(A - B) / B < _AB_cut ) {
        dphi_kj += 2 * n * CLHEP::pi;
        dphi_ji += 2 * m * CLHEP::pi;
        // std::cerr << "Success: n = " << n << " m = " << m << std::endl;
        // std::cerr << "| A - B | / B = " << fabs(A - B)/B << " and AB_cut = " << _AB_cut << "\n";
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
  /* Waiting for Spacepoints to have time added ****
  double dT_first = good_spnts.front()->get_time();
  double dT_last  = good_spnts.back()->get_time();
  */
  double dZ = fabs(good_spnts.back()->get_position().z() - good_spnts.front()->get_position().z());

  double dS;
  if ( _straight_pr_on && !_helical_pr_on ) // If you are ONLY looking at straight tracks
    dS = dZ;
  else if ( _helical_pr_on ) // if you are trying to reconstruc EITHER straight OR helical tracks
    dS = dZ * _Pt_max / _Pz_min; // _Pz_min is a guess right now. (both defined in header)

  double longest_allowed_time = dS / CLHEP::c_light;

  if ( fabs(dT_last - dT_first) > longest_allowed_time )
    return false;
  else
    return true;
}

// For linear Pattern Recognition use
bool PatternRecognition::set_end_stations(const std::vector<int> ignore_stations,
                      int &outer_st_num, int &inner_st_num) {

  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    outer_st_num = 4;
    inner_st_num = 0;
  } else if ( static_cast<int>(ignore_stations.size()) == 1 ) { // 4pt track case
      // Check a valid station number has been supplied
      bool ok = false;
      for ( int i = 0; i < 5; ++i ) {
        if ( ignore_stations[0] == i ) ok = true;
      }
      if ( !ok ) {
        if ( debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
        return false;
      }
      // Set outer station number
      if ( ignore_stations[0] != 4 )
        outer_st_num = 4;
      else
        outer_st_num = 3;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_st_num = 0;
      else
        inner_st_num = 1;
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      bool ok0 = false;
      bool ok1 = false;
      for ( int i = 0; i < 5; ++i ) {
        if ( ignore_stations[0] == i ) ok0 = true;
        if ( ignore_stations[1] == i ) ok1 = true;
      }
      if ( !ok0 || !ok1 ) {
        if ( debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
        return false;
      }
      // Set outer station number
      if ( ignore_stations[0] != 4 && ignore_stations[1] != 4 )
        outer_st_num = 4;
      else if ( ignore_stations[0] != 3 && ignore_stations[1] != 3)
        outer_st_num = 3;
      else
        outer_st_num = 2;
      // Set inner station number
      if ( ignore_stations[0] != 0 && ignore_stations[1] != 0 )
        inner_st_num = 0;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 1 )
        inner_st_num = 1;
      else
        inner_st_num = 2;
  } else {
    if ( debug > 0 ) std::cerr << "Error: Invalid ignore station argument." << std::endl;
    return false;
  }
  return true;
} // ~set_end_stations(...)

// For helical Pattern Recognition use
void PatternRecognition::set_seed_stations(const std::vector<int> ignore_stations,
                      int &outer_st_num, int &inner_st_num, int &mid_st_num) {
  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    outer_st_num = 4;
    inner_st_num = 0;
    mid_st_num = 2;
  } else if ( static_cast<int>(ignore_stations.size()) == 1 ) { // 4pt track case
      // Set outer station number
      if ( ignore_stations[0] != 4 )
        outer_st_num = 4;
      else
        outer_st_num = 3;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_st_num = 0;
      else
        inner_st_num = 1;
       // Set middle station number
       if ( ignore_stations[0] != 2 )
         mid_st_num = 2;
       else
         mid_st_num = 1;
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      // Set outer station number
      if ( ignore_stations[1] != 4 )
        outer_st_num = 4;
      else if ( ignore_stations[0] != 3 )
        outer_st_num = 3;
      else
        outer_st_num = 2;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_st_num = 0;
      else if ( ignore_stations[1] != 1 )
        inner_st_num = 1;
      else
        inner_st_num = 2;
      // Set middle station number
      if ( ignore_stations[0] != 0 && ignore_stations[1] != 1 )
        mid_st_num = 1;
      else if ( ignore_stations[0] != 0 && ignore_stations[1] != 2 )
        mid_st_num = 2;
      else if ( ignore_stations[0] != 0 && ignore_stations[1] != 3 )
        mid_st_num = 3;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 2 )
        mid_st_num = 2;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 3 )
        mid_st_num = 3;
      else // case where   ignore_stations[0] != 2 && ignore_stations[1] != 3
        mid_st_num = 3;
    } else {
    if (debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
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

SpacePoint2dPArray PatternRecognition::sort_by_tracker(
                                                    const std::vector<SciFiSpacePoint*> &spnts) {
  SpacePoint2dPArray spnts_by_tracker(_n_trackers);
  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
    for ( unsigned int i = 0; i < spnts.size(); ++i ) {
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

void PatternRecognition::stations_with_unused_spnts(
                         const SpacePoint2dPArray &spnts_by_station,
                         std::vector<int> &stations_hit, std::vector<int> &stations_not_hit) {
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

void PatternRecognition::set_ignore_stations(const std::vector<int> &ignore_stations,
                         int &ignore_st_1, int &ignore_st_2) {
  ignore_st_1 = -1, ignore_st_2 = -1;
  if ( ignore_stations.size() == 0 ) {
    // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_st_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_st_1 = ignore_stations[0];
    ignore_st_2 = ignore_stations[1];
  } else if ( ignore_stations.size() > 2 ) {
    std::cerr << "Error: Invalid ignore station argument." << std::endl;
  }
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

void PatternRecognition::calc_residual(const SciFiSpacePoint *sp, const SimpleLine &line_x,
                                       const SimpleLine &line_y, double &dx, double &dy) {

    ThreeVector pos = sp->get_position();
    dx = pos.x() - ( line_x.get_c() + ( pos.z() * line_x.get_m() ) );
    dy = pos.y() - ( line_y.get_c() + ( pos.z() * line_y.get_m() ) );
} // ~calc_residual(...)

} // ~namespace MAUS
