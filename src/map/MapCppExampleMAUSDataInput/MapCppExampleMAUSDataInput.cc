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

#include "src/map/MapCppExampleMAUSDataInput/MapCppExampleMAUSDataInput.hh"

#include "src/common_cpp/API/APIExceptions.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppExampleMAUSDataInput(void) {
  PyWrapMapBase<MAUS::MapCppExampleMAUSDataInput>::PyWrapMapBaseModInit
                                ("MapCppExampleMAUSDataInput", "", "", "", "");
}

MapCppExampleMAUSDataInput::MapCppExampleMAUSDataInput()
    : MapBase<MAUS::Data>("MapCppExampleMAUSDataInput") {}

void MapCppExampleMAUSDataInput::_birth(const std::string& argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    throw(Exceptions::Exception(Exceptions::nonRecoverable,
                 "Unable to Parse Json Config",
                 "MapCppExampleMAUSDataInput::_birth"));
  }
}

void MapCppExampleMAUSDataInput::_death() {}

void MapCppExampleMAUSDataInput::_process(Data* data) const {
  int spill_number = data->GetSpill()->GetSpillNumber();
  data->GetSpill()->SetSpillNumber(spill_number+1);
}
} // ~MAUS
