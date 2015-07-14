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

#ifndef _MAUS_INPUTCPPDATA_UNPACKEVENTLIB_HH_
#define _MAUS_INPUTCPPDATA_UNPACKEVENTLIB_HH_

#include <stdint.h>
#include <typeinfo>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include "json/json.h"

#include "unpacking/MDprocessor.h"
#include "unpacking/MDpartEventV1290.h"
#include "unpacking/MDpartEventV1724.h"
#include "unpacking/MDpartEventV1731.h"
#include "unpacking/MDfragmentVLSB.h"
#include "unpacking/MDfragmentVLSB_C.h"
#include "unpacking/MDfragmentV830.h"
#include "unpacking/MDfragmentDBB.h"
#include "unpacking/MDfragmentDBBChain.h"
#include "unpacking/MDequipMap.h"

#include "src/input/InputCppDAQData/fADCDataProcessor.hh"

namespace MAUS {

////////////////////////////////////////////////////////////////////////////////

/** On Particle Event V1290
 * This class unpacks a V1290 (TDC) board hit.
 */
class V1290DataProcessor : public MDarranger {
 public:
  V1290DataProcessor() {_equipment = "V1290";}
  virtual ~V1290DataProcessor() {}

 /** Unpack a single event to JSON.
  *
  * This function unpacks a single particle event,
  * recorded by equipment CAEN V1290 (TDC)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * Will be casted to MDpartEventV1290.
  */
  virtual int Process(MDdataContainer* dc);
};

typedef std::vector<MAUS::V1290>           V1290HitArray;
typedef std::vector<V1290HitArray>   V1290PartEventArray;

class V1290CppDataProcessor : public MDarranger {
 public:
  V1290CppDataProcessor() {_equipment="V1290";}
  virtual ~V1290CppDataProcessor() {}

 /** Unpack a single event to MAUS Data Structure..
  *
  * This function unpacks a single event,
  * into the Data Structure tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment CAEN V1290 (TOF TDC board)
  * Will be casted to MDfragmentV1290.
  */
  virtual int Process(MDdataContainer* dc);

 /**
  * This function uses the Part Event Array of the different detectors
  * to fill into the DAQData object.
  */
  void fill_daq_data();

  void reset();

  private:

  V1290PartEventArray _tof0_spill;
  V1290PartEventArray _tof1_spill;
  V1290PartEventArray _tof2_spill;
  V1290PartEventArray _tr_spill;
  V1290PartEventArray _tr_req_spill;
  V1290PartEventArray _unknown_spill;
};


////////////////////////////////////////////////////////////////////////////////

/** On Particle Event V1724
 * This class unpacks a V1724 (fADC) board hit.
 */
#define V1724_SAMPLES_PER_WORD   2
class V1724DataProcessor : public fADCDataProcessor {
 public:
  V1724DataProcessor() :fADCDataProcessor() {_equipment="V1724";}
  virtual ~V1724DataProcessor() {}

  /** Unpack a single event to JSON.
  *
  * This function unpacks a single particle event,
  * recorded by equipment CAEN V1724 (flash ADC)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * Will be casted to MDpartEventV1724.
  */
  virtual int Process(MDdataContainer* dc);
};

typedef std::vector<MAUS::V1724>           V1724HitArray;
typedef std::vector<V1724HitArray>   V1724PartEventArray;

class V1724CppDataProcessor : public fADCDataProcessor {
 public:
  V1724CppDataProcessor() {_equipment="V1724";}
  virtual ~V1724CppDataProcessor() {}

 /** Unpack a single event to MAUS Data Structure..
  *
  * This function unpacks a single event,
  * into the Data Structure tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment V1724 (TOF or KL board)
  * Will be casted to MDfragmentV1724.
  */
  virtual int Process(MDdataContainer* dc);

 /**
  * This function uses the Part Event Array of the different detectors
  * to fill into the DAQData object.
  */
  void fill_daq_data();

