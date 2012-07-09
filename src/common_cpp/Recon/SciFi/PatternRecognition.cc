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
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>

// External libs headers
#include "gsl/gsl_fit.h"
#include "CLHEP/Matrix/Matrix.h"
#include "TROOT.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"

// namespace MAUS {

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

void PatternRecognition::process(SciFiEvent &evt) {

  if ( debug > 0 ) {
    std::cout << "\nBegining Pattern Recognition" << std::endl;
    std::cout << "Number of spacepoints in spill: " << evt.spacepoints().size() << std::endl;
  }

  if ( static_cast<int>(evt.spacepoints().size()) > 0 ) {

    // Split spacepoints up according to which tracker they occured in and set used flag to false
    std::vector< std::vector<SciFiSpacePoint*> > spnts_by_tracker(_n_trackers);
    for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {  // Loop over trackers
      for ( unsigned int i = 0; i < evt.spacepoints().size(); ++i ) {  // Loop over spacepoints
        evt.spacepoints()[i]->set_used(false);
        if ( evt.spacepoints()[i]->get_tracker() == trker_no ) {
          spnts_by_tracker[trker_no].push_back(evt.spacepoints()[i]);
        }
      } // ~Loop over spacepoints
    } // ~Loop over trackers

    // Loop over trackers
    for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
      if ( debug > 0 )
        std::cout << "Reconstructing for Tracker " << trker_no + 1 << std::endl;

      // Split spacepoints according to which station they occured in
      std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(_n_stations);
      sort_by_station(spnts_by_tracker[trker_no], spnts_by_station);

      // Count how many stations have at least one *unused* spacepoint
      int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

      // Make the tracks depending on how many stations have spacepoints in them
      if (num_stations_hit == 5) {
        std::vector<SciFiStraightPRTrack> strks;
        std::vector<SciFiHelicalPRTrack> htrks;
        make_5tracks(spnts_by_station, strks, htrks);
        for ( int i = 0; i < static_cast<int>(strks.size()); ++i ) {
          strks[i].set_tracker(trker_no);
          evt.add_straightprtrack(strks[i]);
          // evt.set_residuals(residuals);
        }
        for ( int i = 0; i < static_cast<int>(htrks.size()); ++i ) {
          htrks[i].set_tracker(trker_no);
          evt.add_helicalprtrack(htrks[i]);
        }
      }
      if (num_stations_hit > 3) {
        std::vector<SciFiStraightPRTrack> strks;
        std::vector<SciFiHelicalPRTrack> htrks;
        make_4tracks(spnts_by_station, strks, htrks);
        for ( int i = 0; i < static_cast<int>(strks.size()); ++i ) {
          strks[i].set_tracker(trker_no);
          evt.add_straightprtrack(strks[i]);
          // evt.set_residuals(residuals);
        }
        for ( int i = 0; i < static_cast<int>(htrks.size()); ++i ) {
          htrks[i].set_tracker(trker_no);
          evt.add_helicalprtrack(htrks[i]);
        }
      }
      if (num_stations_hit > 2) {
        std::vector<SciFiStraightPRTrack> strks;
        std::vector<SciFiHelicalPRTrack> htrks;
        make_3tracks(spnts_by_station, strks, htrks);
        for ( int i = 0; i < static_cast<int>(strks.size()); ++i ) {
          strks[i].set_tracker(trker_no);
          evt.add_straightprtrack(strks[i]);
          // evt.set_residuals(residuals);
        }
        for ( int i = 0; i < static_cast<int>(htrks.size()); ++i ) {
          htrks[i].set_tracker(trker_no);
          evt.add_helicalprtrack(htrks[i]);
        }
      }
      if ( debug > 0 )
        std::cout << "Finished Tracker " << trker_no + 1 << std::endl;
    }// ~Loop over trackers
    std::cout << "Number of straight tracks found: " << evt.straightprtracks().size() << "\n\n";
    std::cout << "Number of helical tracks found: " << evt.helicalprtracks().size() << "\n\n";
  } else {
    std::cout << "No spacepoints in event" << std::endl;
  }
};

void PatternRecognition::make_5tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<SciFiStraightPRTrack> &strks,
                         std::vector<SciFiHelicalPRTrack> &htrks) {
  if ( debug > 0 ) std::cout << "Making 5 point tracks" << std::endl;
  int num_points = 5;
  std::vector<int> ignore_stations; // A zero size vector sets that all stations are used
  if ( _straight_pr_on )
    make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
  if ( _helical_pr_on )
    make_helix(num_points, ignore_stations, spnts_by_station, htrks);
  if ( debug > 0 ) std::cout << "Finished making 5 pt tracks" << std::endl;
} // ~make_spr_5pt(...)

void PatternRecognition::make_4tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
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
        if ( _straight_pr_on )
          make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
        if ( _helical_pr_on )
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
      if ( _straight_pr_on )
        make_straight_tracks(num_points, stations_not_hit, spnts_by_station, strks);
      if ( _helical_pr_on )
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

