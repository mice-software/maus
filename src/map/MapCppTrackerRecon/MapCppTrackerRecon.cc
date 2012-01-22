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

  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only.
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

  // Check if the JSON document can be parsed, else return error.
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
    if ( check_sanity_daq(daq) )
      RealDataDigitization(spill, daq);
    // ..or try to get digits...
  } else if ( root.isMember("digits") ) {
    Json::Value digits;
    digits = root.get("digits", 0);
    if ( check_sanity_digits(digits) )
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

  // ================= Reconstruction =========================
  for ( int k = 0; k < spill.events_in_spill.size(); k++ ) {
    TrackerEvent event = spill.events_in_spill[k];
    // Build Clusters.
    if ( event.scifidigits.size() ) {
      cluster_recon(event);
    }
    // Build SpacePoints.
    if ( event.scificlusters.size() ) {
      spacepoint_recon(event);
    }
 //   print_event_info(event);

    // Fit straight line.
   // if ( event.scifispacepoints.size() ) {
  //    fit(event);
  //  }
    save_to_json(event);
  }
  // ==========================================================
  return writer.write(root);
}

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

bool MapCppTrackerRecon::check_sanity_daq(Json::Value daq) {
  // Check sanity of the 'daq_data' branch
  if ( !root.isMember("daq_data") || root["daq_data"].isNull() ) {
    Json::Value errors;
    //std::cout << "bad DAQ_DATA, skipping event" << std::endl;
    std::stringstream ss;
    ss << _classname << " says:" << "Empty spill.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return false;
  }
  std::ofstream file;
  file.open("errors.txt", std::ios::out | std::ios::app);

  if ( !daq.isMember("tracker1") ) {
    file << 0 << " " << 1 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << "\n";
    //std::cout << "Missing tracker1." << std::endl;
  }
  if ( !daq.isMember("tracker2") ) {
    file << 0 << " " << 0 << " " << 1 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << "\n";
    //std::cout << "Missing tracker2." << std::endl;
  }

  if ( daq["tracker1"].size() != daq["tracker2"].size() ) {
    file << 0 << " " << 0 << " " << 0 << " " << 1 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << "\n";
    //std::cout << "Different sizes for Tracker1 and Tracker2." << std::endl;
  }

  // A DATE feature... event counting starts at 1.
  if ( !daq["tracker1"][(Json::Value::ArrayIndex)0].isNull()) {
    file << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 1 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << "\n";
    //file << daq["tracker1"][0] << "\n";
    //std::cout << "First event for Tracker1 is not Null!" << std::endl;
  }
  if ( !daq["tracker2"][(Json::Value::ArrayIndex)0].isNull()) {
    file << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 1 << " " << 0 << " " << 0 << " " << 0 << "\n";
    //file << daq["tracker2"][0] << "\n";
    //std::cout << "First event for Tracker2 is not Null!" << std::endl;
  }

  file.close();
  return true;
}

