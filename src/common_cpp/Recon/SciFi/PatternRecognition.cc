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

// External libs headers
#include "gsl/gsl_fit.h"
#include "CLHEP/Matrix/Matrix.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"

// namespace MAUS {

PatternRecognition::PatternRecognition() {
  // Do nothing
};

PatternRecognition::~PatternRecognition() {
  // Do nothing
};

void PatternRecognition::process(SciFiEvent &evt) {

  std::cout << "\nBegining Pattern Recognition" << std::endl;
  std::cout << "Number of spacepoints in spill: " << evt.spacepoints().size() << std::endl;

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
      std::cout << "Reconstructing for Tracker " << trker_no + 1 << std::endl;

      // Vector of ints holding the track residuals for histogramming later
      std::vector< std::vector<int> > residuals(4, std::vector<int>(102, 0));

      // Split spacepoints according to which station they occured in
      std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(_n_stations);
      sort_by_station(spnts_by_tracker[trker_no], spnts_by_station);

      // Count how many stations have at least one *unused* spacepoint
      int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

      // Make the tracks depending on how many stations have spacepoints in them
      if (num_stations_hit == 5) {
        std::vector<SciFiStraightPRTrack> trks;
        // std::vector<SciFiHelicalPRTrack> trks; // use for helical testing
        make_5tracks(spnts_by_station, trks, residuals);
        for ( int i = 0; i < static_cast<int>(trks.size()); ++i ) {
          trks[i].set_tracker(trker_no);
          evt.add_straightprtrack(trks[i]);
          // evt.add_helicalprtrack(tracks[i]); // agian for helical testing purposes
          evt.set_residuals(residuals);
        }
      }
      if (num_stations_hit > 3) {
        std::vector<SciFiStraightPRTrack> trks;
        // std::vector<SciFiHelicalPRTrack> trks;
        make_4tracks(spnts_by_station, trks, residuals);
        for ( int i = 0; i < static_cast<int>(trks.size()); ++i ) {
          trks[i].set_tracker(trker_no);
          evt.add_straightprtrack(trks[i]);
          // evt.add_helicalprtrack(tracks[i]); // agian for helical testing purposes
          evt.set_residuals(residuals);
        }
      }
      if (num_stations_hit > 2) {
        std::vector<SciFiStraightPRTrack> trks;
        // std::vector<SciFiHelicalPRTrack> trks;
        make_3tracks(spnts_by_station, trks, residuals);
        for ( int i = 0; i < static_cast<int>(trks.size()); ++i ) {
          trks[i].set_tracker(trker_no);
          evt.add_straightprtrack(trks[i]);
          // evt.add_helicalprtrack(tracks[i]); // agian for helical testing purposes
          evt.set_residuals(residuals);
        }
      }

      std::cout << "Finished Tracker " << trker_no + 1 << std::endl;
    }// ~Loop over trackers
    std::cout << "Number of tracks found: " << evt.straightprtracks().size() << "\n\n";
    // std::cout << "Number of tracks found: " << evt.helicalprtracks().size() << "\n\n";

  } else {
    std::cout << "No spacepoints in event" << std::endl;
  }
};

void PatternRecognition::make_5tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<SciFiStraightPRTrack> &trks,
                         std::vector< std::vector<int> > &residuals) {
  std::cout << "Making 5 point tracks" << std::endl;

  int num_points = 5;
  std::vector<int> ignore_stations; // A zero size vector sets that all stations are used
  make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks, residuals);

  std::cout << "Finished making 5 pt tracks" << std::endl;
} // ~make_spr_5pt(...)

