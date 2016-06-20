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
    Json::Value* json = Globals::GetConfigurationCards();
    _smear_value = (*json)["SciFiTestVirtualSmear"].asDouble();
    std::string test_method = (*json)["SciFiTestVirtualMethod"].asString();

    if ( test_method == "virtual" ) {
      _test_method = 2;
    } else if ( test_method == "straight" ) {
      _test_method = 0;
    } else if ( test_method == "helix" ) {
      _test_method = 1;
    } else {
      throw MAUS::Exception(Exception::nonRecoverable, "Unknown Test Method for virtual digitiser",
                                                 "MapCppTrackerVirtualsDigitization::_birth(...)");
    }

    Json::Value& straight_desc = (*json)["SciFiTestVirtualTracksStraight"];
    _straight_rms_pos = straight_desc["rms_position"].asDouble();
    _straight_rms_ang = straight_desc["rms_angle"].asDouble();

    Json::Value& helix_desc = (*json)["SciFiTestVirtualTracksHelix"];
    _helix_rms_pos = helix_desc["rms_position"].asDouble();
    _helix_rms_pt = helix_desc["rms_pt"].asDouble();
    _helix_pz = helix_desc["pz"].asDouble();


    static MiceModule* mice_modules = Globals::GetMonteCarloMiceModules();
    std::vector<const MiceModule*> modules =
                        mice_modules->findModulesByPropertyString("SensitiveDetector", "SciFi");
    _geometry_helper = SciFiGeometryHelper(modules);
    _geometry_helper.Build();
    _geometry_map = _geometry_helper.GeometryMap();

    _default_npe = 10.0;
    _assignment_tolerance = 1.0e-6;

    _make_digits = (*json)["SciFiTestVirtualMakeDigits"].asBool();
    _make_clusters = (*json)["SciFiTestVirtualMakeClusters"].asBool();
    _make_spacepoints = (*json)["SciFiTestVirtualMakeSpacepoints"].asBool();

    _spacepoint_plane_num = 0;
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
    if (!spill.GetReconEvents()) {
      ReconEventPArray* revts = new ReconEventPArray();
      spill.SetReconEvents(revts);
    }

    for (size_t event_i = 0; event_i < spill.GetMCEventSize(); event_i++) {
      MCEvent *mc_evt = spill.GetMCEvents()->at(event_i);
      SciFiEvent *sf_evt = new SciFiEvent();

      switch (_test_method) {
        case 0 :
          _process_straight(sf_evt, spill.GetSpillNumber(), event_i);
          break;
        case 1 :
          _process_helix(sf_evt, spill.GetSpillNumber(), event_i);
          break;
        case 2 :
        // Construct digits from virtual plane hits for each MC event
        if (_make_digits) {
          SciFiDigitPArray digits;
          construct_virtual_digits(mc_evt->GetVirtualHits(),
                                                          spill.GetSpillNumber(), event_i, digits);
          sf_evt->set_digits(digits);
        }

        if (_make_clusters) {
          SciFiClusterPArray clusters;
          construct_virtual_clusters(mc_evt->GetVirtualHits(),
                                                        spill.GetSpillNumber(), event_i, clusters);
          sf_evt->set_clusters(clusters);
        }

        if (_make_spacepoints) {
          SciFiSpacePointPArray spacepoints;
          construct_virtual_spacepoints(mc_evt->GetVirtualHits(),
                                                     spill.GetSpillNumber(), event_i, spacepoints);
          sf_evt->set_spacepoints(spacepoints);
        }
        break;
        default:
          break;
      }

      // If there is already a Recon event associated with this MC event, add the SciFiEvent to it,
      // otherwise make a new Recon event to hold the SciFiEvent
      if (spill.GetReconEvents()->size() > event_i) {
        spill.GetReconEvents()->at(event_i)->SetSciFiEvent(sf_evt);
      } else {
        ReconEvent *revt = new ReconEvent();
        revt->SetPartEventNumber(event_i);
        revt->SetSciFiEvent(sf_evt);
        spill.GetReconEvents()->push_back(revt);
      }
    }
  }


  void MapCppTrackerVirtualsDigitization::_process_straight(MAUS::SciFiEvent* sf_event,
                                                              int spill_num, int event_num) const {
    double x0 = CLHEP::RandGauss::shoot(0.0, _straight_rms_pos);
    double y0 = CLHEP::RandGauss::shoot(0.0, _straight_rms_pos);
    double mx = CLHEP::RandGauss::shoot(0.0, _straight_rms_ang);
    double my = CLHEP::RandGauss::shoot(0.0, _straight_rms_ang);

    double time = 0.0;

    SciFiDigitPArray digits;
    SciFiClusterPArray clusters;
    SciFiSpacePointPArray spacepoints;

    int id;
    double Z;
    int tracker;
    int station;
    int plane;
    double alpha;
    int channelNumber;

    for (SciFiTrackerMap::const_iterator gmIt = _geometry_map.begin();
                                                           gmIt != _geometry_map.end(); ++gmIt) {
      const SciFiPlaneMap& planes = gmIt->second.Planes;
      tracker = gmIt->first;
      HepRotation tracker_rotation = gmIt->second.Rotation;
      ThreeVector tracker_position = gmIt->second.Position;

      if (_make_clusters) {
        for (SciFiPlaneMap::const_iterator plIt = planes.begin(); plIt != planes.end(); ++plIt) {
          id = plIt->first;
          station = ((abs(id)-1) / 3) + 1;
          plane = (abs(id) - 1) % 3;

          SciFiPlaneGeometry geo = plIt->second;

          Z = geo.Position.z();
          ThreeVector position(x0 + Z*mx, y0 + Z*my, Z);

          alpha = _compute_alpha(position, geo);
          channelNumber = floor(0.5 + ((7.0 * geo.CentralFibre) - (2.0 * alpha / geo.Pitch)) / 7.0);

          if (_make_digits) {
            SciFiDigit* a_digit = new SciFiDigit(spill_num, event_num,
                                       tracker, station, plane, channelNumber, _default_npe, time);
            digits.push_back(a_digit);
          }

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
          a_cluster->set_alpha(alpha);
          a_cluster->set_id(id);
          a_cluster->set_used(false);
          clusters.push_back(a_cluster);
        }
      }

      if (_make_spacepoints) {
        for (SciFiPlaneMap::const_iterator plIt = planes.begin(); plIt != planes.end(); ++plIt) {
          id = plIt->first;
          station = ((abs(id)-1) / 3) + 1;
          plane = (abs(id) - 1) % 3;
          if ( plane != 1 ) continue;

          SciFiPlaneGeometry geo = plIt->second;
          Z = geo.Position.z();
          ThreeVector position(x0 + Z*mx, y0 + Z*my, Z);
          double smear_x = CLHEP::RandGauss::shoot(0.0, _smear_value);
          double smear_y = CLHEP::RandGauss::shoot(0.0, _smear_value);
          position.SetX(position.x() + smear_x);
          position.SetY(position.y() + smear_y);

          ThreeVector spoint_pos = position;
          spoint_pos *= tracker_rotation;
          spoint_pos.setZ(geo.Position.z());

          SciFiSpacePoint* spoint = new SciFiSpacePoint();
          spoint->set_tracker(tracker);
          spoint->set_station(station);
          spoint->set_npe(10);
          spoint->set_position(spoint_pos);
          spoint->set_global_position(position);

          if (_make_clusters) {
            for ( int plane = 0; plane < 3; ++plane ) {
              for ( SciFiClusterPArray::iterator clIt = clusters.begin();
                                                                 clIt != clusters.end(); ++clIt ) {
                if ( (*clIt)->get_tracker() == tracker &&
                     (*clIt)->get_station() == station &&
                     (*clIt)->get_plane() == plane ) {
                  spoint->add_channel((*clIt));
                }
              }
            }
          }
          spacepoints.push_back(spoint);
        }
      }
    }
    sf_event->set_digits(digits);
    sf_event->set_clusters(clusters);
    sf_event->set_spacepoints(spacepoints);
  }


  void MapCppTrackerVirtualsDigitization::_process_helix(MAUS::SciFiEvent* event,
                                                              int spill_num, int event_num) const {
    // Not Yet Implemented
  }


  void MapCppTrackerVirtualsDigitization::construct_virtual_digits(VirtualHitArray* hits,
                                    int spill_num, int event_num, SciFiDigitPArray& digits) const {
    for (unsigned int hit_i = 0; hit_i < hits->size(); hit_i++) {
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
            position.setZ(geo.Position.z());
            alpha = _compute_alpha(position, geo);
            channelNumber = floor(0.5 + ((7.0 * geo.CentralFibre) +
                                                                 (2.0 * alpha / geo.Pitch)) / 7.0);

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


  void MapCppTrackerVirtualsDigitization::construct_virtual_clusters(VirtualHitArray* hits,
                                int spill_num, int event_num, SciFiClusterPArray& clusters) const {
    for (unsigned int hit_i = 0; hit_i < hits->size(); hit_i++) {
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

        for (SciFiPlaneMap::const_iterator plIt = planes.begin(); plIt != planes.end(); ++plIt) {
          SciFiPlaneGeometry geo = plIt->second;

          if (fabs(geo.GlobalPosition.z() - position.z()) < _assignment_tolerance) {
            int id = plIt->first;
            station = ((abs(id)-1) / 3) + 1;
            plane = (abs(id) - 1) % 3;

            position *= tracker_rotation;
            position.setZ(geo.Position.z());

            alpha = _compute_alpha(position, geo);

            int channelNumber = floor(0.5 + ((7.0 * geo.CentralFibre) -
                                                                 (2.0 * alpha / geo.Pitch)) / 7.0);

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
            a_cluster->set_alpha(alpha);
            a_cluster->set_id(id);
            a_cluster->set_used(false);

            clusters.push_back(a_cluster);

            found = true;
            break;
          }
        }
        if (found) break;
      }
    }
  }


  void MapCppTrackerVirtualsDigitization::construct_virtual_spacepoints(VirtualHitArray* hits,
                          int spill_num, int event_num, SciFiSpacePointPArray& spacepoints) const {
    for (unsigned int hit_i = 0; hit_i < hits->size(); hit_i++) {
      bool found = false;
      VirtualHit* a_hit    = &hits->at(hit_i);
      ThreeVector position = a_hit->GetPosition();

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

        for (SciFiPlaneMap::const_iterator plIt = planes.begin(); plIt != planes.end(); ++plIt) {
          SciFiPlaneGeometry geo = plIt->second;
          int id = plIt->first;
          station = ((abs(id)-1) / 3) + 1;
          plane = (abs(id) - 1) % 3;
          if ( plane != 1 ) continue;

          if (fabs(geo.GlobalPosition.z() - position.z()) < _assignment_tolerance) {
            ThreeVector spoint_pos = position;

            spoint_pos *= tracker_rotation;
            spoint_pos.setZ(geo.Position.z());

            double smear_x = CLHEP::RandGauss::shoot(0.0, _smear_value);
            double smear_y = CLHEP::RandGauss::shoot(0.0, _smear_value);
            spoint_pos.SetX(spoint_pos.x() + smear_x);
            spoint_pos.SetY(spoint_pos.y() + smear_y);

            SciFiSpacePoint* spoint = new SciFiSpacePoint();

            spoint->set_tracker(tracker);
            spoint->set_station(station);
            spoint->set_npe(10);
            spoint->set_position(spoint_pos);
            spoint->set_global_position(position);

            spacepoints.push_back(spoint);

            found = true;
            break;
          }
        }
        if (found) break;
      }
    }
  }


  double MapCppTrackerVirtualsDigitization::_compute_alpha(ThreeVector& position,
                                                SciFiPlaneGeometry& geometry) const {

    ThreeVector vec = geometry.Direction.Orthogonal().Unit();
    double alpha = position.Dot(vec);
    double channelNumber = 0.5 + ((7.0 * geometry.CentralFibre) -
                                                             (2.0 * alpha / geometry.Pitch)) / 7.0;

    channelNumber = floor(channelNumber*2.0) / 2.0; // Force half integer binning

    double new_alpha;
    if (_smear_value < 0.0) {
      new_alpha = 3.5 * geometry.Pitch *
                                      (static_cast<double>(geometry.CentralFibre) - channelNumber);
    } else {
      double smear_amount = CLHEP::RandGauss::shoot(0.0, _smear_value);
      new_alpha = alpha + smear_amount;
    }

    return new_alpha;

//    HepRotation rot = _geometry_helper.GetReferenceRotation(tracker);
//    ThreeVector globalPos = ahit->GetPosition();
//    ThreeVector pos = position - geometry.GlobalPosition;
//    ThreeVector pos = globalPos - geometry.GlobalPosition;
//    ThreeVector pos = globalPos;
//
//    pos *= rot;
//    ThreeVector vec = geometry.Direction.Orthogonal().Unit();
//
//    double alpha = pos.Dot(vec);
//    double alpha = position.Dot(vec);
//    return alpha;
    // The old method alpha = fibre num - central fibre
//    int fibreNumber = (2.0 * dist / geometry.Pitch) + (7.0*geometry.CentralFibre);
//    int alpha = floor(fibreNumber / 7.0);
//    int channelNumber = alpha;// + geometry.CentralFibre;
  }
} // ~namespace MAUS

