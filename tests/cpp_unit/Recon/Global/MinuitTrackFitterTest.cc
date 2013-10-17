/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include <cstdlib>

#include "TLorentzVector.h"
#include "TMinuit.h"
#include "TObject.h"
#include "gtest/gtest.h"
#include "json/reader.h"
#include "json/value.h"

#include "Utils/Exception.hh"
#include "Utils/JsonWrapper.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/MinuitTrackFitter.hh"
#include "Recon/Global/Particle.hh"

namespace GlobalDS = MAUS::DataStructure::Global;
namespace Recon = MAUS::recon::global;

Json::Value SetupConfig(int verbose_level);

class DummyOpticsModel : public MAUS::OpticsModel {
 public:
  explicit DummyOpticsModel(Json::Value const * const configuration)
      : MAUS::OpticsModel(configuration) { }
  void Build() { }
  MAUS::CovarianceMatrix Transport(const MAUS::CovarianceMatrix & covariances,
                                   const double end_plane) const
  {return covariances;}
  MAUS::CovarianceMatrix Transport(const MAUS::CovarianceMatrix & covariances,
                                   const double start_plane,
                                   const double end_plane) const
  {return covariances;}
  MAUS::PhaseSpaceVector Transport(const MAUS::PhaseSpaceVector & vector,
                                   const double end_plane) const
  {return vector;}
  MAUS::PhaseSpaceVector Transport(const MAUS::PhaseSpaceVector & vector,
                                   const double start_plane,
                                   const double end_plane) const
  {return vector;}
};

class MinuitTrackFitterTest : public testing::Test, public TObject {
 public:
  MinuitTrackFitterTest() {
    for (int index = 0; index < 3; ++index) {
      GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
      TLorentzVector four_position(kStartVector[2]+index, kStartVector[4]+index,
                                    0.0, kStartVector[0]+index);
      track_point->set_position(four_position);
      TLorentzVector position_error(1.0, 1.0, 1.0, 1.0);
      track_point->set_position_error(position_error);

      TLorentzVector four_momentum(kStartVector[3]+index, kStartVector[5]+index,
                                    280.7780432, kStartVector[1]+index);
      track_point->set_momentum(four_momentum);
      TLorentzVector momentum_error(1.0, 1.0, 1.0, 1.0);
      track_point->set_momentum_error(momentum_error);
      detector_events_.push_back(track_point);
    }
  }

  ~MinuitTrackFitterTest() {
    for (int index = 0; index < 3; ++index) {
      delete detector_events_[index];
    }
  }

 protected:
  static const std::string kConfigurationString;
  static const Json::Value kConfiguration;
  static const double kMuonMass;
  static const Double_t kStartVector[6];
  std::vector<const GlobalDS::TrackPoint *> detector_events_;
  static const DummyOpticsModel kOpticsModel;
};

// *************************************************
// MinuitTrackFitterTest static const initializations
// *************************************************

const std::string MinuitTrackFitterTest::kConfigurationString = "{"
"\"global_recon_minuit_minimizer\":\"MINIMIZE\",\n"
"\"global_recon_minuit_max_iterations\":2000,\n"
"\"global_recon_minuit_max_edm\":1e-9,\n"
"\"global_recon_minuit_rounds\":7,\n"
"\"global_recon_minuit_parameters\":[\n"
  "{\"name\":\"Time\", \"fixed\":false, \"initial_value\":0.,"
  " \"value_step\":0.1, \"min_value\":-3.0, \"max_value\":3.0},\n"
  "{\"name\":\"Energy\", \"fixed\":false, \"initial_value\":226.19,\n"
  " \"value_step\":0.01, \"min_value\":250., \"max_value\":350.},\n"
  "{\"name\":\"x\", \"fixed\":false, \"initial_value\":0.,\n"
  " \"value_step\":0.1, \"min_value\":-15., \"max_value\":15.},\n"
  "{\"name\":\"Px\", \"fixed\":false, \"initial_value\":0.,\n"
  " \"value_step\":0.1, \"min_value\":-15., \"max_value\":15.},\n"
  "{\"name\":\"y\", \"fixed\":false, \"initial_value\":0.,"
  " \"value_step\":0.1, \"min_value\":-15., \"max_value\":15.},\n"
  "{\"name\":\"Py\", \"fixed\":false, \"initial_value\":0.,"
  " \"value_step\":0.1, \"min_value\":-15., \"max_value\":15.}"
