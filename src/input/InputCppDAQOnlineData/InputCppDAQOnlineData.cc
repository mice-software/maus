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

#include "src/legacy/Interface/Squeak.hh"

#include "src/input/InputCppDAQOnlineData/InputCppDAQOnlineData.hh"

InputCppDAQOnlineData::InputCppDAQOnlineData()
:InputCppDAQData::InputCppDAQData() {
  std::cerr << "MAUS running in online mode" << std::endl;
  _classname = "InputCppDAQOnlineData";
  Squeak::activateCout(true);
}

bool InputCppDAQOnlineData::birth(std::string jsonDataCards) {
  std::cerr << "Initialising MAUS online input" << std::endl;
  if (!InputCppDAQData::birth(jsonDataCards))
    return false;

  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(jsonDataCards, configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  assert(configJSON.isMember("DAQ_hostname"));
  std::string daq_hostname = configJSON["DAQ_hostname"].asString();
  assert(configJSON.isMember("DAQ_monitor_name"));
  std::string monitor_name = configJSON["DAQ_monitor_name"].asString();

  _dataManager = new MDmonitoring(daq_hostname, monitor_name);
  _dataManager->Init();

  // _dataProcessManager.DumpProcessors();

  // online mimic is available for testing purposes only
  if (configJSON.isMember("DAQ_online_file") && 
      configJSON["DAQ_online_file"].asString() != "") {
    Squeak::mout(Squeak::debug) << "Using online file mimic with file "
                                << configJSON["DAQ_online_file"].asString()
                                << std::endl;
    setMonitorSrc(configJSON["DAQ_online_file"].asString());
  }
  return true;
}

bool InputCppDAQOnlineData::readNextEvent() {
  // Use the MDmonitoring object to get the next event.
  _eventPtr = _dataManager->GetNextEvent();
  // std::cerr << "THIS IS THE eventPtr " << (void*)_eventPtr << " REALLY blah" << std::endl;
  if (!_eventPtr)
    return false;

  return true;
}

