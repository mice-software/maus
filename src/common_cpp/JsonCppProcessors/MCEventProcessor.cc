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
 */

#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include <iostream>

namespace MAUS {

MCEventArray* MCEventArrayProcessor::operator()(const Json::Value& data) {
    MCEventArray* event_vector = new MCEventArray();
    for (size_t i = 0; i < data.size(); ++i) {
        event_vector->push_back(MCEventProcessor()(data[i]));
    }
    return event_vector;
}

Json::Value* MCEventArrayProcessor::operator()(const MCEventArray& data) {
    Json::Value* event_array = new Json::Value(Json::arrayValue);
    std::cerr << "MCEventArray size " << data.size() << std::endl;
    for (size_t i = 0; i < data.size(); ++i) {
        event_array->append(MCEventProcessor()(data[i]));
    }
    return event_array;
}

MCEvent* MCEventProcessor::operator()(const Json::Value& data) {
    return new MCEvent();
}

Json::Value* MCEventProcessor::operator()(const MCEvent& data) {
    return new Json::Value();
}

}

