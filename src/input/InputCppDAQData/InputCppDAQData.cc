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

#include "src/input/InputCppDAQData/InputCppDAQData.hh"

InputCppDAQData::InputCppDAQData(std::string pDataPath,
                                   std::string pRunNum) {
  _eventPtr = NULL;
  _dataPaths = pDataPath;
  _datafiles = pRunNum;

  _v1290PartEventProc = NULL;
  _v1724PartEventProc = NULL;
  _v1731PartEventProc = NULL;
  _v830FragmentProc = NULL;
  _vLSBFragmentProc = NULL;
  _DBBFragmentProc = NULL;

  _next_event = Json::Value();
}


bool InputCppDAQData::birth(std::string jsonDataCards) {
  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(jsonDataCards, configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  if ( _dataFileManager.GetNFiles() ) {
     return false;  // Faile because files are already open
  }

  //
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
    Squeak::mout(Squeak::error) << "Unable to load any data files from "
          << "daq_data_path " <<_dataPaths << " and daq_data_file "
          <<  _datafiles << std::endl;
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

  // _dataProcessManager.DumpProcessors();

  return true;
}


bool InputCppDAQData::readNextEvent() {
  // Use the MDfileManager object to get the next event.
  _eventPtr = _dataFileManager.GetNextEvent();
  if (!_eventPtr)
    return false;

  return true;
}

bool InputCppDAQData::getCurEvent() {
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
    Squeak::mout(Squeak::error) << "Unpacking exception,  DAQ Event skipped" << std::endl;
    Squeak::mout(Squeak::error) <<  lExc.GetDescription() << endl;
  }
  catch(std::exception & lExc) {
    Squeak::mout(Squeak::error) << "Standard exception" << std::endl;
    Squeak::mout(Squeak::error) << lExc.what() << std::endl;
  }
  catch(...) {
    Squeak::mout(Squeak::error) << "Unknown exception occurred..." << std::endl;
  }

  // Finally attach the spill to the document root
  xDocRoot["event_data"] = xDocSpill;
  xDocRoot["spill_num"] = _dataProcessManager.GetSpillNumber();
  unsigned int event_type = _dataProcessManager.GetEventType();
  xDocRoot["daq_event_type"] = event_type_to_str(event_type);
  // cout<<xDocRoot<<endl;

  _next_event = xDocRoot;
  return true;
}

bool InputCppDAQData::death() {
	// Free the memory.
  if (_v1290PartEventProc) delete _v1290PartEventProc;
  if (_v1724PartEventProc) delete _v1724PartEventProc;
  if (_v1731PartEventProc) delete _v1731PartEventProc;
  if (_v830FragmentProc) delete _v830FragmentProc;
  if (_vLSBFragmentProc) delete _vLSBFragmentProc;
  if (_DBBFragmentProc) delete _DBBFragmentProc;

  return true;
}

std::string InputCppDAQData::event_type_to_str(int pType) {
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
      xConv << pType << " (unknown)";
      event_type = xConv.str();
      break;
  }
  return event_type;
}

std::string InputCppDAQData::getNextEvent() {
  Json::FastWriter xJsonWr;
  if (readNextEvent()) {
    getCurEvent();
    return xJsonWr.write(_next_event);
  }
  return "";
}

int InputCppDAQData::getSpillNumber(Json::Value xDocRoot) {
  assert(xDocRoot.isMember("spill_num"));
  return xDocRoot["spill_num"].asInt();
}

std::string InputCppDAQData::getNextSpill() {
  // we need to check spill number for next event to decide whether it should
  // be added to the spill; means the first event for the next spill gets filled
  // on the previous loop, which makes the logic a bit more obscure
  Json::FastWriter xJsonWr;

  std::cerr << "NEXT SPILL" << std::endl;
  Json::Value spill(Json::objectValue);
  spill["daq_data"] = Json::Value(Json::arrayValue);
  if(_next_event.isNull()) { // if first event, call unpack once
    getNextEvent();
    if (_next_event.isNull()) {
      return "";
    }
  }
  // next event is always the first event of this spill
  int spill_number = getSpillNumber(_next_event);
  spill["daq_data"].append(_next_event);
  std::cerr << "SPILL NUMBER " << spill_number << std::endl;

  // loop over events; if the spill number changes, then we're done
  bool finished = true;
  while (_eventPtr != NULL) {
    getNextEvent();
    std::cerr << "NEXT EVENT LOOP" << std::endl;
    finished = false;
    if (getSpillNumber(_next_event) != spill_number) {
      return xJsonWr.write(spill);
    }
    spill["daq_data"].append(_next_event);
  }

  std::cerr << "SPILL FINISHED" << std::endl;
  // for the last spill, getNextEvent() goes false but spill buffer not empty
  if (!finished) {
    return xJsonWr.write(spill);
  }
  std::cerr << "RETURN EMPTY" << std::endl;
  return "";
}



