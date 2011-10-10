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

#ifndef _MAUS_INPUTCPPDATA_INPUTCPPDATA_H__
#define _MAUS_INPUTCPPDATA_INPUTCPPDATA_H__

#include <json/json.h>
#include <event.h>
#include <MDdateFile.h>
#include <MDevent.h>
#include <MDfileManager.h>
#include <MDprocessManager.h>

#include <string>
#include <iostream>

#include "src/input/InputCppData/UnpackEventLib.hh"
#include "Utils/DAQChannelMap.hh"
#include "Interface/Squeak.hh"

 /** \class InputCppData
  * Load MICE raw data and unpack it into a JSON stream.
  *
  * This module reads data in the format of the MICE DAQ.  It drives the
  * 'unpacker' library to do this conversion.  The end result is the MICE data
  * in JSON format.  The data includes TDC and flash ADC values, so this
  * information is low-level.
  *
  */

class InputCppData {

 public:

 /** Create an instance of InputCppData.
  * 
  * This is the constructor for InputCppData.
  *
  * \param[in] pDataPath The (directory) path to read the data from
  * \param[in] pFilename The filename to read from the pDataPath directory
  */
  InputCppData(std::string pDataPath = "", std::string pFilename = "");

 /** Initialise the Unpacker.
  *
  * This prepares the unpacker to read the files given in the constructor.
  *
  * \return True if at least one file was opened sucessfully.
  */
  bool birth(std::string pJSONConfig);

 /** Read the next event from the file into memory.
  *
  * This function simply reads an event into memory,
  * it doesn't unpack the event anymore than required to read it.
  *
  * \return True if an event was read ready for unpacking.
  */
  bool readNextEvent();

 /** Unpack the current event into JSON.
  *
  * This unpacks the current event and returns the JSON data for it.
  * Don't call this until readNextEvent() has been called and returned
  * true at least once!
  *
  * \return The current event data in JSON format.
  */
  std::string getCurEvent();

 /** Disable one equipment type.
  * This disables the unpacking of the data produced by all equipment
	* with the specified type.
	*/
	void disableEquipment(std::string pEquipType) {
    _dataProcessManager.Disable(pEquipType);
  }

 /** Close the file and free memory.
  *
  * This function frees all resources allocated by Birth().
  * It is unlikely this will ever fail!
  *
  * \return True if successful.
  */
  bool death();

  /* Functions for python use only!
   * They are written in InputCppData.i so that they
   * can use pure python code in the python bindings!
   */

 /** Internal emitter function.
  *
  * When called from C++, this function does nothing.
  * From python (where it is overriden by the bindings,
  * it returns an iterable result which allows access to all events.
  *
  * \return An empty string from C++.
  */
  std::string emitter() {
     return "";
  };

 private:

 /** Process manager object.
  */
  MDprocessManager _dataProcessManager;

 /** File manager object.
  */
  MDfileManager _dataFileManager;

 /** The DAQ channel map object.
  * It is used to group all measurements belonging to a given detector.
  */
  DAQChannelMap _map;

 /** Processor for TDC particle event data.
  */
  V1290DataProcessor*  _v1290PartEventProc;

 /** Processor for fADC V1724 particle event data.
  */
  V1724DataProcessor*  _v1724PartEventProc;

 /** Processor for fADC V1731 particle event data.
  */
  V1731DataProcessor*  _v1731PartEventProc;

 /** Processor for scaler data.
  */
  V830DataProcessor*  _v830FragmentProc;

 /** Processor for VLSB data.
  */
  VLSBDataProcessor* _vLSBFragmentProc;

 /** Processor for DBB data.
  */
  DBBDataProcessor* _DBBFragmentProc;

 /** Pointer to the start of the current event.
  */
  unsigned char *_eventPtr;

 /** Paths to the data.
  * This string has to contain one or more space separated paths.
  */
  std::string _dataPaths;

 /** File and run names within _dataPaths.
  * This string has to contain one or more space separated
  * file names or run numbers.
  */
  std::string _datafiles;

 /** Max number of DAQ events to be processed.
  */ 
  int _maxNumDaqEvents;

 /** Counter of the DAQ events.
  */
  int _daqEventsCount;

 /** Enum of event types
  */
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
};

#endif  // _MAUS_INPUTCPPDATA_INPUTCPPDATA_H__
