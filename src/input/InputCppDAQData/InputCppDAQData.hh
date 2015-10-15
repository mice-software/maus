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

#ifndef _MAUS_SRC_INPUT_INPUTCPPDAQDATA_INPUTCPPDAQDATA_H__
#define _MAUS_SRC_INPUT_INPUTCPPDAQDATA_INPUTCPPDAQDATA_H__


#include <string>
#include <iostream>
#include <sstream>

#include "json/json.h"

#include "unpacking/MDdateFile.h"
#include "unpacking/MDevent.h"
#include "unpacking/MDfileManager.h"
#include "unpacking/MDprocessManager.h"
#include "unpacking/MDequipMap.h"
#include "unpacking/MDfragment.h"

#include "src/common_cpp/API/InputBase.hh"

#include "src/input/InputCppDAQData/UnpackEventLib.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/DAQData.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

/** \class InputCppDAQData
* Load MICE raw data and unpack it into a JSON stream or a Cpp Data format.
*
* InputCppDAQData is a base imput class and can not be used to access the DAQ data!
* The access to the data is provided by the two daughter classes InputCppDAQOfflineData
* and InputCppDAQOnlineData.
*
* This module reads binary data in the format of the MICE DAQ.  It drives the
* 'unpacker' library to do this conversion.  The end result is the MICE data
* in a JSON or a Cpp Data format.  The data includes TDC and flash ADC values, so this
* information is low-level.
*/

class InputCppDAQData : public InputBase<MAUS::Data> {

 public:

  /** Create an instance of InputCppDAQData.
  *
  * This is the constructor for InputCppDAQData.
  */
  InputCppDAQData();

  /** Dummy function.
  * The access to the data is provided by the two daughter classes
  * InputCppDAQOfflineData and InputCppDAQOnlineData.
  *
  * \return false after printing an error message.
  */
  virtual bool readNextEvent();

  /** Unpack the current event into JSON.
  *
  * This unpacks the current event into a JSON document.
  * Don't call this until readNextEvent() has been called and returned true at
  * least once!
  *
  * \return JSON document containing the unpacked DAQ data.
  */
  std::string getCurEvent();

  /** Unpack the current event into Cpp Data.
  *
  * This unpacks the current event into the Cpp Data Structure.
  * Don't call this until readNextEvent() has been called and returned true at
  * least once!
  *
  * \return JSON document containing the unpacked DAQ data.
  */
  int getCurEvent(MAUS::Data *data);

  /** Disable one equipment type.
  * This disables the unpacking of the data produced by all equipment
  * with the specified type.
  */
  void disableEquipment(std::string pEquipType) {
    _dataProcessManager.Disable(pEquipType);
  }

  /* Functions for python use only!
   * They are written in InputCppDAQData.i so that they
   * can use pure python code in the python bindings!
   */

 protected:

  /** Internal emitter function.
  *
  * \return An event string.
  */
  MAUS::Data _emit_cpp() {
     if (this->readNextEvent()) {
       MAUS::Data  data_cpp;
       MAUS::Spill *spill_cpp = new MAUS::Spill;
       data_cpp.SetSpill(spill_cpp);
       this->getCurEvent(&data_cpp);
       _eventsCount++;
       return data_cpp;
     } else {
       throw(MAUS::Exception(Exception::recoverable,
                             "Failed to read next event",
                             "InputCppDAQData::_emit_cpp()"));
    }
  };

  /** Counter of the DAQ events. */
  int _eventsCount;

  /** File manager object. */
  MDfileManager _dataFileManager;

  /** Pointer to the start of the current event. */
  unsigned char *_eventPtr;

  /** Initialise the Unpacker.
  *
  * This protected birth is intended to be called from children
  *
  * This prepares the unpacker to read the data according to the configuration.
  */
  void _childbirth(const std::string& pJSONConfig);

 private:

  void _birth(const std::string& pJSONConfig) {
    _childbirth(pJSONConfig);
  }

  /** Close the file and free memory.
  *
  * This function frees all resources allocated by Birth().
  * It is unlikely this will ever fail!
  */
  void _death();

  /** Initialise the processor.
  * Template function used to initialise a processor
  * according to the configuration.
  */
  template <class procType>
  bool initProcessor(procType* &processor, Json::Value configJSON);

  /** Configure the zero supression filter. */
  void configureZeroSupression(ZeroSupressionFilter* processor, Json::Value configJSON);
  void configureZeroSupressionTK(ZeroSupressionFilterTK* processor, Json::Value configJSON);

  /** Process manager object. */
  MDprocessManager _dataProcessManager;

  /** The DAQ channel map object.
  * It is used to group all measurements belonging to a given detector.*/
  DAQChannelMap _map;

  /** Processor for TDC particle event data. */
  V1290CppDataProcessor  *_v1290PartEventProc_cpp;

  /** Processor for fADC V1724 particle event data. */
  V1724CppDataProcessor  *_v1724PartEventProc_cpp;

  /** Processor for fADC V1731 particle event data. */
  V1731CppDataProcessor  *_v1731PartEventProc_cpp;

  /** Processor for scaler data. */
  V830CppDataProcessor   *_v830FragmentProc_cpp;

  /** Processor for VLSB data. */
  VLSBCppDataProcessor      *_vLSBFragmentProc_cpp;

  /** Processor for DBB data. */
  DBBCppDataProcessor  *_DBBFragmentProc_cpp;

  /** Processor for DBBChain data. */
  DBBChainCppDataProcessor  *_DBBChainFragmentProc_cpp;

  /** Enum of event types */
  enum {
    VmeTdc = 102,
    VmefAdc1724 = 120,
    VmefAdc1731 = 121,
    VmeScaler = 111,
    DBB = 141,
    VLSB_C = 80
  };

  /** Convert the DAQ event type (as coded in DATE) into string.
  * \param[in] pType The type of the event to be converted.
  * \return The type of the event as string.
  */
  std::string event_type_to_str(int pType);

  void resetAllProcessors();
};
}

#endif  // _MAUS_INPUTCPPDAQDATA_INPUTCPPDAQDATA_H__
