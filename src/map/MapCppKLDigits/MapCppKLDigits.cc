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

#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/KLChannelMap.hh"
#include "Utils/DAQChannelMap.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppKLDigits/MapCppKLDigits.hh"

namespace MAUS {

bool MapCppKLDigits::birth(std::string argJsonConfigDocument) {

  _classname = "MapCppKLDigits";

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?"
                                << std::endl;
    return false;
  }

  // Check if the JSON document can be parsed, else return error only
  try {
    //  JsonCpp setup
    Json::Value configJSON;
    Json::Value map_file_name;
    Json::Value xEnable_V1724_Unpacking;
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    //  this will contain the configuration

    map_file_name = JsonWrapper::GetProperty(configJSON,
                                             "KL_cabling_file",
                                             JsonWrapper::stringValue);

    std::string xMapFile = std::string(pMAUS_ROOT_DIR) + map_file_name.asString();
    bool loaded = _map.InitFromFile(xMapFile);
    if (!loaded)
      return false;

    xEnable_V1724_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                       "Enable_V1724_Unpacking",
                                                       JsonWrapper::booleanValue);

    if (!xEnable_V1724_Unpacking.asBool()) {
      Squeak::mout(Squeak::warning)
      << "WARNING in MapCppKLDigits::birth. The unpacking of the flashADC V1724 is disabled!!!"
      << " Are you shure you want this?"
      << std::endl;
    }

    return true;
  } catch(Exception exc) {
    CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
  } catch(std::exception exc) {
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}


bool MapCppKLDigits::death()  {return true;}

std::string MapCppKLDigits::process(std::string document) {

  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
  // Check if the JSON document can be parsed, else return error only
  try {root = JsonWrapper::StringToJson(document);}
  catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  try {
    xEventType = JsonWrapper::GetProperty(root,
                                          "daq_event_type",
                                          JsonWrapper::stringValue);
    if (xEventType == "physics_event" || xEventType == "calibration_event") {
      Json::Value xDaqData = JsonWrapper::GetProperty(root, "daq_data", JsonWrapper::objectValue);
      Json::Value xDocTrig;
      if (xDaqData.isMember("trigger")) {

	  xDocTrig = JsonWrapper::GetProperty(xDaqData,
					      "trigger",
					      JsonWrapper::arrayValue);


	  Json::Value xDocAllDigits;
	  unsigned int n_events = 0;

	  if (xDaqData.isMember("kl")) {
            Json::Value xDocDetectorData = JsonWrapper::GetProperty(xDaqData,
                                                                    "kl",
                                                                    JsonWrapper::arrayValue);
            xDocAllDigits = makeDigits(xDocDetectorData, xDocTrig);
            n_events = xDocAllDigits.size();
	  }

	  for (unsigned int ev = 0; ev < n_events; ev++) {
            // Json::Value xDocKLDigits(Json::arrayValue);
	    Json::Value xDocKLDigits(Json::objectValue);
	      if (xDocAllDigits[ev].type() == Json::arrayValue) {
		xDocKLDigits["kl"] = xDocAllDigits[ev];
	      } else {
		xDocKLDigits["kl"] = Json::Value(Json::arrayValue);
	      }
	      root["recon_events"][ev]["kl_event"]["kl_digits"] = xDocKLDigits;
	  }
      }
    }
  } catch(Exception exc) {
    root = CppErrorHandler::getInstance()
      ->HandleException(root, exc, _classname);
  } catch(std::exception exc) {
    root = CppErrorHandler::getInstance()
      ->HandleStdExc(root, exc, _classname);
  }

  return writer.write(root);
}

bool MapCppKLDigits::SetConfiguration(std::string json_configuration) {

  return true;
}

Json::Value MapCppKLDigits::makeDigits(Json::Value xDocDetData, Json::Value xDocTrig) {
  Json::Value xDocDigits;
  // Get number of Particle trigger.
  int n_part_event_triggers = xDocTrig.size();
  xDocDigits.resize(n_part_event_triggers);

  if ( xDocDetData.isArray() ) {
    // Get number of digits in the detector record. It can be different from
    // the number of Particle triggers because of the Zero Suppression.
    int n_part_events = xDocDetData.size();

    for ( int PartEvent = 0; PartEvent < n_part_events; PartEvent++ ) {

      // Get the data, trigger and trigger request for this particle event.
      Json::Value xDocPartEvent_data = JsonWrapper::GetItem(xDocDetData,
                                                            PartEvent,
                                                            JsonWrapper::anyValue);

      if (xDocPartEvent_data.isMember("V1724")) {

        Json::Value xDocfAdc = JsonWrapper::GetProperty(xDocPartEvent_data,
                                                        "V1724",
                                                        JsonWrapper::arrayValue);

        int n_adc_hits = xDocfAdc.size();

        Json::Value xDocPmtHits;
        for ( int AdcHitCount = 0; AdcHitCount < n_adc_hits; AdcHitCount++ ) {

          // Get the Adc info from the particle event.
          Json::Value xDocTheDigit = getAdc(xDocfAdc[AdcHitCount]);

          xDocPmtHits.append(xDocTheDigit);
        }
        xDocDigits[PartEvent] = xDocPmtHits;
      }
    }
  }
  return xDocDigits;
}

Json::Value MapCppKLDigits::getAdc(Json::Value xDocAdcHit) {
  std::stringstream xConv;
  Json::Value xDocInfo;

  DAQChannelKey xAdcDaqKey;

  std::string xDaqKey_adc_str = JsonWrapper::GetProperty(xDocAdcHit,
                                                         "channel_key",
                                                         JsonWrapper::stringValue).asString();

  xConv << xDaqKey_adc_str;
  xConv >> xAdcDaqKey;

  KLChannelKey* xKlAdcKey = _map.find(&xAdcDaqKey);

  if (xKlAdcKey) {
    xDocInfo["kl_key"]            = xKlAdcKey->str();
    xDocInfo["cell"]              = xKlAdcKey->cell();
    xDocInfo["pmt"]               = xKlAdcKey->pmt();


    xDocInfo["charge_mm"] = xDocAdcHit["charge_mm"];
    xDocInfo["charge_pm"] = xDocAdcHit["charge_pm"];
    xDocInfo["position_max"] = xDocAdcHit["position_max"];
    xDocInfo["part_event_number"] = xDocAdcHit["part_event_number"];
    xDocInfo["phys_event_number"] = xDocAdcHit["phys_event_number"];


    // std::cout << "phys_event_number= " << xDocInfo["phys_event_number"] << std::endl;
    // std::cout << "part_event_number= " << xDocInfo["part_event_number"] << std::endl;
    // std::cout << "adc= " << xDocAdcHit["charge_mm"] << std::endl;
    // std::cout << "kl_key= " << xDocInfo["kl_key"] << std::endl;
    // std::cout << "kl_cell= " << xDocInfo["cell"] << std::endl;
    // std::cout << "kl_pmt= " << xDocInfo["pmt"] << std::endl;
  }

  return xDocInfo;
}
}
