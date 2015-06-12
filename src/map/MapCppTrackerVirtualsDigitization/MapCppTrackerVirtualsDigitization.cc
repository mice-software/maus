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
#include "src/map/MapCppTrackerVirtualsDigitization/MapCppTrackerVirtualsDigitization.hh"

#include <map>
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

#define QUANTISE_ALPHA

namespace MAUS {
  PyMODINIT_FUNC init_MapCppTrackerVirtualsDigitization(void) {
    PyWrapMapBase<MapCppTrackerVirtualsDigitization>::PyWrapMapBaseModInit
                                          ("MapCppTrackerVirtualsDigitization", "", "", "", "");
  }

  MapCppTrackerVirtualsDigitization::MapCppTrackerVirtualsDigitization() :
    MapBase<Data>("MapCppTrackerVirtualsDigitization"),
    _geometry_helper(),
    _geometry_map() {
  }

  MapCppTrackerVirtualsDigitization::~MapCppTrackerVirtualsDigitization() {
  }

  void MapCppTrackerVirtualsDigitization::_birth(const std::string& argJsonConfigDocument) {
    if (!Globals::HasInstance()) {
      GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }
    static MiceModule* mice_modules = Globals::GetMonteCarloMiceModules();
    std::vector<const MiceModule*> modules =
                        mice_modules->findModulesByPropertyString("SensitiveDetector", "SciFi");
    _geometry_helper = SciFiGeometryHelper(modules);
    _geometry_helper.Build();
    _geometry_map = _geometry_helper.GeometryMap();

    _default_npe = 10.0;
    _assignment_tolerance = 1.0e-6;

    _make_digits = false;
    _make_clusters = true;
    _make_perfect_clusters = false;
    _make_spacepoints = false;
    _make_perfect_spacepoints = false;

    _spacepoint_plane_num = 0;

    _helix_measure = new SciFiHelicalMeasurements(&_geometry_helper);
  }


  void MapCppTrackerVirtualsDigitization::_death() {
  }

  void MapCppTrackerVirtualsDigitization::_process(MAUS::Data* data) const {
    Spill& spill = *(data->GetSpill());

    if (!spill.GetMCEvents()) {
      throw MAUS::Exception(Exception::recoverable,
              "MC event array not initialised.",
              "MapCppTrackerVirtualsDigitization::process");
    }

    // ================= Reconstruction =========================

    // Check the Recon event array is initialised, and if not make it so
    if ( !spill.GetReconEvents() ) {
      ReconEventPArray* revts = new ReconEventPArray();
      spill.SetReconEvents(revts);
    }
    // Construct digits from virtual plane hits for each MC event
    for ( size_t event_i = 0; event_i < spill.GetMCEventSize(); event_i++ ) {
      MCEvent *mc_evt = spill.GetMCEvents()->at(event_i);
      SciFiEvent *sf_evt = new SciFiEvent();

      if (_make_digits) {
        SciFiDigitPArray digits;
        construct_digits(mc_evt->GetVirtualHits(), spill.GetSpillNumber(), event_i, digits);
        sf_evt->set_digits(digits);
      }

      if (_make_clusters) {
        SciFiClusterPArray clusters;
        construct_clusters(mc_evt->GetVirtualHits(), spill.GetSpillNumber(), event_i, clusters);
        sf_evt->set_clusters(clusters);
      }

      if (_make_spacepoints) {
        SciFiSpacePointPArray spacepoints;
        construct_spacepoints(mc_evt->GetVirtualHits(), spill.GetSpillNumber(), event_i, spacepoints);
        sf_evt->set_spacepoints(spacepoints);
      }

      if (_make_perfect_spacepoints) {
        SciFiSpacePointPArray spacepoints;
        construct_perfect_spacepoints(spill.GetSpillNumber(), event_i, spacepoints);
        sf_evt->set_spacepoints(spacepoints);
      }

      // If there is already a Recon event associated with this MC event, add the SciFiEvent to it,
      // otherwise make a new Recon event to hold the SciFiEvent
      if ( spill.GetReconEvents()->size() > event_i ) {
        spill.GetReconEvents()->at(event_i)->SetSciFiEvent(sf_evt);
      } else {
        ReconEvent *revt = new ReconEvent();
        revt->SetPartEventNumber(event_i);
        revt->SetSciFiEvent(sf_evt);
        spill.GetReconEvents()->push_back(revt);
      }
    }
  }