void PatternRecognition::make_4tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<SciFiStraightPRTrack> &trks,
                         std::vector< std::vector<int> > &residuals) {
  std::cout << "Making 4 point tracks" << std::endl;

  int num_points = 4;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

     std::cout << "4pt track: 5 stations with unused spacepoints" << std::endl;

    for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
      // Recount how many stations have at least one unused spacepoint
      num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
      // If there are enough occupied stations left to make a 4 point track, keep making tracks
      if ( num_stations_hit  >= num_points ) {
        std::vector<int> ignore_stations(1, i);
        make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks, residuals);
      } else {
        break;
      }
    } // ~Loop of stations, ignoring each one in turn
  } else if ( num_stations_hit == 4 ) {

     std::cout << "4pt track: 4 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      make_straight_tracks(num_points, stations_not_hit, spnts_by_station, trks, residuals);
    } else {
      std::cout << "Wrong number of stations without spacepoints, ";
      std::cout << "aborting 4 pt track." << std::endl;
    }
  } else if ( num_stations_hit < 4 ) {
    std::cout << "Not enough unused spacepoints, quiting 4 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
    std::cout << "Wrong number of stations with spacepoints, aborting 4 pt track." << std::endl;
  }

  std::cout << "Finished making 4 pt tracks" << std::endl;
} // ~make_straight_4tracks(...)

void PatternRecognition::make_3tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<SciFiStraightPRTrack> &trks,
                         std::vector< std::vector<int> > &residuals) {
  std::cout << "Making 3 point track" << std::endl;

  int num_points = 3;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  bool sufficient_hit_stations = true;

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    std::cout << "3pt track: 5 stations with unused spacepoints" << std::endl;

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
              make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks, residuals);
            } else {
                sufficient_hit_stations = false;
            }
          } // ~if ( sufficient_hit_stations )
        } // ~Loop of second station to ignore
      } // ~if ( sufficient_hit_stations )
    } // ~Loop of first station to ignore
  } else if ( num_stations_hit == 4 ) {

    std::cout << "3pt track: 4 stations with unused spacepoints" << std::endl;

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
          make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks, residuals);
        } else {
          break;
        }
      }
    }
  } else if ( num_stations_hit == 3 ) {

    std::cout << "3pt track: 3 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 2 ) {
      make_straight_tracks(num_points, stations_not_hit, spnts_by_station, trks, residuals);
    } else {
      std::cout << "Wrong number of stations without spacepoints, ";
      std::cout << "aborting 3 pt track." << std::endl;
    }

  } else if ( num_stations_hit < 3 ) {
      std::cout << "Not enough unused spacepoints, quiting 3 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
      std::cout << "Wrong number of stations with spacepoints, aborting 3 pt track." << std::endl;
  }
  std::cout << "Finished making 3 pt tracks" << std::endl;
} // ~make_straight_3tracks(...)

