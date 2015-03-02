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

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "TCanvas.h"
//~ #include "src/map/MapCppGlobalTrackMatching/MapCppGlobalTrackMatching.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/reduce/ReduceCppGlobalReconEfficiency/ReduceCppGlobalReconEfficiency.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

namespace MAUS {

bool ReduceCppGlobalReconEfficiency::birth(std::string aJsonConfigDocument) {

  mClassname = "ReduceCppGlobalReconEfficiency";

  // JsonCpp setup - check file parses correctly, if not return false
  Json::Value configJSON;
  tof0_matches = 0;
  tof0_matches_expected = 0;
  tof1_matches = 0;
  tof1_matches_expected = 0;
  tof2_matches = 0;
  tof2_matches_expected = 0;
  kl_matches = 0;
  kl_matches_expected = 0;
  try {
    configJSON = JsonWrapper::StringToJson(aJsonConfigDocument);
    return true;
  } catch (Exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, mClassname);
  } catch (std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, mClassname);
  }
  return false;
}

std::string ReduceCppGlobalReconEfficiency::process(std::string document) {
  JsonCppSpillConverter json2cppconverter;
  Json::Value *data_json = NULL;
  MAUS::Data *data_cpp = NULL;
  Json::Value imported_json = JsonWrapper::StringToJson(document);
  data_json = new Json::Value(imported_json);
  data_cpp = json2cppconverter(data_json);
  _spill = data_cpp->GetSpill();
  //~ bool read_success = read_in_json(aDocument);
  //~ if (read_success) {
    try {
      if ( ReconEventPArray* recon_events = _spill->GetReconEvents() ) {
        ReconEventPArray::iterator recon_event_iter;
        bool tof0_hit = false;
        bool tof1_hit = false;
        bool tracker0_hit = false;
        bool tracker1_hit = false;
        bool tof2_hit = false;
        bool kl_hit = false;
        bool tof0_match = false;
        bool tof1_match = false;
        bool tof2_match = false;
        bool kl_match = false;
        for (recon_event_iter = recon_events->begin();
             recon_event_iter != recon_events->end();
             ++recon_event_iter) {
          GlobalEvent* global_event = (*recon_event_iter)->GetGlobalEvent();
          // Check for each detector whether hits exist in the event
          std::vector<MAUS::DataStructure::Global::TrackPoint*>*
              global_track_points = global_event->get_track_points();
          std::vector<MAUS::DataStructure::Global::TrackPoint*>::iterator
              global_track_point_iter;
          for (global_track_point_iter = global_track_points->begin();
               global_track_point_iter != global_track_points->end();
               ++global_track_point_iter) {
            if (checkDetector(*global_track_point_iter, 2, 4)) {
              tof0_hit = true;
            }
            if (checkDetector(*global_track_point_iter, 7, 9)) {
              tof1_hit = true;
            }
            if (checkDetector(*global_track_point_iter, 10, 15)) {
              tracker0_hit = true;
            }
            if (checkDetector(*global_track_point_iter, 16, 21)) {
              tracker1_hit = true;
            }
            if (checkDetector(*global_track_point_iter, 22, 24)) {
              tof2_hit = true;
            }
            if (checkDetector(*global_track_point_iter, 25, 25)) {
              kl_hit = true;
            }
          }
          std::vector<MAUS::DataStructure::Global::SpacePoint*>*
              global_space_points = global_event->get_space_points();
          std::vector<MAUS::DataStructure::Global::SpacePoint*>::iterator
              global_space_point_iter;
          for (global_space_point_iter = global_space_points->begin();
               global_space_point_iter != global_space_points->end();
               ++global_space_point_iter) {
            if (checkDetector(*global_space_point_iter, 2, 4)) {
              tof0_hit = true;
            }
            if (checkDetector(*global_space_point_iter, 7, 9)) {
              tof1_hit = true;
            }
            if (checkDetector(*global_space_point_iter, 10, 15)) {
              tracker0_hit = true;
            }
            if (checkDetector(*global_space_point_iter, 16, 21)) {
              tracker1_hit = true;
            }
            if (checkDetector(*global_space_point_iter, 22, 24)) {
              tof2_hit = true;
            }
            if (checkDetector(*global_space_point_iter, 25, 25)) {
              kl_hit = true;
            }
          }
          // Check if each non-Tracker detector has been added to a track, i.e.
          // matched
          std::vector<MAUS::DataStructure::Global::Track*>* global_tracks =
              global_event->get_tracks();
          std::vector<MAUS::DataStructure::Global::Track*>::iterator
              global_track_iter;
          for (global_track_iter = global_tracks->begin();
               global_track_iter != global_tracks->end();
               ++global_track_iter) {
            MAUS::DataStructure::Global::Track* muplus_us_track = NULL;
            MAUS::DataStructure::Global::Track* muplus_ds_track = NULL;

            std::vector<const MAUS::DataStructure::Global::TrackPoint*>::iterator
                track_point_iter;
            if ((*global_track_iter)->get_pid() ==
                MAUS::DataStructure::Global::kMuPlus) {
              if ((*global_track_iter)->get_mapper_name() ==
                  "MapCppGlobalTrackMatching-US") {
                muplus_us_track = *global_track_iter;
                std::vector<const MAUS::DataStructure::Global::TrackPoint*>
                    track_points = muplus_us_track->GetTrackPoints();
                for (track_point_iter = track_points.begin();
                     track_point_iter != track_points.end();
                     ++track_point_iter) {
                  if (checkDetector(*track_point_iter, 2, 4)) {
                    tof0_match = true;
                  }
                  if (checkDetector(*track_point_iter, 7, 9)) {
                    tof1_match = true;
                  }
                }
              } else if ((*global_track_iter)->get_mapper_name() ==
                  "MapCppGlobalTrackMatching-DS") {
                muplus_ds_track = *global_track_iter;
                std::vector<const MAUS::DataStructure::Global::TrackPoint*>
                    track_points = muplus_ds_track->GetTrackPoints();
                for (track_point_iter = track_points.begin();
                     track_point_iter != track_points.end();
                     ++track_point_iter) {
                  if (checkDetector(*track_point_iter, 22, 24)) {
                    tof2_match = true;
                  }
                  if (checkDetector(*track_point_iter, 25, 25)) {
                    kl_match = true;
                  }
                }
              }
            }
          }
        }
        ofstream outfile;
        outfile.open("test.txt", std::ios::out | std::ios::app);
        outfile << tof0_hit << " " << tof1_hit << " " << tracker0_hit << " " << tracker1_hit << " " << tof2_hit << " " << kl_hit << "\n";
        outfile.close();
        if (tracker0_hit and tof0_hit) {
          tof0_matches_expected++;
        }
        if (tracker0_hit and tof1_hit) {
          tof1_matches_expected++;
        }
        if (tracker1_hit and tof2_hit) {
          tof2_matches_expected++;
        }
        if (tracker1_hit and kl_hit) {
          kl_matches_expected++;
        }
        if (tof0_match) {
          tof0_matches++;
        }
        if (tof1_match) {
          tof1_matches++;
        }
        if (tof2_match) {
          tof2_matches++;
        }
        if (kl_match) {
          kl_matches++;
        }
      }
    } catch (Exception exc) {
      Squeak::mout(Squeak::error) << exc.GetMessage() << std::endl;
      //~ mRoot = MAUS::CppErrorHandler::getInstance()->HandleException(mRoot, exc, mClassname);
    } catch (std::exception exc) {
      Squeak::mout(Squeak::error) << exc.what() << std::endl;
      //~ mRoot = MAUS::CppErrorHandler::getInstance()->HandleStdExc(mRoot, exc, mClassname);
    }
  //~ } else {
    //~ std::cerr << mClassname << ": Failed to import json to spill\n";
  //~ }
  //~ std::cerr << "\n#~#\n" << testint << "\n#~#\n";
  std::string output_document = JsonWrapper::JsonToString(*data_json);
  delete data_json;
  delete data_cpp;
  return output_document;
}

