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

#include "src/input/InputCppData/InputCppData.hh"

InputCppData::InputCppData(std::string pDataPath,
                                   std::string pRunNum) {
  _classname = "InputCppDAQData";
  _eventPtr = NULL;
  _dataPaths = pDataPath;
  _datafiles = pRunNum;
  _daqEventsCount = 0;
  _v1290PartEventProc = NULL;
  _v1724PartEventProc = NULL;
  _v1731PartEventProc = NULL;
  _v830FragmentProc = NULL;
  _vLSBFragmentProc = NULL;
  _DBBFragmentProc = NULL;
}


bool InputCppData::birth(std::string jsonDataCards) {
  if ( _dataFileManager.GetNFiles() ) {
     return false;  // Faile because files are already open
  }

  _dataFileManager.SetList(_datafiles);
  _dataFileManager.SetPath(_dataPaths);
  _dataFileManager.OpenFile();
  unsigned int nfiles = _dataFileManager.GetNFiles();
  if (!nfiles) {
    Squeak::mout(Squeak::error) << "Unable to load any data files." << std::endl;
    Squeak::mout(Squeak::error) << "Check your run number (or file name) and data path."
    << std::endl;
    return false;
  }
  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(jsonDataCards, configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  assert(configJSON.isMember("DAQ_cabling_file"));
  std::string map_file_name = configJSON["DAQ_cabling_file"].asString();
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error) << "Could not find the $MAUS_ROOT_DIR environmental variable."
    << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }

  // Initialize the map by using text file.
  bool loaded = _map.InitFromFile(std::string(pMAUS_ROOT_DIR) + map_file_name);
  if (!loaded) {
    return false;
  }

  // Comfigure the V1290 (TDC) data processor.
  assert(configJSON.isMember("Enable_V1290_Unpacking"));
  if ( configJSON["Enable_V1290_Unpacking"].asBool() ) {
    _v1290PartEventProc = new V1290DataProcessor();
    _v1290PartEventProc->set_DAQ_map(&_map);

    _dataProcessManager.SetPartEventProc("V1290", _v1290PartEventProc);
  } else {
    this->disableEquipment("V1290");
  }

  // Comfigure the V1724 (fADC) data processor.
  assert(configJSON.isMember("Enable_V1724_Unpacking"));
  if ( configJSON["Enable_V1724_Unpacking"].asBool() ) {
    _v1724PartEventProc = new V1724DataProcessor();
    _v1724PartEventProc->set_DAQ_map(&_map);

    assert(configJSON.isMember("Do_V1724_Zero_Suppression"));
    bool zs = configJSON["Do_V1724_Zero_Suppression"].asBool();
    _v1724PartEventProc->set_zero_supression(zs);

    assert(configJSON.isMember("V1724_Zero_Suppression_Threshold"));
    int zs_threshold = configJSON["V1724_Zero_Suppression_Threshold"].asInt();
    _v1724PartEventProc->set_zs_threshold(zs_threshold);

    _dataProcessManager.SetPartEventProc("V1724", _v1724PartEventProc);
  } else {
    this->disableEquipment("V1724");
  }

  // Comfigure the V1731 (fADC) data processor.
  assert(configJSON.isMember("Enable_V1731_Unpacking"));
  if ( configJSON["Enable_V1731_Unpacking"].asBool() ) {
    _v1731PartEventProc = new V1731DataProcessor();
    _v1731PartEventProc->set_DAQ_map(&_map);

    assert(configJSON.isMember("Do_V1731_Zero_Suppression"));
    bool zs = configJSON["Do_V1731_Zero_Suppression"].asBool();
    _v1731PartEventProc->set_zero_supression(zs);

    assert(configJSON.isMember("V1731_Zero_Suppression_Threshold"));
    int zs_threshold = configJSON["V1731_Zero_Suppression_Threshold"].asInt();
    _v1731PartEventProc->set_zs_threshold(zs_threshold);

    _dataProcessManager.SetPartEventProc("V1731", _v1731PartEventProc);
  } else {
    this->disableEquipment("V1731");
  }

  // Comfigure the V830 (scaler) data processor.
  assert(configJSON.isMember("Enable_V830_Unpacking"));
  if ( configJSON["Enable_V830_Unpacking"].asBool() ) {
    _v830FragmentProc = new V830DataProcessor();
    _v830FragmentProc->set_DAQ_map(&_map);

    _dataProcessManager.SetFragmentProc("V830", _v830FragmentProc);
  } else {
    this->disableEquipment("V830");
  }

  // Comfigure the VLSB (tracker board) data processor.
  assert(configJSON.isMember("Enable_VLSB_Unpacking"));
  if (configJSON["Enable_VLSB_Unpacking"].asBool()) {
    _vLSBFragmentProc = new VLSBDataProcessor();
    _vLSBFragmentProc->set_DAQ_map(&_map);

    _dataProcessManager.SetFragmentProc("VLSB_C", _vLSBFragmentProc);
  } else {
    this->disableEquipment("VLSB_C");
  }

  // Comfigure the DBB (EMR board) data processor.
  assert(configJSON.isMember("Enable_DBB_Unpacking"));
  if ( configJSON["Enable_DBB_Unpacking"].asBool() ) {
    _DBBFragmentProc = new DBBDataProcessor();
    _DBBFragmentProc->set_DAQ_map(&_map);

    _dataProcessManager.SetFragmentProc("DBB", _DBBFragmentProc);
  } else {
    this->disableEquipment("DBB");
  }

  // Set the number of DAQ events to be processed.
  assert(configJSON.isMember("Number_of_DAQ_Events"));
  _maxNumDaqEvents = configJSON["Number_of_DAQ_Events"].asInt();

  // _dataProcessManager.DumpProcessors();

  return true;
}


