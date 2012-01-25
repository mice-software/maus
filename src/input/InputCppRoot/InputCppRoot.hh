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

#ifndef _MAUS_SRC_INPUT_INPUTCPPROOT_INPUTCPPROOT_HH__
#define _MAUS_SRC_INPUT_INPUTCPPROOT_INPUTCPPROOT_HH__

#include <string>

#include "json/json.h"

#include "src/legacy/Interface/Squeal.hh"

class MausData;
class JsonCppConverter;
class irstream;

namespace MAUS {

/** @class InputCppRoot
 *
 *  InputCppRoot enables us to read in ROOT files and emit them as strings
 *  containing json formatted data. InputCppRoot uses irstream from
 *  JsonCppStreamer to read ROOT files and JsonCppConverter to convert from
 *  ROOT to json.
 */
class InputCppRoot {
 public:
  /** Constructor for InputCppRoot, initialises all members to NULL
   *
   *  @param filename if set forces the Inputter to use the filename rather than
   *         pulling a filename from the datacards at birth
   */
  explicit InputCppRoot(std::string filename = "");
  /** Destructor for InputCppRoot - calls death()
   */
  ~InputCppRoot();

  /** Initialise the inputter
   *
   *  @param json_datacards json formatted string containing the json datacards
   *  - takes root file from "root_input_filename" parameter
   */
  bool birth(std::string json_datacards);

  /** Deletes inputter member data
   */
  bool death();

  /** Gets the next event from the root file. If there are no more events,
   *  returns an empty string ("")
   */
  std::string getNextEvent();

  /** The emitter - should be overloaded by SWIG script
   */
  std::string emitter() {
    throw(Squeal(Squeal::recoverable, "This function should be overloaded",
                "InputCppRoot::emitter"));
    return "";
  }

 private:
  irstream* _infile;
  JsonCppConverter* _jsonCppConverter;
  MausData* _md;
  Json::Value _val;
  std::string _filename;
};
}

#endif

