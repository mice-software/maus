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
  _of1 << "spill\ttracker\tstation\tx\ty\tz\tt\n";

  _of2.open("virtualhit_data.dat");
  _of2 << "spill\ttrack_ID\tPDG_ID\tdet_ID\tB_z\tx\ty\tz\tt\tpx\tpy\tpz\n";

  _of3.open("track_data.dat");
  _of3 << "spill\tn_points\ttracker\tx0\ty0\tcirc_x0\tcirc_y0\trad\tpsi0\tdsdz\n";

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
    // ================= Normal Reconstruction =========================
    if ( spill.GetReconEvents() ) {
      for ( unsigned int i = 0; i < spill.GetReconEvents()->size(); i++ ) {
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

        if ( event->helicalprtracks().size() ) {
          for ( unsigned int j = 0; j < event->helicalprtracks().size(); ++j ) {
            SciFiHelicalPRTrack trk = event->helicalprtracks()[j];
            _of3 << spill.GetSpillNumber() << "\t" << trk.get_num_points() << "\t";
            _of3 << trk.get_tracker() << "\t" << trk.get_x0() << "\t" << trk.get_y0() << "\t";
            _of3 << trk.get_circle_x0() << "\t" << trk.get_circle_y0() << "\t";
            _of3 << trk.get_R() << "\t" << trk.get_psi0() << "\t" << trk.get_dsdz() << "\n";
            for ( unsigned int k = 0; k < trk.get_spacepoints().size(); ++k ) {
              SciFiSpacePoint sp = trk.get_spacepoints()[k];
              ThreeVector pos = sp.get_position();
              _of1 << spill.GetSpillNumber() << "\t" << sp.get_tracker() << "\t";
              _of1 << sp.get_station() << "\t";
              _of1 << pos.x() << "\t" << pos.y() << "\t" << pos.z() << "\t";
              _of1 << sp.get_time() << "\n";
            }
          }
        }

        print_event_info(*event);
      }
    } else {
      std::cout << "No recon events found\n";
    }
    save_to_json(spill);

    // ================= VirtualHit Reconstruction =========================
    if ( spill.GetMCEvents() ) {
      for ( unsigned int k = 0; k < spill.GetMCEvents()->size(); k++ ) {
        VirtualHitArray* hits = spill.GetMCEvents()->at(k)->GetVirtualHits();
        for ( unsigned int i = 0; i < hits->size(); i++ ) {
          VirtualHit hit = hits->at(i);
          ThreeVector pos = hit.GetPosition();
          ThreeVector mom = hit.GetMomentum();
          _of2 << spill.GetSpillNumber() << "\t" << hit.GetTrackId() << "\t";
          _of2 << hit.GetParticleId() << "\t" << hit.GetStationId() << "\t";
          _of2 << hit.GetBField().z() << "\t";
          _of2 << pos.x() << "\t" << pos.y() << "\t" << pos.z() << "\t" << hit.GetTime() << "\t";
          _of2 << mom.x() << "\t" << mom.y() << "\t" << mom.z() << "\n";
        } // ends loop over vhits
      }
    }
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
  SpillProcessor spill_proc;
  root = *spill_proc.CppToJson(spill);
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
  KalmanTrackFit fit;
  if ( evt.helicalprtracks().size() )
    fit.process(evt.helicalprtracks());
  if ( evt.straightprtracks().size() )
    fit.process(evt.straightprtracks());
}

void MapCppTrackerReconTest::print_event_info(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << "; "
            << event.straightprtracks().size() << " "
            << event.helicalprtracks().size() << " " << std::endl;
}

void MapCppTrackerReconTest::vhits_to_ascii(VirtualHitArray * hits) {
  ofstream of2("virtualhit_data.dat");
  for ( unsigned int i = 0; i < hits->size(); i++ ) {
    VirtualHit hit = hits->at(i);
    double x = hit.GetPosition().x();
    double y = hit.GetPosition().x();
    double z = hit.GetPosition().z();
    of2 << x << "\t" << y << "\t" << z << std::endl;
  } // ends loop over vhits
  of2.close();
}


} // ~namespace MAUS

