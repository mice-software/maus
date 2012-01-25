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

#include "json/json.h"

#include "src/legacy/Interface/Squeal.hh"

class MausData;
class JsonCppConverter;
class irstream;

namespace MAUS {

class InputCppRoot {
 public:
  InputCppRoot(std::string filename = "");
  ~InputCppRoot();

  bool birth(std::string json_datacards);
  bool death();
  std::string getNextEvent();

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