void PatternRecognition::make_straight_tracks(const int num_points,
                                     const std::vector<int> ignore_stations,
                                     std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                                     std::vector<SciFiStraightPRTrack> &trks,
                                     std::vector< std::vector<int> > &residuals) {
  // Set inner and outer stations
  int outer_station_num = -1, inner_station_num = -1;
  set_end_stations(ignore_stations, outer_station_num, inner_station_num);

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

              double delta_sq = 1000000;  // A large number so initial value is set as best first

              for ( int sp_no = 0;
                    sp_no < static_cast<int>(spnts_by_station[station_num].size()); ++sp_no ) {

                // If the spacepoint has not already been used in a track fit
                if ( !spnts_by_station[station_num][sp_no]->get_used() ) {
                  Hep3Vector pos = spnts_by_station[station_num][sp_no]->get_position();

                  // Calculate the residuals
                  double dx = pos.x() - ( line_x.get_c() + ( pos.z() * line_x.get_m() ) );
                  double dy = pos.y() - ( line_y.get_c() + ( pos.z() * line_y.get_m() ) );

                  add_residuals(false, dx, dy, residuals);

                  // Apply roadcuts & find the spacepoints with the smallest residuals for the line
                  if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut && delta_sq > (dx*dx + dy*dy) ) {
                    delta_sq = dx*dx + dy*dy;
                    good_spnts.push_back(spnts_by_station[station_num][sp_no]);
                    add_residuals(true, dx, dy, residuals);
                  } // ~If pass roadcuts and beats previous best fit point
                } // ~If spacepoint is unused
              } // ~Loop over spacepoints
            } // ~if (station_num != ignore_station)
          } // ~Loop over intermediate stations

          // Clear the line objects so we can reuse them
          line_x.clear();
          line_y.clear();

          // Check we have at least 1 good spacepoint in each of the intermediate stations
          std::cout << "Num of intermediate stations with good sp: " << good_spnts.size() << "\n";

          if ( static_cast<int>(good_spnts.size()) > (num_points - 3) ) {
            std::cout << "Found good spacepoints in all stations, fitting a track..." << std::endl;

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

              std::cout << "** chisq test passed, adding " << num_points << "pt track **\n";

              SciFiStraightPRTrack track(-1, num_points, line_x, line_y);

              // Set all the good sp to used and convert pointers to variables
              std::vector<SciFiSpacePoint> good_spnts_variables;
              good_spnts_variables.resize(good_spnts.size());

              for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i ) {
                good_spnts[i]->set_used(true);
                good_spnts_variables[i] = *good_spnts[i];
              }

              // Populate the sp of the track and then push the track back into the trks vector
              track.set_spacepoints(good_spnts_variables);
              trks.push_back(track);

            } else {
              std::cout << "x_chisq = " << line_x.get_chisq();
              std::cout << "\ty_chisq = " << line_y.get_chisq() << std::endl;
              std::cout << "chisq test failed, " << num_points << "pt track rejected" << std::endl;
            } // ~Check track passes chisq test
          } // ~ if ( good_spnts.size() > 1 )
        } else {
          // std::cout << "...no" << std::endl;
        }// ~Check the inner spacepoint is unused
      } // ~Loop over sp in station 1
    } else {
      std::cout << "...no" << std::endl;
    }// ~Check the outer spacepoint is unused
  } // ~Loop over sp in station 5
}

