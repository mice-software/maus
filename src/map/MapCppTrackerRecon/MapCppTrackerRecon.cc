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

#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"
#include <iostream>
#include <fstream>

bool MapCppTrackerRecon::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerRecon";

  //  JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  // Get the tracker modules.
  assert(_configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  MiceModule* _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  // Get desired reconstruction level.
  // assert(_configJSON.isMember("SciFiReconLevel"));
  // recon_level = _configJSON["SciFiReconLevel"].asInt();

  // Get minPE cut value.
  assert(_configJSON.isMember("SciFiNPECut"));
  minPE = _configJSON["SciFiNPECut"].asDouble();

  return true;
}

bool MapCppTrackerRecon::death() {
  return true;
}

std::string MapCppTrackerRecon::process(std::string document) {
  // Writes a line in the JSON document.
  Json::FastWriter writer;
  TrackerSpill spill;
  spill.events_in_spill.clear();
  // std::cout << "Events in Spill: " << spill.events_in_spill.size() << std::endl;
  /*event.scifidigits.clear();
  event.scificlusters.clear();
  event.scifispacepoints.clear();*/

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

  // === The Digits vectors is filled, either by running real data ==========
  // === digitization or by reading-in an already existing digits branch.====
  // Try to get dat_data...
  if ( root.isMember("daq_data") ) {
    Json::Value daq;
    daq = root.get("daq_data", 0);
    // if ( check_sanity_daq(daq) )
    RealDataDigitization(spill, daq);
    // ..or try to get digits...
  } else if ( root.isMember("digits") ) {
    Json::Value digits;
    digits = root.get("digits", 0);
    // if ( check_sanity_digits(digits) )
    fill_digits_vector(digits, spill);
    // ... or report error.
  } else {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  std::cout << "Events in Spill: " << spill.events_in_spill.size() << std::endl;
  // ================= Reconstruction =========================
  for ( int k = 0; k < spill.events_in_spill.size(); k++ ) {
    TrackerEvent event = spill.events_in_spill[k];
    // Build Clusters
    std::cout << event.scifidigits.size() << " ";
    if ( event.scifidigits.size() ) { // && recon_level >= 0 ) {
      cluster_recon(event);
    }
    // Build SpacePoints
    std::cout << event.scificlusters.size() << " ";
    if ( event.scificlusters.size() ) { // && recon_level >= 1) {
      spacepoint_recon(event);
    }
    std::cout << event.scifispacepoints.size() << " " << std::endl;
    save_to_json(event);
  }
  // ==========================================================
  return writer.write(root);
}
/*
bool MapCppTrackerRecon::check_sanity_digits(Json::Value digits) {
  // Check if JSON document is of the right type, else return error
  if (!digits.isArray()) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "Digits branch isn't an array";
    errors["bad_type"] = ss.str();
    root["errors"] = errors;
    return false;
  }
  return true;
}
*/
bool MapCppTrackerRecon::check_sanity_daq(Json::Value daq) {
  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("daq_data")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need an daq branch.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return false;
  }

  // Check sanity of the 'daq_data' branch
  if ( !root.isMember("daq_data") || root["daq_data"].isNull() ) {
    Json::Value errors;
    // std::cout << "bad DAQ_DATA, skipping event" << std::endl;
    std::stringstream ss;
    ss << _classname << " says:" << "Empty spill.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return false;
  }

  return true;
}

void MapCppTrackerRecon::fill_digits_vector(Json::Value digits_event, TrackerSpill &a_spill) {
  for ( int i = 0; i < digits_event.size(); i++ ) {
    TrackerEvent an_event;
    Json::Value digits;
    digits = digits_event[i];
    for ( int j = 0; j < digits.size(); j++ ) {
      Json::Value digit;
      digit = digits[j];
      int tracker, station, plane, channel, npe, time;
      tracker = digit["tracker"].asInt();
      station = digit["station"].asInt();
      plane   = digit["plane"].asInt();
      channel = digit["channel"].asInt();
      npe     = digit["npe"].asInt();
      time    = digit["time"].asInt();
      SciFiDigit* a_digit = new SciFiDigit(tracker, station, plane, channel, npe, time);
      an_event.scifidigits.push_back(a_digit);
    } // ends loop over digits in the event
    a_spill.events_in_spill.push_back(an_event);
  } // ends loop over events.
}