  void MapCppTrackerVirtualsDigitization::construct_digits(VirtualHitArray* hits, int spill_num,
                                                   int event_num, SciFiDigitPArray& digits) const {
    for ( unsigned int hit_i = 0; hit_i < hits->size(); hit_i++ ) {
      bool found = false;
      VirtualHit* a_hit    = &hits->at(hit_i);
      double time          = a_hit->GetTime();
      ThreeVector position = a_hit->GetPosition();

      // Pull tracker/station/plane information from geometry
      int tracker;
      int station;
      int plane;
      int channelNumber;
      double alpha;
      for (SciFiTrackerMap::const_iterator gmIt = _geometry_map.begin();
                                                             gmIt != _geometry_map.end(); ++gmIt) {
        const SciFiPlaneMap& planes = gmIt->second.Planes;
        tracker = gmIt->first;

        for (SciFiPlaneMap::const_iterator plIt = planes.begin(); plIt != planes.end(); ++plIt) {
          SciFiPlaneGeometry geo = plIt->second;

          if (fabs(geo.GlobalPosition.z() - position.z()) < _assignment_tolerance) {
            int id = plIt->first;
            station = ((abs(id)-1) / 3) + 1;
            plane = (abs(id) - 1) % 3;

            alpha = _compute_alpha(a_hit, geo, tracker);
            channelNumber = floor(0.5 + ((7.0 * geo.CentralFibre) + (2.0 * alpha / geo.Pitch)) / 7.0);

            SciFiDigit* a_digit = new SciFiDigit(spill_num, event_num,
                                        tracker, station, plane, channelNumber, _default_npe, time);
            digits.push_back(a_digit);
            found = true;
            break;
          }
        }
        if (found) break;
      }
    }
  }


  void MapCppTrackerVirtualsDigitization::construct_clusters(VirtualHitArray* hits, int spill_num,
                                               int event_num, SciFiClusterPArray& clusters) const {
    for ( unsigned int hit_i = 0; hit_i < hits->size(); hit_i++ ) {
      bool found = false;
      VirtualHit* a_hit    = &hits->at(hit_i);
      ThreeVector position = a_hit->GetPosition();

      // Pull tracker/station/plane information from geometry
      int tracker;
      int station;
      int plane;
      double alpha;
      for (SciFiTrackerMap::const_iterator gmIt = _geometry_map.begin();
                                                             gmIt != _geometry_map.end(); ++gmIt) {
        const SciFiPlaneMap& planes = gmIt->second.Planes;
        tracker = gmIt->first;
        ThreeVector tracker_position = gmIt->second.Position;
        HepRotation tracker_rotation = gmIt->second.Rotation;
        HepRotation rotation = tracker_rotation.inverse();

        for (SciFiPlaneMap::const_iterator plIt = planes.begin(); plIt != planes.end(); ++plIt) {
          SciFiPlaneGeometry geo = plIt->second;

          if (fabs(geo.GlobalPosition.z() - position.z()) < _assignment_tolerance) {
            int id = plIt->first;
            station = ((abs(id)-1) / 3) + 1;
            plane = (abs(id) - 1) % 3;
            alpha = _compute_alpha(a_hit, geo, tracker);

            ThreeVector momentum = a_hit->GetMomentum();

            position *= rotation;
            position.setZ(geo.Position.z());

            momentum *= rotation;

            int channelNumber = floor(0.5 + ((7.0 * geo.CentralFibre) + (2.0 * alpha / geo.Pitch)) / 7.0);

            // Change alpha to be quantised by the fibres!
#ifdef QUANTISE_ALPHA
            double new_alpha = 3.5 * geo.Pitch * double(channelNumber - geo.CentralFibre);
#else
            double new_alpha = alpha;
#endif

            SciFiCluster* a_cluster = new SciFiCluster();

            a_cluster->set_plane(plane);
            a_cluster->set_station(station);
            a_cluster->set_tracker(tracker);
            a_cluster->set_channel(channelNumber);
            a_cluster->set_spill(spill_num);
            a_cluster->set_event(event_num);
            a_cluster->set_npe(10);
            a_cluster->set_position(position);
            a_cluster->set_direction(geo.Direction);
            a_cluster->set_alpha(new_alpha);
            a_cluster->set_id(id);
            a_cluster->set_used(false);

//            delete a_cluster;
            clusters.push_back(a_cluster);

            TMatrixD vector(5, 1);
            vector(0, 0) = position.x();
            vector(1, 0) = momentum.x();
            vector(2, 0) = position.y();
            vector(3, 0) = momentum.y();
            vector(4, 0) = 1.0 / momentum.z();

            TMatrix covariance(5, 5);

            Kalman::State temp_state( vector, covariance, position.z() );
            temp_state.SetId( id );
            Kalman::State temp_measurement = _helix_measure->Measure(temp_state);

            found = true;
            break;
          }
        }
        if (found) break;
      }
    }
  }