void PatternRecognition::linear_fit(const std::vector<double> &_x, const std::vector<double> &_y,
                                    const std::vector<double> &_y_err, SimpleLine &line) {

  int num_points = static_cast<int>(_x.size());

  CLHEP::HepMatrix A(num_points, 2); // rows, columns
  CLHEP::HepMatrix V(num_points, num_points); // error matrix
  CLHEP::HepMatrix Y(num_points, 1); // measurements

  for ( int i = 0; i < static_cast<int>(_x.size()); ++i ) {

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
                                    std::vector<SciFiStraightPRTrack> &trks) {
  // Set inner and outer stations
  int outer_station_num = -1, inner_station_num = -1;
  set_end_stations(ignore_stations, outer_station_num, inner_station_num);

  // Set intermediate station number to be inner_station_num + 1
  int intermed_station_num = inner_station_num + 1;

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
             num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

          // Loop over spacepoints in intermediate station
          for ( int station_intermed_sp = 0;
              station_intermed_sp < static_cast<int>(spnts_by_station[intermed_station_num].size());
              ++station_intermed_sp ) {

            // Check the intermediate spacepoints is unused and enough stations are left with
            // unused spacepoints
            if ( !spnts_by_station[intermed_station_num][station_intermed_sp]->get_used() &&
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
              tmp_spnts.push_back(spnts_by_station[intermed_station_num][station_intermed_sp]);
              tmp_spnts.push_back(spnts_by_station[outer_station_num][station_outer_sp]);

              // Fit a circle in the xy projection to 3 sp from inner, outer, and interm stations
              circle_fit(tmp_spnts, circle);

              // Loop over other intemediate stations so that we can try adding them to the circle
              for ( int station_num = intermed_station_num + 1; station_num < outer_station_num;
                    ++station_num ) {
                if (station_num != ignore_station_1 && station_num != ignore_station_2) {

                  for ( int sp_no = 0;
                        sp_no < static_cast<int>(spnts_by_station[station_num].size()); ++sp_no ) {

                    // If the spacepoint has not already been used in a track fit
                    if ( !spnts_by_station[station_num][sp_no]->get_used() ) {
                      Hep3Vector pos = spnts_by_station[station_num][sp_no]->get_position();

                      // Calculate the residual dR
                      double dR = delta_R(circle, pos);

                      // Apply roadcut to see if spacepoint belongs to same circle
                      if ( fabs(dR) < _R_res_cut )
                        good_spnts.push_back(spnts_by_station[station_num][sp_no]);
                    } // ~If intermediate station spacepoint is unused
                  } // ~Loop over intermediate staion spacepoints
                } // ~if (station_num != ignore_station)
              } // ~Loop over intermediate stations not used in initial circle_fit

              // Clear the circle object so we can reuse it
              circle.clear();

              // Check we have at least 1 good spacepoint in each of the intermediate stations
              std::cout << "Num of intermediate stations w/ good sp: " << good_spnts.size() << "\n";

              if ( static_cast<int>(good_spnts.size()) > (num_points - 2) ) {
                std::cout << "Found good spnts in all stations, fitting a track..." << std::endl;

                good_spnts.insert(good_spnts.begin(),
                                  spnts_by_station[inner_station_num][station_inner_sp]);
                good_spnts.push_back(spnts_by_station[outer_station_num][station_outer_sp]);

                // Perform another circle fit now that we have found all of the good spnts
                circle_fit(good_spnts, circle);

                // Check circle fit passes chisq test
                if ( circle.get_chisq() / ( num_points - 2 ) < _chisq_cut ) {
                  std::cout << "** chisq circle test passed, moving onto linear fit in s-z **\n";

                  SimpleLine line_sz;
                  std::vector<double> dphi;
                  calculate_dipangle(good_spnts, circle, dphi, line_sz);

                  // Check linear fit passes chisq test, then perform full helix fit
                  if ( line_sz.get_chisq() / ( num_points - 2 ) < _chisq_cut ) {
                    std::cout << "** line in sz chisq test passed, moving on to full helix fit**\n";
                    circle.set_turning_angle(dphi); // Turning angles will be needed in helix fit
                    SimpleHelix helix;
                    bool good_helix = full_helix_fit(good_spnts, circle, line_sz, helix);
                    if ( good_helix ) {
                      // push helix back into track object once its made
                      std::cout << "Helix fit found, adding " << num_points << "pt track **\n";

                      CLHEP::Hep3Vector pos_0 = good_spnts[0]->get_position();
                      // Need to pester Adam about the inheritance!!!!!!!!
                      // SciFiHelicalPRTrack track(-1, num_points, pos_0, helix);

                     // Set all the good sp to used and convert pointers to variables
                     std::vector<SciFiSpacePoint> good_spnts_variables;
                     good_spnts_variables.resize(good_spnts.size());

                     for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i ) {
                       good_spnts[i]->set_used(true);
                       good_spnts_variables[i] = *good_spnts[i];
                     }
                     // **********ALL OF THIS IS WAITING FOR PROPER TRACK CLASS ************
                     // Populate the sp of the track and then push the track back into trks vector
                     // track.set_spacepoints(good_spnts_variables);
                     // trks.push_back(track);
                    } else {
                      std::cout << "Helix fit did not converge within reasonable # of interations.";
                      std::cout << " Track is rejected...." << std::endl;
                    }
                  } else {
                    std::cout << "sz chisq = " << line_sz.get_chisq();
                    std::cout << "sz chisq test failed, " << num_points << "track rejected" << "\n";
                  } // ~Check s-z line passes chisq test
                } else { // ~ last end bracket was for circle chisq cut test passed
                  std::cout << "circle chisq = " << circle.get_chisq();
                  std::cout << "circle chisq test failed, track rejected" << "\n";
                } // ~circle fail chisq test
              } // ~if enough spacepoints are found for fit (must be at least 3)
            } // ~if intmediate station sp unused
          } // ~Loop over intermediate station spacepoints
        } // ~If inner station spacepoint is unused
      } // ~Loop over inner station spacepoints
    } // ~if outer station spacepoint is unused
  } // ~Loop over outer station spacepoints
} // ~make_helix(...)

