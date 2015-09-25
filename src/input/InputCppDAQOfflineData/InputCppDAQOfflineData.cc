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

#include "src/common_cpp/API/PyWrapInputBase.hh"
#include "src/input/InputCppDAQOfflineData/InputCppDAQOfflineData.hh"

namespace MAUS {
PyMODINIT_FUNC init_InputCppDAQOfflineData(void) {
    PyWrapInputBase<MAUS::InputCppDAQOfflineData>::PyWrapInputBaseModInit(
                  "InputCppDAQOfflineData", "", "", "", "");
}

InputCppDAQOfflineData::InputCppDAQOfflineData()
:InputCppDAQData::InputCppDAQData(), _dataPaths(""), _datafiles("") {
  _classname = "InputCppDAQOfflineData";
}

void InputCppDAQOfflineData::_birth(const std::string& jsonDataCards) {
  if ( _dataFileManager.GetNFiles() ) {
    throw(MAUS::Exception(Exception::recoverable,
                          "Failed to get files - are they open already?",
                          "InputCppDAQOfflineData::_birth"));
  }

  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(jsonDataCards, configJSON);
  if (!parsingSuccessful) {
    throw(MAUS::Exception(Exception::recoverable,
                          "Failed to parse datacards",
                          "InputCppDAQOfflineData::_birth"));
  }

  // Load some data.
  if (_dataPaths == "") {
    assert(configJSON.isMember("daq_data_path"));
    _dataPaths = configJSON["daq_data_path"].asString();
  }

  if (_datafiles == "") {
    assert(configJSON.isMember("daq_data_file"));
    _datafiles = configJSON["daq_data_file"].asString();
  }

  _dataFileManager.SetList(_datafiles);
  _dataFileManager.SetPath(_dataPaths);
  _dataFileManager.OpenFile();
  unsigned int nfiles = _dataFileManager.GetNFiles();
  if (!nfiles) {
    throw(MAUS::Exception(Exception::recoverable,
                        "Unable to load any data files. Check your run"+
                        std::string("number (or file name) and data path."),
                        "InputCppDAQOfflineData::_birth"));
  }

  // Set the number of DAQ events to be processed.
  assert(configJSON.isMember("Number_of_DAQ_Events"));
  _maxNumEvents = configJSON["Number_of_DAQ_Events"].asInt();

  assert(configJSON.isMember("Phys_Events_Only"));
  _phys_Events_Only = configJSON["Phys_Events_Only"].asBool();
  assert(configJSON.isMember("Calib_Events_Only"));
  _calib_Events_Only = configJSON["Calib_Events_Only"].asBool();

  if (_phys_Events_Only && _calib_Events_Only) {
     throw(MAUS::Exception(Exception::recoverable,
                          "There is a contradiction in the configuration:\n"+
                          std::string("Phys_Events_Only and Calib_Events_Only ")+
                          std::string("are both true!!!"),
                          "InputCppDAQOfflineData::_birth"));
  }

  InputCppDAQData::_childbirth(jsonDataCards);
  // Initialize the map from data cards
  bool loaded = _map.InitFromCards(configJSON);
  if (!loaded) {
    throw(MAUS::Exception(Exception::recoverable, "STRING", "InputCppDAQOfflineData InitFromCards"));
  }

  // _dataProcessManager.DumpProcessors();
}


bool InputCppDAQOfflineData::readNextEvent() {
  // Check the max number of DAQ events.
  // If it is negative, run until the end of the loaded DATE files.
  if (_maxNumEvents > -1)
    if (_eventsCount >= _maxNumEvents)
      return false;


//   cerr << "InputCppDAQData::readNextEvent   event: " << _eventsCount << endl;

  // Use the MDfileManager object to get the next event.
  if (_phys_Events_Only && (!_calib_Events_Only))
    _eventPtr = _dataFileManager.GetNextPhysEvent();

  if ((!_phys_Events_Only) && _calib_Events_Only)
    _eventPtr = _dataFileManager.GetNextCalibEvent();

  if ((!_phys_Events_Only) && (!_calib_Events_Only))
    _eventPtr = _dataFileManager.GetNextEvent();

  if (!_eventPtr)
    return false;

  return true;
}
}



