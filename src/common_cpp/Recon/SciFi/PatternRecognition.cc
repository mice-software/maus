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

// External libs headers
#include "gsl/gsl_fit.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"

PatternRecognition::PatternRecognition() {
  // Do nothing
};

PatternRecognition::~PatternRecognition() {
  // Do nothing
};

// For the moment this just calls the straight PR,
// in future it should choose between helical or straight PR
void PatternRecognition::process(SciFiEvent &evt) {
  straightprtrack_recon(evt);
};

void PatternRecognition::straightprtrack_recon(SciFiEvent &evt) {
  std::cout << "\nBegining Pattern Recognition" << std::endl;
  std::cout << "Number of spacepoints in spill: " << evt.scifispacepoints.size() << std::endl;

  // Split spacepoints up according to which tracker they occured in
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_tracker(_n_trackers);
  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {  // Loop over trackers
    for ( int i = 0; i < evt.scifispacepoints.size(); ++i ) {  // Loop over spacepoints
      if ( evt.scifispacepoints[i]->get_tracker() == trker_no ) {
        spnts_by_tracker[trker_no].push_back(evt.scifispacepoints[i]);
      }
    } // ~Loop over spacepoints
  } // ~Loop over trackers

  // Loop over trackers
  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
    // Split spacepoints according to which station they occured in
    std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(_n_stations);
    sort_by_station(spnts_by_tracker[trker_no], spnts_by_station);

    // Count how many stations have at least one spacepoint
    int stations_hit = 0;
    for ( int i = 0; i < spnts_by_station.size(); ++i ) {
      if ( spnts_by_station[i].size() > 0 ) {
        ++stations_hit;
      }
    }

    // Make the tracks depending on how many stations have spacepoints in them
    if (stations_hit == 5)
      make_spr_5pt(spnts_by_tracker[trker_no], evt.scifistraightprtracks);
    if (evt.scifistraightprtracks.size() == 0 && stations_hit > 3)
      make_spr_4pt(spnts_by_tracker[trker_no], evt.scifistraightprtracks);
    if (evt.scifistraightprtracks.size() == 0 && stations_hit > 2)
      make_spr_3pt(spnts_by_tracker[trker_no], evt.scifistraightprtracks);
  }// ~Loop over trackers
  std::cout << "Number of tracks found: " << evt.scifistraightprtracks.size() << "\n\n";
}

void PatternRecognition::make_spr_5pt(const std::vector<SciFiSpacePoint*>& spnts,
                                      std::vector<SciFiStraightPRTrack>& trks) {
  std::cout << "Making 5 point track" << std::endl;

  // Find out if we are in tracker 1 or 2
  int sign = 1;
  /* if ( spnts[0]->get_tracker() == 0 )
    sign = 1;
  else
    sign = -1; */

  std::vector< std::vector<SciFiSpacePoint*> > spnts_stat(_n_stations);
  sort_by_station(spnts, spnts_stat);

  // Form a candidate track between sp in station 5 and station 1
  bool success = false;
  // Loop over sp in station 5
  for ( int stat_outer = 0; stat_outer < spnts_stat[_n_stations - 1].size(); ++stat_outer ) {
    if ( success ) break;
    // Loop over sp in station 1
    for ( int stat_inner = 0; stat_inner < spnts_stat[0].size(); ++stat_inner ) {
      if ( success ) break;

      // Draw a straight line between spacepoints in outer most and inner most stations
      Hep3Vector pos_us = spnts_stat[4][stat_outer]->get_position();
      Hep3Vector pos_ls = spnts_stat[0][stat_inner]->get_position();

      double m_xi = sign * ( pos_ls.x() - pos_us.x()) / (pos_ls.z() - pos_us.z() );
      double m_yi = sign * ( pos_ls.y() - pos_us.y()) / (pos_ls.z() - pos_us.z() );

      double x_0i = pos_us.x() - ( pos_us.z() * m_xi );
      double y_0i = pos_us.y() - ( pos_us.z() * m_yi );

      std::cout << "m_xi = " << m_xi << "\tx_0i = " << x_0i << std::endl;
      std::cout << "m_yi = " << m_yi << "\ty_0i = " << y_0i << std::endl;

      // Map to hold the good sp in each station
      std::map< int, SciFiSpacePoint* > good_spnts;

      // Loop over intermediate stations and compare sp with the line
      for ( int stat_no = 1; stat_no < 4; ++stat_no ) {
        for ( int sp_no = 0; sp_no < spnts_stat[stat_no].size(); ++sp_no ) {
          Hep3Vector pos = spnts_stat[stat_no][sp_no]->get_position();
          // Calculate the residuals
          double dx = pos.x() - ( x_0i + ( pos.z() * m_xi ) );
          double dy = pos.y() - ( y_0i + ( pos.z() * m_yi ) );
          // Apply a road cut using the residuals
          if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut ) {
            good_spnts[stat_no] = spnts_stat[stat_no][sp_no];
            break;
          }
        }
      }

      // Check we have at least 1 good sp in all the intermediate stations
      std::cout << "Number of stations with good sp: " << good_spnts.size() << std::endl;
      if ( good_spnts.size() > 2 ) {
        std::cout << "Found good spacepoints in all stations, fitting a track..." << std::endl;
        good_spnts[4] = spnts_stat[4][stat_outer];
        good_spnts[0] = spnts_stat[0][stat_inner];

        // Fit track
        SimpleLine line_x, line_y;
        linear_fit(good_spnts, line_x, line_y);

        // Check track passes chisq test
        double chisq = line_x.get_chisq() + line_x.get_chisq();
        if ( chisq / ( _n_stations - 2 ) < _chisq_cut ) {
          std:: cout << "chisq test passed, adding track" << std::endl;

          // Create a SciFi PR track and push back in to tracks vector
          SciFiStraightPRTrack track;
          track.set_x0(line_x.get_c());
          track.set_mx(line_x.get_m());
          track.set_y0(line_y.get_c());
          track.set_my(line_y.get_m());

          std::vector<SciFiSpacePoint> good_spacepoints;
          good_spacepoints.push_back(*good_spnts[0]);
          good_spacepoints.push_back(*good_spnts[1]);
          good_spacepoints.push_back(*good_spnts[2]);
          good_spacepoints.push_back(*good_spnts[3]);
          good_spacepoints.push_back(*good_spnts[4]);

          track.set_spacepoints(good_spacepoints);
          trks.push_back(track);

          success = true;
        } else {
          // std:: cout << "chisq test failed, track rejected" << std::endl;
        }
      }
    }
  }
}

