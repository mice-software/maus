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
}

void PatternRecognition::straightprtrack_recon(SciFiEvent &evt) {
  // std::cout << "Begining Pattern Recognition" << std::endl;
  // std::cout << "Number of spacepoints in spill: " << evt.scifispacepoints.size() << std::endl;

  // Split spacepoints up according to which tracker they occured in
  // ----------------------------------------------------------------

  // Construct a 2D matrix to hold spacepoints
  // 1st index differentiates between trackers, 2nd index the spacepoints
  std::vector< std::vector<SciFiSpacePoint*> > spnts(_n_trackers);
  // the output of this is spnts[2].. trackers are separated.
  /*for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
    for ( int i = 0; i < evt.scifispacepoints.size(); ++i ) {
      if ( evt.scifispacepoints[i]->get_tracker() == trker_no ) {
        spnts[trker_no].push_back(evt.scifispacepoints[i]);
      }
    }
     std::cout << "Spacepoints in Tracker " << trker_no << ": ";
     std::cout << spnts[trker_no].size() << std::endl;
  }*/
  for ( int i = 0; i < evt.scifispacepoints.size(); ++i ) {
    int trker_no = evt.scifispacepoints[i]->get_tracker();
    spnts[trker_no].push_back(evt.scifispacepoints[i]);
  }

  // Count the number of spacepoints per tracker per station
  // and the number of stations hit in each tracker
  // --------------------------------------------------------

  // Construct a 2*5 matrix of ints to hold the number of sp per station per tracker
  std::vector< std::vector<int> > spnts_per_station(2, std::vector<int>(_n_stations, 0));
  std::vector<int> stations_hit(2, 0);

  // stations_hit... stores the number of stations hit per tracker.
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
    // std::cout << stations_hit[trker_no] << " stations hit in Tracker " << trker_no+1 << std::endl;
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
  // if ( evt.scifistraightprtracks.size() == 0 )
    // std::cout << "No tracks found" << std::endl;
}

void PatternRecognition::make_spr_5pt(const std::vector<SciFiSpacePoint*>& spnts,
                                      std::vector<SciFiStraightPRTrack>& trks) {
  // std::cout << "Making 5 point track" << std::endl;

  static const int _n_stations = 5;
  static const double res_cut = 15.0;

  std::vector< std::vector<SciFiSpacePoint*> > spnts_stat(_n_stations);
  sort_by_station(spnts, spnts_stat);

  // Form an initial track between sp in station 5 and station 1
  // -----------------------------------------------------------

  bool success = false;

  for ( int stat5 = 0; stat5 < spnts_stat[_n_stations - 1].size(); ++stat5 ) {
    if ( success ) break;
    for ( int stat1 = 0; stat1 < spnts_stat[0].size(); ++stat1 ) {
      if ( success ) break;

      Hep3Vector pos_5 = spnts_stat[4][stat5]->get_position();
      Hep3Vector pos_1 = spnts_stat[0][stat1]->get_position();

      double m_xi = ( pos_1.x() - pos_5.x()) / (pos_1.z() - pos_5.z() );
      double m_yi = ( pos_1.y() - pos_5.y()) / (pos_1.z() - pos_5.z() );

      double x_0i = pos_5.x() - ( pos_5.z() * m_xi );
      double y_0i = pos_5.y() - ( pos_5.z() * m_yi );

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
          if ( fabs(dx) < res_cut && fabs(dy) < res_cut ) {
            good_spnts[stat_no] = spnts_stat[stat_no][sp_no];
            seeds[stat_no] = spnts_stat[stat_no][sp_no];
            // std::cout << "Good sp found" << std::endl;
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
        double x[_n_stations] = { pos_5.x(), good_spnts[3]->get_position().x(),
                                  good_spnts[2]->get_position().x(),
                                  good_spnts[1]->get_position().x(),
                                  pos_1.x() };

        double y[_n_stations] = { pos_5.y(), good_spnts[3]->get_position().y(),
                                  good_spnts[2]->get_position().y(),
                                  good_spnts[1]->get_position().y(),
                                  pos_1.y() };

        double z[_n_stations] = { pos_5.z(), good_spnts[3]->get_position().z(),
                                  good_spnts[2]->get_position().z(),
                                  good_spnts[1]->get_position().z(),
                                  pos_1.z() };

        double c_x, m_x, cov_x00, cov_x01, cov_x11, chisq_x;
        double c_y, m_y, cov_y00, cov_y01, cov_y11, chisq_y;

        gsl_fit_linear(z, 1, x, 1, _n_stations,
                       &c_x, &m_x, &cov_x00, &cov_x01, &cov_x11,
                       &chisq_x);

        gsl_fit_linear(z, 1, y, 1, _n_stations,
                       &c_y, &m_y, &cov_y00, &cov_y01, &cov_y11,
                       &chisq_y);

        /*std::cout << "Track parameters x: c = " << c_x << ", m = ";
        std::cout << m_x << ", chisq = " << chisq_x << std::endl;
        std::cout << "Track parameters y: c = " << c_y << ", m = ";
        std::cout << m_y << ", chisq = " << chisq_y << std::endl;
        std::cout << "sp\tx\ty\tz" << std::endl;
        for ( int i = 0; i < _n_stations; ++i ) {
          std::cout << i << "\t" << x[i] << "\t" << y[i] << "\t" << z[i] << std::endl;
        }
        */
        double chisq = chisq_x + chisq_y;
        if ( chisq / ( _n_stations - 2 ) < _chisq_cut ) {
          // std:: cout << "chisq test passed, adding track" << std::endl;
          SciFiStraightPRTrack track;
          track.set_x0(c_x);
          track.set_mx(m_x);
          track.set_y0(c_y);
          track.set_my(m_y);
          trks.push_back(track);
          success = true;
        } else {
          std:: cout << "chisq test failed, track rejected" << std::endl;
        }
      } else {
        continue;
      }
    }
  }

  if ( success ) {
    run_analysis();
  }

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