void PatternRecognition::circle_fit(const std::vector<SciFiSpacePoint*> &spnts,
                                    SimpleCircle &circle) {

  int num_points = spnts.size();

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

    double tmp_xpos = spnts[i]->get_position().x();
    double tmp_ypos = spnts[i]->get_position().y();

    A[i][0] = ( tmp_xpos * tmp_xpos ) + ( tmp_ypos * tmp_ypos );
    A[i][1] = spnts[i]->get_position().x();
    A[i][2] = spnts[i]->get_position().y();

    V[i][i] = ( sd * sd );

    K[i][0] = 1;
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
  x0 = -beta / 2 * alpha;
  y0 = -gamma / 2 * alpha;
  R = sqrt((4 * alpha) + (beta * beta) + (gamma * gamma)) / (2 * alpha);

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
  double xchi2 = result[0][0];
  circle.set_chisq(xchi2 / num_points);
}

double PatternRecognition::delta_R(const SimpleCircle &circle, const CLHEP::Hep3Vector &pos) {

  double alpha = circle.get_alpha();
  double beta = circle.get_beta();
  double gamma = circle.get_gamma();
  double kappa = circle.get_kappa();

  // Expression too long for one line so its broken down into terms
  double t_1 = pos.x()*pos.x() + pos.y()*pos.y(); // First term
  double t_2 = ((beta*beta) + (gamma*gamma)) / (4.*alpha*alpha); // second term
  double t_3 = ((beta*pos.x()) + (gamma*pos.y())) / alpha; // third term
  double t_4 = t_2 - (kappa/alpha); // Radius

  double delta_R = sqrt(t_1 + t_2 + t_3 - t_4);
  // double R_i = sqrt((pos.x() - x0)*(pos.x() - x0) + (pos.y() - y0)*(pos.y() - y0));

  return delta_R;
}

void PatternRecognition::calculate_dipangle(const std::vector<SciFiSpacePoint*> &spnts,
                                            const SimpleCircle &circle, std::vector<double> &dphi,
                                            SimpleLine &line_sz) {
  double R = circle.get_R();

  std::vector<double> dz;
  std::vector<double> dphi_err;

  // For the linear fit in s-z, we care about the change in z vs change in s
  // So here we calculate the values dz_ji and dphi_ji (because ds_ji = R*dphi_ji)
  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    int j = i + 1;

    double z_i = spnts[i]->get_position().z();
    double z_j = spnts[j]->get_position().z();
    double dz_ji = z_j - z_i;
    dz.push_back(dz_ji);

    double x_i = spnts[i]->get_position().x();
    double y_i = spnts[i]->get_position().y();
    double phi_i = calculate_Phi(x_i, y_i, circle);

    double x_j = spnts[j]->get_position().x();
    double y_j = spnts[j]->get_position().y();
    double phi_j = calculate_Phi(x_j, y_j, circle);

    double dphi_ji = phi_j - phi_i;
    dphi.push_back(dphi_ji);

    double sd_phi = -1.0;
    if ( (spnts[i]->get_station() == 5) || (spnts[j]->get_station() == 5) )
      sd_phi = _sd_phi_5;
    else
      sd_phi = _sd_phi_1to4;

    dphi_err.push_back(sd_phi);
  }

    bool ok = turns_between_stations(dz, dphi);

    if ( ok ) {
      std::vector<double> ds;
      // Multiply each element of dphi by R so that each element dphi_ji--> ds_ji
      dphi_to_ds(R, dphi, ds);
      linear_fit(dz, ds, dphi_err, line_sz); // Need to change dphi_err to ds_err.
    }
}