void PatternRecognition::make_3tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
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
              if ( _straight_pr_on )
                make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
              if ( _helical_pr_on )
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
          if ( _straight_pr_on )
            make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);
          if ( _helical_pr_on )
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
      if ( _straight_pr_on )
        make_straight_tracks(num_points, stations_not_hit, spnts_by_station, strks);
      if ( _helical_pr_on )
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
                                     std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                                     std::vector<SciFiStraightPRTrack> &strks) {
  // Set inner and outer stations
  int outer_station_num = -1, inner_station_num = -1;
  set_end_stations(ignore_stations, outer_station_num, inner_station_num);

  if (spnts_by_station.size() == _n_stations
      && outer_station_num > -1 && outer_station_num < _n_stations
      && inner_station_num > -1 && inner_station_num < _n_stations) {
  // Loop over spacepoints in outer station
    for ( unsigned int station_outer_sp = 0;
          station_outer_sp < spnts_by_station[outer_station_num].size(); ++station_outer_sp ) {

      // Check the outer spacepoint is unused and enough stations are left with unused sp
      if ( !spnts_by_station[outer_station_num][station_outer_sp]->get_used() &&
          num_stations_with_unused_spnts(spnts_by_station) >= num_points) {

        // Loop over spacepoints in inner station
        for ( unsigned int station_inner_sp = 0;
            station_inner_sp < spnts_by_station[inner_station_num].size(); ++station_inner_sp ) {

          // Check the inner spacepoint is unused and enough stations are left with unused sp
          if ( !spnts_by_station[inner_station_num][station_inner_sp]->get_used() &&
              num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

            // Vector to hold the good spacepoints in each station
            std::vector<SciFiSpacePoint*> good_spnts;
            // good_spnts.resize(num_points);

            // Set variables to hold which stations are to be ignored
            int ignore_station_1 = -1, ignore_station_2 = -1;
            set_ignore_stations(ignore_stations, ignore_station_1, ignore_station_2);

            // Draw a straight line between spacepoints in outer most and inner most stations
            SimpleLine line_x, line_y;
            draw_line(spnts_by_station[outer_station_num][station_outer_sp],
                      spnts_by_station[inner_station_num][station_inner_sp],
                      line_x, line_y);

            // Loop over intermediate stations and compare spacepoints with the line
            for ( int station_num = inner_station_num + 1;
                  station_num < outer_station_num; ++station_num ) {
              if (station_num != ignore_station_1 && station_num != ignore_station_2) {

                // A large number so initial value is set as best first
                double delta_sq = 1000000;
                int best_sp = -1;

                // Loop over spacepoints
                for ( int sp_no = 0;
                      sp_no < static_cast<int>(spnts_by_station[station_num].size()); ++sp_no ) {
                  // If the spacepoint has not already been used in a track fit
                  if ( !spnts_by_station[station_num][sp_no]->get_used() ) {
                    SciFiSpacePoint *sp = spnts_by_station[station_num][sp_no];
                    double dx = 0, dy = 0;
                    calc_residual(sp, line_x, line_y, dx, dy);
                    if ( debug > 1 ) {
                      *_f_res << station_num << "\t" << num_points << "\t";
                      *_f_res << dx << "\t" << dy << "\n";
                    }

                    // Apply roadcuts & find the spoints with the smallest residuals for the line
                    if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut )  {
                      if ( debug > 1 ) {
                        *_f_res_good << station_num << "\t" << num_points << "\t";
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
                  SciFiSpacePoint * sp = spnts_by_station[station_num][best_sp];
                  good_spnts.push_back(sp);
                  double dx = 0, dy = 0;
                  calc_residual(sp, line_x, line_y, dx, dy);
                  if ( debug > 1 ) {
                    *_f_res_chosen << station_num << "\t" << num_points << "\t";
                    *_f_res_chosen << dx << "\t" << dy << "\n";
                  }
                }// ~if (counter > 0)
              } // ~if (station_num != ignore_station)
            } // ~Loop over intermediate stations

            // Clear the line objects so we can reuse them
            line_x.clear();
            line_y.clear();

            // Check we have at least 1 good spacepoint in each of the intermediate stations
            // std::cout << "# intermediate stations with good sp: " << good_spnts.size() << "\n";

            if ( static_cast<int>(good_spnts.size()) > (num_points - 3) ) {
              // std::cout << "Found good spacepoints in all stations, fitting a track...\n";

              good_spnts.insert(good_spnts.begin(),
                                spnts_by_station[inner_station_num][station_inner_sp]);
              good_spnts.push_back(spnts_by_station[outer_station_num][station_outer_sp]);

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
              // linear_fit(good_spnts, line_x, line_y);

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
}

void PatternRecognition::linear_fit(const std::vector<double> &_x, const std::vector<double> &_y,
                                    const std::vector<double> &_y_err, SimpleLine &line) {


  // std::cout << "Linear fit info (x,y):" << std::endl;
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
}

void PatternRecognition::make_helix(const int num_points, const std::vector<int> ignore_stations,
                                    std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                                    std::vector<SciFiHelicalPRTrack> &htrks) {

  if ( debug > 0 ) std::cout << "Begining helix fit... " << std::endl;
  // Set inner and outer stations
  int outer_station_num = -1, inner_station_num = -1, middle_station_num = -1;
  set_seed_stations(ignore_stations, outer_station_num, inner_station_num, middle_station_num);

  // Loop over spacepoints in outer station
  for ( int station_outer_sp = 0;
        station_outer_sp < static_cast<int>(spnts_by_station[outer_station_num].size());
        ++station_outer_sp ) {

    // Check the outer spacepoint is unused and enough stations are left with unused sp
    if ( !spnts_by_station[outer_station_num][station_outer_sp]->get_used() &&
         num_stations_with_unused_spnts(spnts_by_station) >= num_points) {

      // Loop over spacepoints in inner station
      for ( int station_inner_sp = 0;
          station_inner_sp < static_cast<int>(spnts_by_station[inner_station_num].size());
          ++station_inner_sp ) {

        // Check the inner spacepoint is unused and enough stations are left with unused sp
        if ( !spnts_by_station[inner_station_num][station_inner_sp]->get_used() &&
             !spnts_by_station[outer_station_num][station_outer_sp]->get_used() &&
             num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

          // Loop over spacepoints in middle station
          for ( int station_middle_sp = 0;
              station_middle_sp < static_cast<int>(spnts_by_station[middle_station_num].size());
              ++station_middle_sp ) {
            // Check the intermediate spacepoints is unused and enough stations are left with
            // unused spacepoints
            if ( !spnts_by_station[middle_station_num][station_middle_sp]->get_used() &&
                 !spnts_by_station[inner_station_num][station_inner_sp]->get_used() &&
                 !spnts_by_station[outer_station_num][station_outer_sp]->get_used() &&
                 num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

              // Vector to hold the good spacepoints in each station
              std::vector<SciFiSpacePoint*> good_spnts;
              // good_spnts.resize(num_points);
              // Set variables to hold which stations are to be ignored
              int ignore_station_1 = -1, ignore_station_2 = -1;
              set_ignore_stations(ignore_stations, ignore_station_1, ignore_station_2);

              SimpleCircle circle;
              // Collect the spacepoints into a vector so that we can pass it to circle_fit
              std::vector<SciFiSpacePoint*> tmp_spnts;
              tmp_spnts.push_back(spnts_by_station[inner_station_num][station_inner_sp]);
              tmp_spnts.push_back(spnts_by_station[middle_station_num][station_middle_sp]);
              tmp_spnts.push_back(spnts_by_station[outer_station_num][station_outer_sp]);

              // Fit a circle in the xy projection to 3 sp from inner, outer, and interm stations
              circle_fit(tmp_spnts, circle);

              // Loop over other intemediate stations so that we can try adding them to the circle
              for ( int station_num = inner_station_num + 1; station_num < outer_station_num;
                    ++station_num ) {
                if (station_num != middle_station_num &&
                    station_num != ignore_station_1 && station_num != ignore_station_2) {

                  double best_from_this_station = 1000000;
                  SciFiSpacePoint* tmp_best_sp = new SciFiSpacePoint();
                  bool dR_passed = false; // initiate to false

                  for ( int sp_no = 0;
                        sp_no < static_cast<int>(spnts_by_station[station_num].size()); ++sp_no ) {

                    // If the spacepoint has not already been used in a track fit
                    if ( !spnts_by_station[station_num][sp_no]->get_used() ) {
                      Hep3Vector pos = spnts_by_station[station_num][sp_no]->get_position();

                      // Calculate the residual dR
                      double dR = delta_R(circle, pos);
                      if ( debug > 1 ) {
                        std::ofstream outdR_all("dR_all.txt", std::ios::out | std::ios::app);
                        outdR_all << dR <<std::endl;
                      }
                      if ( debug > 0 ) std::cout << dR << std::endl;

                      // Apply roadcut to see if spacepoint belongs to same circle
                      if ( fabs(dR) < _R_res_cut && fabs(dR) < fabs(best_from_this_station) ) {
                         if ( debug > 1 ) {
                           std::ofstream outdR_passed_cut("dR_passed_cut.txt",
                                                          std::ios::out | std::ios::app);
                           outdR_passed_cut << dR << std::endl;
                         }
                         best_from_this_station = dR;
                         dR_passed = true;
                         // std::cout <<tmp_best_sp->get_position()<<std::endl;
                         // std::cout << dR <<std::endl;
                         tmp_best_sp = spnts_by_station[station_num][sp_no];
                      } else if ( fabs(dR) > _R_res_cut || fabs(best_from_this_station) < (dR) ) {
                        if ( debug > 0 ) std::cout << "dR test not passed..." <<std::endl;
                        // bool dR_passed = false;
                      }
                    } // ~If intermediate station spacepoint is unused
                  } // ~Loop over intermediate staion spacepoints
                  // Found best sp from this intermediate station:
                  if ( dR_passed )
                    good_spnts.push_back(tmp_best_sp);
                } // ~if (station_num != ignore_station)
              } // ~Loop over intermediate stations not used in initial circle_fit

              if ( good_spnts.size() > 0 && good_spnts[0]->get_station() > middle_station_num+1 ) {
                good_spnts.insert(good_spnts.begin(),
                                  spnts_by_station[middle_station_num][station_middle_sp]);
              } else if ( good_spnts.size() > 0 &&
                          good_spnts[0]->get_station() < middle_station_num+1 ) {
                good_spnts.insert(good_spnts.begin() + 1,
                                  spnts_by_station[middle_station_num][station_middle_sp]);
              } else {
                good_spnts.push_back(spnts_by_station[middle_station_num][station_middle_sp]);
              }

              // Clear the circle object so we can reuse it
              circle.clear();

              // Check we have at least 1 good spacepoint in each of the intermediate stations
              if ( debug > 0 )
                std::cout << "Num of intermediate stats w/ good sp: " << good_spnts.size() << "\n";

              if ( static_cast<int>(good_spnts.size()) >= (num_points - 3) ) {
                if ( debug > 0 ) std::cout << "Found good spnts in all stats, fitting a track...\n";

                good_spnts.insert(good_spnts.begin(),
                                  spnts_by_station[inner_station_num][station_inner_sp]);
                good_spnts.push_back(spnts_by_station[outer_station_num][station_outer_sp]);

                // To see what spacepoints you are using in the fit - for debugging
                if ( debug > 0 ) {
                  std::cout<< "Using spacepoints:  ";
                  for (int i = 0; i < static_cast<int>(good_spnts.size()); ++i) {
                    std::cout<< good_spnts[i]->get_position() << "\t";
                  }
                  std::cout << std::endl;
                }

                // Perform another circle fit now that we have found all of the good spnts
                circle_fit(good_spnts, circle);

             /* int num_points = good_spnts.size();
                std::string num;
                std::stringstream ss;
                ss << num_points;
                num = ss.str();
                std::ofstream out("circle.txt", std::ios::out | std::ios::app);
                for (int t = 0; t < num_points; ++t) {
                  out << good_spnts[t]->get_position().x() << "\t" << good_spnts[t]->get_position().y() << std::endl;
                }
                gROOT->ProcessLine(" .x fitCircle.C(num) "); */
                // Check circle fit passes chisq test
                if ( debug > 1 ) {
                  std::ofstream out_circ("circle_red_chisq.txt", std::ios::out | std::ios::app);
                  out_circ << circle.get_chisq() / (num_points - 2) << std::endl;

                  std::ofstream out_circ2("circle_chisq.txt", std::ios::out | std::ios::app);
                  out_circ2 << circle.get_chisq() << std::endl;
                }

                if ( circle.get_chisq() / ( num_points - 2 ) < _chisq_cut ) {
                  if ( debug > 0 )
                    std::cout << "** chisq circle test passed, moving onto linear fit in s-z **\n";
                  SimpleLine line_sz;
                  std::vector<double> dphi; // to hold change between turning angles
                  double Phi_0; // initial turning angle

                  calculate_dipangle(good_spnts, circle, dphi, line_sz, Phi_0);
                  // Check linear fit passes chisq test, then perform full helix fit

                  if ( debug > 0 ) std::cout << "linesz chisq = " << line_sz.get_chisq() << "\n";

                  if ( line_sz.get_chisq() / ( num_points - 2 ) < _sz_chisq_cut ) {
                    if ( debug > 0 )
                      std::cout << "line in sz chisq test passed, moving on to full helix fit\n";
                    std::vector<double> azm_angles;

                  if ( debug > 1 ) {
                    std::ofstream out_line("szline_red_chisq.txt", std::ios::out | std::ios::app);
                    out_line << line_sz.get_chisq() / (num_points - 2) << std::endl;

                    std::ofstream out_line2("szline_chisq.txt", std::ios::out | std::ios::app);
                    out_line2 << line_sz.get_chisq() << std::endl;
                  }

                    // Calculate turning angles w.r.t. Phi_0
                    azm_angles.push_back(Phi_0);
                    for ( int i = 0; i < static_cast<int>(good_spnts.size()) - 1; ++i ) {
                      azm_angles.push_back(dphi[i] + azm_angles[0]);
                    }

                    if ( debug > 0 ) {
                      for ( int i = 0; i < static_cast<int>(azm_angles.size()); ++i ) {
                        std::cout << "azm_angles_" << i<< " = " << azm_angles[i] << std::endl;
                      }
                    }

                    circle.set_turning_angle(azm_angles); // Turning angles needed in helix fit
                    SimpleHelix helix; // create a helix to hold the helix seed parameters
                    helix.set_Phi_0(Phi_0);
                    double dsdz = line_sz.get_m();
                    if ( debug > 0 ) std::cout << "dsdz = " << dsdz << std:: endl;
                    double tan_lambda = 1/dsdz;

                    double pt = circle.get_R() * 1.2;
                    double pz = pt / dsdz;

                    if ( debug > 1 ) {
                      std::ofstream out1("params_recon.txt", std::ios::out | std::ios::app);
                      out1 << circle.get_R() << "\t" << tan_lambda << "\t";
                      out1 << Phi_0 << "\t" << pt << "\t" << pz << "\t";
                      out1 << 0 << std::endl;
                    }

                    // NOTE this isn't working right now.
                    bool good_helix = full_helix_fit(good_spnts, circle, line_sz, helix);

                    if ( debug > 1 ) {
                      std::ofstream out_helix("helix_red_chisq.txt", std::ios::out | std::ios::app);
                      out_helix << helix.get_chisq() / (num_points - 2) << std::endl;

                      std::ofstream out_helix2("helix_chisq.txt", std::ios::out | std::ios::app);
                      out_helix2 << helix.get_chisq() << std::endl;
                    }

                    if ( good_helix ) {
                      // push helix back into track object once its made
                      if ( debug > 0 )
                        std::cout << "Helix fit found, adding " << num_points << "pt track **\n";

                      CLHEP::Hep3Vector pos_0 = good_spnts[0]->get_position();
                      SciFiHelicalPRTrack track(-1, num_points, pos_0, helix);

                      if ( debug > 1 ) {
                        std::ofstream outblank("sp_per_track.txt", std::ios::out | std::ios::app);
                        outblank << num_points << std::endl;
                      }
                      /* if ( num_points == 4 ) {
                        std::ofstream out4trk("4_sp_per_track.txt", std::ios::out | std::ios::app);
                        // for ( int d = 0; d< good_spnts.size(); ++d) {
                        // out4trk << good_spnts[d]->get_position() << "\t";
                        // }
                        out4trk << helix.get_R() << "\t" << helix.get_tan_lambda();
                        out4trk << helix.get_Phi_0() << "\t" << pt <<"\t"<< pz <<   std::endl;
                      } */
                      /* if ( num_points == 3 ) {
                        std::ofstream out3trk("3_sp_per_track.txt", std::ios::out | std::ios::app);
                        // for ( int d = 0; d< good_spnts.size(); ++d) {
                        // out3trk << good_spnts[d]->get_position() << "\t";
                        // }
                        out3trk << helix.get_R() << "\t" << 1/helix.get_dsdz() <<"\t";
                        out3trk << helix.get_Phi_0() << "\t" << pt <<"\t"<< pz <<   std::endl;
                      } */
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
                    } else { // Debugging **************
                      if ( debug > 0 ) {
                        std::cout << "Helix fit didn't converge for reasonable # of interations.";
                        std::cout << " Track is rejected...." << std::endl;
                      }
                      // std::ofstream out5("rejected_tracks_helix_fit.txt",
                      //                   std::ios::out | std::ios::app);
                      // for (int t = 0; t < static_cast<int>(good_spnts.size()); ++t) {
                      //  out5 << good_spnts[t]->get_position() << "\t";
                      // }  // } // ************************************************Dbugging

                      // out5 <<  circle.get_R() << "\t"<< Phi_0 <<"\t"<< tan_lambda << std::endl;
                    }
                  } else { // Debugging **************
                    if ( debug > 0 ) {
                      std::cout << "sz chisq = " << line_sz.get_chisq();
                      std::cout << "sz chisq test failed, " << num_points << "trk rejected" << "\n";
                    }
                    if ( debug > 1 ) {
                      std::ofstream out4("rejected_tracks_line.txt", std::ios::out | std::ios::app);
                      for (int t = 0; t < static_cast<int>(good_spnts.size()); ++t) {
                        out4 << good_spnts[t]->get_position() << "\t";
                      }
                      double t_L = line_sz.get_m();
                      t_L = 1/t_L;
                      out4 << circle.get_R() << "\t" << t_L << "\t";
                      out4 << line_sz.get_chisq() << std::endl;
                    }
                  } // ~Check s-z line passes chisq test
                } else { // ~ last end bracket was for circle chisq cut test passed
                  if ( debug > 1 ) {
                    std::ofstream out3("rejected_tracks_circle_chisq.txt",
                                        std::ios::out | std::ios::app);
                    for (int t = 0; t < static_cast<int>(good_spnts.size()); ++t)
                      out3 << good_spnts[t]->get_position() << "\t";
                    out3 << circle.get_R() << "\t" << circle.get_chisq() << std::endl;
                  }

                  if ( debug > 0 ) {
                    std::cout << "circle chisq = " << circle.get_chisq();
                    std::cout << "circle chisq test failed, track rejected" << "\n";
                  }
                } // ~circle fail chisq test
              } // ~if enough spacepoints are found for fit (must be at least 3)
            } // ~if middle station sp unused
          } // ~Loop over middle station spacepoints
        } // ~If inner station spacepoint is unused
      } // ~Loop over inner station spacepoints
    } // ~if outer station spacepoint is unused
  } // ~Loop over outer station spacepoints
} // ~make_helix(...)

void PatternRecognition::circle_fit(const std::vector<SciFiSpacePoint*> &spnts,
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

  /* double a, b, x_c, y_c, R;
  a = tmp_params[0][0]; // X - x_c
  b = tmp_params[1][0]; // Y - y_c

  R = sqrt((a*a) + (b*b));
  x_c = a - X;
  y_c = b - Y;

  circle.set_R(R);
  circle.set_x0(x_c);
  circle.set_y0(y_c);  */
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
}

double PatternRecognition::delta_R(const SimpleCircle &circle, const CLHEP::Hep3Vector &pos) {

  double x0 = circle.get_x0();
  double y0 = circle.get_y0();
  double R = circle.get_R();
  //  std::cout << "x0 =  " << x0 << std::endl;
  //  std::cout << "y0 =  " << y0 << std::endl;
  //  std::cout << "R =  " << R << std::endl;

  double R_i = sqrt((pos.x() - x0)*(pos.x() - x0) + (pos.y() - y0)*(pos.y() - y0));
  double delta_R = R - R_i;

  return delta_R;
}

void PatternRecognition::calculate_dipangle(const std::vector<SciFiSpacePoint*> &spnts,
                                            const SimpleCircle &circle, std::vector<double> &dphi,
                                            SimpleLine &line_sz, double &Phi_0) {

  if ( debug > 0 ) {
    std::cout << "R = "<< circle.get_R() << std::endl;
    std::cout << "x_c = " << circle.get_x0() << std::endl;
    std::cout << "y_c = " << circle.get_y0() << std::endl;
  }

  double R = circle.get_R();

  std::vector<double> dz;
  std::vector<double> dphi_err;

  double x0 = spnts[0]->get_position().x();
  double y0 = spnts[0]->get_position().y();
  double z0 = spnts[0]->get_position().z();

  Phi_0 = calculate_Phi(x0, y0, circle);
  // For the linear fit in s-z, we care about the change in z vs change in s
  // So here we calculate the values dz_ji and dphi_ji (because ds_ji = R*dphi_ji)
  for ( int i = 1; i < static_cast<int>(spnts.size()); ++i ) {

    double z_i = spnts[i]->get_position().z();
    double dz_ji = z_i - z0;
    dz.push_back(dz_ji);

    double x_i = spnts[i]->get_position().x();
    double y_i = spnts[i]->get_position().y();
    double phi_i = calculate_Phi(x_i, y_i, circle);

    double dphi_ji = phi_i - Phi_0;
    dphi.push_back(dphi_ji);

    double sd_phi = -1.0;
    /*
      if ( (spnts[i]->get_station() == 5) || (spnts[j]->get_station() == 5) )
        sd_phi = _sd_phi_5;
      else
        sd_phi = _sd_phi_1to4;
    */
      dphi_err.push_back(sd_phi);
  }

    bool ok = turns_between_stations(dz, dphi);

    if ( ok ) {
      std::vector<double> ds;
      // Multiply each element of dphi by R so that each element dphi_ji goes to ds_ji
      dphi_to_ds(R, dphi, ds);
      if ( debug > 0 ) {
        for ( int i = 0; i < ds.size(); i++ ) {
          std::cout << "ds = " << ds[i] << std::endl;
          std::cout << "dz = " << dz[i] << std::endl;
        }
      }
      linear_fit(dz, ds, dphi_err, line_sz); // Need to change dphi_err to ds_err.
    }
}

double PatternRecognition::calculate_Phi(double xpos, double ypos, const SimpleCircle &circle) {

  /* double alpha = circle.get_alpha();
  double beta = circle.get_beta();
  double gamma = circle.get_gamma(); */
  double x_c = circle.get_x0();
  double y_c = circle.get_y0();

  double angle = atan2(ypos - y_c, xpos - x_c);

  if ( angle < 0. )
    angle += 2. * pi;
  if ( debug > 0 ) std::cout << "Phi = " << angle << " (This is just from circle fit)\n";
  return angle;
}

bool PatternRecognition::turns_between_stations(const std::vector<double> &dz,
                                                std::vector<double> &dphi) {
  // Make sure that you have enough points to make a line (2)
  if ( dz.size() < 2 || dphi.size() < 2 )
    return false;
  for ( int n = 0; n < 2; ++n ) {
    for ( int i = 0; i < static_cast<int>(dphi.size()) - 1; ++i ) {
      if ( dphi[i] < 0 )
        dphi[i] += 2. * pi;

      int j = i + 1;
      if ( dphi[j] < dphi[i] )
        dphi[j] += 2. * pi;

      double z_ratio = dz[j] / dz[i];
      double phi_ratio = dphi[j] / dphi[i];

      if ( debug > 0 )
        std::cout << "RATIOS.... " << fabs(phi_ratio - z_ratio) /  z_ratio << std::endl;

      if ( fabs(phi_ratio - z_ratio) /  z_ratio > _AB_cut ) {
        // try
        bool passed_cut = AB_ratio(dphi[i], dphi[j], dz[i], dz[j]);
        if ( !passed_cut )
          return false;
      }
    }
    return true;
/*
  if ( dphi[0] < 0 )
    dphi[0] += 2 * pi;

  //  Make sure that dphi is always increasing between stations
  for ( int i = 0; i < static_cast<int>(dphi.size()); i++ ) {
    while ( dphi[i] > dphi[i+1] ) {
      dphi[i+1] += 2 * pi;
    }
  }
  for ( int i = 0; i < dphi.size(); i++ )
    std::cout << "dphi[i] = " << dphi[i] << std::endl;

  bool good_AB = 1; // set to true to begin loop
  int counter;
  for ( int i = static_cast<int>(dphi.size())-1; i > 0 && good_AB; i-- ) { // note: counts down!!
    int j = i-1;
    good_AB = AB_ratio(dphi[j] , dphi[i], dz[j], dz[i]);
    ++counter;
    // if good_AB is returned as false at any point, this loop will stop iterating.
  }
  //  Again make sure that dphi is always increasing between stations **need something better here!
  for ( int i = 0; i < static_cast<int>(dphi.size()); i++ ) {
    while ( dphi[i] > dphi[i+1] ) {
      dphi[i+1] += 2 * pi;
    }
  }

  // i = dphi.size() if AB_ratio was returned true for each station interval
  if ( counter+1 == static_cast<int>(dphi.size()) )
    return true;
  else
    return false; // if i < dphi.size(), then the iterations were stopped after some AB_ratio was
                  // returned false.

   */
  }
}

bool PatternRecognition::AB_ratio(double &dphi_ji, double &dphi_kj, double dz_ji,
                                  double dz_kj) {
  // double A, B;

  for ( int n = 0; n < 10; ++n ) // {
    for ( int m = 0; m < n+1; ++m ) { // m always less than or equal to n
      double A, B;
      A = ( dphi_kj + ( 2 * n * pi ) ) / ( dphi_ji + ( 2 * m * pi ) ); // phi_ratio
      B = dz_kj / dz_ji; // z_ratio
      if ( debug > 0 ) std::cout <<  " A - B  = " << fabs(A - B)/B << std::endl;
      if ( fabs(A - B) / B < _AB_cut ) {
        dphi_kj += 2 * n * pi;
        dphi_ji += 2 * m * pi;

        return true;
      }
    } // end m loop
  // } // end n loop
  return false; // Return false if _ABcut is never satisfied
}

void PatternRecognition::dphi_to_ds(double R, const std::vector<double> &dphi,
                                    std::vector<double> &ds) {
  // This function performs scaler multiplication on the input vector.
  for ( int i = 0; i < static_cast<int>(dphi.size()); ++i ) {
    double ds_ji = dphi[i] * R;
    ds.push_back(ds_ji);
  }
}

bool PatternRecognition::full_helix_fit(const std::vector<SciFiSpacePoint*> &spnts,
                                        const SimpleCircle &circle, const SimpleLine &line_sz,
                                        SimpleHelix &helix) {
  // If the counter reaches 10 before an acceptable chisq value is reached, then we reject the track
  // This is because we've already done the

  // Initial parameters
  double R = circle.get_R();
  double Phi_0 = helix.get_Phi_0();

  double dsdz = line_sz.get_m();

  double tan_lambda = 1 / dsdz;

  if ( debug > 0 ) std::cout << "dsdz = " << dsdz << std::endl;

  if ( debug > 0 ) std::cout << "tan_lambda = " << tan_lambda << std::endl;
  CLHEP::Hep3Vector starting_point = spnts[0]->get_position();

  // Calculate chisq with initial params
  double best_chisq = calculate_chisq(spnts, circle.get_turning_angle(), Phi_0, tan_lambda, R);
  // double best_chi2dof = best_chisq / static_cast<int>(spnts.size());

  if ( debug > 0 ) std::cout<< "initial best chisq = " << best_chisq << std::endl;

  // double R_prime, Phi_0_prime, tan_lambda_prime, dsdz_prime = 0.;

  if ( best_chisq/ (static_cast<int>(spnts.size()) - 2) < _helix_chisq_cut ) {
    if ( debug > 0 ) {
      std::cout << "best_chisq = " << best_chisq << std::endl;
      std::cout << "yay, finished" << std::endl;
    }
    helix.set_R(R);
    helix.set_Phi_0(Phi_0);
    helix.set_dsdz(dsdz);
    helix.set_tan_lambda(tan_lambda);
    helix.set_chisq_dof(best_chisq / static_cast<int>(spnts.size()));
    helix.set_chisq(best_chisq);
    return true;
  }

  // Declare adjustments to parameters and chisq that will be calculated after adjuments are made
  for ( int i = 0; i < 10; ++i ) {
    double dR, dPhi_0, dtan_lambda = 0.; // Adjustments reset to zero with every iteration
    calculate_adjustments(spnts, circle.get_turning_angle(), R, Phi_0, tan_lambda, dR, dPhi_0,
                          dtan_lambda);

    R -= dR;
    Phi_0 -= dPhi_0;
    tan_lambda -= dtan_lambda;

    dsdz = 1/tan_lambda;
    double chisq = calculate_chisq(spnts, circle.get_turning_angle(), Phi_0, tan_lambda, R);

    if ( fabs(best_chisq - chisq) < _chisq_diff ||
         best_chisq/ (static_cast<int>(spnts.size()) - 2) < _helix_chisq_cut ) {
      if ( debug > 0 ) std::cout << "yay, finished" << std::endl;
      best_chisq = chisq;
      helix.set_R(R);
      helix.set_Phi_0(Phi_0);
      helix.set_dsdz(dsdz);
      helix.set_tan_lambda(tan_lambda);
      helix.set_chisq_dof(best_chisq / (static_cast<int>(spnts.size()) -2 ));
      helix.set_chisq(best_chisq);
      return true;
      // returns the helix too
    }

    if ( chisq > best_chisq && fabs(best_chisq - chisq) > _chisq_diff ) {
      if ( debug > 0 ) std::cout << "** chisq > best_chisq.... and chisq = "<< chisq << "\n";

      // If the new chi2 you calculate is larger than previous, then the minimum has been passed

      while (chisq > best_chisq && chisq/(static_cast<int>(spnts.size()) - 2) > _helix_chisq_cut) {
        dR /= 2;
        dPhi_0 /= 2;
        dtan_lambda /= 2;

        R += dR;
        Phi_0 += dPhi_0;
        tan_lambda += dtan_lambda;

        dsdz = 1/ tan_lambda;
        ++i; // This counts as an iteration.
        chisq = calculate_chisq(spnts, circle.get_turning_angle(), Phi_0, tan_lambda, R);
        // double chisq_dof = chisq / static_cast<int>(spnts.size());
        if ( debug > 0 ) std::cout << "new chisq = " << chisq << std::endl;

        if ( fabs(best_chisq - chisq) < _chisq_diff ||
          best_chisq/ (static_cast<int>(spnts.size()) - 2) < _helix_chisq_cut ) {
          if ( debug > 0 ) std::cout << "yay, finished" << std::endl;
          best_chisq = chisq;
          helix.set_R(R);
          helix.set_Phi_0(Phi_0);
          helix.set_dsdz(dsdz);
          helix.set_tan_lambda(tan_lambda);
          helix.set_chisq_dof(best_chisq / (static_cast<int>(spnts.size()) -2 ));
          helix.set_chisq(best_chisq);

          return true;
          // returns the helix too
        }

        if ( i == 10 || fabs(chisq - best_chisq) < _chisq_diff)
          break;
      }
    } // ~ chisq > best_chisq && fabs(chisq - best_chisq) > 1

      if ( debug > 0 ) {
        std::cout << "chisq = " << chisq << std::endl;
        std::cout << "best_chisq = " << best_chisq << std::endl;
        std::cout << "chisq difference = " << fabs(chisq - best_chisq) << std::endl;
      }
      if ( fabs(best_chisq - chisq) < _chisq_diff ||
           chisq/ (static_cast<int>(spnts.size()) - 2) < _helix_chisq_cut ) {
        if ( debug > 0 ) std::cout << "yay, finished" << std::endl;
        best_chisq = chisq;
        helix.set_R(R);
        helix.set_Phi_0(Phi_0);
        helix.set_dsdz(dsdz);
        helix.set_tan_lambda(tan_lambda);
        helix.set_chisq_dof(best_chisq / (static_cast<int>(spnts.size()) -2 ));
        helix.set_chisq(best_chisq);
        return true;
        // returns the helix too
      } else if ( chisq < best_chisq && fabs(best_chisq - chisq) > _chisq_diff ) {
        best_chisq = chisq; // If we reduced chisq but still not at miniumum
      } // i will iterate.
    } // ~i
  // Return false if the loop above hasn't returned true, on in other words, i reached 10 before
  // we reached a good chisq convergence then we return false.
  return false;
}

double PatternRecognition::calculate_chisq(const std::vector<SciFiSpacePoint*> &spnts,
                                           const std::vector<double> &turning_angles, double Phi_0,
                                           double tan_lambda, double R) {
  double chi2 = 0;
  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    CLHEP::Hep3Vector p = spnts[i]->get_position();
    double phi_i;
    // if ( i == 0 )
    //  phi_i = Phi_0;
    // else
      phi_i = turning_angles[i];

    phi_i -= Phi_0;
    double A, B, C;
    A = spnts[0]->get_position().x();
    B = spnts[0]->get_position().y();
    C = spnts[0]->get_position().z();

    double xi, yi, zi;

    helix_function_at_i(R, Phi_0, tan_lambda, A, B, C, phi_i, xi, yi, zi);
    if ( debug > 0 ) {
      std::cout << "Making sure helix is reconstructing properly..." << std::endl;
      std::cout << "x_recon = " << xi<< " should equal   x_sp = " << p.x() << std::endl;
      std::cout << "y_recon = " << yi<< " should equal   y_sp = " << p.y() << std::endl;
      std::cout << "z_recon = " << zi<< " should equal   z_sp = " << p.z() << std::endl;
    }

    double sd = -1;
    if ( spnts[i]->get_station() == 5 )
      sd = _sd_5;
    else
      sd = _sd_1to4;

    double w = 1 / (sd * sd);

    chi2 += w*((p.x()-xi)*(p.x()-xi)) + w*((p.y()-yi)*(p.y()-yi)) + ((p.z()-zi)*(p.z()-zi));
  }
  return chi2;
}

void PatternRecognition::helix_function_at_i(double R, double phi_0, double tan_lambda,
                                             double A, double B, double C, double phi_i,
                                             double &xi, double &yi, double &zi) {
    xi = A + R * (cos(phi_i) - 1) * cos(phi_0) - R * sin(phi_i) * sin(phi_0);
    yi = B + R * (cos(phi_i) - 1) * sin(phi_0) + R * sin(phi_i) * cos(phi_0);
    zi = C + (R * phi_i * tan_lambda);
}

void PatternRecognition::calculate_adjustments(const std::vector<SciFiSpacePoint*> &spnts,
                                               const std::vector<double> &turning_angles, double R,
                                               double phi_0, double tan_lambda, double &dR,
                                               double &dphi_0, double &dtan_lambda) {
  CLHEP::HepMatrix G(3, 3); // symmetric matrix containing second derivatives w.r.t. each parameter
  CLHEP::HepMatrix g(3, 1); // vector containing first derivatives w.r.t. each parameter

  dR = 0.;
  dphi_0 = 0;
  dtan_lambda = 0;

  double A, B, C;
  A = spnts[0]->get_position().x();
  B = spnts[0]->get_position().y();
  C = spnts[0]->get_position().z();

  // construct the individual matrix elements as 0 to begin with.
  double G_rr = 0., G_bb = 0., G_ll = 0., G_rb = 0., G_br = 0., G_rl = 0., G_lr = 0.;
  double G_bl = 0., G_lb = 0.; // These will remain 0.

  double g_r = 0., g_b = 0., g_l = 0.;

  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    CLHEP::Hep3Vector p = spnts[i]->get_position();
    double phi_i;
    // if ( i == 0 )
    //  phi_i = phi_0;
    // else
      phi_i = turning_angles[i];

    phi_i -= phi_0; // Everything relative to starting point.

    // Get errors on x and y measurements (equal).  Note error on z negligible.
    double sd = -1;
    if ( spnts[i]->get_station() == 5 )
      sd = _sd_5;
    else
      sd = _sd_1to4;

    double w = 1 / (sd * sd);

    double xi = 0., yi = 0., zi = 0;
    helix_function_at_i(R, phi_0, tan_lambda, A, B, C, phi_i, xi, yi, zi);

    // Calculate the elements of the gradient vector g
    g_r += w * (((p.x() - xi)*(xi - A)) + ((p.y() - yi)*(yi - B))) + ((p.z() - zi)*(zi - C));
    g_b += w * (-1*((p.x() - xi)*(yi - B)) + ((p.y() - yi)*(xi - A)));
    g_l += (p.z() - zi) * phi_i;

    // Calculate the elements of the matrix G
    G_rr += (w*((xi - A)*(xi - A) + (yi - B)*(yi - B))) + (zi - C)*(zi - C);
    G_rb += w *(((p.x() - xi)*(yi - B)) - ((p.y() - yi)*(xi - A)));
    G_rl += ((zi - C) - (p.z() - zi)) * phi_i;
    G_bb += w * ((p.x() - A) * (xi - A) + (p.y() - B) * (yi - B));
    G_ll += phi_i * phi_i;
  } // end i

  g_r = (-2 / R) * g_r;
  g_b = -2 * g_b;
  g_l = -2 * R * g_l;

  G_rr = (2 / (R * R)) * G_rr;
  G_rb = (2 / R) * G_rb;
  G_br = G_rb;
  G_rl = 2 * G_rl;
  G_lr = G_rl;
  G_bb = 2 * G_bb;
  G_ll = 2 * R * R * G_ll;

  // Now we fill the matrix and gradient vector.
  G[0][0] = G_rr;
  G[0][1] = G_rb;
  G[0][2] = G_rl;

  G[1][0] = G_br;
  G[1][1] = G_bb;
  G[1][2] = G_bl; // Should be 0

  G[2][0] = G_lr;
  G[2][1] = G_lb; // Should be 0
  G[2][2] = G_ll;

  // Column vector
  g[0][0] = g_r;
  g[1][0] = g_b;
  g[2][0] = g_l;

  // Now we calculate the corrections to the parameters
  int ierr;
  G.invert(ierr);
  CLHEP::HepMatrix d_params = G * g; // (3x3)(3x1) = (3x1) vector of corrections

  if ( debug > 0 ) std::cout<< d_params << std::endl;

  dR = d_params[0][0];
  dphi_0 = d_params[1][0];
  dtan_lambda = d_params[2][0];

  // Calculate errors here for now, but I don't think we need them unless its just for book-keeping
  double R_err = 2*G[0][0];
  double Phi_0_err = 2*G[1][1];
  double tan_lambda_err = 2*G[2][2];
}

void PatternRecognition::set_end_stations(const std::vector<int> ignore_stations,
                      int &outer_station_num, int &inner_station_num) {
  // For linear Pattern Recognition Use
  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    outer_station_num = 4;
    inner_station_num = 0;
  } else if ( static_cast<int>(ignore_stations.size()) == 1 ) { // 4pt track case
      // Set outer station number
      if ( ignore_stations[0] != 4 )
        outer_station_num = 4;
      else
        outer_station_num = 3;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_station_num = 0;
      else
        inner_station_num = 1;
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      // std::cout << "stations " << ignore_stations[0] << " and " << ignore_stations[1] << "\n";
      // Set outer station number
      if ( ignore_stations[1] != 4 )
        outer_station_num = 4;
      else if ( ignore_stations[0] != 3 )
        outer_station_num = 3;
      else
        outer_station_num = 2;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_station_num = 0;
      else if ( ignore_stations[1] != 1 )
        inner_station_num = 1;
      else
        inner_station_num = 2;
  } else {
    if (debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
}

void PatternRecognition::set_seed_stations(const std::vector<int> ignore_stations,
                      int &outer_station_num, int &inner_station_num, int &middle_station_num) {
  // For helical Pattern Recognition Use
  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    outer_station_num = 4;
    inner_station_num = 0;
    middle_station_num = 2;
  } else if ( static_cast<int>(ignore_stations.size()) == 1 ) { // 4pt track case
      // Set outer station number
      if ( ignore_stations[0] != 4 )
        outer_station_num = 4;
      else
        outer_station_num = 3;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_station_num = 0;
      else
        inner_station_num = 1;
       // Set middle station number
       if ( ignore_stations[0] != 2 )
         middle_station_num = 2;
       else
         middle_station_num = 1;
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      // Set outer station number
      if ( ignore_stations[1] != 4 )
        outer_station_num = 4;
      else if ( ignore_stations[0] != 3 )
        outer_station_num = 3;
      else
        outer_station_num = 2;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_station_num = 0;
      else if ( ignore_stations[1] != 1 )
        inner_station_num = 1;
      else
        inner_station_num = 2;
      // Set middle station number
      if ( ignore_stations[0] != 0 && ignore_stations[1] != 1 )
        middle_station_num = 1;
      else if ( ignore_stations[0] != 0 && ignore_stations[1] != 2 )
        middle_station_num = 2;
      else if ( ignore_stations[0] != 0 && ignore_stations[1] != 3 )
        middle_station_num = 3;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 2 )
        middle_station_num = 2;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 3 )
        middle_station_num = 3;
      else // case where   ignore_stations[0] != 2 && ignore_stations[1] != 3
        middle_station_num = 3;
    } else {
    if (debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
}

void PatternRecognition::sort_by_station(const std::vector<SciFiSpacePoint*> &spnts,
                                 std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station) {
  for ( int station_num = 0;
        station_num < static_cast<int>(spnts_by_station.size()); ++station_num ) {
    for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
      if ( spnts[i]->get_station() == station_num + 1 ) {
        spnts_by_station[station_num].push_back(spnts[i]);
      }
    }
  }
}

int PatternRecognition::num_stations_with_unused_spnts(
    const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station) {

  int stations_hit = 0;

  // std::cout << "Unused spacepoints per station:\t";

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
    // std::cout << unused_sp << "  ";
  }
  // std::cout << std::endl;
  return stations_hit;
}

void PatternRecognition::stations_with_unused_spnts(
                         const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<int> &stations_hit, std::vector<int> &stations_not_hit) {
  stations_hit.clear();
  stations_not_hit.clear();
  // std::cout << "Unused spacepoints per station:\t";

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
    // std::cout << unused_sp << "  ";
  }
  // std::cout << std::endl;
}

void PatternRecognition::set_ignore_stations(const std::vector<int> &ignore_stations,
                         int &ignore_station_1, int &ignore_station_2) {
  if (debug > 0) std::cout << "ignore_stations.size() = " << ignore_stations.size() << std::endl;
  ignore_station_1 = -1, ignore_station_2 = -1;
  if ( ignore_stations.size() == 0 ) {
    // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_station_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_station_1 = ignore_stations[0];
    ignore_station_2 = ignore_stations[1];
  } else if ( ignore_stations.size() > 2 ) {
    if (debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
}

void PatternRecognition::draw_line(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2,
                                   SimpleLine &line_x, SimpleLine &line_y) {

  Hep3Vector pos_outer = sp1->get_position();
  Hep3Vector pos_inner = sp2->get_position();

  line_x.set_m(( pos_inner.x() - pos_outer.x()) / (pos_inner.z() - pos_outer.z() ));
  line_x.set_c(pos_outer.x() - ( pos_outer.z() * line_x.get_m()) );

  line_y.set_m(( pos_inner.y() - pos_outer.y()) / (pos_inner.z() - pos_outer.z() ));
  line_y.set_c(pos_outer.y() - ( pos_outer.z() *  line_y.get_m() ));

  // std::cout << "m_xi = " << line_x.get_m()  << "\tx_0i = " << line_x.get_c() << std::endl;
  // std::cout << "m_yi = " << line_y.get_m() << "\ty_0i = " << line_y.get_c() << std::endl;
}

void PatternRecognition::calc_residual(const SciFiSpacePoint *sp,
                                       const SimpleLine &line_x, const SimpleLine &line_y,
                                       double &dx, double &dy) {
    Hep3Vector pos = sp->get_position();
    dx = pos.x() - ( line_x.get_c() + ( pos.z() * line_x.get_m() ) );
    dy = pos.y() - ( line_y.get_c() + ( pos.z() * line_y.get_m() ) );
}

// } // ~namespace MAUS
