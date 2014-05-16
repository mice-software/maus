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

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/map/MapCppTrackerDigits/MapCppTrackerDigits.hh"

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTrackerDigits(void) {
  PyWrapMapBase<MAUS::MapCppTrackerDigits>::PyWrapMapBaseModInit
                                        ("MapCppTrackerDigits", "", "", "", "");
}

MapCppTrackerDigits::MapCppTrackerDigits()
    : MapBase<Data>("MapCppTrackerDigits") {}

MapCppTrackerDigits::~MapCppTrackerDigits() {
}

void MapCppTrackerDigits::_birth(const std::string& argJsonConfigDocument) {
}

void MapCppTrackerDigits::_death() {
}

void MapCppTrackerDigits::_process(Data* data) const {
  Json::Value& json_root = *(ConverterFactory().convert<Data, Json::Value>(data));
  if ( json_root.isMember("daq_data") && !(json_root["daq_data"].isNull()) ) {
    // Get daq data.
    Json::Value daq = json_root.get("daq_data", 0);
    // Fill spill object with
    RealDataDigitization real;
    real.initialise();
    real.process(data->GetSpill(), daq);
  }
  delete &json_root;
}
} // ~namespace MAUS
