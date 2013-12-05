/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#ifndef _SRC_OUTPUT_OUTPUTCPPSOCKET_OUTPUTCPPSOCKET_HH_
#define _SRC_OUTPUT_OUTPUTCPPSOCKET_OUTPUTCPPSOCKET_HH_

#include <fstream>
#include <string>
#include <vector>

#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/API/OutputBase.hh"

class TSocket;

namespace MAUS {

class ImageData;

/** @class OutputCppSocket
 *  
 *  OutputCppSocket exports the data structure out as a ROOT TSocket
 */

class OutputCppSocket : public OutputBase<std::string> {
 public:
  /** OutputCppSocket constructor - initialise to NULL
   */
  OutputCppSocket();

  /** OutputCppSocket destructor - calls death()
   */
  ~OutputCppSocket() {death();}

  /** Initialise member data using datacards
   *
   *  @param json_datacards Json document containing datacards. Only card used
   *         is root_output_file [string], the root file name
   *
   *  Calls, in a roundabout way, _death(); required to force SWIG to see the
   *  base class method
   */
  inline void birth(const std::string& json_datacards) {
      ModuleBase::birth(json_datacards);
  }

  /** Delete member data
   *
   *  Calls, in a roundabout way, _death(); required to force SWIG to see the
   *  base class method
   */
  inline void death() {
      ModuleBase::death();
  }

 private:
  void _birth(const std::string& json_datacards);

  void _death();

  bool _save(ImageData* image);

  TSocket* _socket;
};
}

#endif

