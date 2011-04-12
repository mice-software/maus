#include "MapCppTrackerDigitization.h"

//  This stuff is *still* needed until persistency move is done
dataCards MyDataCards("Digitization");
MICEEvent simEvent;

bool MapCppTrackerDigitization::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerDigitization";

  //  JsonCpp string -> JSON::Value converer
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  return true;
}

bool MapCppTrackerDigitization::death() {
  return true;
}

std::string MapCppTrackerDigitization::process(std::string document){
  //  JsonCpp setup
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  Json::FastWriter writer;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(document, root);
  if (!parsingSuccessful) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("mc")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need an MC branch to simulate.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  Json::Value mc = root.get("mc", 0);

  // Check if JSON document is of the right type, else return error
  if (!mc.isArray()) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "MC branch isn't an array";
    errors["bad_type"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  //  Loop over particles and hits
  for (int i=0; i<mc.size(); i++){  //  i-th particle
    Json::Value particle = mc[i];
    if (!particle.isMember("hits")) {
      // if there are no MC hits, skip
      continue;
    }

    Json::Value hits = particle["hits"];
    for(int j=0; j < hits.size(); j++){  //  j-th hit
      Json::Value hit = hits[j]; 
      Json::StyledWriter writer; 
      
      // sanity check
      assert(hit.isMember("channel_id"));
      Json::Value channel_id = hit["channel_id"];
      
      assert(channel_id.isMember("type"));
      if (channel_id["type"].asString() != "Tracker") {
        continue;
      }

      assert(hit.isMember("energy_deposited"));
      double edep = hit["energy_deposited"].asDouble();
      
      // implement dead channels
      assert(_configJSON.isMember("SciFiFiberConvFactor"));
      double nPE = _configJSON["SciFiFiberConvFactor"].asDouble() * edep;

      assert(_configJSON.isMember("SciFiFiberTrappingEff"));
      nPE *= _configJSON["SciFiFiberTrappingEff"].asDouble();

      assert(_configJSON.isMember("SciFiFiberMirrorEff"));  
      nPE *= ( 1.0 + _configJSON["SciFiFiberMirrorEff"].asDouble());

      assert(_configJSON.isMember("SciFiFiberTransmissionEff")); 
      nPE *= _configJSON["SciFiFiberTransmissionEff"].asDouble();

      assert(_configJSON.isMember("SciFiMUXTransmissionEff")); 
      nPE *= _configJSON["SciFiMUXTransmissionEff"].asDouble();

      assert(_configJSON.isMember("SciFivlpcQE")); 
      nPE *= _configJSON["SciFivlpcQE"].asDouble();
      
      // Throw the dice and generate the ADC count value
      assert(_configJSON.isMember("SciFivlpcEnergyRes"));
      double tmpcounts = edep == 0 ? 0 : CLHEP::RandGauss::shoot((double)nPE,
                                                                 _configJSON["SciFivlpcEnergyRes"].asDouble());
      nPE = tmpcounts;
      
      assert(_configJSON.isMember("SciFiadcFactor"));
      tmpcounts *= _configJSON["SciFiadcFactor"].asDouble();

      // Check for saturation of ADCs 
      assert(_configJSON.isMember("SciFitdcBits")); 
      if (tmpcounts > pow( 2.0, _configJSON["SciFitdcBits"].asDouble() ) - 1.0 )
        tmpcounts = pow( 2.0, _configJSON["SciFitdcBits"].asDouble() ) - 1.0;

      int adcCounts = (int) floor( tmpcounts );

      assert(_configJSON.isMember("SciFivlpcTimeRes"));
      assert(_configJSON.isMember("SciFitdcFactor"));
      assert(hit.isMember("time"));
      tmpcounts = CLHEP::RandGauss::shoot( hit["time"].asDouble(), _configJSON["SciFivlpcTimeRes"].asDouble() ) * _configJSON["SciFitdcFactor"].asDouble();

      // Check for saturation of TDCs
      assert(_configJSON.isMember("SciFitdcBits")); 
      if( tmpcounts > pow( 2.0, _configJSON["SciFitdcBits"].asDouble()) - 1.0 ) {
        tmpcounts = pow( 2.0, _configJSON["SciFitdcBits"].asDouble()) - 1.0;
      }
      
      int tdcCounts = (int) floor( tmpcounts );
      
      if( nPE < 0.5 ) {
        continue;
      }
      
      if (!root.isMember("digits")) {
        root["digits"] = Json::Value(Json::arrayValue);
      }
      assert(root["digits"].isArray());

      Json::Value digit;
      digit["tdc_counts"] = tdcCounts;
      digit["adc_counts"] = adcCounts;
      //digit["number_photoelectrons"] = nPE;
      digit["channel_id"] = hit["channel_id"];
      root["digits"].append(digit);
    }  //  end for with var 'j' to loop hits
  }  //  end for with var 'i' to loop particles

  return writer.write(root);
}  
