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

#include "src/common_cpp/API/PyWrapInputBase.hh"

#include "src/input/InputCppDAQOnlineData/InputCppDAQOnlineData.hh"

namespace MAUS {


PyMODINIT_FUNC init_InputCppDAQOnlineData(void) {
  PyWrapInputBase<MAUS::InputCppDAQOnlineData>::PyWrapInputBaseModInit
                              ("InputCppDAQOnlineData",
                               "",
                               "",
                               "",
                               "");
}

InputCppDAQOnlineData::InputCppDAQOnlineData()
:InputCppDAQData::InputCppDAQData() {
  std::cerr << "MAUS running in online mode" << std::endl;
  _classname = "InputCppDAQOnlineData";
  Squeak::activateCout(true);
  _sleep_time.tv_sec = 0;
  _sleep_time.tv_nsec = 0;
}

void InputCppDAQOnlineData::_birth(const std::string& jsonDataCards) {
  std::cerr << "Initialising MAUS online input" << std::endl;
  InputCppDAQData::_childbirth(jsonDataCards);
  // frankliuao: Skip the InitFromCards() method, directly use InitFromCurrentCDB()
  bool loaded = _map.InitFromCurrentCDB();
  if (!loaded) {
    throw(MAUS::Exception(Exception::recoverable, "STRING", "InputCppDAQOnlineData InitFromCurrentCDB"));
  }
  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(jsonDataCards, configJSON);
  if (!parsingSuccessful) {
      throw MAUS::Exception(Exception::recoverable,
                            "Failed to parse configuration to json",
                            "InputCppDAQOnlineData::_birth(std::string)");
  }

  assert(configJSON.isMember("DAQ_hostname"));
  std::string daq_hostname = configJSON["DAQ_hostname"].asString();
  assert(configJSON.isMember("DAQ_monitor_name"));
  std::string monitor_name = configJSON["DAQ_monitor_name"].asString();

  _dataManager = new MDmonitoring(daq_hostname, monitor_name);
  _dataManager->Init();

  // _dataProcessManager.DumpProcessors();

  // online mimic is available for testing purposes only
  if (configJSON.isMember("daq_online_file") &&
      configJSON["daq_online_file"].asString() != "") {
    Squeak::mout(Squeak::debug) << "Using online file mimic with file "
                                << configJSON["daq_online_file"].asString()
                                << std::endl;
    setMonitorSrc(configJSON["daq_online_file"].asString());
  }
  if (configJSON.isMember("daq_online_spill_delay_time")) {
      double delay_time = configJSON["daq_online_spill_delay_time"].asDouble();
      _sleep_time.tv_sec = static_cast<long>(delay_time);
      _sleep_time.tv_nsec = 1000000000L*(delay_time-_sleep_time.tv_sec);
  }
}

bool InputCppDAQOnlineData::readNextEvent() {
  // Use the MDmonitoring object to get the next event.
  _eventPtr = _dataManager->GetNextEvent();
  // Add a delay (for mocking target time structure)
  nanosleep(&_sleep_time, (struct timespec *)NULL);
  // std::cerr << "THIS IS THE eventPtr " << (void*)_eventPtr << " REALLY blah" << std::endl;
  if (!_eventPtr)
    return false;

  return true;
}

void InputCppDAQOnlineData::setMonitorSrc(std::string mon) {
  _dataManager->setMonSrc(mon);
  _dataManager->Init();
}
}
