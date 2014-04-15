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

#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Interface/dataCards.hh"
#include "DataStructure/MCEvent.hh"
#include "DataStructure/Spill.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/map/MapCppEmrMCDigitization/MapCppEmrMCDigitization.hh"

namespace MAUS {

bool MapCppEmrMCDigitization::birth(std::string argJsonConfigDocument) {
//   std::cerr << "*** MapCppEmrMCDigitization::birth ***" << std::endl;
  _classname = "MapCppEmrMCDigitization";

  try {
    //  JsonCpp setup
    Json::Value configJSON;
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

    return true;
  } catch (Exception exc) {
    CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
  } catch (std::exception exc) {
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

bool MapCppEmrMCDigitization::death() {
  return true;
}

std::string MapCppEmrMCDigitization::process(std::string document) {
  std::cerr << "*** MapCppEmrMCDigitization::process(string) ***" << std::endl;
  Json::Value spill_json;
  try {
    spill_json = JsonWrapper::StringToJson(document);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
//     ss << squee.what() << std::endl;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

  Data* spill_cpp;
  try {
    spill_cpp = JsonCppSpillConverter().convert(&spill_json);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
//     ss << squee.what() << std::endl;
    ss << _classname << " says: Failed to convert the input document into Data";
    errors["bad_json_document"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

  process(spill_cpp);

  Json::Value* spill_json_out;
  try {
    spill_json_out = CppJsonSpillConverter().convert(spill_cpp);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
//     ss << squee.what() << std::endl;
//     ss << _classname << " says: Failed to convert the output Data into Json";
    errors["bad_cpp_data"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

//   std::cerr << (*spill_json_out)["recon_events"] << std::endl;
  document = JsonWrapper::JsonToString(*spill_json_out);

  delete spill_cpp;
  delete spill_json_out;
  return document;
}

void MapCppEmrMCDigitization::process(MAUS::Data *data) {
//   std::cerr << "*** MapCppEmrMCDigitization::process(Data) ***" << std::endl;
  int nPartEvents = data->GetSpill()->GetMCEventSize();
//   std::cerr << " MC events: " << nPartEvents << std::endl;
  if (!nPartEvents)
      return;

  for (int xPe = 0; xPe < nPartEvents; xPe++) {
//     std::cerr << " MC event " << xPe << std::endl;
    EMRHitArray *hits = data->GetSpill()->GetAnMCEvent(xPe).GetEMRHits();
    if (hits) {
      int nHits = hits->size();
//       std::cerr << " numEMRHits: " << nHits << std::endl;
      for (int xHit = 0; xHit < nHits; xHit++) {
//         std::cerr << " hit " << xHit << std::endl;
        EMRChannelId *id = hits->at(xHit).GetChannelId();
        std::cerr << "hit in EMR bar: " << id->GetBar()<< std::endl;
      }
    }
  }
}
}


