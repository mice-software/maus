#include "MapCppBeamMaker.h"

bool MapCppBeamMaker::Birth(std::string argJsonConfigDocument) {
  _classname = "MapCppBeamMaker";

  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, configJSON);
  if (!parsingSuccessful) {
    return false;
  }
  
  // Here is where you grab any 'datacards' that you want and it checks
  // things like types.  You probably want things like sigma_x, sigma_px,
  // and number of events per spill (30?).  These values get set in
  // core/ConfigurationDefaults.py

  /*
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
  std::string filename = configJSON["reconstruction_geometry_filename"].asString();*/

  _particlesPerSpill = 30;
        
  return true;  // return false if there is an error
}

bool MapCppBeamMaker::Death() {
  return true;
}

std::string  MapCppBeamMaker::Process(std::string document){
  //  JsonCpp setup
  Json::Value spill;   // will contains the spill value after parsing.
  Json::Reader reader;
  Json::FastWriter writer;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(document, spill);
  if (!parsingSuccessful) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["MapCppBeamMaker"] = ss.str();
    spill["errors"] = errors;
    return writer.write(spill);
  }

  // Check if the JSON document has a 'mc' branch, else create it
  if (spill.isMember("mc")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: there already exists an MC branch, skipping...\n";
    errors["MapCppBeamMaker"] = ss.str();
    spill["errors"] = errors;
    return writer.write(spill);
  }

  spill["mc"] = Json::Value(Json::arrayValue);
  

  //  Loop over particles and hits
  for (int i=0; i<_particlesPerSpill; i++){  //  i-th particle
    Json::Value particle;

    //
    //  INSERT LOGIC HERE
    //

    // Consult for the format:
    // http://micewww.pp.rl.ac.uk/projects/maus/wiki/DataStructure#MC-Particles

    Json::Value position; // mm
    position["x"] = 0.;
    position["y"] = 0.;
    position["z"] = 0.;

    particle["position"] = position;

    Json::Value unit_momentum; // all components less than 1
    unit_momentum["x"] = 0.;
    unit_momentum["y"] = 0.;
    unit_momentum["z"] = 0.;  

    particle["unit_momentum"] = unit_momentum;

    particle["energy"] = 0.; // MeV

    particle["particle_id"] = 13; // PDG PID
    
  }  //  end for with var 'i' to loop particles

  return writer.write(spill);
}  
