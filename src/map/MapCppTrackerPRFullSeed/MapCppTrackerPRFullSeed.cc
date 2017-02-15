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
#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

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
  std::vector<double> PR_cov_vector = helical->get_covariance();
  TMatrixD vector(5, 1);
  TMatrixD jacobian(5, 5);
  TMatrixD PR_cov(5, 5);
  TMatrixD covariance(5, 5);

  vector(0, 0) = position.x();
  vector(1, 0) = momentum.x();
  vector(2, 0) = position.y();
  vector(3, 0) = momentum.y();
  vector(4, 0) = helical->get_charge() / momentum.z();

  // Format: x_0x_0, x_0y_0, x_0r, y_0x_0, y_0y_0, y_0r, rx_0, ry_0, rr, z_0z_0, z_0m, mz_0, mm
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      PR_cov(i, j) = PR_cov_vector[i*3 + j];
    }
  }
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      PR_cov(i+3, j+3) = PR_cov_vector[9 + i*2 + j];
    }
  }


  int tracker = helical->get_tracker();
  double length = Globals::GetSciFiGeometryHelper()->GetSeedDistance(tracker);

  double r  = helical->get_R();
  double Bz = Globals::GetSciFiGeometryHelper()->GetFieldValue(tracker);
  double pt = - helical->get_charge()*CLHEP::c_light*Bz*r;
  double m = - helical->get_dsdz();
  double x0 = helical->get_circle_x0();
  double y0 = helical->get_circle_y0();
  double s = (helical->get_line_sz_c() - length*m);
  double phi = s / r;

  double C = cos(phi);
  double S = sin(phi);

  double dphi_dr = -s/(r*r);
  double dphi_dz = 1.0/r;
  double dphi_dm = length/r;
  double dpt_dr = helical->get_charge()*CLHEP::c_light*Bz;

  jacobian(0, 0) = 1.0;
  jacobian(0, 1) = 0.0;
  jacobian(0, 2) = C - r*S*dphi_dr;
  jacobian(0, 3) = -r*S*dphi_dz;
  jacobian(0, 4) = -r*S*dphi_dm;
  jacobian(1, 0) = 0.0;
  jacobian(1, 1) = 0.0;
  jacobian(1, 2) = -S*dpt_dr - pt*C*dphi_dr;
  jacobian(1, 3) = -pt*C*dphi_dz;
  jacobian(1, 4) = -pt*C*dphi_dm;
  jacobian(2, 0) = 0.0;
  jacobian(2, 1) = 1.0;
  jacobian(2, 2) = S + r*C*dphi_dr;
  jacobian(2, 3) = r*C*dphi_dz;
  jacobian(2, 4) = r*C*dphi_dm;
  jacobian(3, 0) = 0.0;
  jacobian(3, 1) = 0.0;
  jacobian(3, 2) = C*dpt_dr - pt*S*dphi_dr;
  jacobian(3, 3) = -pt*S*dphi_dz;
  jacobian(3, 4) = -pt*S*dphi_dm;
  jacobian(4, 0) = 0.0;
  jacobian(4, 1) = 0.0;
  jacobian(4, 2) = -(m/(pt*pt))*dpt_dr;
  jacobian(4, 3) = 0.0;
  jacobian(4, 4) = 1.0/pt;

  TMatrixD jacobianT(TMatrixD::kTransposed, jacobian);

  covariance = jacobian*PR_cov*jacobianT;

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

  // Format: xx, xm_x, m_xx, m_xm_x, yy, ym_y, m_yy, m_ym_y
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      covariance(i, j) = PR_cov[i*2 + j];
    }
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 2; ++j) {
      covariance(i, j+2) = PR_cov[i*2 + j];
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

