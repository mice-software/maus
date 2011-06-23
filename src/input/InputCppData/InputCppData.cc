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

#include <json/json.h>

#include <string>
#include <iostream>

#include "src/input/InputCppData/UnpackEventLib.hh"

InputCppData::InputCppData(std::string pDataPath,
                                   std::string pFilename) {
  _debug = false;
  _eventPtr = NULL;
  _inputFile = NULL;
  _dataPath = pDataPath;
  _filename = pFilename;
}


bool InputCppData::birth(std::string jsonDataCards) {
  if (_inputFile)
    return false;  // Fail because file is open

  if (_debug)
    std::cerr << "Data file = " << _filename << " ("
              << _dataPath << ")" << std::endl;


  _inputFile = new MDdateFile(_filename,
                              _dataPath);

  // Actually try opening the file
  if (_inputFile->OpenFile() != DATE_FILE_OK) {
    if (_debug)
      std::cerr << "File Error." << std::endl;
    return false;
  }

  return true;
}


bool InputCppData::readNextEvent() {
  _eventPtr = _inputFile->GetNextEvent();
  if (!_eventPtr)
    return false;
  return true;
}


std::string InputCppData::getCurEvent() {
  Json::Value xDocRoot;  // Root of the event
  Json::FastWriter xJSONWr;

  MDevent xEvent;

  if (!_eventPtr)
    return xJSONWr.write(xDocRoot);  // Return an empty doc

  xEvent.SetDataPtr(_eventPtr);
  // We can dump the event here as a test
  // xEvent.Dump();

  Json::Value xDocSpill;

  if (xEvent.IsSuperEvent()) {
    if (_debug)
      std::cerr << "Processing a super event." << std::endl;
    // Process all the events in a spill
    unsigned int xSubEvntCount = xEvent.GetNSubEvents();
    for (unsigned int i = 0; i < xSubEvntCount; i++) {
      MDevent xSubEvent;
      xSubEvent.SetDataPtr(xEvent.GetSubEventPtr(i));
      this->processLDCEvent(&xSubEvent, xDocSpill);
    }
  } else {
    if (_debug)
      std::cerr << "This event isn't super." << std::endl;
    this->processLDCEvent(&xEvent, xDocSpill);
  }

  // Finally attach the spill to the document root
  xDocRoot["daq_data"] = xDocSpill;

  if (_debug)
    std::cerr << "Writing JSON..." << std::endl;
  return xJSONWr.write(xDocRoot);
}


void InputCppData::processLDCEvent(MDevent *pEvent,
                                       Json::Value &pDoc) {
  // Decide what to do with this type of event
  switch (*(pEvent->EventTypePtr())) {
    case START_OF_RUN: {
      if (_debug)
        std::cerr << "====> Start of run <====" << std::endl;
      break;
    }

    case END_OF_RUN: {
      if (_debug)
        std::cerr << "====> End of run <====" << std::endl;
      break;
    }

    case START_OF_RUN_FILES: {
      if (_debug)
        std::cerr << "====> Start of run files <====" << std::endl;
      break;
    }

    case END_OF_RUN_FILES: {
      if (_debug)
        std::cerr << "====> End of run files <====" << std::endl;
      break;
    }

    case START_OF_BURST: {
      if (_debug)
        std::cerr << "====> Start of burst <====" << std::endl;
      break;
    }

    case END_OF_BURST: {
      if (_debug)
        std::cerr << "====> End of burst <====" << std::endl;
      break;
    }

    case EVENT_FORMAT_ERROR: {
      if (_debug)
        std::cerr << "====> Event format error! <====" << std::endl;
      break;
    }

    case START_OF_DATA: {
      if (_debug)
        std::cerr << "====> Start of data <====" << std::endl;
      break;
    }

    case END_OF_DATA: {
      if (_debug)
        std::cerr << "====> End of data <====" << std::endl;
      break;
    }

    case DETECTOR_SOFTWARE_TRIGGER_EVENT: {
      if (_debug)
        std::cerr << "====> Detector software trigger <====" << std::endl;
      break;
    }

    case PHYSICS_EVENT:
    case CALIBRATION_EVENT: {
      if (_debug)
        std::cerr << "====> Physics or Calib. data <====" << std::endl;

      // Get the LdcId
      unsigned int xLdcId = pEvent->LdcId();

      // Break the event into its fragments
      unsigned int xFragCount = pEvent->GetNFragments();

      for (unsigned int i = 0; i < xFragCount; i++) {
        // Each fragment needs a part in the output
        Json::Value xDocFragment;

        MDeventFragment xEvntFrag(pEvent->GetFragmentPtr(i));
        // Is this step actually needed?
        xEvntFrag.SetDataPtr(pEvent->GetFragmentPtr(i));

        unsigned int xEquipType = *(xEvntFrag.EquipmentTypePtr());

        // Check if the equipment type is one we can process
        // (TDC, fADC or Scalar hits apparently...)
        if ((xEquipType == VmeTdc) ||
            //  (xEquipType == VmefAdc1724) ||
            //  (xEquipType == VmefAdc1731) ||
            (xEquipType == VmeScaler)) {
          xEvntFrag.InitPartEventVector();
          // Each fragment is made up of a series of parts
          unsigned int xPartCounts = xEvntFrag.GetNPartEvents();
          for (unsigned int j = 0; j < xPartCounts; j++) {
            // Actually process the hits from the event
            void *xPartPtr = xEvntFrag.GetPartEventPtr(j);
            this->processHits(xPartPtr,
                              xEquipType,
                              xLdcId,
                              xDocFragment);
          }
        }

        // Attach the fragment to the spill...
        pDoc.append(xDocFragment);
      }

      break;
    }

    default:
    {
      if (_debug)
        std::cerr << "====> UNKNOWN EVENT! <====" << std::endl;
      break;
    }
  }
}


void InputCppData::processHits(void *pPartEvntPtr,
                                   unsigned int pEquipType,
                                   unsigned int pLdcId,
                                   Json::Value &pDoc) {
  if (!pPartEvntPtr) {
    if (_debug)
      std::cerr << "Error: Bad pointer passed to processHits()?" << std::endl;
    return;
  }

  switch (pEquipType) {
    case VmeTdc: {
      V1290Hit::getJSON(pPartEvntPtr,
                        pDoc["tdc"]);
      pDoc["tdc"]["ldc_id"] = pLdcId;
      break;
    }

    case VmefAdc1724: {
      V1724Hit::getJSON(pPartEvntPtr,
                        pDoc["adc1724"]);
      break;
    }

    case VmefAdc1731: {
      pDoc["adc1731"] = Json::Value(8);
      break;
    }

    case VmeScaler: {
      V830Hit::getJSON(pPartEvntPtr,
                       pDoc["scaler"]);
      break;
    }

    default: {
      pDoc["unknown"] = Json::Value();
      break;
    }
  }
}


bool InputCppData::death() {
  if (_inputFile) {
    delete _inputFile;
    _inputFile = NULL;
  }
  return true;
}


InputCppData::~InputCppData() {
  return;
}

