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

#include "TCanvas.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Utils/Squeak.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/reduce/ReduceCppPatternRecognition/ReduceCppPatternRecognition.hh"

namespace MAUS {

bool ReduceCppPatternRecognition::birth(std::string aJsonConfigDocument) {

  mClassname = "ReduceCppPatternRecognition";
  mDataManager.set_print_tracks(false);               // Do not print info about the PR tracks
  mDataManager.set_print_seeds(false);               // Do not print info about track seeds
  mXYZPlotter = new TrackerDataPlotterXYZ();         // The spacepoint and track plotter
  mInfoBoxPlotter = new TrackerDataPlotterInfoBox(); // The infobox plotter
  mPlotters.push_back(mXYZPlotter);
  mPlotters.push_back(mInfoBoxPlotter);

  // JsonCpp setup - check file parses correctly, if not return false
  Json::Value configJSON;
  try {
    configJSON = JsonWrapper::StringToJson(aJsonConfigDocument);
    return true;
  } catch (Exceptions::Exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, mClassname);
  } catch (std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, mClassname);
  }
  return false;
}

std::string ReduceCppPatternRecognition::process(std::string aDocument) {
  bool read_success = read_in_json(aDocument);
  if (read_success) {
    try {
      if ( mSpill->GetReconEvents() ) {
        mDataManager.process(mSpill);
        mDataManager.draw(mPlotters);
      }
    } catch (Exceptions::Exception exc) {
      Squeak::mout(Squeak::error) << exc.GetMessage() << std::endl;
      mRoot = MAUS::CppErrorHandler::getInstance()->HandleException(mRoot, exc, mClassname);
    } catch (std::exception exc) {
      Squeak::mout(Squeak::error) << exc.what() << std::endl;
      mRoot = MAUS::CppErrorHandler::getInstance()->HandleStdExc(mRoot, exc, mClassname);
    }
    mDataManager.clear_spill();
  } else {
    std::cerr << mClassname << ": Failed to import json to spill\n";
  }
  return JsonWrapper::JsonToString(mRoot);
}

bool ReduceCppPatternRecognition::death()  {
  return true;
}

bool ReduceCppPatternRecognition::read_in_json(std::string aJsonData) {
  Json::FastWriter writer;
  Json::Reader reader;
  try {
    mRoot = JsonWrapper::StringToJson(aJsonData);
    SpillProcessor spill_proc;
    mSpill = spill_proc.JsonToCpp(mRoot);
    return true;
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
    ss << mClassname << ": Failed when importing JSON to Spill";
    errors["bad_json_document"] = ss.str();
    mRoot["errors"] = errors;
    writer.write(mRoot);
    return false;
  }
}

} // ~namespace MAUS