void PatternRecognition::linear_fit(std::map<int, SciFiSpacePoint*> &spnts,
                                     SimpleLine &line_x,
                                     SimpleLine &line_y) {
  // Set up arrays for gsl fitter
  double x[_n_stations] = { spnts[4]->get_position().x(),
                            spnts[3]->get_position().x(),
                            spnts[2]->get_position().x(),
                            spnts[1]->get_position().x(),
                            spnts[0]->get_position().x() };

  double y[_n_stations] = { spnts[4]->get_position().y(),
                            spnts[3]->get_position().y(),
                            spnts[2]->get_position().y(),
                            spnts[1]->get_position().y(),
                            spnts[0]->get_position().y() };

  double z[_n_stations] = { spnts[4]->get_position().z(),
                            spnts[3]->get_position().z(),
                            spnts[2]->get_position().z(),
                            spnts[1]->get_position().z(),
                            spnts[0]->get_position().z() };

  // Set up variables to hold gsl fitter results
  double c_x, m_x, cov_x00, cov_x01, cov_x11, chisq_x;
  double c_y, m_y, cov_y00, cov_y01, cov_y11, chisq_y;

  // Perform the fit
  gsl_fit_linear(z, 1, x, 1, _n_stations,
                  &c_x, &m_x, &cov_x00, &cov_x01, &cov_x11,
                  &chisq_x);

  gsl_fit_linear(z, 1, y, 1, _n_stations,
                  &c_y, &m_y, &cov_y00, &cov_y01, &cov_y11,
                  &chisq_y);

  // Write some output to screen
  std::cout << "Track parameters x: c = " << c_x << ", m = ";
  std::cout << m_x << ", chisq = " << chisq_x << std::endl;
  std::cout << "Track parameters y: c = " << c_y << ", m = ";
  std::cout << m_y << ", chisq = " << chisq_y << std::endl;
  std::cout << "sp\tx\ty\tz" << std::endl;
  for ( int i = 0; i < _n_stations; ++i ) {
    std::cout << i << "\t" << x[i] << "\t" << y[i] << "\t" << z[i] << std::endl;
  }

  // Return the lines in x and y
  line_x.set_parameters(c_x, m_x, chisq_x);
  line_y.set_parameters(c_y, m_y, chisq_y);
}

void PatternRecognition::make_spr_4pt(const std::vector<SciFiSpacePoint*>& spnts,
                                      std::vector<SciFiStraightPRTrack>& trks) {
  // std::cout << "Making 4 point track" << std::endl;
}

void PatternRecognition::make_spr_3pt(const std::vector<SciFiSpacePoint*>& spnts,
                                      std::vector<SciFiStraightPRTrack>& trks) {
  // std::cout << "Making 3 point track" << std::endl;
}

void PatternRecognition::sort_by_station(const std::vector<SciFiSpacePoint*>& spnts,
                                 std::vector< std::vector<SciFiSpacePoint*> >& spnts_stat) {
  for ( int stat_no = 0; stat_no < spnts_stat.size(); ++stat_no ) {
    for ( int i = 0; i < spnts.size(); ++i ) {
      if ( spnts[i]->get_station() == stat_no + 1 ) {
        spnts_stat[stat_no].push_back(spnts[i]);
      }
    }
  }
}
