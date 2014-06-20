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

#ifndef _MAUS_SRC_INPUT_INPUTCPPDAQONLINEDATA_INPUTCPPDAQONLINEDATA_H__
#define _MAUS_SRC_INPUT_INPUTCPPDAQONLINEDATA_INPUTCPPDAQONLINEDATA_H__

#include <time.h>

#include <string>

#include "daq/MDmonitoring.hh"

#include "src/input/InputCppDAQData/InputCppDAQData.hh"

namespace MAUS {
/** \class InputCppDAQOnlineData
* This class is used to access the DAQ data online.
*/
class InputCppDAQOnlineData : public InputCppDAQData {

 public:

  /** Create an instance of InputCppDAQOnlineData.
  *
  * This is the constructor for InputCppDAQOnlineData.
  */
  InputCppDAQOnlineData();

  /** Read the next event from the file into memory.
  *
  * This function simply reads an event into memory,
  * it doesn't unpack the event anymore than required to read it.
  *
  * \return True if an event was read ready for unpacking.
  */
  bool readNextEvent();

  /** Set the monitor source by a filename
   *
   *  \param mon file name of the source
   */
  void setMonitorSrc(std::string mon);

 private:
  /** Initialise the Unpacker.
  *
  * This prepares the unpacker to read from a socket given in the configuration.
  *
  * \return True if initialised sucessfully.
  */
  void _birth(const std::string& pJSONConfig);


  /** Data manager object. */
  MDmonitoring * _dataManager;
  struct timespec _sleep_time; // time.h
};
}

#endif  // _MAUS_INPUTCPPDAQDATA_INPUTCPPDAQDATA_H__