double PatternRecognition::calculate_Phi(double xpos, double ypos, const SimpleCircle &circle) {

  double alpha = circle.get_alpha();
  double beta = circle.get_beta();
  double gamma = circle.get_gamma();

  double angle = atan2(ypos - ( -gamma / (2 * alpha) ), xpos - ( -beta / (2 * alpha) ));

  if ( angle < 0. )
    angle += 2. * pi;

  return angle;
}

bool PatternRecognition::turns_between_stations(const std::vector<double> &dz,
                                                std::vector<double> &dphi) {

  //  Make sure that you have enough points to make a line (2)
  if ( dz.size() < 2 || dphi.size() < 2 )
    return false;

  if ( dphi[0] < 0 )
    dphi[0] += 2 * pi;

  //  Make sure that dphi is always increasing between stations
  for ( int i = 0; i < static_cast<int>(dphi.size()); i++ ) {
    while ( dphi[i] > dphi[i+1] )
      dphi[i+1] += 2 * pi;
  }

  bool good_AB = 1; // set to true to begin loop
  int i;
  for ( i = 0; i < static_cast<int>(dphi.size()) && good_AB ; i++ ) {

    good_AB = AB_ratio(dphi[i] , dphi[i+1], dz[i], dz[i+1]);
    // if good_AB is returned as false at anypoint, this loop will stop iterating.
  }
  // i = dphi.size() if AB_ratio was returned true for each station interval
  if ( i == static_cast<int>(dphi.size()) )
    return true;
  else
    return false; // if i < dphi.size(), then the iterations were stopped after some AB_ratio was
                  // returned false.
}

bool PatternRecognition::AB_ratio(double &dphi_kj, double &dphi_ji, double dz_kj,
                                  double dz_ji) {
  double A, B;
  for ( int n = 0; n < 5; ++n )
    for ( int m = 0; m < 5; ++m ) {
      A = ( dphi_kj + ( 2 * n * pi ) ) / ( dphi_ji + ( 2 * m * pi ) );
      B = dz_kj / dz_ji;

      std::cout << "A-B = " << fabs(A-B) << std::endl;

      if ( fabs(A - B) < _AB_cut ) {
        dphi_kj += 2 * n * pi;
        dphi_ji += 2 * m * pi;

        return true;
      } // end m loop
    } // end n loop
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
  double Phi_0 = circle.get_turning_angle()[0];
  double dsdz = line_sz.get_m();
  double tan_lambda = 1 / dsdz;
  CLHEP::Hep3Vector starting_point = spnts[0]->get_position();

  // Calculate chisq with initial params
  double chisq = calculate_chisq(spnts, circle.get_turning_angle(), Phi_0, tan_lambda, R);
  double best_chi2dof = chisq / static_cast<int>(spnts.size());

  // Declare adjustments to parameters and chisq that will be calculated after adjuments are made
  double dR, dPhi_0, dtan_lambda;
  for ( int i = 0; i < 10; ++i ) {
    calculate_adjustments(spnts, circle.get_turning_angle(), R, Phi_0, tan_lambda, dR, dPhi_0,
                          dtan_lambda);
    // Adjust the parameters to calculate new chisq_dof.
    R += dR;
    Phi_0 += dPhi_0;
    tan_lambda += dtan_lambda;
    chisq = calculate_chisq(spnts, circle.get_turning_angle(), Phi_0, tan_lambda, R);
    double chisq_dof = chisq / static_cast<int>(spnts.size());

    if ( chisq_dof > best_chi2dof && fabs(best_chi2dof - chisq_dof) > 0.001 ) {
      // If the new chi2 you calculate is larger than previous, then the minimum has been passed
      // over
      // Cut the previous adjustments in half, and then re-calculate chisq
      while ( chisq_dof > best_chi2dof ) {
        R -= dR / 2;
        Phi_0 -= dPhi_0 / 2;
        tan_lambda -= dtan_lambda / 2;
        ++i; // This counts as an iteration.
        chisq = calculate_chisq(spnts, circle.get_turning_angle(), Phi_0, tan_lambda, R);
        chisq_dof = chisq / static_cast<int>(spnts.size());
      } // now we have chisq_dof < best_chi2dof, at most you do this twice
    }
    if ( chisq_dof < best_chi2dof && fabs(best_chi2dof - chisq_dof) < 0.001 ) {
      std::cout << "yay, finished" << std::endl;
      best_chi2dof = chisq_dof;
      helix.set_R(R);
      helix.set_Phi_0(Phi_0);
      helix.set_tan_lambda(tan_lambda);
      helix.set_chisq_dof(best_chi2dof);
      return true;
      // returns the helix too
    }  else if ( chisq_dof < best_chi2dof && fabs(best_chi2dof - chisq_dof) > 0.001 ) {
     best_chi2dof = chisq_dof; // If we reduced chisq but still not at miniumum
    }
  } // end i
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
    double phi_i = turning_angles[i];
    phi_i -= Phi_0; // Everything relative to starting point.
    double A, B, C;
    A = spnts[0]->get_position().x();
    B = spnts[0]->get_position().y();
    C = spnts[0]->get_position().z();

    double xi, yi, zi;
    helix_function_at_i(R, Phi_0, tan_lambda, A, B, C, phi_i, xi, yi, zi);
    chi2 += (p.x() - xi)*(p.x() - xi) + (p.y() - yi)*(p.y() - yi) + (p.z() - zi)*(p.z() - zi);
  }
  return chi2;
}

