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

#include "src/legacy/Interface/Squeal.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppStreamer/ORStream.hh"
#include "src/common_cpp/JsonCppStreamer/JsonCppConverter.hh"

#include "src/output/OutputCppRoot/OutputCppRoot.hh"

namespace MAUS {

// BUG - segmentation fault if I don't death() - probably not destructing in
//       proper order

bool OutputCppRoot::birth(std::string json_datacards) {
  try {
    // load datacards
    Json::Value datacards = JsonWrapper::StringToJson(json_datacards);
    std::string root_output = JsonWrapper::GetProperty(datacards,
                   "root_output_filename", JsonWrapper::stringValue).asString();
    // Setup output stream
    _outfile = new orstream(root_output.c_str(), "MausData");
    Digits* d = new Digits();
    MC* mc = new MC();
    _md = new MausData(d, mc);

    // Set branch addresses
    (*_outfile) << oneArgManip<const char*>::branchName("digits") << d;
    (*_outfile) << oneArgManip<const char*>::branchName("mc") << mc;
    _jsonCppConverter = new JsonCppConverter(_md);
    return true;
  } catch(Squeal squee) {
    // call error handler
    return false;
  } catch(std::exception exc) {
    // call error handler
    return false;
  }
}

bool OutputCppRoot::save(std::string json_spill_document) {
  try {
    if (_jsonCppConverter == NULL || _md == NULL || _outfile == NULL) {
      throw(Squeal(Squeal(
        Squeal::recoverable,
        "OutputCppRoot was not initialised properly",
        "OutputCppRoot::save"
      )));
    }

    (*_jsonCppConverter)(json_spill_document);
    (*_outfile) << fillEvent;
    return true;
  } catch(Squeal squeal) {
    // call error handler
    return false;
  } catch(std::exception exc) {
    // call error handler
    return false;
  }
}

bool OutputCppRoot::death() {
  if (_outfile != NULL) {
    _outfile->close();
    delete _outfile;
    _outfile = NULL;
  }
  if (_md != NULL) {
    delete _md;
    _md = NULL;
  }
  return true;
}
}