void MapCppTrackerRecon::cluster_recon(TrackerEvent &evt) {
  // Create seeds vector:
  // digits with npe > min/2.
  std::vector<SciFiDigit*>   seeds;
  for ( unsigned int dig = 0; dig < evt.scifidigits.size(); dig++ ) {
    if ( evt.scifidigits[dig]->get_npe() > minPE/2.0 )
      seeds.push_back(evt.scifidigits[dig]);
  }

  // Sort seeds so that we use higher npe first.

  // SciFiDigit* neigh;// = NULL;
  // SciFiDigit* seed;
  // double pe;

  for ( unsigned int i = 0; i < seeds.size(); i++ ) {
    if ( !seeds[i]->isUsed() ) {
      SciFiDigit* neigh = NULL;
      SciFiDigit* seed = seeds[i];

      int tracker = seed->get_tracker();
      int station = seed->get_station();
      int plane   = seed->get_plane();
      int fibre   = seed->get_channel();
      double pe   = seed->get_npe();
      // Look for a neighbour.
      for ( unsigned int j = i+1; j < seeds.size(); j++ ) {
        if ( seeds[j]->get_tracker() == tracker &&
             seeds[j]->get_station() == station &&
             seeds[j]->get_plane() == plane &&
             (seeds[j]->get_channel() - fibre) < 2 )
          neigh = seeds[j];
      }
      // If there is a neighbour, sum npe contribution.
      if ( neigh ) pe += neigh->get_npe();
      // Save cluster if it's above npe cut.
      if ( pe > minPE ) {
        SciFiCluster* clust = new SciFiCluster(seed);
        seed->setUsed();
        if ( neigh ) clust->addDigit(neigh);
        clust->construct(modules, seed);
        evt.scificlusters.push_back(clust);
      }
    }
  } // ends loop over seeds
}

void MapCppTrackerRecon::spacepoint_recon(TrackerEvent &evt) {
  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  double ClustException = _configJSON["SciFiClustExcept"].asDouble();

  // Get the number of clusters. If too large, abort reconstruction.
  int clusters_size = evt.scificlusters.size();
  if ( clusters_size > ClustException )
    return;

  int tracker, station, plane;
  // Store clusters in a vector.
  std::vector<SciFiCluster*> clusters[2][6][3];
  for ( int cl = 0; cl < clusters_size; cl++ ) {
    SciFiCluster* a_cluster = evt.scificlusters[cl];
    tracker = a_cluster->get_tracker();
    station = a_cluster->get_station();
    plane   = a_cluster->get_plane();
    clusters[tracker][station][plane].push_back(a_cluster);
  }

  // For each tracker,
  for ( int Tracker = 0; Tracker < 2; Tracker++ ) {
    // For each station,
    for ( int Station = 0; Station < 6; Station++ ) {
      // Make all possible combinations of doublet
      // clusters from each of the 3 views...
      // looping over all clusters in plane 0 (view v)
      int numb_clusters_in_v = clusters[Tracker][Station][0].size();
      int numb_clusters_in_w = clusters[Tracker][Station][1].size();
      int numb_clusters_in_u = clusters[Tracker][Station][2].size();
      for ( int cla = 0; cla < numb_clusters_in_v; cla++ ) {
        SciFiCluster* candidate_A = (clusters[Tracker][Station][0])[cla];

        // Looping over all clusters in plane 1 (view w)
        for ( int clb = 0; clb < numb_clusters_in_w; clb++ ) {
          SciFiCluster* candidate_B = (clusters[Tracker][Station][1])[clb];

          // Looping over all clusters in plane 2 (view u)
          for ( int clc = 0; clc < numb_clusters_in_u; clc++ ) {
            SciFiCluster* candidate_C = (clusters[Tracker][Station][2])[clc];

            if ( kuno_accepts(candidate_A, candidate_B, candidate_C) &&
                 clusters_are_not_used(candidate_A, candidate_B, candidate_C) ) {
              SciFiSpacePoint* triplet = new SciFiSpacePoint(candidate_A, candidate_B, candidate_C);
              evt.scifispacepoints.push_back(triplet);
              // triplet_counter += 1;
            }
          }  // ends plane 2
        }  // ends plane 1
      }  // ends plane 0
    }  // end loop over stations
  }  // end loop over trackers

  // Run over left-overs and make duplets without any selection criteria
  for ( int a_plane = 0; a_plane < 2; a_plane++ ) {
    for ( int another_plane = a_plane+1; another_plane < 3; another_plane++ ) {
      for ( int Tracker = 0; Tracker < 2; Tracker++ ) {  // for each tracker
        for ( int Station = 0; Station < 6; Station++ ) {  // for each station
          // Make all possible combinations of doublet clusters from views 0 & 1
          // looping over all clusters in view 0, then 1
          for ( unsigned int cla = 0;
                cla < clusters[Tracker][Station][a_plane].size(); cla++ ) {
          SciFiCluster* candidate_A =
                          (clusters[Tracker][Station][a_plane])[cla];

            // Looping over all clusters in view 1,2, then 2
            for ( unsigned int clb = 0;
                  clb < clusters[Tracker][Station][another_plane].size();
                  clb++ ) {
              SciFiCluster* candidate_B =
                           (clusters[Tracker][Station][another_plane])[clb];

              if ( clusters_are_not_used(candidate_A, candidate_B) &&
                   candidate_A->get_plane() != candidate_B->get_plane() ) {
                SciFiSpacePoint* duplet = new SciFiSpacePoint(candidate_A, candidate_B);
                evt.scifispacepoints.push_back(duplet);
              //  duplet_counter += 1;
              }
            }
          }
        }
      }
    }
  }
}