"]"
"}";

const double MinuitTrackFitterTest::kMuonMass
  = Recon::Particle::GetInstance().GetMass(GlobalDS::kMuMinus);

const Json::Value MinuitTrackFitterTest::kConfiguration
  = JsonWrapper::StringToJson(kConfigurationString);

const DummyOpticsModel MinuitTrackFitterTest::kOpticsModel
  = DummyOpticsModel(&kConfiguration);

const Double_t MinuitTrackFitterTest::kStartVector[6] = {
  0.0, 300.0, 5.0, 7.0, 9.0, 11.0 };

// ***********
// test cases
// ***********

TEST_F(MinuitTrackFitterTest, Constructor) {
  bool success = true;
  try {
    Recon::MinuitTrackFitter fitter(&kOpticsModel, 0.);
  } catch (MAUS::Exception exception) {
    success = false;
  }
  EXPECT_TRUE(success);
}

TEST_F(MinuitTrackFitterTest, ScoreTrack) {
  Double_t score = Recon::MinuitTrackFitter::ScoreTrack(
                      kStartVector,
                      kOpticsModel,
                      kMuonMass,
                      detector_events_);
  // chi^2 = 6*0^2 + 6*1^2 + 6*2^2 = 30. (see init. of detector_events_)
  EXPECT_NEAR(30.0, score, 1e-9);
}

TEST_F(MinuitTrackFitterTest, Fit) {
    Recon::MinuitTrackFitter fitter(&kOpticsModel, 0.);

    std::vector<const GlobalDS::TrackPoint *> raw_track_points;
    GlobalDS::Track raw_track;
    for (int index = 0; index < 3; ++index) {
      GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
      TLorentzVector four_position(kStartVector[2]+index, kStartVector[4]+index,
                                    0.0, kStartVector[0]+index);
      track_point->set_position(four_position);
      TLorentzVector position_error(1.0, 1.0, 1.0, 1.0);
      track_point->set_position_error(position_error);

      TLorentzVector four_momentum(kStartVector[3]+index, kStartVector[5]+index,
                                    280.7780432, kStartVector[1]+index);
      track_point->set_momentum(four_momentum);
      TLorentzVector momentum_error(1.0, 1.0, 1.0, 1.0);
      track_point->set_momentum_error(momentum_error);

      raw_track_points.push_back(track_point);
      raw_track.AddTrackPoint(track_point);
    }

    GlobalDS::Track fit_track;
    fitter.Fit(&raw_track, &fit_track, "MinuitTrackFitterTest");

    // Check fit_track
    for (int index = 0; index < 3; ++index) {
      delete raw_track_points[index];
    }

    std::vector<const GlobalDS::TrackPoint *> fit_track_points
      = fit_track.GetTrackPoints();
    ASSERT_EQ(size_t(4), fit_track_points.size());

    const GlobalDS::TrackPoint * fit_primary_point = fit_track_points[0];
    Recon::DataStructureHelper helper = Recon::DataStructureHelper::GetInstance();
    MAUS::PhaseSpaceVector fit_primary_vector
      = helper.TrackPoint2PhaseSpaceVector(*fit_primary_point);

    /* Since the dummy optics model does identity transport, one would expect
     * the fit to settle on the 2nd detector event given the way the
     * detector events are defined
     */
    for (int index = 0; index < 6; ++index) {
      EXPECT_NEAR(kStartVector[index]+1., fit_primary_vector[index], 1.e-6);
    }
}
