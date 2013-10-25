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

InputCppDAQData::InputCppDAQData() {
  _classname = "InputCppDAQData";
  _eventPtr = NULL;
  _eventsCount = 0;

  _v1290PartEventProc_cpp  = NULL;
  _v1290PartEventProc_json = NULL;

  _v1724PartEventProc_cpp  = NULL;
  _v1724PartEventProc_json = NULL;

  _v1731PartEventProc_cpp  = NULL;
  _v1731PartEventProc_json = NULL;

  _v830FragmentProc_cpp    = NULL;
  _v830FragmentProc_json   = NULL;

  _vLSB_cFragmentProc_json = NULL;

  _vLSBFragmentProc_json   = NULL;

  _DBBFragmentProc_cpp     = NULL;
  _DBBFragmentProc_json    = NULL;

  _DBBChainFragmentProc_cpp = NULL;
//   _DBBChainFragmentProc_json = NULL;
}


bool InputCppDAQData::birth(std::string jsonDataCards) {
  if ( _dataFileManager.GetNFiles() ) {
     return false;  // Faile because files are already open
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

  assert(configJSON.isMember("Input_Use_JSON"));
  bool json_out = configJSON["Input_Use_JSON"].asBool();

  if (json_out) {
    // Comfigure the V830 (scaler) data processor.
    initProcessor(_v830FragmentProc_json, configJSON);

    // Comfigure the V1290 (TDC) data processor.
    initProcessor(_v1290PartEventProc_json, configJSON);

    // Comfigure the V1724 (fADC) data processor.
    initProcessor(_v1724PartEventProc_json, configJSON);
    configureZeroSupression(_v1724PartEventProc_json, configJSON);

    // Comfigure the V1731 (fADC) data processor.
    initProcessor(_v1731PartEventProc_json, configJSON);
    configureZeroSupression(_v1731PartEventProc_json, configJSON);

    // Comfigure the VLSB (tracker board) data processor.
    initProcessor<VLSBDataProcessor>(_vLSBFragmentProc_json, configJSON);
    configureZeroSupression(_vLSBFragmentProc_json, configJSON);

    // Comfigure the VLSB (tracker board) data processor.
    // This is for the old version of the VLSB equipment used in the cosmic test in lab7
    initProcessor<VLSB_CDataProcessor>(_vLSB_cFragmentProc_json, configJSON);
    configureZeroSupression(_vLSB_cFragmentProc_json, configJSON);

    // Comfigure the DBB (EMR board) data processor.
    initProcessor(_DBBFragmentProc_json, configJSON);

    // Comfigure the DBB (EMR board) data processor.
//     initProcessor(_DBBChainFragmentProc_json, configJSON);
  } else {
    // Comfigure the V830 (scaler) data processor.
    initProcessor(_v830FragmentProc_cpp, configJSON);

    // Comfigure the V1290 (TDC) data processor.
    initProcessor(_v1290PartEventProc_cpp,  configJSON);

    // Comfigure the V1724 (fADC) data processor.
    initProcessor(_v1724PartEventProc_cpp, configJSON);
    configureZeroSupression(_v1724PartEventProc_cpp, configJSON);

    // Comfigure the V1731 (fADC) data processor.
    initProcessor(_v1731PartEventProc_cpp, configJSON);
    configureZeroSupression(_v1731PartEventProc_cpp, configJSON);

    // Comfigure the VLSB (tracker board) data processor.
//     initProcessor<VLSBDataProcessor>(_vLSBFragmentProc_cpp, configJSON);
//     configureZeroSupression(_vLSBFragmentProc_cpp, configJSON);

    // Comfigure the VLSB (tracker board) data processor.
    // This is for the old version of the VLSB equipment used in the cosmic test in lab7
//     initProcessor<VLSB_CDataProcessor>(_vLSB_cFragmentProc_cpp, configJSON);
//     configureZeroSupression(_vLSB_cFragmentProc_cpp, configJSON);

    // Comfigure the DBB (EMR board) data processor.
    initProcessor(_DBBFragmentProc_cpp, configJSON);

    // Comfigure the DBB (EMR board) data processor.
    initProcessor(_DBBChainFragmentProc_cpp, configJSON);
  }

  // _dataProcessManager.DumpProcessors();

  return true;
}


bool InputCppDAQData::readNextEvent() {

  Squeak::mout(Squeak::error)
  << "ERROR : InputCppDAQData is a base imput class and can not be used to access the DAQ data!"
  << std::endl << "*** Use InputCppDAQOfflineData or InputCppDAQOnlineData instead. ***"
  << std::endl << std::endl;
  std::cerr << "22 _DBBFragmentProc_cpp: " << _DBBFragmentProc_cpp << std::endl;
  return false;
}

void InputCppDAQData::getCurEvent(MAUS::Data *data) {

  MAUS::DAQData *daq_data = data->GetSpill()->GetDAQData();
  try {
    // Now do the loop over the binary DAQ data.
    _dataProcessManager.Process(_eventPtr);

     if (_DBBFragmentProc_cpp) {
      _DBBFragmentProc_cpp->set_daq_data(daq_data);
      _DBBFragmentProc_cpp->fill_daq_data();
     }

    if (_DBBChainFragmentProc_cpp) {
      _DBBChainFragmentProc_cpp->set_daq_data(daq_data);
      _DBBChainFragmentProc_cpp->fill_daq_data();
    }

    if (_v1731PartEventProc_cpp) {
      _v1731PartEventProc_cpp->set_daq_data(daq_data);
      _v1731PartEventProc_cpp->fill_daq_data();
    }

    if (_v1724PartEventProc_cpp) {
      _v1724PartEventProc_cpp->set_daq_data(daq_data);
      _v1724PartEventProc_cpp->fill_daq_data();
    }

    if (_v1290PartEventProc_cpp) {
      _v1290PartEventProc_cpp->set_daq_data(daq_data);
      _v1290PartEventProc_cpp->fill_daq_data();
    }

    if (_v830FragmentProc_cpp) {
      _v830FragmentProc_cpp->set_daq_data(daq_data);
      _v830FragmentProc_cpp->fill_daq_data();
    }

    data->GetSpill()->SetDAQData(daq_data);
  }
  // Deal with exceptions
  catch(MDexception & lExc) {
    Squeak::mout(Squeak::error) << lExc.GetDescription() << std::endl
    << "*** Unpacking exception in void  "
    << "InputCppDAQData::getCurEvent(MAUS::Data *data) : " << std::endl;
    Squeak::mout(Squeak::error) <<"DAQ Event skipped!" << std::endl << std::endl;

    std::stringstream ss;
    ss << _classname << " says:" << lExc.GetDescription() << "  Phys. Event " << std::endl
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    MAUS::ErrorsMap errors = data->GetSpill()->GetErrors();
    errors["bad_data_input"] = ss.str();
    data->GetSpill()->SetErrors(errors);
  }
  catch(std::exception & lExc) {
    Squeak::mout(Squeak::error) << lExc.what() << std::endl
    << "*** Standard exception in "
    << "void InputCppDAQData::getCurEvent(MAUS::Data *data) : " << std::endl;
    Squeak::mout(Squeak::error) <<"DAQ Event skipped!" << std::endl << std::endl;

    std::stringstream ss;
    ss << _classname << " says:" << lExc.what() << " Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    MAUS::ErrorsMap errors = data->GetSpill()->GetErrors();
    errors["bad_data_input"] = ss.str();
    data->GetSpill()->SetErrors(errors);
  }
  catch(...) {
    Squeak::mout(Squeak::error) << "*** void InputCppDAQData::getCurEvent(MAUS::Data *data) : "
    << "Unknown exception occurred." << std::endl;
    Squeak::mout(Squeak::error) << "DAQ Event skipped!" << std::endl << std::endl;

    std::stringstream ss;
    ss << _classname << " says: Unknown exception occurred. Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    MAUS::ErrorsMap errors = data->GetSpill()->GetErrors();
    errors["bad_data_input"] = ss.str();
    data->GetSpill()->SetErrors(errors);
  }

  this->resetAllProcessors();
}

std::string InputCppDAQData::getCurEvent() {
  // Create new Json documents.
  Json::Value xDocRoot;  // Root of the event
  Json::FastWriter xJSONWr;
  Json::Value xDocSpill;

  // Order all processor classes to fill in xDocSpill.
  if (_v1290PartEventProc_json)
    _v1290PartEventProc_json->set_JSON_doc(&xDocSpill);

  if (_v1724PartEventProc_json)
    _v1724PartEventProc_json->set_JSON_doc(&xDocSpill);

  if (_v1731PartEventProc_json)
    _v1731PartEventProc_json->set_JSON_doc(&xDocSpill);

  if (_v830FragmentProc_json)
    _v830FragmentProc_json->set_JSON_doc(&xDocSpill);

  if (_vLSBFragmentProc_json)
    _vLSBFragmentProc_json->set_JSON_doc(&xDocSpill);

  if (_vLSB_cFragmentProc_json)
    _vLSB_cFragmentProc_json->set_JSON_doc(&xDocSpill);

  if (_DBBFragmentProc_json)
    _DBBFragmentProc_json->set_JSON_doc(&xDocSpill);

//   if (_DBBChainFragmentProc_json)
//     _DBBChainFragmentProc_json->set_JSON_doc(&xDocSpill);

  // Now do the loop over the binary DAQ data.
  try {
    _dataProcessManager.Process(_eventPtr);
  }
  // Deal with exceptions
  catch(MDexception & lExc) {
    Squeak::mout(Squeak::error) << lExc.GetDescription() << std::endl
    << "*** Unpacking exception in InputCppDAQData::getCurEvent() : " << std::endl;
    Squeak::mout(Squeak::error) <<"DAQ Event skipped!" << std::endl << std::endl;
    xDocSpill.clear();
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << lExc.GetDescription() << "  Phys. Event " << std::endl
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    errors["bad_data_input"] = ss.str();
    xDocRoot["errors"] = errors;
  }
  catch(std::exception & lExc) {
    Squeak::mout(Squeak::error) << lExc.what() << std::endl
    << "*** Standard exception in InputCppDAQData::getCurEvent() : " << std::endl;
    Squeak::mout(Squeak::error) <<"DAQ Event skipped!" << std::endl << std::endl;
    xDocSpill.clear();
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << lExc.what() << " Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    errors["bad_data_input"] = ss.str();
    xDocRoot["errors"] = errors;
  }
  catch(...) {
    Squeak::mout(Squeak::error) <<
    "*** InputCppDAQData::getCurEvent() : Unknown exception occurred." << std::endl;
    Squeak::mout(Squeak::error) << "DAQ Event skipped!" << std::endl << std::endl;
    xDocSpill.clear();
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Unknown exception occurred. Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    errors["bad_data_input"] = ss.str();
    xDocRoot["errors"] = errors;
  }

  // Finally attach the spill to the document root
  if (xDocSpill.type() != Json::nullValue) {
    xDocRoot["daq_data"] = xDocSpill;
  }
  xDocRoot["spill_number"] = _dataProcessManager.GetSpillNumber();
  unsigned int event_type = _dataProcessManager.GetEventType();
  xDocRoot["daq_event_type"] = event_type_to_str(event_type);
  xDocRoot["run_number"] = _dataProcessManager.GetRunNumber();
  xDocRoot["maus_event_type"] = Json::Value("Spill");
  // cout << xDocRoot << endl;

  _eventsCount++;
  return xJSONWr.write(xDocRoot);
}

bool InputCppDAQData::death() {
  // Free the memory.
  if (_v1290PartEventProc_cpp)    delete _v1290PartEventProc_cpp;
  if (_v1290PartEventProc_json)   delete _v1290PartEventProc_json;

  if (_v1724PartEventProc_cpp)    delete _v1724PartEventProc_cpp;
  if (_v1724PartEventProc_json)   delete _v1724PartEventProc_json;

  if (_v1731PartEventProc_cpp)    delete _v1731PartEventProc_cpp;
  if (_v1731PartEventProc_json)   delete _v1731PartEventProc_json;

  if (_v830FragmentProc_cpp)      delete _v830FragmentProc_cpp;
  if (_v830FragmentProc_json)     delete _v830FragmentProc_json;

  if (_vLSBFragmentProc_json)     delete _vLSBFragmentProc_json;

  if (_vLSB_cFragmentProc_json)   delete _vLSB_cFragmentProc_json;

  if (_DBBFragmentProc_cpp)       delete _DBBFragmentProc_cpp;
  if (_DBBFragmentProc_json)      delete _DBBFragmentProc_json;

  if (_DBBChainFragmentProc_cpp)  delete _DBBChainFragmentProc_cpp;
//   if (_DBBChainFragmentProc_json) delete _DBBChainFragmentProc_json;

  return true;
}

void InputCppDAQData::resetAllProcessors() {
  if (_v1290PartEventProc_cpp)     _v1290PartEventProc_cpp->reset();
  if (_v1724PartEventProc_cpp)     _v1724PartEventProc_cpp->reset();
  if (_v1731PartEventProc_cpp)     _v1731PartEventProc_cpp->reset();
  if (_v830FragmentProc_cpp)       _v830FragmentProc_cpp->reset();
//   if (_vLSBFragmentProc_cpp)       _vLSBFragmentProc_cpp->reset();
//   if (_vLSB_cFragmentProc_cpp)     _vLSB_cFragmentProc_cpp->reset();
  if (_DBBFragmentProc_cpp)        _DBBFragmentProc_cpp->reset();
  if (_DBBChainFragmentProc_cpp)   _DBBChainFragmentProc_cpp->reset();
}

template <class procType>
bool InputCppDAQData::initProcessor(procType* &processor, Json::Value configJSON) {
  processor = new procType();
  string xName, xDataCard;
  xName = processor->get_equipment_name();
  xDataCard = "Enable_" + xName + "_Unpacking";

  // Enable or disable this equipment.
  assert(configJSON.isMember(xDataCard));
  bool enableThis = configJSON[xDataCard].asBool();

  if (enableThis) {
    processor->set_DAQ_map(&_map);

    // Get a pointer to the equipment fragment object from the static equipment map.
    unsigned int xFragType = MDequipMap::GetType(xName);
    MDfragment* xFragPtr = MDequipMap::GetFragmentPtr(xFragType);

    // Check is the data from this equipment is made of particle events.
    try {
      if (xFragPtr->IsMadeOfParticles()) {
        // Set a processor for particle events.
        _dataProcessManager.SetPartEventProc(xName, processor);
      } else {
        // Set a processor for the entire equipment fragment
        _dataProcessManager.SetFragmentProc(xName, processor);
      }
    }
    // Deal with exceptions
    catch(MDexception & lExc) {
      Squeak::mout(Squeak::error) << lExc.GetDescription() << std::endl
      << "*** Unpacking exception in InputCppDAQData::initProcessor() : " << endl;
    }
    return true;
  } else {
    this->disableEquipment(xName);
    return false;
  }
}

void InputCppDAQData::configureZeroSupression(ZeroSupressionFilter* processor,
                                              Json::Value configJSON) {
  string xName, xDataCard;
  xName = processor->get_equipment_name();
  xDataCard = "Do_" + xName + "_Zero_Suppression";

  // Enable or disable zero supression.
  assert(configJSON.isMember(xDataCard));
  bool zs = configJSON[xDataCard].asBool();
  processor->set_zero_supression(zs);

  if (zs) {
    xDataCard = xName + "_Zero_Suppression_Threshold";
    assert(configJSON.isMember(xDataCard));
    int zs_threshold = configJSON[xDataCard].asInt();
    processor->set_zs_threshold(zs_threshold);
  }
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
      xConv << pType << "unknown";
      event_type = xConv.str();
      break;
  }
  return event_type;
}







