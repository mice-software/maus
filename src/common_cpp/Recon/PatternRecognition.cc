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

#include "src/common_cpp/Recon/PatternRecognition.hh"
#include <gsl/gsl_fit.h>
#include <iostream>
#include <fstream>
#include <map>

PatternRecognition::PatternRecognition() {
  // Do nothing
};

PatternRecognition::~PatternRecognition() {
  // Do nothing
};

// For the moment this just calls the straight pr,
// in future it should choose between helical or straight pr
void PatternRecognition::process(SciFiEvent &evt) {
  straightprtrack_recon(evt);
};

void PatternRecognition::straightprtrack_recon(SciFiEvent &evt) {
  // std::cout << "\nBegining Pattern Recognition" << std::endl;
  // std::cout << "Number of spacepoints in spill: " << evt.scifispacepoints.size() << std::endl;

  // Split spacepoints up according to which tracker they occured in
  // ----------------------------------------------------------------

  // Construct a 2D matrix to hold spacepoints
  // 1st index differentiates between trackers, 2nd index the spacepoints
  std::vector< std::vector<SciFiSpacePoint*> > spnts(_n_trackers);

  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
    for ( int i = 0; i < evt.scifispacepoints.size(); ++i ) {
      if ( evt.scifispacepoints[i]->get_tracker() == trker_no ) {
        spnts[trker_no].push_back(evt.scifispacepoints[i]);
      }
    }
    // std::cout << "Spacepoints in Tracker " << trker_no+1 << ": ";
    // std::cout << spnts[trker_no].size() << std::endl;
  }

  // Count the number of spacepoints per tracker per station
  // and the number of stations hit in each tracker
  // --------------------------------------------------------

  // Construct a 2*5 matrix of ints to hold the number of sp per station per tracker
  std::vector< std::vector<int> > spnts_per_station(2, std::vector<int>(_n_stations, 0));
  std::vector<int> stations_hit(2, 0);

  // stations_hit stores the number of stations hit per tracker.
  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
    // Count the number of space points in each station
    for ( int i = 0; i < spnts[trker_no].size(); ++i ) {
      for ( int j = 1; j < spnts_per_station[trker_no].size() + 1; ++j ) {
        if ( spnts[trker_no][i]->get_station() == j ) {
          ++spnts_per_station[trker_no][j-1];
          break; // If we match this sp to a station, move to the next sp
        }
      }
    }
    // Count how many stations have at least one spacepoint
    for ( int i = 0; i < spnts_per_station[trker_no].size(); ++i ) {
      // std::cout << "Tracker " << trker_no+1 << " Station " << i+1;
      // std::cout << " sp: " << spnts_per_station[trker_no][i] << std::endl;
      if ( spnts_per_station[trker_no][i] > 0 ) {
        ++stations_hit[trker_no];
      }
    }
  }

  // Make the tracks depending on how many stations have spacepoints in them
  // -----------------------------------------------------------------------

  // make_spr_tracks( spnts , spnts.size() , lTrackSpnts , lBreakCondition );
  // at this point, we know how many stations were hit and we have the spacepoints in a vector,
  // sorted by tracker.
  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
    if (stations_hit[trker_no] == 5)
      make_spr_5pt(spnts[trker_no], evt.scifistraightprtracks);
    if (evt.scifistraightprtracks.size() == 0 && stations_hit[trker_no] > 3)
      make_spr_4pt(spnts[trker_no], evt.scifistraightprtracks);
    if (evt.scifistraightprtracks.size() == 0 && stations_hit[trker_no] > 2)
      make_spr_3pt(spnts[trker_no], evt.scifistraightprtracks);
  }
  // std::cout << "Number of tracks found: " << evt.scifistraightprtracks.size() << "\n\n";
}

