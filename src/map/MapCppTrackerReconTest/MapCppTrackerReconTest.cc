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

#include <algorithm>
#include <map>
#include <cmath>

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/map/MapCppTrackerReconTest/MapCppTrackerReconTest.hh"

#include "Riostream.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TGraphErrors.h"
#include "TDecompChol.h"
#include "TDecompSVD.h"
#include "TF1.h"

namespace MAUS {

bool MapCppTrackerReconTest::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerReconTest";

  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  // Initialise output file streams
  _of1.open("seed_data.dat");
  _of1 << "spill\tevent\ttrack\ttracker\tstation\tx\ty\tz\tt\n";

  _of2.open("scifihit_data.dat");
  _of2 << "spill\tmc_event\ttrack_ID\tPDG_ID\ttracker\tstation";
  _of2 << "\tplane\tfiber\tx\ty\tz\tt\tpx\tpy\tpz\n";

  _of3.open("track_data.dat");
  _of3 << "spill\tevent\ttrack\tn_points\ttracker\tx0\ty0\t";
  _of3 << "circ_x0\tcirc_y0\trad\tpsi0\tdsdz\tn_matched_t1\tn_matched_t2\n";

  _of4.open("event_data.dat");
  _of4 << "spill\tevent\tpt_mc_t1\tpz_mc_t1\tpt_mc_t2\tpz_mc_t2\t";
  _of4 << "pt_rec_t1\tpz_rec_t1\tpt_rec_t2\tpz_rec_t2\tn_matched_t1\tn_avail_t1\t";
  _of4 << "n_matched_t2\tn_avail_t2\tn_hits_t1\tn_hits_t2\n";

  _of5.open("MCSpace_data.dat");
  _of5 << "spill\tmc_event\ttrack_ID\tPDG_ID\ttracker\tstation";
  _of5 << "\tplane\tfiber\tx\ty\tz\tt\tpx\tpy\tpz\n";

  _of6.open("DataGrid.dat");

