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

/** @class MapCppTrackerMCDigitization
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _COMPONENTS_MAP_MapCppTrackerMCDigitization_H_
#define _COMPONENTS_MAP_MapCppTrackerMCDigitization_H_

// C headers
#include <json/json.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <cmath>
#include <vector>
#include <string>

#include "Config/MiceModule.hh"
#include "Utils/Squeak.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/API/MapBase.hh"

namespace MAUS {

class MapCppTrackerMCDigitization : public MapBase<Data> {
  public:
    typedef std::vector<int> IntChannelArray;

  /** Constructor - initialises pointers to NULL */
  MapCppTrackerMCDigitization();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerMCDigitization();

 private:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  void _birth(const std::string& argJsonConfigDocument);

  /** @brief Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  void _death();

  /** @brief process MAUS Data object
   *
   *  Receive a object with MC hits and return
   *  a object with digits
   */
  void _process(MAUS::Data * data) const;

  /** @brief builds digits
   */
  void construct_digits(MAUS::SciFiHitArray *hits,
                        int spill_num,
                        int event_num,
                        MAUS::SciFiDigitPArray &digits) const;

  void add_noise(MAUS::SciFiNoiseHitArray *noises,
                 MAUS::SciFiDigitPArray &digits) const;

  /** @brief computes scifi chan numb from GEANT fibre copy numb
   */
  int compute_chan_no(MAUS::SciFiHit *ahit) const;

  /** @brief computes tdc from time.
   */
  int compute_tdc_counts(double time) const;

  /** @brief computes adc from npe.
   */
  double compute_adc_counts(MAUS::SciFiDigit *digit_j) const;

  /** @brief checks if hits belong to the same scifi channel.
   */
  bool check_param(MAUS::SciFiHit *hit1, MAUS::SciFiHit *hit2) const;

//  void discriminator(SciFiDigitPArray &digits) const;

  /** @brief load calibration data
  *
  *  Used to determine calibrated values
  *  for pe from simulated adc.
  *  Identical to data digitization for completeness
  */
  bool load_mapping(std::string file);
  bool load_calibration(std::string file);
  bool load_bad_channels(std::string file);

 private:
  /// The ratio of deposited eV to NPE
  double _eV_to_phe;
  double _SciFiNPECut;
  double _SciFivlpcRes;
  double _SciFitdcBits;
  double _SciFivlpcTimeRes;
  double _SciFitdcFactor;
  double _SciFiFiberConvFactor;
  double _SciFiFiberTrappingEff;
  double _SciFiFiberMirrorEff;
  double _SciFivlpcQE;
  double _SciFiFiberTransmissionEff;
  double _SciFiMUXTransmissionEff;
  double _SciFiadcBits;
//  double _SciFiDisCut;
//  int _disc_sim_on;
  bool _SciFiCalibrateMC;

  static const int _number_channels       = 128;
  static const int _number_banks          = 64;
  static const int _number_boards         = 16;
  static const int _total_number_channels = 6403;

  IntChannelArray _board;
  IntChannelArray _bank;
  IntChannelArray _chan_ro;
  IntChannelArray _tracker;
  IntChannelArray _station;
  IntChannelArray _view;
  IntChannelArray _fibre;
  IntChannelArray _extWG;
  IntChannelArray _inWG;
  IntChannelArray _WGfib;
  std::string _mapping_file;
  std::string _calibration_file;
  std::string _bad_chan_file;
  Json::Value _calibration[_number_banks][_number_channels];
  bool _good_chan[_number_banks][_number_channels];

  /// an array contaning all MiceModules
  std::vector<const MiceModule*> modules;
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
