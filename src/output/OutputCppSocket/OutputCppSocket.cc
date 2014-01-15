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

#include "TSocket.h"

#include "src/common_cpp/DataStructure/ImageData.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/output/OutputCppSocket/OutputCppSocket.hh"

namespace MAUS {

OutputCppSocket::OutputCppSocket() : OutputBase<std::string>("OutputCppSocket"),
                                     _socket(NULL) {
}

void OutputCppSocket::_birth(const std::string& json_datacards) {
    Json::Value cards = JsonWrapper::StringToJson(json_datacards);
    std::string socket_host = cards["output_cpp_socket_host"].asString();
    int socket_port = cards["output_cpp_socket_port"].asInt();
    if (_socket != NULL) {
        delete _socket;
    }
    _socket = new TSocket(socket_host.c_str(), socket_port);
}

void OutputCppSocket::_death() {
    if (_socket != NULL) {
        delete _socket;
    }
    _socket = NULL;
}
`
bool OutputCppSocket::_save(ImageData* image) {
    return false;
}
} // namespace MAUS