bool InputCppData::readNextEvent() {
  // Check the max number of DAQ events.
  // If it is negative, run until the end of the loaded DATE files.
  if (_maxNumDaqEvents > -1)
    if (_daqEventsCount >= _maxNumDaqEvents)
      return false;

  // Use the MDfileManager object to get the next event.
  _eventPtr = _dataFileManager.GetNextEvent();
  if (!_eventPtr)
    return false;

  return true;
}

std::string InputCppData::getCurEvent() {
  // Create new Json documents.
  Json::Value xDocRoot;  // Root of the event
  Json::FastWriter xJSONWr;
  Json::Value xDocSpill;

  // Order all processor classes to fill in xDocSpill.
  if (_v1290PartEventProc)
    _v1290PartEventProc->set_JSON_doc(&xDocSpill);

  if (_v1724PartEventProc)
    _v1724PartEventProc->set_JSON_doc(&xDocSpill);

  if (_v1731PartEventProc)
    _v1731PartEventProc->set_JSON_doc(&xDocSpill);

  if (_v830FragmentProc)
    _v830FragmentProc->set_JSON_doc(&xDocSpill);

  if (_vLSBFragmentProc)
    _vLSBFragmentProc->set_JSON_doc(&xDocSpill);

  if (_DBBFragmentProc)
    _DBBFragmentProc->set_JSON_doc(&xDocSpill);

  // Now do the loop over the binary DAQ data.
  try {
    _dataProcessManager.Process(_eventPtr);
  }
  // Deal with exceptions
  catch(MDexception & lExc) {
    Squeak::mout(Squeak::error) << "InputCppData : Unpacking exception." <<
    std::endl << "DAQ Event skipped!" << std::endl;
    Squeak::mout(Squeak::error) <<  lExc.GetDescription() << endl;
    xDocSpill.clear();
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << lExc.GetDescription() << "  Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    errors["bad_data_input"] = ss.str();
    xDocRoot["errors"] = errors;
  }
  catch(std::exception & lExc) {
    Squeak::mout(Squeak::error) << "InputCppData : Standard exception."
    << std::endl << "DAQ Event skipped!" << std::endl;
    Squeak::mout(Squeak::error) << lExc.what() << std::endl;
    xDocSpill.clear();
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << lExc.what() << " Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    errors["bad_data_input"] = ss.str();
    xDocRoot["errors"] = errors;
  }
  catch(...) {
    Squeak::mout(Squeak::error) << "InputCppData : Unknown exception occurred."
    << std::endl << "DAQ Event skipped!" << std::endl;
    xDocSpill.clear();
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Unknown exception occurred. Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    errors["bad_data_input"] = ss.str();
    xDocRoot["errors"] = errors;
  }

  // Finally attach the spill to the document root
  xDocRoot["daq_data"] = xDocSpill;
  xDocRoot["spill_num"] = _dataProcessManager.GetSpillNumber();
  unsigned int event_type = _dataProcessManager.GetEventType();
  xDocRoot["daq_event_type"] = event_type_to_str(event_type);
  // cout<<xDocRoot<<endl;

  _daqEventsCount++;
  return xJSONWr.write(xDocRoot);
}

bool InputCppData::death() {
	// Free the memory.
  if (_v1290PartEventProc) delete _v1290PartEventProc;
  if (_v1724PartEventProc) delete _v1724PartEventProc;
  if (_v1731PartEventProc) delete _v1731PartEventProc;
  if (_v830FragmentProc) delete _v830FragmentProc;
  if (_vLSBFragmentProc) delete _vLSBFragmentProc;
  if (_DBBFragmentProc) delete _DBBFragmentProc;

  return true;
}

std::string InputCppData::event_type_to_str(int pType) {
  std::string event_type;
  switch (pType) {
    case START_OF_BURST :
      event_type = "start_of_burst";
      break;

    case  END_OF_BURST:
      event_type = "end_of_burst";
      break;

    case PHYSICS_EVENT :
      event_type = "physics_event";
      break;

    case CALIBRATION_EVENT :
      event_type = "calibration_event";
      break;

    case START_OF_RUN :
      event_type = "start_of_run";
      break;

    case  END_OF_RUN:
      event_type = "end_of_run";
      break;

    default :
      std::stringstream xConv;
      xConv << pType << "unknown";
      event_type = xConv.str();
      break;
  }
  return event_type;
}







