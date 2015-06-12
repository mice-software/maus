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

#include <getopt.h>

#include <string>
#include <sstream>

#include "gtest/gtest.h"

#include "json/value.h"
#include "TApplication.h"
#include "TROOT.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"

extern char *optarg;

// return value is verbose_level, if we change the flags we will need to change
// the call structure slightly...
int parse_flags(int argc, char **argv) {
    int _verbose_level = 1;
    int _help = 2;
    option long_options[] = {
              {"verbose_level", required_argument, NULL, _verbose_level},
              {"help", no_argument, NULL, _help},
              {"gtest_list_tests", no_argument, NULL, 0},
              {"gtest_filter", required_argument, NULL, 0},
              {"gtest_also_run_disabled_tests", no_argument, NULL, 0},
              {"gtest_repeat", required_argument, NULL, 0},
              {"gtest_shuffle", no_argument, NULL, 0},
              {"gtest_random_seed", required_argument, NULL, 0},
              {NULL, 0, NULL, 0}
    };
    int option = 0;
    int option_index = 0;
    while (option != -1) {
        option = getopt_long_only(argc, argv, "", long_options, &option_index);
        if (option == _verbose_level) {
            std::stringstream ss(optarg);
            int verbose_level = 2;
            ss >> verbose_level;
            return verbose_level;
        } else if (option == _help) {
            std::cerr << "\n\nThis test program takes MAUS defined options:\n"
                      << "      --verbose_level=(integer)\n"
                      << "      verbose level for MAUS IO\n"
                      << std::endl;
        }// else probably a gtest flag, so ignore
    }
    return 2;
}

Json::Value SetupConfig(int verbose_level) {
  std::cerr << "Running with verbose level " << verbose_level << std::endl;
  Json::Value config(Json::objectValue);
  config["maximum_module_depth"] = 50;
  config["check_volume_overlaps"] = true;
  config["reconstruction_geometry_filename"] = "Test.dat";
  config["simulation_geometry_filename"] = "Test.dat";
  config["simulation_geometry_debug"] = false;
  config["maximum_number_of_steps"] = 10000;
  config["will_do_stack_trace"] = true;
  config["keep_tracks"] = true;
  config["keep_steps"] = true;
  config["verbose_level"] = verbose_level;
  config["geant4_visualisation"] = false;
  config["physics_model"] = "QGSP_BERT";
  config["reference_physics_processes"] = "mean_energy_loss";
  config["physics_processes"] = "standard";
  config["particle_decay"] = true;
  config["polarised_decay"] = true;
  config["charged_pion_half_life"] = -1.;
  config["muon_half_life"] = -1.;
  config["production_threshold"] = 0.5;
  config["fine_grained_production_threshold"] = Json::Value(Json::objectValue);
  config["default_keep_or_kill"] = true;
  config["spin_tracking"] = true;
  config["keep_or_kill_particles"] = "{\"neutron\":False}";
  config["kinetic_energy_threshold"] = 0.1;
  config["max_step_length"] = 100.;
  config["max_track_time"] = 1.e9;
  config["max_track_length"] = 1.e8;
  config["simulation_reference_particle"] = JsonWrapper::StringToJson(
    std::string("{\"position\":{\"x\":0.0,\"y\":-0.0,\"z\":-5500.0},")+
    std::string("\"spin\":{\"x\":0.0,\"y\":-0.0,\"z\":1.0},")+
    std::string("\"momentum\":{\"x\":0.0,\"y\":0.0,\"z\":1.0},")+
    std::string("\"particle_id\":-13,\"energy\":226.0,\"time\":0.0,")+
    std::string("\"random_seed\":10}")
  );
  config["stepping_algorithm"] = "ClassicalRK4";
  config["delta_one_step"] = -1.;
  config["delta_intersection"] = -1.;
  config["epsilon_min"] = -1.;
  config["epsilon_max"] = -1.;
  config["miss_distance"] = -1.;
  config["everything_special_virtual"] = false;
  config["field_tracker_absolute_error"] = 1.e-4;
  config["field_tracker_relative_error"] = 1.e-4;

  return config;
}

int main(int argc, char **argv) {
  int test_out = -1;
  try {
      std::cout << "Initialising Globals" << std::endl;
      int verbose_level = parse_flags(argc, argv);
      MAUS::GlobalsManager::InitialiseGlobals(
                JsonWrapper::JsonToString(SetupConfig(verbose_level))
      );
      ::testing::InitGoogleTest(&argc, argv);
      std::cout << "Running tests" << std::endl;
      // Required for ImageTest, otherwise we get a segv; we get a segv from
      // ORStreamTest if TApplication is instantiated then deleted, so best
      // instantiate at this level.
      TApplication theApp("App", &argc, argv);
      gROOT->SetBatch(true);
      test_out = RUN_ALL_TESTS();
  } catch (MAUS::Exception exc) {
      std::cerr << exc.GetMessage() << "\n" << exc.GetLocation() << "\n"
                << exc.GetStackTrace() << std::endl;
  } catch (std::exception exc) {
      std::cerr << "Caught std::exception" << "\n" << exc.what() << std::endl;
  }
  MAUS::GlobalsManager::Finally();
  return test_out;
}


