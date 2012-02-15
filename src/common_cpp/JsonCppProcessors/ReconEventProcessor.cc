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

#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"

namespace MAUS {

ReconEventArray* ReconEventArrayProcessor::operator()(const Json::Value& data) {
    ReconEventArray* event_vector = new ReconEventArray();
    for (size_t i = 0; i < data.size(); ++i) {
        ReconEvent* ev = ReconEventProcessor()(data[i]);
        event_vector->push_back(ev);
    }
    return event_vector;
}

Json::Value* ReconEventArrayProcessor::operator()(const std::vector<ReconEvent*>& data) {
    Json::Value* event_array = new Json::Value(Json::arrayValue);
    for (size_t i = 0; i < data.size(); ++i) {
        event_array->append(ReconEventProcessor()(data[i]));
    }
    return event_array;
}

ReconEvent* ReconEventProcessor::operator()(const Json::Value& data) {
    return new ReconEvent();
}

Json::Value* ReconEventProcessor::operator()(const ReconEvent& data) {
    return new Json::Value();
}

}