void PatternRecognition::helix_function_at_i(double R, double phi_0, double tan_lambda,
                                             double A, double B, double C, double phi_i,
                                             double &xi, double &yi, double &zi) {
    xi = A + R * (cos(phi_i) - 1) * cos(phi_0) - R * sin(phi_i) * sin(phi_0);
    yi = B + R * (cos(phi_i) - 1) * sin(phi_0) - R * sin(phi_i) * cos(phi_0);
    zi = C + R * phi_i * tan_lambda;
}

void PatternRecognition::calculate_adjustments(const std::vector<SciFiSpacePoint*> &spnts,
                                               const std::vector<double> &turning_angles, double &R,
                                               double &phi_0, double &tan_lambda, double &dR,
                                               double &dphi_0, double &dtan_lambda) {
  CLHEP::HepMatrix G(3, 3); // symmetric matrix containing second derivatives w.r.t. each parameter
  CLHEP::HepMatrix g(3, 1); // vector containing first derivatives w.r.t. each parameter

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
    double phi_i = turning_angles[i];
    phi_i -= phi_0; // Everything relative to starting point.

    // Get errors on x and y measurements (equal).  Note error on z negligible.
    double sd;
    if ( spnts[i]->get_station() == 5 )
      sd = _sd_5;
    else
      sd = _sd_1to4;

    double w = 1 / (sd * sd);

    double xi, yi, zi;
    helix_function_at_i(R, phi_0, tan_lambda, A, B, C, phi_i, xi, yi, zi);

    // Calculate the elements of the gradient vector g
    g_r += w * ((p.x() - xi) * (xi - A) + (p.y() - yi) * (yi - B)) + (p.z() - zi) * (zi - C);
    g_b += w * (-(p.x() - xi) * (yi - B) + (p.y() - yi) * (xi - A));
    g_l += (p.z() - zi) * phi_i;

    // Calculate the elements of the matrix G
    G_rr += w * ((xi - A) * (xi - A) + (yi - B) * (yi - B)) + (zi - C) * (zi - C);
    G_rb += w * ((p.x() - xi) * (yi - B) + (p.y() - yi) * (xi - A));
    G_rl += ((zi - C) - (p.z() - zi)) * phi_i;
    G_bb += w * ((p.x() - A) * (xi - A) + (p.y() - B) * (yi - B));
    G_ll += phi_i * phi_i;
  } // end i

  g_r = -2 / R * g_r;
  g_b = -2 * g_b;
  g_l = -2 * R * g_l;

  G_rr = 2 / (R * R) * G_rr;
  G_rb = 2 / R * G_rb;
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
  g[1][0] = g_l;

  // Now we calculate the corrections to the parameters
  int ierr;
  G.invert(ierr);
  CLHEP::HepMatrix d_params = G * g; // (3x3)(3x1) = (3x1) vector of corrections

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
      std::cout << "stations " << ignore_stations[0] << " and " << ignore_stations[1] << std::endl;
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
    std::cout << "Error: Invalid ignore station argument." << std::endl;
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

  std::cout << "Unused spacepoints per station:\t";

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
    std::cout << unused_sp << "  ";
  }
  std::cout << std::endl;
  return stations_hit;
}