bool MapCppTrackerRecon::kuno_accepts(SciFiCluster* cluster1,
                                      SciFiCluster* cluster2,
                                      SciFiCluster* cluster3) {
  // The 3 clusters passed to the kuno_accepts function belong
  // to the same station, only the planes are different
  int tracker = cluster1->get_tracker();
  int station = cluster1->get_station();

  double uvwSum = cluster1->get_channel() +
                  cluster2->get_channel() +
                  cluster3->get_channel();

  if ((tracker == 0 && station == 5 && uvwSum < 322 && uvwSum > 318) ||
     (!(tracker == 0 && station == 5) && uvwSum < 321 && uvwSum > 316)) {
    return true;
  } else {
    return false;
  }
}

bool MapCppTrackerRecon::clusters_are_not_used(SciFiCluster* candidate_A,
                                               SciFiCluster* candidate_B) {
  if ( candidate_A->isUsed() || candidate_B->isUsed() ) {
    return false;
  } else {
    return true;
  }
}

bool MapCppTrackerRecon::clusters_are_not_used(SciFiCluster* candidate_A,
                                               SciFiCluster* candidate_B,
                                               SciFiCluster* candidate_C) {
  if ( candidate_A->isUsed() || candidate_B->isUsed() || candidate_C->isUsed() ) {
    return false;
  } else {
    return true;
  }
}


void MapCppTrackerRecon::save_to_json(TrackerEvent &evt) {
// Digits
  Json::Value digits;
  for ( unsigned int evt_i; evt_i < evt.scifidigits.size(); evt_i++ ) {
    Json::Value digits_in_event;
    digits_in_event["tracker"]= evt.scifidigits[evt_i]->get_tracker();
    digits_in_event["station"]= evt.scifidigits[evt_i]->get_station();
    digits_in_event["plane"]  = evt.scifidigits[evt_i]->get_plane();
    digits_in_event["channel"]= evt.scifidigits[evt_i]->get_channel();
    digits_in_event["npe"]    = evt.scifidigits[evt_i]->get_npe();
    digits_in_event["time"]   = evt.scifidigits[evt_i]->get_time();
    digits.append(digits_in_event);
  }
  root["digits"].append(digits);

  // Space Points
  Json::Value spacepoints;
  Json::Value channels;
  for ( unsigned int evt_i; evt_i < evt.scifispacepoints.size(); evt_i++ ) {
    Json::Value spacepoints_in_event;
    channels = Json::Value(Json::arrayValue);
    channels.clear();
    // spacepoints_in_event["spill"]  = event.scifispacepoints[evt_i]->get_spill();
    // spacepoints_in_event["event"]  = event.scifispacepoints[evt_i]->get_eventNo();
    spacepoints_in_event["tracker"]= evt.scifispacepoints[evt_i]->get_tracker();
    spacepoints_in_event["station"]= evt.scifispacepoints[evt_i]->get_station();
    spacepoints_in_event["npe"]    = evt.scifispacepoints[evt_i]->get_npe();
    spacepoints_in_event["time"]   = evt.scifispacepoints[evt_i]->get_time();
    spacepoints_in_event["type"]   = evt.scifispacepoints[evt_i]->get_type();
    Hep3Vector pos = evt.scifispacepoints[evt_i]->get_position();
    spacepoints_in_event["position"]["x"]   = pos.x();
    spacepoints_in_event["position"]["y"]   = pos.y();
    spacepoints_in_event["position"]["z"]   = pos.z();
    std::vector<SciFiCluster*> clusters_in_spacepoint = evt.scifispacepoints[evt_i]->get_channels();
    for ( int cl = 0; cl < clusters_in_spacepoint.size(); cl++ ) {
      Json::Value cluster;
      cluster["plane_number"] = clusters_in_spacepoint[cl]->get_plane();
      cluster["channel_number"] = clusters_in_spacepoint[cl]->get_channel();
      cluster["npe"] = clusters_in_spacepoint[cl]->get_npe();
      channels.append(cluster);
    }
    spacepoints_in_event["channels"] = channels;
    spacepoints.append(spacepoints_in_event);
  }
  root["spacepoints"].append(spacepoints);
}
