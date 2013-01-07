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

#include "Interface/Squeal.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

bool MapCppTrackerDigits::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerDigits";
}

bool MapCppTrackerDigits::death() {
  return true;
}

std::string MapCppTrackerDigits::process(std::string document) {
  Json::FastWriter writer;
  Spill spill;

  try {
    // Load input.
    root = JsonWrapper::StringToJson(document);
    if ( root.isMember("daq_data") && !(root["daq_data"].isNull()) ) {
      // Get daq data.
      Json::Value daq = root.get("daq_data", 0);
      // Process the input.
      RealDataDigitization real;
      real.process(spill, daq);
      // Save to JSON output.
      save_to_json(spill);
    }
  } catch(Squeal& squee) {
    squee.Print();
    root = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(root, squee, _classname);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  return writer.write(root);
}

void MapCppTrackerDigits::save_to_json(Spill &spill) {
  SpillProcessor spill_proc;
  root = *spill_proc.CppToJson(spill, "");
}

} // ~namespace MAUS
