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

#ifndef _MAUS_SRC_INPUT_INPUTCPPDAQDATA_INPUTCPPDAOFFLINEQDATA_H__
#define _MAUS_SRC_INPUT_INPUTCPPDAQDATA_INPUTCPPDAQOFFLINEDATA_H__

#include <string>

#include "unpacking/MDfileManager.h"

#include "src/input/InputCppDAQData/InputCppDAQData.hh"

namespace MAUS {

class InputCppDAQOfflineData : public InputCppDAQData {

 public:

  /** Create an instance of InputCppDAQData.
  * 
  * This is the constructor for InputCppDAQData.
  *
  * \param[in] pDataPath The (directory) path to read the data from
  * \param[in] pFilename The filename to read from the pDataPath directory
  */
  InputCppDAQOfflineData(std::string pDataPath = "", std::string pFilename = "");

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

 private:

  /** File manager object. */
  MDfileManager _dataFileManager;

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
  int _maxNumEvents;

 /** Counter of the DAQ events.
  */
  int _eventsCount;

 /** If this is true only the phys. evens will be processed.
  */
  bool _phys_Events_Only;

   /** If this is true only the calib. evens will be processed.
  */
  bool _calib_Events_Only;
};
}

#endif




