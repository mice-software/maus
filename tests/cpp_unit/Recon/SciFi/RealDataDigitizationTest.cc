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

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Recon/SciFi/RealDataDigitization.hh"

namespace MAUS {

class RealDataDigitizationTest : public ::testing::Test {
 protected:
  RealDataDigitizationTest()  {}
  virtual ~RealDataDigitizationTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(RealDataDigitizationTest, test_calibration_load) {
  RealDataDigitization test_case_1;
  bool bad_calib = test_case_1.load_calibration("nonsense.txt");
  EXPECT_FALSE(bad_calib);
  RealDataDigitization test_case_2;
  bool good_calib = test_case_2.load_calibration("scifi_calibration_30_09_2011.txt");
  EXPECT_TRUE(good_calib);
/*
  std::vector<Json::Value> calibration_data[16][4];
  calibration_data = test_case_2.get_calibration();
  int num_boards = 16;
  int num_banks = 4;
  int num_channels = 128;
  EXPECT_EQ(num_boards, calibration_data.size());
  for ( int board_i = 0; board_i < num_boards; board_i++ ) {
    EXPECT_EQ(num_banks, calibration_data[board_i].size());
    for ( int bank_i = 0; bank_i < num_banks; bank_i++ ) {
      EXPECT_EQ(num_channels, calibration_data[board_i][bank_i].size());
    }
  }
*/
}

TEST_F(RealDataDigitizationTest, test_mapping_load) {
  RealDataDigitization test_case;
  bool bad_map = test_case.load_mapping("nonsense.txt");
  EXPECT_FALSE(bad_map);
  bool good_map = test_case.load_mapping("mapping_7.txt");
  EXPECT_TRUE(good_map);
  int missing_channel_counter = 0;
  for ( int board = 0; board < 16; board++ ) {
    for ( int bank = 0; bank < 4; bank++ ) {
      for ( int chan_ro = 0; chan_ro < 128; chan_ro++ ) {
        int tracker, station, plane, channel;
        test_case.get_StatPlaneChannel(board, bank, chan_ro,
                                       tracker, station, plane, channel);
        if ( tracker == -1 ) {
          missing_channel_counter += 1;
        }
      }
    }
  }
  // 14 waveguides missing (7 per tracker)
  // means 14 (wg) * 128 (chan/wg) = 1792 channels missing
  int number_missing_channels = 14*128;
  // have to take this test out till we sort out
  // our calibration...
  EXPECT_EQ(number_missing_channels, missing_channel_counter);
  int board = 0;
  int bank = 0;
  int chan_ro = 0;
  int tracker = 0;
  int station = 0;
  int plane = 0;
  int channel = 0;
  int bad_bank = 99;

  bool correct = test_case.get_StatPlaneChannel(board, bank, chan_ro,
                                                tracker, station, plane, channel);
  EXPECT_TRUE(correct);
  bool wrong = test_case.get_StatPlaneChannel(board, bad_bank, chan_ro,
                                                tracker, station, plane, channel);
  EXPECT_FALSE(wrong);
}

TEST_F(RealDataDigitizationTest, test_bad_channel_load) {
  RealDataDigitization test_case;
  bool bad_channels = test_case.load_bad_channels();
  EXPECT_TRUE(bad_channels);
  int bad_channel[3] = {1, 1, 17};
  int good_channel[3]= {1, 2, 3};
  int out_of_range[3]= {16, 4, 128};
  EXPECT_FALSE(test_case.is_good_channel(bad_channel[0], bad_channel[1], bad_channel[2]));
  EXPECT_TRUE(test_case.is_good_channel(good_channel[0], good_channel[1], good_channel[2]));
  EXPECT_FALSE(test_case.is_good_channel(out_of_range[0], out_of_range[1], out_of_range[2]));
}

TEST_F(RealDataDigitizationTest, test_read_in_all_boards) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string file="scifi_calibration_30_09_2011.txt";
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerDigits/"+file;
  std::ifstream inf(fname.c_str());

  RealDataDigitization test_case;
  test_case.read_in_all_Boards(inf);
}

TEST_F(RealDataDigitizationTest, test_process) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  std::string file="lab7_unpacked";
  std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/map/MapCppTrackerDigits/"+file;
  std::ifstream inf(fname.c_str());

  Spill spill;

  std::string line;
  getline(inf, line);
  getline(inf, line);

  Json::Value root = JsonWrapper::StringToJson(line);
  Json::Value daq = root.get("daq_data", 0);

  RealDataDigitization test_case;
  test_case.process(spill, daq);
  ASSERT_TRUE(spill.GetReconEvents()->size() > 0);
  // Needs more...
}

} // ~namespace MAUS