void PatternRecognition::make_spr_5pt(const std::vector<SciFiSpacePoint*>& spnts,
                                      std::vector<SciFiStraightPRTrack>& trks) {
  // std::cout << "Making 5 point track" << std::endl;

  // Find out if we are in tracker 1 or 2
  // std::cout << "Forming track in Tracker " << spnts[0]->get_tracker() << std::endl;
  int sign = 1;
  /*
  if ( spnts[0]->get_tracker() == 0 )
    sign = 1;
  else
    sign = -1;
 */

  std::vector< std::vector<SciFiSpacePoint*> > spnts_stat(_n_stations);
  sort_by_station(spnts, spnts_stat);

  // Form a candidate track between sp in station 5 and station 1
  // -----------------------------------------------------------

  bool success = false;
  bool roadcuts_ok = false;

  // Loop over sp in station 5
  for ( int stat_outer = 0; stat_outer < spnts_stat[_n_stations - 1].size(); ++stat_outer ) {
    if ( success ) break;
    // Loop over sp in station 1
    for ( int stat_inner = 0; stat_inner < spnts_stat[0].size(); ++stat_inner ) {
      if ( success ) break;

      Hep3Vector pos_us = spnts_stat[4][stat_outer]->get_position();
      Hep3Vector pos_ls = spnts_stat[0][stat_inner]->get_position();

      double m_xi = sign * ( pos_ls.x() - pos_us.x()) / (pos_ls.z() - pos_us.z() );
      double m_yi = sign * ( pos_ls.y() - pos_us.y()) / (pos_ls.z() - pos_us.z() );

      double x_0i = pos_us.x() - ( pos_us.z() * m_xi );
      double y_0i = pos_us.y() - ( pos_us.z() * m_yi );

      // std::cout << "m_xi = " << m_xi << "\tx_0i = " << x_0i << std::endl;
      // std::cout << "m_yi = " << m_yi << "\ty_0i = " << y_0i << std::endl;

      // Map to hold the good sp in each station
      std::map< int, SciFiSpacePoint* > good_spnts;

      // Loop over intermediate stations (station 2 to station 4)
      for ( int stat_no = 1; stat_no < 4; ++stat_no ) {
        for ( int sp_no = 0; sp_no < spnts_stat[stat_no].size(); ++sp_no ) {
          Hep3Vector pos = spnts_stat[stat_no][sp_no]->get_position();
          double dx = pos.x() - ( x_0i + ( pos.z() * m_xi ) );
          double dy = pos.y() - ( y_0i + ( pos.z() * m_yi ) );
          if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut ) {
            good_spnts[stat_no] = spnts_stat[stat_no][sp_no];
            // std::cout << "Good sp found with residuals of ";
            // std::cout << "dx = " << dx << "\tdy = " << dy << std::endl;
            break;
          }
        }
      }

      // Check we have at least 1 good sp in all the intermediate stations
      // std::cout << "Number of stations with good sp: " << good_spnts.size() << std::endl;
      if ( good_spnts.size() > 2 ) {
        // std::cout << "Found good spacepoints in all stations, fitting a track..." << std::endl;
        // Fit track
        double x[_n_stations] = { pos_us.x(), good_spnts[3]->get_position().x(),
                                  good_spnts[2]->get_position().x(),
                                  good_spnts[1]->get_position().x(),
                                  pos_ls.x() };

        double y[_n_stations] = { pos_us.y(), good_spnts[3]->get_position().y(),
                                  good_spnts[2]->get_position().y(),
                                  good_spnts[1]->get_position().y(),
                                  pos_ls.y() };

        double z[_n_stations] = { pos_us.z(), good_spnts[3]->get_position().z(),
                                  good_spnts[2]->get_position().z(),
                                  good_spnts[1]->get_position().z(),
                                  pos_ls.z() };

        double c_x, m_x, cov_x00, cov_x01, cov_x11, chisq_x;
        double c_y, m_y, cov_y00, cov_y01, cov_y11, chisq_y;

        gsl_fit_linear(z, 1, x, 1, _n_stations,
                       &c_x, &m_x, &cov_x00, &cov_x01, &cov_x11,
                       &chisq_x);

        gsl_fit_linear(z, 1, y, 1, _n_stations,
                       &c_y, &m_y, &cov_y00, &cov_y01, &cov_y11,
                       &chisq_y);
        // std::cout << "Track parameters x: c = " << c_x << ", m = ";
        // std::cout << m_x << ", chisq = " << chisq_x << std::endl;
        // std::cout << "Track parameters y: c = " << c_y << ", m = ";
        // std::cout << m_y << ", chisq = " << chisq_y << std::endl;
        // std::cout << "sp\tx\ty\tz" << std::endl;
        for ( int i = 0; i < _n_stations; ++i ) {
          std::cout << i << "\t" << x[i] << "\t" << y[i] << "\t" << z[i] << std::endl;
        }

        double chisq = chisq_x + chisq_y;
        if ( chisq / ( _n_stations - 2 ) < _chisq_cut ) {
          // std:: cout << "chisq test passed, adding track" << std::endl;
          SciFiStraightPRTrack track;
          track.set_x0(c_x);
          track.set_mx(m_x);
          track.set_y0(c_y);
          track.set_my(m_y);
          std::vector<SciFiSpacePoint> good_spacepoints;
          good_spacepoints.push_back(*spnts_stat[0][stat_inner]);
          good_spacepoints.push_back(*good_spnts[1]);
          good_spacepoints.push_back(*good_spnts[2]);
          good_spacepoints.push_back(*good_spnts[3]);
          good_spacepoints.push_back(*spnts_stat[4][stat_outer]);
          track.set_spacepoints(good_spacepoints);
          trks.push_back(track);
          success = true;
        } else {
          // std:: cout << "chisq test failed, track rejected" << std::endl;
        }
      } else {
        continue;
      }
    }
  }
  /* if ( success ) {
    // run_analysis(trks);
  } */
}

