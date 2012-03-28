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

// namespace MAUS {

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
  std::cout << "Number of spacepoints in spill: " << evt.spacepoints().size() << std::endl;

  // Split spacepoints up according to which tracker they occured in
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_tracker(_n_trackers);
  for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {  // Loop over trackers
    for ( unsigned int i = 0; i < evt.spacepoints().size(); ++i ) {  // Loop over spacepoints
      if ( evt.spacepoints()[i]->get_tracker() == trker_no ) {
        spnts_by_tracker[trker_no].push_back(evt.spacepoints()[i]);
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
    std::cout << "Station\tSpacepoints\n";
    for ( int i = 0; i < static_cast<int>(spnts_by_station.size()); ++i ) {
      std::cout << i+1 << "\t" <<spnts_by_station[i].size() << std::endl;
      if ( spnts_by_station[i].size() > 0 ) {
        ++stations_hit;
      }
    }

    // Make the tracks depending on how many stations have spacepoints in them
    if (stations_hit == 5) {
      std::vector<SciFiStraightPRTrack> trks;
      make_spr_5pt(spnts_by_tracker[trker_no], trks );
      for ( int i = 0; i < static_cast<int>(trks.size()); ++i ) {
        evt.add_straightprtrack(trks[i]);
      }
    }
    // if (evt.straightprtracks().size() == 0 && stations_hit > 3)
    //  make_spr_4pt(spnts_by_tracker[trker_no], evt.straightprtracks());
    // if (evt.straightprtracks().size() == 0 && stations_hit > 2)
    //   make_spr_3pt(spnts_by_tracker[trker_no], evt.straightprtracks());
  }// ~Loop over trackers
  std::cout << "Number of tracks found: " << evt.straightprtracks().size() << "\n\n";
}

void PatternRecognition::make_spr_5pt(const std::vector<SciFiSpacePoint*>& spnts,
                                      std::vector<SciFiStraightPRTrack>& trks) {
} // ~make_spr_5pt

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
  for ( int stat_no = 0; stat_no < static_cast<int>(spnts_stat.size()); ++stat_no ) {
    for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
      if ( spnts[i]->get_station() == stat_no + 1 ) {
        spnts_stat[stat_no].push_back(spnts[i]);
      }
    }
  }
}
// } // ~namespace MAUS
