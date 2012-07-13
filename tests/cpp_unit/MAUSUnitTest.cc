/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

// Will run tests in all files included below
// If you just want to run against a subset of files
// comment the rest out and recompile

#include <string>

#include "gtest/gtest.h"

#include "json/value.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/GlobalsHandling/GlobalsManagerFactory.hh"

Json::Value SetupConfig() {
  Json::Value config(Json::objectValue);
  config["reconstruction_geometry_filename"] = "Test.dat";
  config["simulation_geometry_filename"] = "Test.dat";
  config["maximum_number_of_steps"] = 10000;
  config["keep_tracks"] = true;
  config["keep_steps"] = true;
  config["verbose_level"] = 2;
  config["geant4_visualisation"] = false;
  config["physics_model"] = "QGSP_BERT";
  config["reference_physics_processes"] = "mean_energy_loss";
  config["physics_processes"] = "standard";
  config["particle_decay"] = true;
  config["charged_pion_half_life"] = -1.;
  config["muon_half_life"] = -1.;
  config["production_threshold"] = 0.5;
  config["production_threshold"] = 0.5;
  return config;
}

int main(int argc, char **argv) {
  MAUS::GlobalsManagerFactory::InitialiseGlobalsManager
                                     (JsonWrapper::JsonToString(SetupConfig()));
  /*
  MICERun::getInstance()->jsonConfiguration = new Json::Value(SetupConfig());
  std::cerr << MICERun::getInstance()->jsonConfiguration << std::endl;
  std::cerr << JsonWrapper::JsonToString(*(MICERun::getInstance()->jsonConfiguration)) << std::endl;
  dataCards MyDataCards(0);
  MICERun::getInstance()->DataCards = &MyDataCards;
  MICERun::getInstance()->miceModule = new MiceModule("Test.dat");  // delete
  MICERun::getInstance()->miceMaterials = new MiceMaterials();  // delete
  fillMaterials(*MICERun::getInstance());
  Squeak::setOutput(Squeak::debug, Squeak::nullOut());
  Squeak::setStandardOutputs();
  */
  ::testing::InitGoogleTest(&argc, argv);
  int test_out = -1;
  try {
      test_out = RUN_ALL_TESTS();
  } catch(Squeal squee) {
      std::cerr << squee.GetMessage() << "\n" << squee.GetLocation() << "\n"
                << squee.GetStackTrace() << std::endl;
  } catch(std::exception exc) {
      std::cerr << "Caught std::exception" << "\n" << exc.what() << std::endl;
  }
  MAUS::GlobalsManagerFactory::DeleteGlobalsManager();
  return test_out;
}