void PatternRecognition::stations_with_unused_spnts(
                         const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<int> &stations_hit, std::vector<int> &stations_not_hit) {

  stations_hit.clear();
  stations_not_hit.clear();

  std::cout << "Unused spacepoints per station:\t";

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
    std::cout << unused_sp << "  ";
  }
  std::cout << std::endl;
}

void PatternRecognition::set_ignore_stations(const std::vector<int> &ignore_stations,
                         int &ignore_station_1, int &ignore_station_2) {

  ignore_station_1 = -1, ignore_station_2 = -1;
  if ( ignore_stations.size() == 0 ) {
    // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_station_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_station_1 = ignore_stations[0];
    ignore_station_2 = ignore_stations[1];
  } else if ( ignore_stations.size() > 2 ) {
    std::cout << "Error: Invalid ignore station argument." << std::endl;
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

          std::cout << "m_xi = " << line_x.get_m()  << "\tx_0i = " << line_x.get_c() << std::endl;
          std::cout << "m_yi = " << line_y.get_m() << "\ty_0i = " << line_y.get_c() << std::endl;
}

bool PatternRecognition::add_residuals(const bool passed, const double dx, const double dy,
                                       std::vector< std::vector<int> > &residuals) {

  if (residuals.size() == 4) {
    double bin_width = _active_diameter / _n_bins;
    std::cout << "Bin width is " << bin_width << std::endl;

    if (passed) {  // if we have only residuals that passed the road cuts
      if (dx < 0.0 )
        ++residuals[2][0]; // Underflow
      else if ( dx > _active_diameter )
        ++residuals[2][_n_bins+1]; // Overflow
      else
        ++residuals[2][ceil(dx/bin_width)];

      if (dy < 0.0 )
        ++residuals[3][0]; // Underflow
      else if ( dy > _active_diameter )
        ++residuals[3][_n_bins+1]; // Overflow
      else
        ++residuals[3][ceil(dy/bin_width)];

      return true;

    } else {  // all the residuals, passed or not
      if (dx < 0.0 )
        ++residuals[0][0]; // Underflow
      else if ( dx > _active_diameter )
        ++residuals[0][_n_bins+1]; // Overflow
      else
        ++residuals[0][ceil(dx/bin_width)];

      if (dy < 0.0 )
        ++residuals[1][0]; // Underflow
      else if ( dy > _active_diameter )
        ++residuals[1][_n_bins+1]; // Overflow
      else
        ++residuals[1][ceil(dy/bin_width)];

      return true;
    }

  } else {
    std::cout << "Warning: Bad residuals vector passed to add_residuals function" << std::endl;
    return false;
  }
}

// } // ~namespace MAUS