  void MapCppTrackerVirtualsDigitization::construct_spacepoints(VirtualHitArray* hits, int spill_num,
                                               int event_num, SciFiSpacePointPArray& spacepoints) const {
    for ( unsigned int hit_i = 0; hit_i < hits->size(); hit_i++ ) {
      bool found = false;
      VirtualHit* a_hit    = &hits->at(hit_i);
      ThreeVector position = a_hit->GetPosition();
      ThreeVector momentum = a_hit->GetMomentum();

      // Pull tracker/station/plane information from geometry
      int tracker;
      int station;
      int plane;
      for (SciFiTrackerMap::const_iterator gmIt = _geometry_map.begin();
                                                             gmIt != _geometry_map.end(); ++gmIt) {
        const SciFiPlaneMap& planes = gmIt->second.Planes;
        tracker = gmIt->first;
        ThreeVector tracker_position = gmIt->second.Position;
        HepRotation tracker_rotation = gmIt->second.Rotation;
        HepRotation rotation = tracker_rotation.inverse();

        for (SciFiPlaneMap::const_iterator plIt = planes.begin(); plIt != planes.end(); ++plIt) {
          SciFiPlaneGeometry geo = plIt->second;
          int id = plIt->first;
          station = ((abs(id)-1) / 3) + 1;
          plane = (abs(id) - 1) % 3;

          if ( plane != _spacepoint_plane_num ) continue;

          if (fabs(geo.GlobalPosition.z() - position.z()) < _assignment_tolerance) {

            position *= rotation;
            position.setZ(geo.Position.z());

            momentum *= rotation;

            SciFiSpacePoint* spoint = new SciFiSpacePoint();

            spoint->set_tracker(tracker);
            spoint->set_station(station);
            spoint->set_npe(10);
            spoint->set_position(position);

            spacepoints.push_back(spoint);

            found = true;
            break;
          }
        }
        if (found) break;
      }
    }
  }

  void MapCppTrackerVirtualsDigitization::construct_perfect_spacepoints(int spill_num,
                                               int event_num, SciFiSpacePointPArray& spacepoints) const {

    //TODO : Maybe later...



  }

  double MapCppTrackerVirtualsDigitization::_compute_alpha(VirtualHit* ahit,
                                                SciFiPlaneGeometry& geometry, int tracker ) const {
    HepRotation rot = _geometry_helper.GetReferenceRotation(tracker);
    ThreeVector globalPos = ahit->GetPosition();
//    ThreeVector pos = globalPos - geometry.GlobalPosition;
    ThreeVector pos = globalPos;

    pos *= rot;
    ThreeVector vec = geometry.Direction.Orthogonal().Unit();

    double alpha = pos.Dot(vec);
    return alpha;

    // The old method alpha = fibre num - central fibre
//    int fibreNumber = ( 2.0 * dist / geometry.Pitch ) + (7.0*geometry.CentralFibre);
//    int alpha = floor( fibreNumber / 7.0 );
//    int channelNumber = alpha;// + geometry.CentralFibre;
  }
} // ~namespace MAUS