void MapCppTrackerRecon::fill_digits_vector(Json::Value digits_event, TrackerSpill &a_spill) {
  for ( unsigned int i = 0; i < digits_event.size(); i++ ) {
    TrackerEvent an_event;
    Json::Value digits;
    digits = digits_event[i];
    for ( unsigned int j = 0; j < digits.size(); j++ ) {
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
  // Create and fill the seeds vector.
  std::vector<SciFiDigit*>   seeds;
  for ( unsigned int dig = 0; dig < evt.scifidigits.size(); dig++ ) {
    if ( evt.scifidigits[dig]->get_npe() > minPE/2.0 )
      seeds.push_back(evt.scifidigits[dig]);
  }
  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  double ClustException = _configJSON["SciFiClustExcept"].asDouble();
  // Get the number of clusters. If too large, abort reconstruction.
  int seeds_size = seeds.size();
  if ( seeds_size > ClustException ) {
    std::cout << "Massive event, won't bother to reconstruct." << std::endl;
    std::ofstream file;
    file.open("errors.txt", std::ios::out | std::ios::app);
    file << 1 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << "\n";
    file.close();

    return;
  }
  // Sort seeds so that we use higher npe first.
  // sort(seeds.begin(),seeds.end());

  // SciFiDigit* neigh;// = NULL;
  // SciFiDigit* seed;
  // double pe;

  for ( unsigned int i = 0; i < seeds_size; i++ ) {
    if ( !seeds[i]->is_used() ) {
      SciFiDigit* neigh = NULL;
      SciFiDigit* seed = seeds[i];

      int tracker = seed->get_tracker();
      int station = seed->get_station();
      int plane   = seed->get_plane();
      int fibre   = seed->get_channel();
      double pe   = seed->get_npe();
      // Look for a neighbour.
      for ( unsigned int j = i+1; j < seeds_size; j++ ) {
        if ( !seeds[j]->is_used() && seeds[j]->get_tracker() == tracker &&
             seeds[j]->get_station() == station && seeds[j]->get_plane()   == plane &&
             abs( seeds[j]->get_channel() - fibre ) < 2 ) {
          neigh = seeds[j];
         // std::cout << "Chans ns: " << seeds[j]->get_channel() << " " << fibre << ". Diff is " << seeds[j]->get_channel() - fibre << std::endl;
         // std::cout << "Found neigh " << j << " for digit " << i <<std::endl;
        }
      }
      // If there is a neighbour, sum npe contribution.
      if ( neigh ) {
        pe += neigh->get_npe();
      }
      // Save cluster if it's above npe cut.
      if ( pe > minPE ) {
        SciFiCluster* clust = new SciFiCluster(seed);
        if ( neigh ) {
          clust->add_digit(neigh);
        //  std::cout << "adding digit" << std::endl;
        }
        clust->construct(modules);
        evt.scificlusters.push_back(clust);
      }
    }
  } // ends loop over seeds
}

void MapCppTrackerRecon::spacepoint_recon(TrackerEvent &evt) {
  int tracker, station, plane;
  int clusters_size = evt.scificlusters.size();
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
      //std::cout << "Number of clusters: " << numb_clusters_in_v << " " << numb_clusters_in_w << " " << numb_clusters_in_u << std::endl;
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
              assert( !candidate_A->is_used() && !candidate_B->is_used() && !candidate_C->is_used());
              SciFiSpacePoint* triplet = new SciFiSpacePoint(candidate_A, candidate_B, candidate_C);
              evt.scifispacepoints.push_back(triplet);
              // std::cout << candidate_A->get_channel() << " " <<
              //              candidate_B->get_channel() << " " <<
              //              candidate_C->get_channel() << std::endl;
              assert(candidate_A->is_used() && candidate_B->is_used() && candidate_C->is_used());
              dump_info(candidate_A, candidate_B, candidate_C);
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
          for ( int cla = 0;
                cla < clusters[Tracker][Station][a_plane].size(); cla++ ) {
          SciFiCluster* candidate_A =
                          (clusters[Tracker][Station][a_plane])[cla];

            // Looping over all clusters in view 1,2, then 2
            for ( int clb = 0;
                  clb < clusters[Tracker][Station][another_plane].size();
                  clb++ ) {
              SciFiCluster* candidate_B =
                           (clusters[Tracker][Station][another_plane])[clb];

              if ( clusters_are_not_used(candidate_A, candidate_B) &&
                   candidate_A->get_plane() != candidate_B->get_plane() ) {
                assert(!candidate_A->is_used() && !candidate_B->is_used() );
                SciFiSpacePoint* duplet = new SciFiSpacePoint(candidate_A, candidate_B);
                evt.scifispacepoints.push_back(duplet);
                assert(candidate_A->is_used() && candidate_B->is_used());
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
  const int kuno_0_5   = 320;
  const int kuno_else  = 318;
  const int kuno_toler = 2;
  // The 3 clusters passed to the kuno_accepts function belong
  // to the same station, only the planes are different
  int tracker = cluster1->get_tracker();
  int station = cluster1->get_station();

  double uvwSum = cluster1->get_channel() +
                  cluster2->get_channel() +
                  cluster3->get_channel();

  if ( (tracker == 0 && station == 5 && (uvwSum < (kuno_0_5+kuno_toler))
                                     && (uvwSum > (kuno_0_5-kuno_toler))) ||
     (!(tracker == 0 && station == 5)&& (uvwSum < (kuno_else+kuno_toler))
                                     && (uvwSum > (kuno_else-kuno_toler))) ) {
    return true;
  } else {
    return false;
  }
}

bool MapCppTrackerRecon::clusters_are_not_used(SciFiCluster* candidate_A,
                                               SciFiCluster* candidate_B) {
  if ( candidate_A->is_used() || candidate_B->is_used() ) {
    return false;
  } else {
    return true;
  }
}

bool MapCppTrackerRecon::clusters_are_not_used(SciFiCluster* candidate_A,
                                               SciFiCluster* candidate_B,
                                               SciFiCluster* candidate_C) {
  if ( candidate_A->is_used() || candidate_B->is_used() || candidate_C->is_used() ) {
    return false;
  } else {
    return true;
  }
}


void MapCppTrackerRecon::save_to_json(TrackerEvent &evt) {
  Json::Value digits;
  for ( unsigned int evt_i=0; evt_i < evt.scifidigits.size(); evt_i++ ) {
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

  Json::Value tracker0;
  Json::Value tracker1;
  Json::Value channels;
  for ( unsigned int evt_i=0; evt_i < evt.scifispacepoints.size(); evt_i++ ) {
    Json::Value spacepoints_in_event;
    channels = Json::Value(Json::arrayValue);
    channels.clear();
    int tracker = evt.scifispacepoints[evt_i]->get_tracker();
    spacepoints_in_event["tracker"]= tracker;
    spacepoints_in_event["station"]= evt.scifispacepoints[evt_i]->get_station();
    spacepoints_in_event["npe"]    = evt.scifispacepoints[evt_i]->get_npe();
    spacepoints_in_event["time"]   = evt.scifispacepoints[evt_i]->get_time();
    spacepoints_in_event["type"]   = evt.scifispacepoints[evt_i]->get_type();
    Hep3Vector pos = evt.scifispacepoints[evt_i]->get_position();
    spacepoints_in_event["position"]["x"]   = pos.x();
    spacepoints_in_event["position"]["y"]   = pos.y();
    spacepoints_in_event["position"]["z"]   = pos.z();
    std::vector<SciFiCluster*> clusters_in_spacepoint = evt.scifispacepoints[evt_i]->get_channels();
    for ( unsigned int cl = 0; cl < clusters_in_spacepoint.size(); cl++ ) {
      Json::Value cluster;
      cluster["plane_number"] = clusters_in_spacepoint[cl]->get_plane();
      cluster["channel_number"] = clusters_in_spacepoint[cl]->get_channel();
      cluster["npe"] = clusters_in_spacepoint[cl]->get_npe();
      channels.append(cluster);
    }
    spacepoints_in_event["channels"] = channels;
    if (tracker == 0 ) {
      tracker0.append(spacepoints_in_event);
    }
    if ( tracker == 1 ) {
      tracker1.append(spacepoints_in_event);
    }
  }
  root["space_points"]["tracker1"].append(tracker0);
  root["space_points"]["tracker2"].append(tracker1);
}

void MapCppTrackerRecon::fit(TrackerEvent &evt) {
  bool station_hit[2][6] = { {false, false, false, false, false, false},
                             {false, false, false, false, false, false}};

  std::ofstream file;
  file.open("triplets_ana.txt", std::ios::out | std::ios::app);

  std::vector<SciFiSpacePoint*> event[2][6];
  for ( unsigned int sp_i=0; sp_i < evt.scifispacepoints.size(); sp_i++ ) {
    int tracker = evt.scifispacepoints[sp_i]->get_tracker();
    int station = evt.scifispacepoints[sp_i]->get_station();
    // int evt.scifispacepoints[evt_i]->get_npe();
    // evt.scifispacepoints[evt_i]->get_time();
    std::string type = evt.scifispacepoints[sp_i]->get_type();
    event[tracker][station].push_back(evt.scifispacepoints[sp_i]);

    if (type == "triplet" ) {
      file << tracker << " " << station << " " << 3 << "\n";
    } else {
      file << tracker << " " << station << " " << 2 << "\n";
    }

    station_hit[tracker][station]=true;
  }
  file.close();

  std::ofstream file1;
  file1.open("efficiency_ana.txt", std::ios::out | std::ios::app);
  for ( int tr = 0; tr < 2; tr++ ) {
    int hit_counter = 0;
    if ( station_hit[tr][1] && station_hit[tr][5] ) {
      for ( int i = 2; i < 5; i++ ) {
        if ( station_hit[i] )
          hit_counter+=1;
      }
      file1 << tr << " " << hit_counter << "\n";
    }
  }
  file1.close();

}



 
void MapCppTrackerRecon::print_event_info(TrackerEvent &event) {
  std::cout << event.scifidigits.size() << " "
            << event.scificlusters.size() << " "
            << event.scifispacepoints.size() << " " << std::endl;
}

void MapCppTrackerRecon::dump_info(SciFiCluster* candidate_A, SciFiCluster* candidate_B,
                                   SciFiCluster* candidate_C) {
  std::ofstream file;
  file.open("map_help.txt", std::ios::out | std::ios::app);
  file << candidate_A->get_tracker() << " " << candidate_A->get_station() << " " <<
  candidate_A->get_plane() << " " << candidate_A->get_channel() << "\n";
  file << candidate_B->get_tracker() << " " << candidate_B->get_station() << " " <<
  candidate_B->get_plane() << " " << candidate_B->get_channel() << "\n";
  file << candidate_C->get_tracker() << " " << candidate_C->get_station() << " " <<
  candidate_C->get_plane() << " " << candidate_C->get_channel() << "\n";
  file.close();
}
