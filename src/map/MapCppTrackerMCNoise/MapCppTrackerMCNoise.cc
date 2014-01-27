
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

#include "src/map/MapCppTrackerMCNoise/MapCppTrackerMCNoise.hh"

namespace MAUS {
MapCppTrackerMCNoise::MapCppTrackerMCNoise()
    : _spill_json(NULL), _spill_cpp(NULL) {
}

MapCppTrackerMCNoise::~MapCppTrackerMCNoise() {
    if (_spill_json != NULL) {
        delete _spill_json;
    }
    if (_spill_cpp != NULL) {
        delete _spill_cpp;
    }
}

bool MapCppTrackerMCNoise::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerMCNoise";

  try {
    if (!Globals::HasInstance()) {
      GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }

    static MiceModule* _mice_modules = Globals::GetMonteCarloMiceModules();
    SF_modules = _mice_modules->findModulesByPropertyString("SensitiveDetector", "SciFi");
    _configJSON = Globals::GetConfigurationCards();
    poisson_mean = -log(1.0-(*_configJSON)["SciFiDarkCountProababilty"].asDouble());
    return true;
  } catch(Exception& exception) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exception, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

bool MapCppTrackerMCNoise::death() {
  return true;
}

std::string MapCppTrackerMCNoise::process(std::string document) {
  Json::FastWriter writer;

  // Set up a spill object, then continue only if MC event array is initialised
  read_in_json(document);
  Spill& spill = *_spill_cpp;

  if ( spill.GetMCEvents() ) {
  } else {
    std::cerr << "MC event array not initialised, aborting noise for this spill\n";
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << "MC event array not initialised, aborting noise";
    errors["missing_branch"] = ss.str();
    save_to_json(spill);
    return writer.write(*_spill_json);
  }

  // ================= Noise ==================================

  // Adds Effects of Noise from Electrons to MC
  dark_count(spill);
  // ==========================================================

  save_to_json(spill);
  return writer.write(*_spill_json);
}

void MapCppTrackerMCNoise::read_in_json(std::string json_data) {
  Json::Reader reader;
  Json::Value json_root;
  Json::FastWriter writer;

  if (_spill_cpp != NULL) {
    delete _spill_cpp;
    _spill_cpp = NULL;
  }

  try {
    json_root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    _spill_cpp = spill_proc.JsonToCpp(json_root);
  } catch(...) {
    std::cerr << "Bad json document" << std::endl;
    _spill_cpp = new Spill();
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    _spill_cpp->GetErrors();
  }
}

void MapCppTrackerMCNoise::save_to_json(Spill &spill) {
    SpillProcessor spill_proc;
    if (_spill_json != NULL) {
        delete _spill_json;
        _spill_json = NULL;
    }
    _spill_json = spill_proc.CppToJson(spill, "");
}

void MapCppTrackerMCNoise::dark_count(Spill &spill) {
  int spill_n = spill.GetSpillNumber();
  double time1 = 0.;  // Fix: Need to assign a value to this!
  double D_NPE;

  /*************************************************************************************
  * Description:
  * This block of code examines each SciFi channel and determines how many dark count
  * PE are present as described by a Poisson distribution (as determined by a CLHEP
  * library).  The Poisson mean is determined when the map is first called by the by a
  * a simple calculation in the birth function
  **************************************************************************************/

  for ( unsigned int event_i = 0; event_i < spill.GetMCEvents()->size(); event_i++ ) {
    SciFiNoiseHitArray* noise_hits = new SciFiNoiseHitArray();
    for ( unsigned int mod_i = 0; mod_i < SF_modules.size(); mod_i++ ) {
      int nChannels = static_cast <int>
                      (2*((SF_modules[mod_i]->propertyDouble("CentralFibre"))+0.5));
      for ( int chan_i = 0; chan_i < nChannels; chan_i++ ) {
        D_NPE = CLHEP::RandPoisson::shoot(poisson_mean);

        /********************************************************************************
        * Description:
        * If there is a dark count a new digit is created.  A check is made to determine
        * if a digit already exist here, if so the npe is just added, otherwise the
        * noise digit is added to a temp digit array.
        ********************************************************************************/

        if ( D_NPE ) {
          int tracker = SF_modules[mod_i]->propertyInt("Tracker");
          int station = SF_modules[mod_i]->propertyInt("Station");
          int plane   = SF_modules[mod_i]->propertyInt("Plane");
          SciFiNoiseHit a_noise_hit(spill_n, event_i,
                                    tracker, station, plane,
                                    chan_i, D_NPE, time1);
		  noise_hits->push_back(a_noise_hit);
        }
      }
    }

    /************************************************************************************
    * Description:
    * All the digits that do not overlap with a digit from an SciFiHit are now added
    * into the SciFiDigit array and merged back into the spill.
    ************************************************************************************/

    spill.GetMCEvents()->at(event_i)->SetSciFiNoiseHits(noise_hits);
  } // end of event_i of spill_n, start of event_i+1.
}

} // ~namespace MAUS

