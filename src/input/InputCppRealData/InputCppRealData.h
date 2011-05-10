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
#include <MDdateFile.h>
#include <MDevent.h>

#include <string>


/** \class InputCppRealData
  * Load MICE raw data and unpack it into a JSON stream.
  *
  * This class is a prototype "RealData" module for MAUS.
  * This is based on the original MICE RealData Module.
  *
  */
class InputCppRealData {
private
:
/** Debug flag,
  * 
  * When set to true, "random" debugging strings will be sent to cerr!
  */
  bool _debug;

/** Pointer to open file object. */
  MDdateFile *_inputFile;

/** Pointer to the start of the current event. */
  unsigned char *_eventPtr;

/** Process an event.
  *
  * This function processes an MDevent into parts,
  * returning the relevant JSON sub-tree in pDoc.
  *
  * \param[in,out] pEvent The event to unpack.
  * \param[in,out] pDoc A reference to the JSON (sub)root node to fill.
  */
  void processLDCEvent(MDevent *pEvent,
                       Json::Value &pDoc);

/** Process a detector specific event fragment.
  *
  * This function simply unpacks a single piece of event,
  * using the UnpackEventLib classes.
  *
  * \param[in,out] pPartEvntPtr The event part to process.
  * \param[in] pEquipType The equipment type ID.
  * \param[in] pLdcId The crate number the event came from (I think).
  * \param[in,out] pDoc A reference to the JSON (sub)root node to fill.
  */
  void processHits(void *pPartEvntPtr,
                   unsigned int pEquipType,
                   unsigned int pLdcId,
                   Json::Value &pDoc);

  /* Enum of event types */
  enum {
    VmeTdc = 102,
    VmefAdc1724 = 120,
    VmefAdc1731 = 121,
    VmeScaler = 111
  };

public
:

/** Create an instance of InputCppRealData.
  * 
  * This is the constructor for InputCppRealData.
  *
  * \param[in] pDataPath The (directory) path to read the data from
  * \param[in] pFilename The filename to read from the pDataPath directory
  */
  InputCppRealData(std::string pDataPath = "", std::string pFilename = "");

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

/** Close the file and free memory.
  * 
  * This function frees all resources allocated by Birth().
  * It is unlikely this will ever fail!
  *
  * \return True if successful.
  */
  bool death();

  ~InputCppRealData();

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
