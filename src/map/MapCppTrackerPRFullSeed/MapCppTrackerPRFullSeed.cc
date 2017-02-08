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

#include <string>

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/map/MapCppTrackerPRFullSeed/MapCppTrackerPRFullSeed.hh"

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTrackerPRFullSeed(void) {
  PyWrapMapBase<MAUS::MapCppTrackerPRFullSeed>::PyWrapMapBaseModInit
                                        ("MapCppTrackerPRFullSeed", "", "", "", "");
}

MapCppTrackerPRFullSeed::MapCppTrackerPRFullSeed()
    : MapBase<Data>("MapCppTrackerPRFullSeed") {
}

MapCppTrackerPRFullSeed::~MapCppTrackerPRFullSeed() {
}

void MapCppTrackerPRFullSeed::_birth(const std::string& argJsonConfigDocument) {
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
//  Json::Value *json = Globals::GetConfigurationCards();
}

void MapCppTrackerPRFullSeed::_death() {}

void MapCppTrackerPRFullSeed::_process(Data* data) const {
  Spill *spill = data->GetSpill();

  /* return if not physics spill */
  if (spill->GetDaqEventType() != "physics_event")
    return;

  if (spill->GetReconEvents()) {
    for (unsigned int k = 0; k < spill->GetReconEvents()->size(); k++) {
      SciFiEvent *event = spill->GetReconEvents()->at(k)->GetSciFiEvent();
      if (!event) {
        continue;
      }

      event->clear_seeds();

      const SciFiHelicalPRTrackPArray& helical = event->helicalprtracks();
      const SciFiStraightPRTrackPArray& straight = event->straightprtracks();
  
      SciFiSeedPArray seeds;

      for (SciFiHelicalPRTrackPArray::const_iterator it = helical.begin(); 
          it != helical.end(); ++it) {
        seeds.push_back(_make_helical_seed(*it));
      }

      for (SciFiStraightPRTrackPArray::const_iterator it = straight.begin(); 
          it != straight.end(); ++it) {
        seeds.push_back(_make_straight_seed(*it));
      }

      event->set_seeds(seeds);
    }
  } else {
    std::cout << "No recon events found\n";
  }
}


SciFiSeed* MapCppTrackerPRFullSeed::_make_helical_seed(SciFiHelicalPRTrack* helical) const {
  ThreeVector position = helical->get_seed_position();
  ThreeVector momentum = helical->get_seed_momentum();
  std::vector<double> PR_cov = helical->get_covariance();
  TMatrixD vector(5, 1);
  TMatrixD covariance(5, 5);

  vector(0, 0) = position.x();
  vector(1, 0) = momentum.x();
  vector(2, 0) = position.y();
  vector(3, 0) = momentum.y();
  vector(4, 0) = helical->get_charge() / momentum.z();

  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      covariance(i, j) = PR_cov[i*5 + j];
    }
  }

  SciFiSeed* seed = new SciFiSeed();

  seed->setTracker(helical->get_tracker());
  seed->setStateVector(vector);
  seed->setCovariance(covariance);
  seed->setPRTrackTobject(helical);
  seed->setAlgorithm(1);

  return seed;
}

SciFiSeed* MapCppTrackerPRFullSeed::_make_straight_seed(SciFiStraightPRTrack* straight) const {
  ThreeVector position = straight->get_seed_position();
  ThreeVector momentum = straight->get_seed_momentum();
  std::vector<double> PR_cov = straight->get_covariance();
  TMatrixD vector(4, 1);
  TMatrixD covariance(4, 4);

  vector(0, 0) = position.x();
  vector(1, 0) = momentum.x()/momentum.z();
  vector(2, 0) = position.y();
  vector(3, 0) = momentum.y()/momentum.z();

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      covariance(i, j) = PR_cov[i*4 + j];
    }
  }

  SciFiSeed* seed = new SciFiSeed();

  seed->setTracker(straight->get_tracker());
  seed->setStateVector(vector);
  seed->setCovariance(covariance);
  seed->setPRTrackTobject(straight);
  seed->setAlgorithm(0);

  return seed;
}

} // ~namespace MAUS

