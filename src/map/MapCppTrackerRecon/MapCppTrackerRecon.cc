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
#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"

#include "Riostream.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TGraphErrors.h"
#include "TDecompChol.h"
#include "TDecompSVD.h"
#include "TF1.h"

namespace MAUS {

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

  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  ClustException = _configJSON["SciFiClustExcept"].asInt();

  return true;
}

bool MapCppTrackerRecon::death() {
  return true;
}

bool sort_by_station(SciFiSpacePoint *a, SciFiSpacePoint *b ) {
  //  Ascending station number.
  return ( a->get_station() < b->get_station() );
  // }
}

std::string MapCppTrackerRecon::process(std::string document) {
  std::cout << "Reconstructing tracker data\n";
  Json::FastWriter writer;

  // Read in json data
  Spill spill = read_in_json(document);


  try { // ================= Reconstruction =========================
    if ( spill.GetReconEvents() ) {
      for ( unsigned int k = 0; k < spill.GetReconEvents()->size(); k++ ) {
        SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();
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
          pattern_recognition(*event);
          std::cout << "Pattern Recognition complete." << std::endl;
        }
        // Kalman Track Fit.
         if ( event->straightprtracks().size() || event->helicalprtracks().size() ) {
        //  track_fit(*event);
         }
        // Perform alignment study.
        // if ( event->spacepoints().size() == 5 ) {
        //  perform_alignment_study(*event);
        // }

        print_event_info(*event);
      }
    } else {
      std::cout << "No recon events found\n";
    }
    save_to_json(spill);
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

Spill MapCppTrackerRecon::read_in_json(std::string json_data) {

  Json::FastWriter writer;
  Spill spill;

  try {
    root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    spill = *spill_proc.JsonToCpp(root);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    writer.write(root);
  }
  return spill;
}

void MapCppTrackerRecon::save_to_json(Spill &spill) {
  SpillProcessor spill_proc;
  root = *spill_proc.CppToJson(spill);
}

void MapCppTrackerRecon::perform_alignment_study(SciFiEvent &evt) {
  std::vector<SciFiSpacePoint*> spacepoints = evt.spacepoints();
  std::sort(spacepoints.begin(), spacepoints.end(), sort_by_station);
  int tracker = spacepoints[0]->get_tracker();
  for ( int i = 0; i < 5; ++i ) {
    if ( spacepoints[i]->get_tracker() != tracker ) return;
    if ( spacepoints[i]->get_station() != i+1 ) return;
  }

  double x_const, x_slope, y_const, y_slope;
  fit(spacepoints, x_const, x_slope, y_const, y_slope);

  // Values of full fit.
  double full_x_st2 = x_const + x_slope*spacepoints[1]->get_position().z();
  double full_x_st3 = x_const + x_slope*spacepoints[2]->get_position().z();
  double full_x_st4 = x_const + x_slope*spacepoints[3]->get_position().z();
  double full_y_st2 = y_const + y_slope*spacepoints[1]->get_position().z();
  double full_y_st3 = y_const + y_slope*spacepoints[2]->get_position().z();
  double full_y_st4 = y_const + y_slope*spacepoints[3]->get_position().z();

  double excluded_x_st2 = spacepoints[1]->get_position().x();
  double excluded_x_st3 = spacepoints[2]->get_position().x();
  double excluded_x_st4 = spacepoints[3]->get_position().x();
  double excluded_y_st2 = spacepoints[1]->get_position().y();
  double excluded_y_st3 = spacepoints[2]->get_position().y();
  double excluded_y_st4 = spacepoints[3]->get_position().y();

  std::ofstream out("align.txt", std::ios::out | std::ios::app);
  out << tracker << " " << full_x_st2 << " " << excluded_x_st2 << " "
      << full_x_st3 << " " << excluded_x_st3 << " "
      << full_x_st4 << " " << excluded_x_st4 << " "
      << full_y_st2 << " " << excluded_y_st2 << " "
      << full_y_st3 << " " << excluded_y_st3 << " "
      << full_y_st4 << " " << excluded_y_st4 << std::endl;
  out.close();
}

void MapCppTrackerRecon::fit(std::vector<SciFiSpacePoint*> spacepoints,
                             double &x_const, double &x_slope,
                             double &y_const, double &y_slope) {
#ifdef __CINT__
  gSystem->Load("libMatrix");
#endif
  const Int_t nrVar  = 2;
  // const Int_t nrPnts = 5;
  int nrPnts = spacepoints.size();
  double az[nrPnts];
  double ax[nrPnts];
  double ay[nrPnts];
  double ae[nrPnts];
  for ( int i = 0; i < nrPnts; ++i ) {
    az[i] = spacepoints[i]->get_position().z();
    ax[i] = spacepoints[i]->get_position().x();
    ay[i] = spacepoints[i]->get_position().y();
    ae[i] = 0.9;
  }

  TVectorD z;
  z.Use(nrPnts, az);
  TVectorD x;
  x.Use(nrPnts, ax);
  TVectorD y;
  y.Use(nrPnts, ay);
  TVectorD e;
  e.Use(nrPnts, ae);

  TMatrixD A(nrPnts, nrVar);
  TMatrixDColumn(A, 0) = 1.0;
  TMatrixDColumn(A, 1) = z;
  TMatrixD B(nrPnts, nrVar);
  TMatrixDColumn(B, 0) = 1.0;
  TMatrixDColumn(B, 1) = y;

  const TVectorD solve_x = NormalEqn(A, x, e);
  solve_x.Print();
  x_const = solve_x(0);
  x_slope = solve_x(1);

  const TVectorD solve_y = NormalEqn(A, y, e);
  solve_y.Print();
  y_const = solve_y(0);
  y_slope = solve_y(1);
}

void MapCppTrackerRecon::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering(ClustException, minPE);
  clustering.process(evt, modules);
}

void MapCppTrackerRecon::spacepoint_recon(SciFiEvent &evt) {
  SciFiSpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppTrackerRecon::pattern_recognition(SciFiEvent &evt) {
  PatternRecognition pr1;
  pr1.process(evt);
}

void MapCppTrackerRecon::track_fit(SciFiEvent &evt) {
  KalmanTrackFit fit;
  if ( evt.helicalprtracks().size() )
    fit.process(evt.helicalprtracks());
  if ( evt.straightprtracks().size() )
    fit.process(evt.straightprtracks());
}

void MapCppTrackerRecon::print_event_info(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << "; "
            << event.straightprtracks().size() << " "
            << event.helicalprtracks().size() << " " << std::endl;
}

// The following two functions are added for testing purposes only
Json::Value MapCppTrackerRecon::ConvertToJson(std::string jsonString) {
  Json::Value newJson;
  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  newJson = JsonWrapper::StringToJson(jsonString);
  return newJson;
}

std::string MapCppTrackerRecon::JsonToString(Json::Value json_in) {
  std::stringstream ss_io;
  JsonWrapper::Print(ss_io, json_in);
  return ss_io.str();
}

} // ~namespace MAUS