  // Get the tracker modules.
  assert(_configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  std::cerr << "Using geomtry: " << filename << std::endl;
  MiceModule* _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  // Get minPE cut value.
  assert(_configJSON.isMember("SciFiNPECut"));
  minPE = _configJSON["SciFiNPECut"].asDouble();

  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  ClustException = _configJSON["SciFiClustExcept"].asInt();

  // Get the flags for turning straight and helical pr on or off
  assert(_configJSON.isMember("SciFiPRHelicalOn"));
  _helical_pr_on = _configJSON["SciFiPRHelicalOn"].asBool();
  assert(_configJSON.isMember("SciFiPRStraightOn"));
  _straight_pr_on = _configJSON["SciFiPRStraightOn"].asBool();

  return true;
}

bool MapCppTrackerReconTest::death() {
  _of1.close();
  _of2.close();
  _of3.close();
  _of4.close();
  _of5.close();
  _of6.close();
  return true;
}

std::string MapCppTrackerReconTest::process(std::string document) {
  std::cout << "Reconstructing tracker data\n";
  Json::FastWriter writer;

  // Read in json data
  Spill spill;
  bool success = read_in_json(document, spill);
  if (!success)
    return writer.write(root);

  try {
    // If we have both Recon and MC events (or at least the array pointers are initialised)
    if ( spill.GetReconEvents() && spill.GetMCEvents() ) {
      // Loop over MC events
      for ( unsigned int i = 0; i < spill.GetMCEvents()->size(); i++ ) {
        // ================= Compute SciFiHit Data=========================
        double pt_mc_t1 = 0.0;
        double pz_mc_t1 = 0.0;
        double pt_mc_t2 = 0.0;
        double pz_mc_t2 = 0.0;
        int num_hits_t1 = 0;
        int num_hits_t2 = 0;

        SciFiHitArray* hits = spill.GetMCEvents()->at(i)->GetSciFiHits();
        std::vector<SciFiHit> virt_scifi_hit;
        std::vector<double> num_virt;

        // Loop over scifi hits
        for ( unsigned int j = 0; j < hits->size(); j++ ) {
          SciFiHit hit = hits->at(j);
          ThreeVector pos = hit.GetPosition();
          ThreeVector mom = hit.GetMomentum();
          if ( mom.z() > 50. ) {
            _of2 << spill.GetSpillNumber() << "\t" << i << "\t" << hit.GetTrackId() << "\t";
            _of2 << hit.GetParticleId() << "\t" << hit.GetChannelId()->GetTrackerNumber() << "\t";
            _of2 << hit.GetChannelId()->GetStationNumber() << "\t";
            _of2 << hit.GetChannelId()->GetPlaneNumber() << "\t";
            _of2 << hit.GetChannelId()->GetFibreNumber() << "\t";
            _of2 << pos.x() << "\t" << pos.y() << "\t" << pos.z() << "\t" << hit.GetTime() << "\t";
            _of2 << mom.x() << "\t" << mom.y() << "\t" << mom.z() << "\n";
            int g = 0;
            if ( virt_scifi_hit.size() == 0 ) {
            virt_scifi_hit.push_back(hit);
            num_virt.push_back(1.);
            } else {
              for ( unsigned int h = 0; h < virt_scifi_hit.size(); h++ ) {
                if ( virt_scifi_hit[h].GetChannelId()->GetTrackerNumber()   ==
                     hit.GetChannelId()->GetTrackerNumber()    &&
                     virt_scifi_hit[h].GetChannelId()->GetStationNumber()   ==
                     hit.GetChannelId()->GetStationNumber()    &&
                     virt_scifi_hit[h].GetParticleId() == hit.GetParticleId() &&
                     virt_scifi_hit[h].GetTrackId() == hit.GetTrackId() &&
                     fabs(virt_scifi_hit[h].GetTime() - hit.GetTime()) < 2. &&
                     fabs(virt_scifi_hit[h].GetPosition().x() - hit.GetPosition().x()) < 0.5 &&
                     fabs(virt_scifi_hit[h].GetPosition().y() - hit.GetPosition().y()) < 0.5 &&
                     fabs(virt_scifi_hit[h].GetPosition().z() - hit.GetPosition().z()) < 10. ) {
                  virt_scifi_hit[h].SetTime((virt_scifi_hit[h].GetTime() +
                                            (hit.GetTime()/num_virt[h])) /
                                            ((num_virt[h]+1.)/num_virt[h]));
                  virt_scifi_hit[h].SetPosition((virt_scifi_hit[h].GetPosition() +
                                                (hit.GetPosition()/num_virt[h])) /
                                                ((num_virt[h]+1.)/num_virt[h]));
                  virt_scifi_hit[h].SetMomentum((virt_scifi_hit[h].GetMomentum() +
                                                (hit.GetMomentum()/num_virt[h])) /
                                                ((num_virt[h]+1.)/num_virt[h]));
                  num_virt[h]++;
                  g = 1;
                  continue;
                }
              }
              if ( g == 0 ) {
                virt_scifi_hit.push_back(hit);
                num_virt.push_back(1.);
              }
            }
            // Write MC momentum data in tracker 1
            if ( hit.GetChannelId()->GetTrackerNumber() == 0 ) {
              pt_mc_t1 += sqrt(mom.x()*mom.x() + mom.y()*mom.y());
              pz_mc_t1 += mom.z();
              ++num_hits_t1;
            } else { // Write MC momentum data in tracker 2
              pt_mc_t2 += sqrt(mom.x()*mom.x() + mom.y()*mom.y());
              pz_mc_t2 += mom.z();
              ++num_hits_t2;
            }
          }
        } // Ends loop over scifihits
        // Calculate the average MC truth momentum per tracker
        pt_mc_t1 /= num_hits_t1;
        pz_mc_t1 /= num_hits_t1;
        pt_mc_t2 /= num_hits_t2;
        pz_mc_t2 /= num_hits_t2;

        // Write the MC truth momentum to the current event file
        _of4 << spill.GetSpillNumber() << "\t" << i << "\t";
        _of4 << pt_mc_t1 << "\t" << pz_mc_t1 << "\t" << pt_mc_t2 << "\t" << pz_mc_t2 << "\t";

        for ( unsigned int l = 0; l < virt_scifi_hit.size(); l++ ) {
            ThreeVector v_pos = virt_scifi_hit[l].GetPosition();
            ThreeVector v_mom = virt_scifi_hit[l].GetMomentum();
            _of5 << spill.GetSpillNumber() << "\t" << l << "\t";
            _of5 << virt_scifi_hit[l].GetTrackId() << "\t";
            _of5 << virt_scifi_hit[l].GetParticleId() << "\t";
            _of5 << virt_scifi_hit[l].GetChannelId()->GetTrackerNumber() << "\t";
            _of5 << virt_scifi_hit[l].GetChannelId()->GetStationNumber() << "\t";
            _of5 << virt_scifi_hit[l].GetChannelId()->GetPlaneNumber() << "\t";
            _of5 << virt_scifi_hit[l].GetChannelId()->GetFibreNumber() << "\t";
            _of5 << v_pos.x() << "\t" << v_pos.y() << "\t" << v_pos.z() << "\t";
            _of5 << virt_scifi_hit[l].GetTime() << "\t";
            _of5 << v_mom.x() << "\t" << v_mom.y() << "\t" << v_mom.z() << "\n";
        }

        // ================= Normal Tracker Reconstruction =========================
        double pt_rec_t1 = 0.0;
        double pz_rec_t1 = 0.0;
        double pt_rec_t2 = 0.0;
        double pz_rec_t2 = 0.0;

        int n_matched_sp_evt_t1 = 0;
        int n_matched_sp_evt_t2 = 0;
        int n_sp_avail_t1 = 0;
        int n_sp_avail_t2 = 0;

        // If a recon event exists for this MC event
        if ( spill.GetReconEvents()->at(i) ) {
          SciFiEvent *event = spill.GetReconEvents()->at(i)->GetSciFiEvent();
          // Build Clusters.
          if ( event->digits().size() ) {
            std::cout << "Building clusters\n";
            cluster_recon(*event);
          }
          // Build SpacePoints.
          if ( event->clusters().size() ) {
            std::cout << "Building spacepoints\n";
            spacepoint_recon(*event);
          }
          // Pattern Recognition.
          if ( event->spacepoints().size() ) {
            std::cout << "Calling Pattern Recognition..." << std::endl;
            pattern_recognition(_helical_pr_on, _straight_pr_on, *event);
            std::cout << "Pattern Recognition complete." << std::endl;
          }

          // ================= Compute Reconstruction Momentum Data =========================
          n_spoints(event->spacepoints(), n_sp_avail_t1, n_sp_avail_t2);

          bool t1_set = false;
          bool t2_set = false;

          // If we have helical tracks
          if ( event->helicalprtracks().size() ) {
            // Loop over the helical tracks in the recon event
            for ( unsigned int j = 0; j < event->helicalprtracks().size(); ++j ) {
              SciFiHelicalPRTrack * trk = event->helicalprtracks()[j];
              _of3 << spill.GetSpillNumber() << "\t" << i << "\t" << j << "\t";
              _of3 << trk->get_num_points() << "\t" << trk->get_tracker() << "\t";
              _of3 << trk->get_x0() << "\t" << trk->get_y0() << "\t";
              _of3 << trk->get_circle_x0() << "\t" << trk->get_circle_y0() << "\t";
              _of3 << trk->get_R() << "\t" << trk->get_psi0() << "\t" << trk->get_dsdz() << "\t";
              // Write momentum data for the first track in the event only
              if ( !t1_set && trk->get_tracker() == 0 ) {
                pt_rec_t1 = 1.2 * trk->get_R();
                pz_rec_t1 = pt_rec_t1 / trk->get_dsdz();
                t1_set = true;
              } else if ( !t2_set && trk->get_tracker() == 1 ) {
                pt_rec_t2 = 1.2 * trk->get_R();
                pz_rec_t2 = pt_rec_t2 / trk->get_dsdz();
                t2_set = true;
              }

              // ================= Match Seed Spacepoints with SciFi Hits =====================
              int n_matched_sp_t1 = 0;
              int n_matched_sp_t2 = 0;
              // Loop over spacepoints in the track
              for ( unsigned int k = 0; k < trk->get_spacepoints().size(); ++k ) {
                SciFiSpacePoint *sp = trk->get_spacepoints()[k];
                ThreeVector pos = sp->get_position();
                _of1 << spill.GetSpillNumber() << "\t" << i << "\t" << j << "\t";
                _of1 << sp->get_tracker() << "\t" << sp->get_station() << "\t";
                _of1 << pos.x() << "\t" << pos.y() << "\t" << pos.z() << "\t";
                _of1 << sp->get_time() << "\n";
                for ( unsigned int j = 0; j < virt_scifi_hit.size(); j++ ) {
                  SciFiHit vhit = virt_scifi_hit[j];
                  // Is the vhit in the same tracker and station as the spacepoint
                  if ( sp->get_tracker() == 0 &&
                       vhit.GetChannelId()->GetTrackerNumber() == 0 &&
                       sp->get_station() == vhit.GetChannelId()->GetStationNumber() ) {
                    if ( ( fabs(- pos.x() - vhit.GetPosition().x()) < _cut1 ) &&
                         ( fabs(pos.y() - vhit.GetPosition().y()) < _cut1) ) {
                      ++n_matched_sp_t1;
                    }
                  } else if ( sp->get_tracker() == 1 &&
                              vhit.GetChannelId()->GetTrackerNumber() == 1 &&
                              sp->get_station() == vhit.GetChannelId()->GetStationNumber() ) {
                    if ( ( fabs(pos.x() - vhit.GetPosition().x()) < _cut1 ) &&
                         ( fabs(pos.y() - vhit.GetPosition().y()) < _cut1 ) ) {
                      ++n_matched_sp_t2;
                    }
                  }
                }
              }  // ~Loop over spacepoints in the track
              _of3 << n_matched_sp_t1 << "\t" << n_matched_sp_t2 << "\n";
              n_matched_sp_evt_t1 += n_matched_sp_t1;
              n_matched_sp_evt_t2 += n_matched_sp_t2;
            } // ~Loop over the helical tracks in the recon event
          } // ~if ( event->helicalprtracks().size() )
          print_event_info(*event);
        } // ~if ( spill.GetReconEvents()->at(i) )

        int t1_hits = 0;
        int t2_hits = 0;
        vhits_per_tracker(virt_scifi_hit, t1_hits, t2_hits);

        // ================= Write Data For The Current Event =========================
        _of4 << pt_rec_t1 << "\t" << pz_rec_t1 << "\t" << pt_rec_t2 << "\t" << pz_rec_t2 << "\t";
        _of4 << n_matched_sp_evt_t1 << "\t" << n_sp_avail_t1 << "\t";
        _of4 << n_matched_sp_evt_t2 << "\t" << n_sp_avail_t2 << "\t";
        _of4 << t1_hits << "\t" << t2_hits << "\n";

        if ( spill.GetReconEvents()->at(i) ) {
          SciFiEvent *recon = spill.GetReconEvents()->at(i)->GetSciFiEvent();
          mc_v_recon(*recon, *hits);
        }
      }  // ~Loop over MC events
    } else {
      std::cout << "No mc or recon events found\n";
    }
    // save_to_json(spill);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["recon_failed"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  return writer.write(root);
}

void MapCppTrackerReconTest::vhits_per_tracker(std::vector<SciFiHit> &virt_scifi_hit,
                                               int &t1,
                                               int &t2) {
  t1 = 0;
  t2 = 0;

  if ( virt_scifi_hit.size() ) {
    for ( unsigned int i = 0; i < virt_scifi_hit.size(); ++i ) {
      SciFiHit vhit = virt_scifi_hit[i];
      if ( vhit.GetChannelId()->GetTrackerNumber() == 0 ) {
        ++t1;
      } else if ( vhit.GetChannelId()->GetTrackerNumber() == 1 ) {
        ++t2;
      }
    }
  } else {
    std::cerr << "Invalid pointer supplied\n";
  }
}

int MapCppTrackerReconTest::stat_id_to_stat_num(const int vhit_stat_id) {
  int stat_num = 0;

  // Tracker 1
  if ( vhit_stat_id < 6 ) {
    stat_num = 6 - vhit_stat_id;
  } else if ( vhit_stat_id > 5 ) {
    stat_num = vhit_stat_id - 5;
  }
  return stat_num;
}

void MapCppTrackerReconTest::n_spoints(std::vector<SciFiSpacePoint*> spoints,
                                       int &n_sp_t1, int &n_sp_t2) {

  n_sp_t1 = 0;
  n_sp_t2 = 0;

  for ( unsigned int i = 0; i < spoints.size(); ++i ) {
    if ( spoints[i]->get_tracker() == 0 ) ++n_sp_t1;
    if ( spoints[i]->get_tracker() == 1 ) ++n_sp_t2;
  }
}

bool MapCppTrackerReconTest::read_in_json(std::string json_data, Spill &spill) {

  Json::Reader reader;
  Json::FastWriter writer;

  try {
    root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    spill = *spill_proc.JsonToCpp(root);
    return true;
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    writer.write(root);
    return false;
  }
}

void MapCppTrackerReconTest::save_to_json(Spill &spill) {
  // SpillProcessor spill_proc;
  // root = *spill_proc.CppToJson(spill);
}

void MapCppTrackerReconTest::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering(ClustException, minPE, modules);
  clustering.process(evt);
}

void MapCppTrackerReconTest::spacepoint_recon(SciFiEvent &evt) {
  SciFiSpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppTrackerReconTest::pattern_recognition(const bool helical_pr_on,
                                                 const bool straight_pr_on,
                                                 SciFiEvent &evt) {
  PatternRecognition pr1;
  pr1.process(helical_pr_on, straight_pr_on, evt);
}

void MapCppTrackerReconTest::track_fit(SciFiEvent &evt) {
  // KalmanTrackFit fit;
  // if ( evt.helicalprtracks().size() )
  //  fit.process(evt.helicalprtracks());
  // if ( evt.straightprtracks().size() )
  //  fit.process(evt.straightprtracks());
}

void MapCppTrackerReconTest::print_event_info(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << "; "
            << event.straightprtracks().size() << " "
            << event.helicalprtracks().size() << " " << std::endl;
}

void MapCppTrackerReconTest::mc_v_recon(SciFiEvent &event, SciFiHitArray &hits) {
  for ( unsigned int a = 0; a < event.spacepoints().size(); a++ ) {
    SciFiSpacePoint *sp1 = event.spacepoints().at(a);
    SciFiSpacePoint *sp2;
    _of6 << "\n" << sp1->get_position() << "\t";
    bool match = false;
    for ( unsigned int b = 0; b < event.helicalprtracks().size(); b++ ) {
      SciFiHelicalPRTrack *trk = event.helicalprtracks().at(b);
      if ( match == true ) {
        continue;
      }
      for ( unsigned int c = 0; c < trk->get_spacepoints().size(); c++ ) {
        sp2 = trk->get_spacepoints().at(c);
        if ( sp1->get_position().x() == sp2->get_position().x() &&
             sp1->get_position().y() == sp2->get_position().y() &&
             sp1->get_position().z() == sp2->get_position().z() ) {
          match = true;
          continue;
        }
      }
    }
    _of6 << match << "\t";

    SciFiClusterPArray clusters = sp1->get_channels();
    for ( unsigned int d = 0; d < clusters.size(); d++ ) {
      double channel1 = clusters[d]->get_channel();
      int plane1   = clusters[d]->get_plane();
      int station1 = clusters[d]->get_station();
      int tracker1 = clusters[d]->get_tracker();
      for ( unsigned int e = 0; e < hits.size(); e++ ) {
        SciFiHit hit = hits[e];
        double channel2 = compute_chan_no(&hit);
        int plane2   = hit.GetChannelId()->GetPlaneNumber();
        int station2 = hit.GetChannelId()->GetStationNumber();
        int tracker2 = hit.GetChannelId()->GetTrackerNumber();
        if ( fabs(channel1 - channel2) < 1 &&
             plane1   == plane2   &&
             station1 == station2 &&
             tracker1 == tracker2 ) {
          _of6 << hit.GetPosition() << "\t\n\t\t";
        }
      }
    }
  }
}

double MapCppTrackerReconTest::compute_chan_no(MAUS::SciFiHit *ahit) {
  int tracker = ahit->GetChannelId()->GetTrackerNumber();
  int station = ahit->GetChannelId()->GetStationNumber();
  int plane   = ahit->GetChannelId()->GetPlaneNumber();

  // std::cout << "Time: " << ahit->GetTime() << std::endl;
  // std::cout << tracker << " " << station << " " << plane << std::endl;
  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); j++ ) {
    if ( modules[j]->propertyExists("Tracker", "int") &&
         modules[j]->propertyInt("Tracker") == tracker &&
         modules[j]->propertyExists("Station", "int") &&
         modules[j]->propertyInt("Station") == station &&
         modules[j]->propertyExists("Plane", "int") &&
         modules[j]->propertyInt("Plane") == plane )
      // save the module corresponding to this plane
      this_plane = modules[j];
  }
  // std:: << tracker << " " << station << " " << plane << std::endl;
  assert(this_plane != NULL);

  int numberFibres = static_cast<int> (7*2*(this_plane->propertyDouble("CentralFibre")+0.5));
  int fiberNumber = ahit->GetChannelId()->GetFibreNumber();
  double chanNo;

  if ( tracker == 0 ) {
    // start counting from the other end
    chanNo = static_cast<int> (floor((numberFibres-fiberNumber)/7.0));
  } else {
    chanNo = static_cast<int> (floor(fiberNumber/7.0));
  }

  return chanNo;
}

} // ~namespace MAUS
