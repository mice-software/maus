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

#ifndef _MAUS_INPUTCPPREALDATA_INPUTCPPREALDATA_H__
#define _MAUS_INPUTCPPREALDATA_INPUTCPPREALDATA_H__

#include <json/json.h>
#include <event.h>
#include <MDdateFile.h>
#include <MDevent.h>
#include <MDfileManager.h>
#include <MDprocessManager.h>

#include <string>
#include <iostream>
#include "UnpackEventLib.hh"
#include "RawDataProcessing.hh"
#include "Utils/DAQChannelMap.hh"

/** \class InputCppRealData
  * Load MICE raw data and unpack it into a JSON stream.
  *
  * This class is a prototype "RealData" module for MAUS.
  * This is based on the original MICE RealData Module.
  *
  */
class InputCppRealData{
private
:
/** Debug flag,
  *
  * When set to true, "random" debugging strings will be sent to cerr!
  */
  bool _debug;

/** Pointer to open file object. */
  MDdateFile *_inputFile;

/** Process manager object. */
  MDprocessManager _dataProcessManager;

/** File manager object. */
  MDfileManager _dataFileManager;

/** The DAQ channel map object.
* It is used to group all channels belonging to a given detector.*/
  DAQChannelMap map;

/** Processor for TDC particle event data. */
  V1290DataProcessor*  _v1290PartEventProc;

/** Processor for fADC V1724 particle event data. */
  V1724DataProcessor*  _v1724PartEventProc;

/** Processor for fADC V1731 particle event data. */
  V1731DataProcessor*  _v1731PartEventProc;

/** Processor for scaler data. */
  V830DataProcessor*  _v830FragmentProc;

/** Processor for VLSB cosmic data. */
  VLSBDataProcessor* _vLSBFragmentProc;


  DBBDataProcessor* _DBBFragmentProc;

/** Pointer to the start of the current event. */
  unsigned char *_eventPtr;

  /** Path to the data. */
  std::string _dataPath;

  /** Filename within _dataPath. */
  std::string _runNum;

  /** Enum of event types */
  enum {
    VmeTdc = 102,
    VmefAdc1724 = 120,
    VmefAdc1731 = 121,
    VmeScaler = 111,
		DBB = 141
  };

	/** Convert the DAQ event type (as coded in DATE) into string
	* \param[in] pType The type of the event to be converted
	*/
  string event_type_to_str(int pType);

public
:

/** Create an instance of InputCppRealData.
  *
  * This is the constructor for InputCppRealData.
  *
  * \param[in] pDataPath The (directory) path to read the data from
  * \param[in] pFilename The filename to read from the pDataPath directory
  */
  InputCppRealData(std::string pDataPath = "", std::string pRunNum = "");

/** Initialise the Unpacker.
  *
  * This prepares the unpacker to read the file given in the constructor.
  *
  * \return True if the file was opened sucessfully.
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
	void disableEquipment(std::string pEquipType)
	{ _dataProcessManager.Disable(pEquipType);};

/** Close the file and free memory.
  *
  * This function frees all resources allocated by Birth().
  * It is unlikely this will ever fail!
  *
  * \return True if successful.
  */
  bool death();

  /* Functions for python use only!
   * They are written in InputCppRealData.i so that they
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
};

#endif  // _MAUS_INPUTCPPREALDATA_INPUTCPPREALDATA_H__
