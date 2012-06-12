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

// C headers
#include <assert.h>

// Other headers
#include "Config/MiceModule.hh"
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"

#include "gtest/gtest.h"

// MAUS namespace {
class SciFiClusterRecTest : public ::testing::Test {
 protected:
  SciFiClusterRecTest()  {
    cluster_exception = 80;
    min_npe = 2.0;
    // Get the tracker modules.
    std::string filename;
    filename = "Stage6.dat";
    MiceModule*      _module;
    _module = new MiceModule(filename);
    modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

    SciFiCluster *cluster = new SciFiCluster();

    event.add_cluster(cluster);
  }
  virtual ~SciFiClusterRecTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}

  std::vector<const MiceModule*> modules;
  int cluster_exception;
  double min_npe;
  SciFiEvent event;
};

// process(SciFiEvent &evt, std::vector<const MiceModule*> modules)

TEST_F(SciFiClusterRecTest, test_process) {
  SciFiClusterRec clustering(cluster_exception, min_npe);
  clustering.process(event, modules);

  // EXPECT_EQ(tracker,1);
  // EXPECT_EQ(station,2.0);
  /*
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel1 = 106;
  double npe1 = 3.2;
  double time = 12.2;
  // Construct a digit.
  SciFiDigit *digit = new SciFiDigit(tracker, station, plane, channel1, npe1, time);
  // Start a cluster from "digit"
  SciFiCluster *cluster = new SciFiCluster(digit);

  cluster->construct(modules);*/
  // think of tests for direction and position...
}
// } // namespace