bool ReduceCppGlobalReconEfficiency::death()  {
  double tof0_eff, tof1_eff, tof2_eff, kl_eff;
  if (tof0_matches_expected > 0) {
    tof0_eff = (tof0_matches + 0.0) / tof0_matches_expected;
    //~ std::cerr << tof0_eff << "###TOF0Y\n";
  } else {
    tof0_eff = -1;
    //~ std::cerr << tof0_eff << "###TOF0N\n";
  }
  if (tof1_matches_expected > 0) {
    tof1_eff = (tof1_matches + 0.0) / tof1_matches_expected;
    //~ std::cerr << tof1_eff << "###TOF1Y\n";
  } else {
    tof1_eff = -1;
  }
  if (tof2_matches_expected > 0) {
    tof2_eff = (tof2_matches + 0.0) / tof2_matches_expected;
  } else {
    tof2_eff = -1;
  }
  if (kl_matches_expected > 0) {
    kl_eff = (kl_matches + 0.0) / kl_matches_expected;
  } else {
    kl_eff = -1;
  }
  Squeak::mout(Squeak::error) << "EFFICIENCIES:\n"
      << "TOF0: " << tof0_eff*100 << "% " << tof0_matches << "/" << tof0_matches_expected << "\n"
      << "TOF1: " << tof1_eff*100 << "% " << tof1_matches << "/" << tof1_matches_expected << "\n"
      << "TOF2: " << tof2_eff*100 << "% " << tof2_matches << "/" << tof2_matches_expected << "\n"
      << "KL: " << kl_eff*100 << "% " << kl_matches << "/" << kl_matches_expected << "\n";
  return true;
}

bool ReduceCppGlobalReconEfficiency::checkDetector(const
    MAUS::DataStructure::Global::TrackPoint* track_point, int min, int max) {
  int enum_key = static_cast<int>(track_point->get_detector());
  if ((enum_key >= min) and (enum_key <= max)) {
    return true;
  } else {
    return false;
  }
}

bool ReduceCppGlobalReconEfficiency::checkDetector(const
    MAUS::DataStructure::Global::SpacePoint* space_point, int min, int max) {
  int enum_key = static_cast<int>(space_point->get_detector());
  if ((enum_key >= min) and (enum_key <= max)) {
    return true;
  } else {
    return false;
  }
}

//~ bool ReduceCppGlobalReconEfficiency::read_in_json(std::string document) {
  //~ Json::FastWriter writer;
  //~ Json::Reader reader;
  //~ try {
    //~ mRoot = JsonWrapper::StringToJson(aJsonData);
    //~ SpillProcessor spill_proc;
    //~ mSpill = spill_proc.JsonToCpp(mRoot);
    //~ Json::Value imported_json = JsonWrapper::StringToJson(document);
    //~ data_json = new Json::Value(imported_json);
    //~ data_cpp = json2cppconverter(data_json);
    //~ spill = data_cpp->GetSpill();
    //~ return true;
  //~ } catch (...) {
    //~ Json::Value errors;
    //~ std::stringstream ss;
    //~ ss << mClassname << ": Failed when importing JSON to Spill";
    //~ errors["bad_json_document"] = ss.str();
    //~ mRoot["errors"] = errors;
    //~ writer.write(mRoot);
    //~ return false;
  //~ }
//~ }

} // ~namespace MAUS
