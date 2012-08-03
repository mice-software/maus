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
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Units/PhysicalConstants.h>

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
  int outer_st_num = -1, inner_st_num = -1;
  set_end_stations(ignore_stations, outer_st_num, inner_st_num);

  if (spnts_by_station.size() == _n_stations && outer_st_num > -1 && outer_st_num < _n_stations
      && inner_st_num > -1 && inner_st_num < _n_stations) {

    // Loop over spacepoints in outer station
    for ( unsigned int st_outer_sp = 0; st_outer_sp < spnts_by_station[outer_st_num].size();
          ++st_outer_sp ) {

      // Check the outer spacepoint is unused and enough stations are left with unused sp
      if ( !spnts_by_station[outer_st_num][st_outer_sp]->get_used() &&
           num_stations_with_unused_spnts(spnts_by_station) >= num_points) {

        // Loop over spacepoints in inner station
        for ( unsigned int st_inner_sp = 0;
            st_inner_sp < spnts_by_station[inner_st_num].size(); ++st_inner_sp ) {

          // Check the inner spacepoint is unused and enough stations are left with unused sp
          if ( !spnts_by_station[inner_st_num][st_inner_sp]->get_used() &&
              num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

            // Vector to hold the good spacepoints in each station
            std::vector<SciFiSpacePoint*> good_spnts;
            // good_spnts.resize(num_points);

            // Set variables to hold which stations are to be ignored
            int ignore_st_1 = -1, ignore_st_2 = -1;
            set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

            // Draw a straight line between spacepoints in outer most and inner most stations
            SimpleLine line_x, line_y;
            draw_line(spnts_by_station[outer_st_num][st_outer_sp],
                      spnts_by_station[inner_st_num][st_inner_sp], line_x, line_y);

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

              good_spnts.insert(good_spnts.begin(), spnts_by_station[inner_st_num][st_inner_sp]);
              good_spnts.push_back(spnts_by_station[outer_st_num][st_outer_sp]);
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
                                    std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                                    std::vector<SciFiHelicalPRTrack> &htrks) {

  if ( debug > 0 ) std::cout << "Begining helix fit... " << std::endl;

  // Set inner and outer stations
  int outer_st_num = -1, inner_st_num = -1, middle_st_num = -1;
  set_seed_stations(ignore_stations, outer_st_num, inner_st_num, middle_st_num);

  // Loop over spacepoints in outer station
  for ( unsigned int st_outer_sp = 0;
       st_outer_sp <spnts_by_station[outer_st_num].size(); ++st_outer_sp ) {

    // Check the outer spacepoint is unused and enough stations are left with unused sp
    if ( !spnts_by_station[outer_st_num][st_outer_sp]->get_used() &&
         num_stations_with_unused_spnts(spnts_by_station) >= num_points) {

      // Loop over spacepoints in inner station
      for ( int st_inner_sp = 0;
          st_inner_sp < static_cast<int>(spnts_by_station[inner_st_num].size()); ++st_inner_sp ) {

        // Check the inner spacepoint is unused and enough stations are left with unused sp
        if ( !spnts_by_station[inner_st_num][st_inner_sp]->get_used() &&
             !spnts_by_station[outer_st_num][st_outer_sp]->get_used() &&
             num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

          // Loop over spacepoints in middle station
          for ( unsigned int st_middle_sp = 0;
                st_middle_sp < spnts_by_station[middle_st_num].size(); ++st_middle_sp ) {

            // Check intermediate spnts are unused and enough stations are left with unused spnts
            if ( !spnts_by_station[middle_st_num][st_middle_sp]->get_used() &&
                 !spnts_by_station[inner_st_num][st_inner_sp]->get_used() &&
                 !spnts_by_station[outer_st_num][st_outer_sp]->get_used() &&
                 num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

              // Vector to hold the good spacepoints in each station
              std::vector<SciFiSpacePoint*> good_spnts;

              // Set variables to hold which stations are to be ignored
              int ignore_st_1 = -1, ignore_st_2 = -1;
              set_ignore_stations(ignore_stations, ignore_st_1, ignore_st_2);

              // Collect the spacepoints into a vector so that we can pass it to circle_fit
              SimpleCircle circle;
              std::vector<SciFiSpacePoint*> tmp_spnts;
              tmp_spnts.push_back(spnts_by_station[inner_st_num][st_inner_sp]);
              tmp_spnts.push_back(spnts_by_station[middle_st_num][st_middle_sp]);
              tmp_spnts.push_back(spnts_by_station[outer_st_num][st_outer_sp]);

              // Fit a circle in the xy projection to 3 sp from inner, outer, and interm stations
              circle_fit(tmp_spnts, circle);

              // Loop over other intemediate stations so that we can try adding them to the circle
              for ( int st_num = inner_st_num + 1; st_num < outer_st_num; ++st_num ) {
                if (st_num != middle_st_num && st_num != ignore_st_1 && st_num != ignore_st_2) {

                  double best_from_this_station = 1000000;
                  SciFiSpacePoint* tmp_best_sp = new SciFiSpacePoint();
                  bool dR_passed = false;

                  for (unsigned int sp_no = 0; sp_no < spnts_by_station[st_num].size(); ++sp_no) {
                    // If the spacepoint has not already been used in a track fit
                    if ( !spnts_by_station[st_num][sp_no]->get_used() ) {

                      CLHEP::Hep3Vector pos = spnts_by_station[st_num][sp_no]->get_position();
                      double dR = delta_R(circle, pos); // Calculate the residual dR

                      if ( debug > 1 ) {
                        std::ofstream outdR_all("dR_all.txt", std::ios::out |
                                                        std::ios::app);
                        outdR_all << dR << "\t";
                        outdR_all << spnts_by_station[st_num][sp_no]->get_tracker() << "\n";
                      }
                      if ( debug > 0 ) std::cout << dR << std::endl;

                      // Apply roadcut to see if spacepoint belongs to same circle
                      if ( fabs(dR) < _R_res_cut && fabs(dR) < fabs(best_from_this_station) ) {
                         if ( debug > 1 ) {
                           std::ofstream outdR_passed_cut("dR_passed_cut.txt", std::ios::out |
                                                          std::ios::app);
                           outdR_passed_cut << dR <<"\t";
                           outdR_passed_cut << spnts_by_station[st_num][sp_no]->get_tracker();
                           outdR_passed_cut << std::endl;
                         }

                         best_from_this_station = dR;
                         dR_passed = true;
                         // std::cout <<tmp_best_sp->get_position()<<std::endl;
                         // std::cout << dR <<std::endl;
                         tmp_best_sp = spnts_by_station[st_num][sp_no];
                      } else if ( fabs(dR) > _R_res_cut || fabs(best_from_this_station) < (dR) ) {
                        if ( debug > 0 ) std::cout << "dR test not passed..." <<std::endl;
                        // bool dR_passed = false;
                      }
                    } // ~If intermediate station spacepoint is unused
                  } // ~Loop over intermediate staion spacepoints
                  // Found best sp from this intermediate station:
                  if ( dR_passed )
                    good_spnts.push_back(tmp_best_sp);
                } // ~if (st_num != ignore_station)
              } // ~Loop over intermediate stations not used in initial circle_fit

              if ( good_spnts.size() > 0 && good_spnts[0]->get_station() > middle_st_num+1 ) {
                good_spnts.insert(good_spnts.begin(),
                                  spnts_by_station[middle_st_num][st_middle_sp]);
              } else if ( good_spnts.size() > 0 &&
                          good_spnts[0]->get_station() < middle_st_num+1 ) {
                good_spnts.insert(good_spnts.begin() + 1,
                                  spnts_by_station[middle_st_num][st_middle_sp]);
              } else {
                good_spnts.push_back(spnts_by_station[middle_st_num][st_middle_sp]);
              }

              // Clear the circle object so we can reuse it
              circle.clear();

              // Check we have at least 1 good spacepoint in each of the intermediate stations
              if ( debug > 0 )
                std::cout << "Num of intermediate stats w/ good sp: " << good_spnts.size() << "\n";

              if ( static_cast<int>(good_spnts.size()) >= (num_points - 3) ) {
                if ( debug > 0 ) std::cout << "Found good spnts in all stats, fitting a track...\n";

                good_spnts.insert(good_spnts.begin(),
                                  spnts_by_station[inner_st_num][st_inner_sp]);
                good_spnts.push_back(spnts_by_station[outer_st_num][st_outer_sp]);

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

                if ( debug > 1 ) {
                  std::ofstream out_circ("circle_red_chisq.txt", std::ios::out | std::ios::app);
                  out_circ << circle.get_chisq()/(num_points-2) << "\t";
                  out_circ << good_spnts[0]->get_tracker() << std::endl;

                  std::ofstream out_circ2("circle_chisq.txt", std::ios::out | std::ios::app);
                  out_circ2 << circle.get_chisq() << "\t"<< good_spnts[0]->get_tracker() << "\n";
                }


                if ( circle.get_chisq() / ( num_points - 2 ) < _chisq_cut ) {
                  if ( debug > 0 )
                    std::cout << "** chisq circle test passed, moving onto linear fit in s-z **\n";
                  SimpleLine line_sz;
                  std::vector<double> dphi; // to hold change between turning angles
                  double Phi_0; // initial turning angle

                  calculate_dipangle(good_spnts, circle, dphi, line_sz, Phi_0);

                  // Check linear fit passes chisq test, then perform full helix fit
                  if ( debug > 0 )
                    std::cout << "linesz chisq = " << line_sz.get_chisq() << "\n";
                  if ( line_sz.get_chisq() / ( num_points - 2 ) < _sz_chisq_cut ) {
                    if ( debug > 0 )
                      std::cout << "line in sz chisq test passed, moving on to full helix fit\n";

                    std::vector<double> azm_angles;

                    if ( debug > 1 ) {
                      std::ofstream out_line("szline_red_chisq.txt", std::ios::out | std::ios::app);
                      out_line << line_sz.get_chisq() / (num_points - 2) << "\t";
                      out_line << good_spnts[0]->get_tracker()<< std::endl;

                      std::ofstream out_line2("szline_chisq.txt", std::ios::out | std::ios::app);
                      out_line2 << line_sz.get_chisq()<< "\t";
                      out_line2 << good_spnts[0]->get_tracker() << std::endl;
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
                    double tan_lambda = 1/dsdz;
                    double pt = circle.get_R() * 1.2;
                    double pz = pt / dsdz;
                    if ( debug > 0 ) std::cout << "dsdz = " << dsdz << std:: endl;

                    if ( debug > 1 ) {
                      std::ofstream out1("params_recon.txt", std::ios::out | std::ios::app);
                      out1 << circle.get_R() << "\t" << tan_lambda << "\t";
                      out1 << Phi_0 << "\t" << pt << "\t" << pz << "\t";
                      out1 << 0  <<"\t"<< good_spnts[0]->get_tracker() << std::endl;
                    }

                    if ( !_use_full_helix_fit ) {
                      helix.set_dsdz(line_sz.get_m());
                      helix.set_R(circle.get_R());
                      helix.set_tan_lambda(1 / line_sz.get_m());
                      CLHEP::Hep3Vector pos_0 = good_spnts[0]->get_position();

                      SciFiHelicalPRTrack track(-1, num_points, pos_0, helix);

                      if ( debug > 1 ) {
                        std::ofstream outblank("sp_per_track.txt", std::ios::out | std::ios::app);
                        outblank << num_points << "\t" << good_spnts[0]->get_tracker() << std::endl;
                      }

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
  // std::cout << "alpha = " << alpha << std::endl;
  // std::cout << "beta = " << beta << std::endl;
  // std::cout << "gamma = " << gamma << std::endl;
  // if ( R < 0. )
  //   std::cout << "R was < 0 geometrically but taking abs_val for physical correctness\n";
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
} // ~circle_fit(...)

double PatternRecognition::delta_R(const SimpleCircle &circle, const CLHEP::Hep3Vector &pos) {

  double x0 = circle.get_x0();
  double y0 = circle.get_y0();
  double R = circle.get_R();

  double R_i = sqrt((pos.x() - x0)*(pos.x() - x0) + (pos.y() - y0)*(pos.y() - y0));
  double delta_R = R - R_i;
  // std::cout << "dR =  " << delta_R << std::endl;

  return delta_R;
} // ~delta_R

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
} // ~calculate_dipangle(...)

double PatternRecognition::calculate_Phi(double xpos, double ypos, const SimpleCircle &circle) {

  /* double alpha = circle.get_alpha();
  double beta = circle.get_beta();
  double gamma = circle.get_gamma(); */
  double x_c = circle.get_x0();
  double y_c = circle.get_y0();
  double angle = atan2(ypos - y_c, xpos - x_c);

  if ( angle < 0. )
    angle += 2. * CLHEP::pi;
  if ( debug > 0 ) std::cout << "Phi = " << angle << " (This is just from circle fit)\n";
  return angle;
} // ~calculate_Phi(...)

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

      if ( debug > 0 )
        std::cout << "RATIOS.... " << fabs(phi_ratio - z_ratio) /  z_ratio << std::endl;

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

  for ( int n = 0; n < 10; ++n ) // {
    for ( int m = 0; m < 10; ++m ) { // m always less than or equal to n
      double A, B;
      A = ( dphi_kj + ( 2 * m * CLHEP::pi ) ) / ( dphi_ji + ( 2 * n * CLHEP::pi ) ); // phi_ratio
      B = dz_kj / dz_ji; // z_ratio
     // if ( debug > 0 ) std::cout <<  " A - B  = " << fabs(A - B)/B << std::endl;
      if ( fabs(A - B) / B < _AB_cut ) {
        dphi_kj += 2 * n * CLHEP::pi;
        dphi_ji += 2 * m * CLHEP::pi;

        return true;
      }
    } // end m loop
  // } // end n loop
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

// For linear Pattern Recognition use
void PatternRecognition::set_end_stations(const std::vector<int> ignore_stations,
                      int &outer_st_num, int &inner_st_num) {

  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    outer_st_num = 4;
    inner_st_num = 0;
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
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      // std::cout << "stations " << ignore_stations[0] << " and " << ignore_stations[1] << "\n";
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
  } else {
    if (debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
} // ~set_end_stations(...)

// For helical Pattern Recognition use
void PatternRecognition::set_seed_stations(const std::vector<int> ignore_stations,
                      int &outer_st_num, int &inner_st_num, int &middle_st_num) {
  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    outer_st_num = 4;
    inner_st_num = 0;
    middle_st_num = 2;
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
         middle_st_num = 2;
       else
         middle_st_num = 1;
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
        middle_st_num = 1;
      else if ( ignore_stations[0] != 0 && ignore_stations[1] != 2 )
        middle_st_num = 2;
      else if ( ignore_stations[0] != 0 && ignore_stations[1] != 3 )
        middle_st_num = 3;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 2 )
        middle_st_num = 2;
      else if ( ignore_stations[0] != 1 && ignore_stations[1] != 3 )
        middle_st_num = 3;
      else // case where   ignore_stations[0] != 2 && ignore_stations[1] != 3
        middle_st_num = 3;
    } else {
    if (debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
} // ~set_seed_stations(...)

void PatternRecognition::sort_by_station(const std::vector<SciFiSpacePoint*> &spnts,
                                 std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station) {
  for ( int st_num = 0;
        st_num < static_cast<int>(spnts_by_station.size()); ++st_num ) {
    for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
      if ( spnts[i]->get_station() == st_num + 1 ) {
        spnts_by_station[st_num].push_back(spnts[i]);
      }
    }
  }
} // ~sort_by_station(...)

int PatternRecognition::num_stations_with_unused_spnts(
    const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station) {

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
                         const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
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
  if (debug > 0) std::cout << "ignore_stations.size() = " << ignore_stations.size() << std::endl;
  ignore_st_1 = -1, ignore_st_2 = -1;
  if ( ignore_stations.size() == 0 ) {
    // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_st_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_st_1 = ignore_stations[0];
    ignore_st_2 = ignore_stations[1];
  } else if ( ignore_stations.size() > 2 ) {
    if (debug > 0) std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
} // ~set_ignore_stations(...)

void PatternRecognition::draw_line(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2,
                                   SimpleLine &line_x, SimpleLine &line_y) {

  CLHEP::Hep3Vector pos_outer = sp1->get_position();
  CLHEP::Hep3Vector pos_inner = sp2->get_position();

  line_x.set_m(( pos_inner.x() - pos_outer.x()) / (pos_inner.z() - pos_outer.z() ));
  line_x.set_c(pos_outer.x() - ( pos_outer.z() * line_x.get_m()) );
  line_y.set_m(( pos_inner.y() - pos_outer.y()) / (pos_inner.z() - pos_outer.z() ));
  line_y.set_c(pos_outer.y() - ( pos_outer.z() *  line_y.get_m() ));
} // ~draw_line(...)

void PatternRecognition::calc_residual(const SciFiSpacePoint *sp, const SimpleLine &line_x,
                                       const SimpleLine &line_y, double &dx, double &dy) {

    CLHEP::Hep3Vector pos = sp->get_position();
    dx = pos.x() - ( line_x.get_c() + ( pos.z() * line_x.get_m() ) );
    dy = pos.y() - ( line_y.get_c() + ( pos.z() * line_y.get_m() ) );
} // ~calc_residual(...)

// } // ~namespace MAUS