  void reset();

 private:

  V1724PartEventArray _tof0_spill;
  V1724PartEventArray _tof1_spill;
  V1724PartEventArray _tof2_spill;
  V1724PartEventArray _kl_spill;
  V1724PartEventArray _tag_spill;
  V1724PartEventArray _unknown_spill;
};

////////////////////////////////////////////////////////////////////////////////

/** On Particle Event V1731
 * This class unpacks a V1731 (fADC) board hit.
 */
#define V1731_SAMPLES_PER_WORD   4
class V1731DataProcessor : public fADCDataProcessor {
 public:
  V1731DataProcessor() :fADCDataProcessor() {_equipment="V1731";}
  virtual ~V1731DataProcessor() {}

 /** Unpack a single event to JSON.
  *
  * This function unpacks a single particle event,
  * recorded by equipment CAEN V1731 (flash ADC)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * Will be casted to MDpartEventV1731.
  */
  virtual int Process(MDdataContainer* dc);
};

typedef std::vector<MAUS::V1731>           V1731HitArray;
typedef std::vector<V1731HitArray>   V1731PartEventArray;

class V1731CppDataProcessor : public fADCDataProcessor {
 public:
  V1731CppDataProcessor() {_equipment="V1731";}
  virtual ~V1731CppDataProcessor() {}

 /** Unpack a single event to MAUS Data Structure..
  *
  * This function unpacks a single event,
  * into the Data Structure tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment V1731 (EMR or CKOV board)
  * Will be casted to MDfragmentV1731.
  */
  virtual int Process(MDdataContainer* dc);

 /**
  * This function uses the Part Event Array of the different detectors
  * to fill into the DAQData object.
  */
  void fill_daq_data();

  void reset();

 private:

  V1731PartEventArray _emr_spill;
  V1731PartEventArray _ckov_spill;
  V1731PartEventArray _unknown_spill;
};

////////////////////////////////////////////////////////////////////////////////

/** On Fragment Event V830
 * This class unpacks a V830 (scaler) board hit.
 */
class V830DataProcessor : public MDarranger {
 public:
  V830DataProcessor() {_equipment="V830";}
  virtual ~V830DataProcessor() {}

 /** Unpack a single event to JSON.
  *
  * This function unpacks a single spill event,
  * recorded by equipment CAEN V830 (scaler)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * Will be casted to MDpartEventV830.
  */
  virtual int Process(MDdataContainer* dc);
};

class V830CppDataProcessor : public MDarranger {
 public:
  V830CppDataProcessor() {_equipment="V830";}
  virtual ~V830CppDataProcessor() {}

 /** Unpack a single event to MAUS Data Structure..
  *
  * This function unpacks a single event,
  * into the Data Structure tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment V830 (scaler)
  * Will be casted to MDfragmentV830.
  */
  virtual int Process(MDdataContainer* dc);

 /**
  * This function uses the Part Event Array of the different detectors
  * to fill into the DAQData object.
  */
  void fill_daq_data();

  void reset();

  private:

  MAUS::V830 _v830_spill;
};

////////////////////////////////////////////////////////////////////////////////

/** On Fragment Event VLSB
 * This class unpacks a VLSB board hit (tracker cosmic test in Lab7).
 */
class VLSBDataProcessor : public ZeroSupressionFilter {
 public:
  VLSBDataProcessor() :ZeroSupressionFilter() {_equipment="VLSB";}
  virtual ~VLSBDataProcessor() {}

 /** Unpack a single event part to JSON.
  *
  * This function unpacks a single particle event,
  * recorded by equipment VLSB (tracker board)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * Will be casted to MDfragmentVLSB.
  */
  virtual int Process(MDdataContainer* dc);
};

/** On Fragment Event VLSB
 * This class unpacks a VLSB board hit (tracker cosmic test in Lab7).
 */

typedef std::vector<MAUS::VLSB>           TrackerHitArray;
typedef std::vector<TrackerHitArray>      TtackerPartEventArray;

class VLSBCppDataProcessor : public ZeroSupressionFilter {
 public:
  VLSBCppDataProcessor()
  : ZeroSupressionFilter(), _current_pEvent(-1),
  _n_pEvent(0), _is_first(true) {_equipment="VLSB";}

