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
void PatternRecognition::process(TrackerEvent &evt) {
  straightprtrack_recon(evt);
}

void PatternRecognition::straightprtrack_recon(TrackerEvent &evt) {
  std::cout << "Begining Pattern Recognition" << std::endl;
  std::cout << "Number of spacepoints in spill: " << evt.scifispacepoints.size() << std::endl;

  static const int n_trackers = 2;
  static const int n_stations = 5;

  // Split spacepoints up according to which tracker they occured in
  // ----------------------------------------------------------------

  // Construct a 2D matrix to hold spacepoints
  // 1st index differentiates between trackers, 2nd index the spacepoints
  std::vector< std::vector<SciFiSpacePoint*> > spnts(n_trackers);

  for ( int trker_no = 0; trker_no < n_trackers; ++trker_no ) {
    for ( int i = 0; i < evt.scifispacepoints.size(); ++i ) {
      if ( evt.scifispacepoints[i]->get_tracker() == trker_no ) {
        spnts[trker_no].push_back(evt.scifispacepoints[i]);
      }
    }
    std::cout << "Spacepoints in Tracker " << trker_no << ": ";
    std::cout << spnts[trker_no].size() << std::endl;
  }

  // Count the number of spacepoints per tracker per station
  // and the number of stations hit in each tracker
  // --------------------------------------------------------

  // Construct a 2*5 matrix of ints to hold the number of sp per station per tracker
  std::vector< std::vector<int> > spnts_per_station(2, std::vector<int>(n_stations, 0));

  std::vector<int> stations_hit(2, 0);

  for ( int trker_no = 0; trker_no < n_trackers; ++trker_no ) {
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
      std::cout << "Tracker " << trker_no+1 << " Station " << i+1;
      std::cout << " sp: " << spnts_per_station[trker_no][i] << std::endl;
      if ( spnts_per_station[trker_no][i] > 0 ) {
        ++stations_hit[trker_no];
      }
    }
    std::cout << stations_hit[trker_no] << " stations hit in Tracker " << trker_no+1 << std::endl;
  }

  // Make the tracks depending on how many stations have spacepoints in them
  // -----------------------------------------------------------------------

  for ( int trker_no = 0; trker_no < n_trackers; ++trker_no ) {
    if (stations_hit[trker_no] == 5)
      make_spr_5pt(spnts[trker_no], evt.scifistraightprtracks);
    if (evt.scifistraightprtracks.size() == 0 && stations_hit[trker_no] > 3)
      make_spr_4pt(spnts[trker_no], evt.scifistraightprtracks);
    if (evt.scifistraightprtracks.size() == 0 && stations_hit[trker_no] > 2)
      make_spr_3pt(spnts[trker_no], evt.scifistraightprtracks);
  }
  if ( evt.scifistraightprtracks.size() == 0 )
    std::cout << "No tracks found" << std::endl;
}

void PatternRecognition::make_spr_5pt(const std::vector<SciFiSpacePoint*>& spnts,
                                      std::vector<SciFiStraightPRTrack>& trks) {
  std::cout << "Making 5 point track" << std::endl;

  static const int n_stations = 5;
  static const double res_cut = 15.0;

  std::vector< std::vector<SciFiSpacePoint*> > spnts_stat(n_stations);
  sort_by_station(spnts, spnts_stat);

  // Form an initial track between sp in station 5 and station 1
  // -----------------------------------------------------------

  Hep3Vector pos_5 = spnts_stat[4][0]->get_position();
  Hep3Vector pos_1 = spnts_stat[0][0]->get_position();

  double m_xi = ( pos_5.x() - pos_1.x()) / (pos_5.z() - pos_1.z() );
  double m_yi = ( pos_5.y() - pos_1.y()) / (pos_5.z() - pos_1.z() );

  double x_0i = pos_1.x() - ( pos_1.z() * m_xi );
  double y_0i = pos_1.y() - ( pos_1.z() * m_yi );

  std::cout << "m_xi = " << m_xi << "\tx_0i = " << x_0i << std::endl;
  std::cout << "m_yi = " << m_yi << "\ty_0i = " << y_0i << std::endl;

  // Map to hold deviation of each intermediate sp from initial track
  // std::map< int, std::vector<double> > dx_i;
  // std::map< int, std::vector<double> > dy_i;

  // Map to hold the good sp in each station
  std::map< int, std::vector<SciFiSpacePoint*> > good_spnts;

  // Loop over intermediate stations
  for ( int stat_no = 1; stat_no < 4; ++stat_no ) {
    // Loop over sp in a station
    for ( int i = 0; i < spnts_stat[stat_no].size(); ++i ) {
      Hep3Vector pos = spnts_stat[stat_no][i]->get_position();
      double dx = pos.x() - ( x_0i + ( pos.z() * m_xi ) );
      double dy = pos.y() - ( y_0i + ( pos.z() * m_yi ) );
      if (fabs(dx) < res_cut && fabs(dy) < res_cut) {
        good_spnts[stat_no].push_back(spnts_stat[stat_no][i]);
	std::cout << "Good sp found" << std::endl;
      }
      std::cout << "dx = " << dx << "\tdy = " << dy << std::endl;
    }
  }

  // Check we have at least 1 good sp in all the intermediate stations
  std::cout << "Number of stations with good sp: " << good_spnts.size() << std::endl;
  if (good_spnts.size() < 3) {
    std::cout << "Failed to find good sp in all stations" << std::endl;
  }

  for (std::map< int, std::vector<SciFiSpacePoint*> >::iterator ii = good_spnts.begin();
      ii != good_spnts.end(); ++ii) {
    std::cout << (*ii).first << ": x = ";
    std::cout << ( ( ( (*ii).second)[0] )->get_position() ).x() << std::endl;
  }
}

void PatternRecognition::make_spr_4pt(const std::vector<SciFiSpacePoint*>& spnts,
				      std::vector<SciFiStraightPRTrack>& trks) {
  std::cout << "Making 4 point track" << std::endl;
}

void PatternRecognition::make_spr_3pt(const std::vector<SciFiSpacePoint*>& spnts,
				      std::vector<SciFiStraightPRTrack>& trks) {
  std::cout << "Making 3 point track" << std::endl;
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