void PatternRecognition::run_analysis(std::vector<SciFiStraightPRTrack>& trks) {
  // std::cout << trks.size() << std::endl;
  for ( int st_i = 0; st_i < 5; st_i++ ) {
    std::vector<SciFiSpacePoint> space_points = trks[0].get_spacepoints();
    Hep3Vector position = space_points[st_i].get_position();
    space_points.erase(space_points.begin() + st_i);
    a_4_station_fit(space_points, position, st_i);
  // std::cout << space_points.size() << std::endl;
  }
}

void PatternRecognition::a_4_station_fit(std::vector<SciFiSpacePoint> space_points,
                                         Hep3Vector position, int st_i) {
  int _nstations = 4;
  // std::cout << space_points[0].get_position().x() << std::endl;
  int tracker = space_points[0].get_tracker();
  int station = st_i;

  double x[4] = { space_points[0].get_position().x(),
                            space_points[1].get_position().x(),
                            space_points[2].get_position().x(),
                            space_points[3].get_position().x() };

  double y[4] = { space_points[0].get_position().y(),
                            space_points[1].get_position().y(),
                            space_points[2].get_position().y(),
                            space_points[3].get_position().y() };

  double z[4] = { space_points[0].get_position().z(),
                            space_points[1].get_position().z(),
                            space_points[2].get_position().z(),
                            space_points[3].get_position().z() };

  double c_x, m_x, cov_x00, cov_x01, cov_x11, chisq_x;
  double c_y, m_y, cov_y00, cov_y01, cov_y11, chisq_y;

  gsl_fit_linear(z, 1, x, 1, 4,
                 &c_x, &m_x, &cov_x00, &cov_x01, &cov_x11,
                 &chisq_x);

  gsl_fit_linear(z, 1, y, 1, 4,
                 &c_y, &m_y, &cov_y00, &cov_y01, &cov_y11,
                 &chisq_y);

  double expected_z = position.z();
  double expected_x = c_x + m_x*expected_z;
  double expected_y = c_y + m_y*expected_z;
  double res_x = position.x()-expected_x;
  double res_y = position.y()-expected_y;
  std::ofstream file("resolution.txt", std::ios::out|std::ios::app);
  file << res_x << " " << res_y << " " << tracker << " " << station << "\n";
  file.close();
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
