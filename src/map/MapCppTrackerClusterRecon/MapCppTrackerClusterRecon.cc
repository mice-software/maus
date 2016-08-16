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

#include "src/map/MapCppTrackerClusterRecon/MapCppTrackerClusterRecon.hh"
#include <sstream>
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppTrackerClusterRecon(void) {
  PyWrapMapBase<MapCppTrackerClusterRecon>::PyWrapMapBaseModInit
                                        ("MapCppTrackerClusterRecon", "", "", "", "");
}


MapCppTrackerClusterRecon::MapCppTrackerClusterRecon() : MapBase<Data>("MapCppTrackerClusterRecon"),
                                           _clusters_on(true) {
    // Do nothing
}


MapCppTrackerClusterRecon::~MapCppTrackerClusterRecon() {
  // Do nothing
}


void MapCppTrackerClusterRecon::_birth(const std::string& argJsonConfigDocument) {
  // Pull out the global settings
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  Json::Value* json = Globals::GetConfigurationCards();
  _clusters_on        = (*json)["SciFiClusterReconOn"].asBool();
  _size_exception     = (*json)["SciFiClustExcept"].asInt();
  _min_npe            = (*json)["SciFiNPECut"].asDouble();

  // Build the geometery helper instance
  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();
  SciFiTrackerMap& geo_map = _geometry_helper.GeometryMap();

  // Set up cluster reconstruction
  _cluster_recon = SciFiClusterRec(_size_exception, _min_npe, geo_map);
}


void MapCppTrackerClusterRecon::_death() {
  // Do nothing
}


void MapCppTrackerClusterRecon::_process(Data* data) const {
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

      _set_field_values(event);

      if ( _clusters_on ) {
      // Clear any exising higher level data
        event->clear_clusters();
        // Build Clusters.
        if (event->digits().size()) {
          _cluster_recon.process(*event);
        }
      }
    }
  } else {
    std::cout << "No recon events found\n";
  }
}

void MapCppTrackerClusterRecon::_set_field_values(SciFiEvent* event) const {
  event->set_mean_field_up(_geometry_helper.GetFieldValue(0));
  event->set_mean_field_down(_geometry_helper.GetFieldValue(1));

  event->set_variance_field_up(_geometry_helper.GetFieldVariance(0));
  event->set_variance_field_down(_geometry_helper.GetFieldVariance(1));

  event->set_range_field_up(_geometry_helper.GetFieldRange(0));
  event->set_range_field_down(_geometry_helper.GetFieldRange(1));
}

} // ~namespace MAUS
