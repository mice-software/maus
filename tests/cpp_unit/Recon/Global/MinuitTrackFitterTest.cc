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

#include "TMinuit.h"
#include "TObject.h"
#include "gtest/gtest.h"
#include "json/reader.h"
#include "json/value.h"

#include "Utils/Exception.hh"
#include "Utils/JsonWrapper.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "Recon/Global/MinuitTrackFitter.hh"

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
    configuration_ = JsonWrapper::StringToJson(kConfigurationString);
  }

  ~MinuitTrackFitterTest() {
  }

 protected:
  static const std::string kConfigurationString;
  Json::Value configuration_;
};

// *************************************************
// MinuitTrackFitterTest static const initializations
// *************************************************

const std::string MinuitTrackFitterTest::kConfigurationString = "{"
"\"global_recon_minuit_minimizer\": \'MINIMIZE\'"
"\"global_recon_minuit_max_iterations\": 2000"
"\"global_recon_minuit_max_edm\": 1e-4"
"\"global_recon_minuit_rounds\": 7"
"\"global_recon_minuit_parameters\": ["
"  {\"name\":\"Time\", \"fixed\":False, \"initial_value\":0.,"
"   \"value_step\":0.1, \"min_value\":-3.0, \"max_value\":3.0},"
"  {\"name\":\"Energy\", \"fixed\":False, \"initial_value\":226.19,"
"   \"value_step\":0.01, \"min_value\":150., \"max_value\":300.},"
"  {\"name\":\"x\", \"fixed\":False, \"initial_value\":0.,"
"   \"value_step\":0.1, \"min_value\":-30., \"max_value\":30.},"
"  {\"name\":\"Px\", \"fixed\":False, \"initial_value\":0.,"
"   \"value_step\":0.1, \"min_value\":-5., \"max_value\":5.},"
"  {\"name\":\"y\", \"fixed\":False, \"initial_value\":0.,"
"   \"value_step\":0.1, \"min_value\":-30., \"max_value\":30.},"
"  {\"name\":\"Py\", \"fixed\":False, \"initial_value\":0.,"
"   \"value_step\":0.1, \"min_value\":-5., \"max_value\":5.}"
"]"
"}";

// ***********
// test cases
// ***********

TEST_F(MinuitTrackFitterTest, Constructor) {
  TMinuit * minimizer = new TMinuit(6);

  DummyOpticsModel optics_model(&configuration_);
  minimizer->SetObjectFit(this);

  minimizer->SetFCN(
    Recon::common_cpp_optics_recon_minuit_track_fitter_score_function);
  // Recon::common_cpp_optics_recon_minuit_track_fitter_score_function();
}

