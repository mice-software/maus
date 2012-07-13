/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include "json/json.h"

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/GlobalsHandling/RunActionManager.hh"
#include "src/common_cpp/GlobalsHandling/GlobalsManager.hh"
#include "src/map/MapCppStartOfRun/MapCppStartOfRun.hh"

namespace MAUS {
MapCppStartOfRun::MapCppStartOfRun() : _last_run_number(0), _first(true),
                                       _classname("MapCppStartOfRun") {
}

MapCppStartOfRun::~MapCppStartOfRun() {}

bool MapCppStartOfRun::birth(std::string argJsonConfigDocument) {
    return true;
}

bool MapCppStartOfRun::death() {
    return true;
}

std::string MapCppStartOfRun::process(std::string json_doc) {
    Json::Value json;
    try {json = JsonWrapper::StringToJson(json_doc);}
    catch(...) {
      return std::string("{\"errors\":{\"bad_json_document\":")+
             std::string("\"Failed to parse input document\"}}");
    }

    try {
        int run_number = JsonWrapper::GetProperty
                          (json, "run_number", JsonWrapper::intValue).asInt();
        if (run_number != _last_run_number || _first) {
            _first = false;
            _last_run_number = run_number;
            RunHeader* head = new RunHeader();
            head->SetRunNumber(run_number);
            GlobalsManager::GetInstance()->GetRunActionManager()->
                                                               StartOfRun(head);

            // TODO (Rogers): now hand head off to Go()
        }
    } catch(Squeal& squee) {
        json = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(json, squee, _classname);
    } catch(std::exception& exc) {
        json = MAUS::CppErrorHandler::getInstance()
                                         ->HandleStdExc(json, exc, _classname);
    }
    Json::FastWriter writer;
    std::string output = writer.write(json);
    return output;
}
}


