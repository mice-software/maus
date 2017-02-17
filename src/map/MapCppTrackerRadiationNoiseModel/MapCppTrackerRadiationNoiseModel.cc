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

#include "src/map/MapCppTrackerRadiationNoiseModel/MapCppTrackerRadiationNoiseModel.hh"
#include <sstream>
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppTrackerRadiationNoiseModel(void) {
  PyWrapMapBase<MapCppTrackerRadiationNoiseModel>::PyWrapMapBaseModInit
                                        ("MapCppTrackerRadiationNoiseModel", "", "", "", "");
}


MapCppTrackerRadiationNoiseModel::MapCppTrackerRadiationNoiseModel()
                                              : MapBase<Data>("MapCppTrackerRadiationNoiseModel") {
    // Do nothing
}


MapCppTrackerRadiationNoiseModel::~MapCppTrackerRadiationNoiseModel() {
  delete _random;
  // Do nothing
}


void MapCppTrackerRadiationNoiseModel::_birth(const std::string& argJsonConfigDocument) {
  // Pull out the global settings
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  Json::Value* json = Globals::GetConfigurationCards();
//  _clusters_on        = (*json)["SciFiClusterReconOn"].asBool();


  _random = new TRandom3();

  // Build the geometery helper instance
  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();

  _cavityBrightness = (*json)["SciFiRadiationNoise_CavityBrightness"].asDouble();

  // Make a list of the cavities in the experment
  std::vector<double> cavityPositions;
  std::vector<const MiceModule*> cavity_modules =
    module->findModulesByPropertyExists("string", "CavityMode");

  for (std::vector<const MiceModule*>::const_iterator it = cavity_modules.begin();
      it != cavity_modules.end(); ++it) {
    cavityPositions.push_back((*it)->globalPosition().z());
  }

  if (cavityPositions.size() == 0) {
    std::cerr << "No cavities found. No RF noise will be modelled.\n";
  } else {
    // Cycle through planes, find the nearest cavity and apply 1/r^2 reduction to
    //  the nominal rate.
    for (std::vector<const MiceModule*>::const_iterator it = modules.begin();
        it != modules.end(); ++it) {
      int tracker = (*it)->propertyInt("Tracker");
      int station = (*it)->propertyInt("Station");
      int plane = (*it)->propertyInt("Plane");

      int plane_id = (station-1)*3+plane+1;
      if (tracker == 0) plane_id *= -1;

      double plane_z = (*it)->globalPosition().z();
      double nearestCavity = 1.0e300;
      for (std::vector<double>::iterator cav_it = cavityPositions.begin();
        cav_it != cavityPositions.end(); ++cav_it) {

        double separation = fabs((*cav_it) - plane_z);
        if (separation < nearestCavity) {
          nearestCavity = separation;
        }
      }

      // 1/r^2 losses assuming spherical emission
      double planeRadius = (*it)->propertyDouble("ActiveRadius");
      _planeNoiseRates[plane_id] = _cavityBrightness*planeRadius*planeRadius /
                                                                 (4.0*nearestCavity*nearestCavity);
    }
  }

  // Need a more precise determination - this will do for now!
//  for (int plane_it = -15; plane_it <= 15; ++plane_it) {
//    _planeNoiseRates[plane_it] = 100.0;
//  }

  _npe_mean = 5.0;
  _npe_rms = 4.0;
  _centralFibre = 106.5;
}


void MapCppTrackerRadiationNoiseModel::_death() {
  // Do nothing
}


void MapCppTrackerRadiationNoiseModel::_process(Data* data) const {
  Spill& spill = *(data->GetSpill());

  /* return if not physics spill */
  if (spill.GetDaqEventType() != "physics_event")
    return;

  if (spill.GetReconEvents()) {
    for (unsigned int k = 0; k < spill.GetReconEvents()->size(); k++) {
      SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();
      if (!event) {
        continue;
      }

      int spillNum = spill.GetSpillNumber();
      int eventNum = spill.GetReconEvents()->at(k)->GetPartEventNumber();

      for (std::map<int, double>::const_iterator plane_it = _planeNoiseRates.begin();
          plane_it != _planeNoiseRates.end(); ++plane_it) {

        int plane_id = plane_it->first;

        int tracker = (plane_id < 0?0:1);
        int temp_plane_id = abs(plane_id);
        int station = ((temp_plane_id - 1) / 3) + 1;
        int plane = (temp_plane_id - 1) % 3;

        int number_digits = _getNumberDigits(plane_id);
          for (int digit_num = 0; digit_num < number_digits; ++digit_num) {
            int channel = _getRandomChannel();
            double npe = fabs(_random->Gaus(_npe_mean, _npe_rms));
            double time = 0.0;

            SciFiDigit* digit = new SciFiDigit(spillNum, eventNum,
                                     tracker, station, plane, channel, npe, time);

            event->add_digit(digit);
          }
      }
    }
  } else {
    std::cout << "No recon events found\n";
  }
}

int MapCppTrackerRadiationNoiseModel::_getNumberDigits(int plane) const {
  std::map<int, double>::const_iterator result = _planeNoiseRates.find(plane);
  if (result == _planeNoiseRates.end()) {
    throw Exceptions::Exception(Exceptions::nonRecoverable,
        "Looking for a plane that doesn't exist!",
        "MapCppTrackerRadiationNoiseModel::_getNumberDigits(int plane)");
  }

  double rate = _planeNoiseRates.find(plane)->second;
  return _random->Poisson(rate);
}

int MapCppTrackerRadiationNoiseModel::_getRandomChannel() const {
  return static_cast<int>(_random->Uniform(0.0, _centralFibre*2.0));
}

} // ~namespace MAUS
