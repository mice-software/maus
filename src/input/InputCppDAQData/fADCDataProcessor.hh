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

#ifndef _MAUS_INPUTCPPDATA_FADCDATAPROCESSOR_HH_
#define _MAUS_INPUTCPPDATA_FADCDATAPROCESSOR_HH_

#include <string>
#include <vector>
#include <utility>
#include <algorithm>

#include "json/json.h"

#include "unpacking/MDprocessor.h"

#include "Utils/DAQChannelMap.hh"
#include "DataStructure/DAQData.hh"

////////////////////////////////////////////////////////////////////////////////

/** MDarranger
 * Simple class to hold and arrange the Json document
 * containing all the daq spill information.
 */

namespace MAUS {

class MDarranger : public MDprocessor {
 public:
  MDarranger() {}
  virtual ~MDarranger() {}

  /** Sets the json document to which the MDarranger will write
   */
  void set_JSON_doc(Json::Value *doc) {_docSpill = doc;}

  /** Gets the json document where DAQ data is written
   */
  Json::Value* get_JSON_doc() {return _docSpill;}

 /** This function sets the pointer to the DAQData object.
  * \param[in] dd The pointer to the DAQData objevt.
  */
  static void set_daq_data(MAUS::DAQData *dd) {_daq_data = dd;}

  /** Gets the json document where DAQ data is written
   */
  static MAUS::DAQData* get_daq_data() {return _daq_data;}

  /**
  * This function sets the DAQ map.
  * \param[in] map The DAQ channel map.
  */
  static void set_DAQ_map(DAQChannelMap* map) {_chMap = map;}

  string get_equipment_name() {return _equipment;}

 protected:
  /** The JSON node to put the data under.
  * It is created at the beginning of the spill.
  **/
  Json::Value* _docSpill;


  /**The DAQData object to put the data under.
  * It is created at the beginning of the spill.
  *
  **/
  static MAUS::DAQData  *_daq_data;

  /** The DAQ channel map.
  * It is used to group all channels belonging to a given detector.
  **/
  static DAQChannelMap* _chMap;

  /** The name of the DAQ equipment. **/
  string _equipment;
};

class ZeroSupressionFilter : public MDarranger {

 public:

  ZeroSupressionFilter() :_zero_suppression(0), _zs_threshold(0) {}
  virtual ~ZeroSupressionFilter() {}

  void set_zero_supression(bool zs) { _zero_suppression = zs; }
  void set_zs_threshold(int zst)    { _zs_threshold = zst; }

 protected:

  /// If true - do zero suppression
  bool _zero_suppression;

  /// Value of the threshold used for zero suppression */
  int _zs_threshold;
};

typedef std::vector< std::vector<std::pair<double, double> > > TrackerCalibMap;

class ZeroSupressionFilterTK : public MDarranger {

 public:

  ZeroSupressionFilterTK()
  :_zero_suppression(0), _zs_threshold(0), _calibration(_number_banks) {
    for (auto &c:_calibration)
      c.resize(_number_channels);
  }

  virtual ~ZeroSupressionFilterTK() {}

  void set_zero_supression(bool zs) { _zero_suppression = zs; }
  void set_zs_threshold(int zst)    { _zs_threshold = zst; }
  void set_calibration(TrackerCalibMap calibration) { _calibration = calibration;}
  TrackerCalibMap* get_calibration_ptr() {return &_calibration;}

 protected:

  /// If true - do zero suppression
  bool _zero_suppression;

  /// Value of the threshold used for zero suppression */
  int _zs_threshold;


  /// Arrays containing calibration values for every channel in the 4 banks of the 16 boards.
  static const int _number_channels       = 128;
  static const int _number_banks          = 64;

  TrackerCalibMap _calibration;
};


////////////////////////////////////////////////////////////////////////////////


// do not increment inside following macro! (e.g.: MAX( ++a, ++b );
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define SIGNAL_INTEGRATION_WINDOW 30

class fADCDataProcessor : public ZeroSupressionFilter {
 public:

  fADCDataProcessor() {}
  ~fADCDataProcessor() {}

  /** Return the position of the maximum.
  * This function returns the number of the sample having maximum amplitude.
  */
  int get_max_position();

  /** Return the position of the minimum.
   * This function returns the number of the sample having minimum amplitude.
   */
  int get_min_position();

  int get_arrival_time();

  int print_data();

  /** Return the area of the signal.
  * This function returns the area of the signal. The pedestal is subtracted.
  * The integration is done in window starting 10 samples before the maximum
  * and ending 20 samples after the maximum.
  * \param[out] pos The number of the sample having maximum amplitude.
  */
  int get_pos_signal_area(int &pos);

  int get_neg_signal_area(int &pos);

  int get_pedestal_area(int &pos);

  /** Return the measured value for the given sample
  * \param[in] i The number of the sample.
  */
  int get_point(int i) const { return _data[i]; }

  /// Return the vector of samples
  vector<int> get_data() const { return _data; }

  /** Return the area of the signal.
  * This function returns the area of the signal. The pedestal is subtracted.
  * The integration is done using the whole acquisition window.
  */

  int get_area();

  /// Return the data member _pedestal
  double get_pedestal() const { return _pedestal; }

  /** Return the charge of the signal.
  * This function returns the charge of the signal calculated by the specified
  * algorithm.
  * \param[in] Algorithm identifier of the algorithm.
  */
  int get_charge(int Algorithm = ceaPedMax);

  enum chargeEstimationAlgorithm {
    ceaMinMax, /// Simplest algorithm
    ceaFractionDescriminatorThreshold, /// not implemented
    ceaPedMax,
    ceaPedMin,
  };

  /**Return the charge of the Cherenkov Singal (negative pulse)
  Uses same Algorithm.
  */
  int get_charge_ckov(int Algorithm = ceaPedMin);

  Json::Value get_samples();

 protected:

  void set_pedestal();
  int chargeMinMax();
  int chargePedMax();
  int chargePedMin();
  /// vector of samples (measurements) */
  vector<int> _data;

  /// Pedestal level of the signal */
  double _pedestal;
};
}
#endif
