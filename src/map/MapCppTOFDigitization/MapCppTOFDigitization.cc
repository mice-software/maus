#include "MapCppTOFDigitization.hh"

//  This stuff is *still* needed until persistency move is done
dataCards MyDataCards("Digitization");
MICEEvent simEvent;

bool MapCppTOFDigitization::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTOFDigitization";

  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  assert(configJSON.isMember("TOFconversionFactor"));
  _conversionFactor = configJSON["TOFconversionFactor"].asDouble();

  assert(configJSON.isMember("TOFpmtTimeResolution"));
  _pmtTimeResolution = configJSON["TOFpmtTimeResolution"].asDouble();

  assert(configJSON.isMember("TOFattenuationLength"));
  _attenuationLength = configJSON["TOFattenuationLength"].asDouble();

  assert(configJSON.isMember("TOFadcConversionFactor"));
  _adcConversionFactor = configJSON["TOFadcConversionFactor"].asDouble();

  assert(configJSON.isMember("TOFtdcConversionFactor"));
  _tdcConversionFactor = configJSON["TOFtdcConversionFactor"].asDouble();

  assert(configJSON.isMember("TOFpmtQuantumEfficiency"));
  _pmtQuantumEfficiency = configJSON["TOFpmtQuantumEfficiency"].asDouble();

  assert(configJSON.isMember("TOFscintLightSpeed"));
  _scintLightSpeed = configJSON["TOFscintLightSpeed"].asDouble();

  assert(configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = configJSON["reconstruction_geometry_filename"].asString();
  _module = new MiceModule(filename);

  return true;
}

bool MapCppTOFDigitization::death() {
  return true;
}

std::string  MapCppTOFDigitization::process(std::string document) {
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

  // This gets all the TOF special detectors
  std::vector<const MiceModule*> modules;
  modules = _module->findModulesByPropertyExists("Hep3Vector", "Pmt1Pos");

  //  Loop over particles and hits
  for (int i = 0; i < mc.size(); i++) {  //  i-th particle
    Json::Value particle = mc[i];
    if (!particle.isMember("hits")) {
      // if there are no MC hits, skip
      continue;
    }

    Json::Value hits = particle["hits"];

    for (int j = 0; j < hits.size(); j++) {  //  j-th hit
      Json::Value hit = hits[j];
      Json::StyledWriter writer;

      // sanity check
      assert(hit.isMember("channel_id"));
      Json::Value channel_id = hit["channel_id"];

      assert(channel_id.isMember("type"));
      if (channel_id["type"].asString() != "TOF") {
        continue;
      }

      // find module and copy the pointer to 'mod'
      const MiceModule* mod = NULL;
      for (unsigned int k = 0; !mod && k < modules.size(); ++k) {
        // Check if it's a TOF
        if (!modules[k]->propertyExists("Station", "int") ||
            !modules[k]->propertyExists("Plane", "int") ||
            !modules[k]->propertyExists("Slab", "int")) {
          //  if any of these don't exist, it's not a TOF module
          continue;
        }

        // First get MICE module property
        int modStation  = modules[k]->propertyInt("Station");
        int modPlane    = modules[k]->propertyInt("Plane");
        int modSlab     = modules[k]->propertyInt("Slab");

        // Then verify that this is a well-syntaxed TOF hit
        assert(channel_id.isMember("station_number"));
        assert(channel_id.isMember("plane"));
        assert(channel_id.isMember("slab"));

        if (modStation != channel_id["station_number"].asInt() ||
            modPlane   != channel_id["plane"].asInt() ||
            modSlab    != channel_id["slab"].asInt()) {
          // it's not this module, so continue
          continue;
        }

        mod = modules[k];  // save module pointer for later
        break;  // break loop since found module
      }  //  end for with var 'k' to find module

      //  We should have found the module by now
      assert(mod != NULL);

      if (hit["energy_deposited"] < (100 * keV)) {
        continue;
      }

      for (int k = 1; k < 3; k++) {  //  'k' is PMT number
        assert(k == 1 || k == 2);

        std::string pmtPosVar;
        if (k == 1) {
          pmtPosVar = "Pmt1Pos";
        } else {
          pmtPosVar = "Pmt2Pos";
        }

        if (!mod->propertyExists(pmtPosVar, "Hep3Vector")) {
          //  if there's no PMT positon, continue...
          continue;
        }

        Hep3Vector pmtPos = mod->propertyHep3Vector(pmtPosVar);
        Hep3Vector slabPos = mod->globalPosition();


        assert(hit.isMember("position"));

        assert(hit["position"].isMember("x"));
        assert(hit["position"].isMember("y"));
        assert(hit["position"].isMember("z"));
        assert(hit["position"]["x"].isDouble());
        assert(hit["position"]["y"].isDouble());
        assert(hit["position"]["z"].isDouble());

        Hep3Vector hitPos(hit["position"]["x"].asDouble(),
                          hit["position"]["y"].asDouble(),
                          hit["position"]["z"].asDouble());

        double dist = (hitPos - (slabPos + pmtPos)).mag();

        assert(hit.isMember("energy_deposited"));
        assert(hit["energy_deposited"].isDouble());
        double nPE = hit["energy_deposited"].asDouble() / (_conversionFactor);
        nPE *= exp(-dist / _attenuationLength);
        nPE *= _pmtQuantumEfficiency;

        double peRes = 1e-4;  //  now smear the PE by the PE resolution
        nPE = CLHEP::RandGauss::shoot(nPE, peRes);

        double gain = _adcConversionFactor;
        double tdc2time = _tdcConversionFactor;
        double t_res = _pmtTimeResolution;
        int adc = static_cast<int>(nPE / gain);

        double c =  _scintLightSpeed;
        assert(hit.isMember("time"));
        assert(hit["time"].isDouble());

        double t_mean = hit["time"].asDouble() + dist/c;

        double time = CLHEP::RandGauss::shoot(t_mean, t_res);
        int tdc = static_cast<int>(time / tdc2time);

        if (!root.isMember("digits")) {
          root["digits"] = Json::Value(Json::arrayValue);
        }
        assert(root["digits"].isArray());

        Json::Value digit;
        digit["tdc_counts"] = tdc;
        digit["adc_counts"] = adc;
        digit["channel_id"] = hit["channel_id"];
        digit["channel_id"]["pmt"] = k;
        root["digits"].append(digit);
      }  //  end for with var 'k' to loop PMTs
    }  //  end for with var 'j' to loop hits
  }  //  end for with var 'i' to loop particles

  return writer.write(root);
}