  virtual ~VLSBCppDataProcessor() {}

 /** Unpack a single event part to JSON.
  *
  * This function unpacks a single particle event,
  * recorded by equipment VLSB (tracker board)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * Will be casted to MDfragmentVLSB.
  */
  virtual int Process(MDdataContainer* dc);

  /**
  * This function uses the Part Event Array of the different detectors
  * to fill into the DAQData object.
  */
  void fill_daq_data();

  void reset();

  private:

  int _current_pEvent;
  int _n_pEvent;
  bool _is_first;

  TtackerPartEventArray _tracker1_spill;
  TtackerPartEventArray _tracker0_spill;
//   TtackerPartEventArray _single_st_spill;
};

////////////////////////////////////////////////////////////////////////////////

/** On Fragment Event VLSB_C
 * This class unpacks a VLSB_C board hit (tracker cosmic test in Lab7).
 */
class VLSB_CDataProcessor : public ZeroSupressionFilter {
 public:
  VLSB_CDataProcessor() :ZeroSupressionFilter() {_equipment="VLSB_C";}
  virtual ~VLSB_CDataProcessor() {}

 /** Unpack a single event part to JSON.
  *
  * This function unpacks a single particle event,
  * recorded by equipment VLSB_C (tracker board)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * Will be casted to MDfragmentVLSB_C.
  */
  virtual int Process(MDdataContainer* dc);
};

////////////////////////////////////////////////////////////////////////////////

/** On Fragment Event DBB
 * This class unpacks a DBB (EMR) board hit.
 */
class DBBDataProcessor : public MDarranger {
 public:
  DBBDataProcessor() {_equipment="DBB";}
  virtual ~DBBDataProcessor() {}

 /** Unpack a single event part to JSON.
  *
  * This function unpacks a single particleevent,
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment DBB (EMR board)
  * Will be casted to MDfragmentDBB.
  */
  virtual int Process(MDdataContainer* dc);
};

#define DBB_TRIGGER_CHANNEL 4

class DBBCppDataProcessor : public MDarranger {
 public:
  DBBCppDataProcessor()  {_equipment="DBB";}

  virtual ~DBBCppDataProcessor() {}

 /** Unpack a single event part to MAUS Data Structure.
  *
  * This function unpacks a single particleevent,
  * into the Data Structure tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment DBB (EMR board)
  * Will be casted to MDfragmentDBB.
  */
  virtual int Process(MDdataContainer* dc);

  static void set_spill_data(MDfragmentDBB *fragment, MAUS::DBBSpillData *dbb_spill);

 /**
  * This function uses the Part Event Array of the different detectors
  * to fill into the DAQData object.
  */
  void fill_daq_data();

  void reset() {_spill.clear();}

 private:

  MAUS::DBBArray _spill;
};

class DBBChainCppDataProcessor : public MDarranger {
 public:
  DBBChainCppDataProcessor()  {_equipment="DBBChain";}

  virtual ~DBBChainCppDataProcessor() {}

 /** Unpack a single event part to the MAUS Data Structure.
  *
  * This function unpacks a single particleevent,
  * into the MAUS Data Structur tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment a chain of 6 DBBs (EMR boards)
  * Will be casted to MDfragmentDBBChain.
  */
  virtual int Process(MDdataContainer* dc);

 /**
  * This function uses the Part Event Array of the different detectors
  * to fill into the DAQData object.
  */
  void fill_daq_data();

  void reset() {_spill.clear();}

 private:

  MAUS::DBBArray _spill;
};
}

#endif  // _MAUS_INPUTCPPDATA_UNPACKEVENTLIB_H__



