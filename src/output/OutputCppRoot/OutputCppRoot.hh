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

#ifndef _SRC_OUTPUT_OUTPUTCPPROOT_OUTPUTCPPROOT_HH_
#define _SRC_OUTPUT_OUTPUTCPPROOT_OUTPUTCPPROOT_HH_

#include <string>

#include "src/common_cpp/API/OutputBase.hh"

class Data;
class JsonCppConverter;
class JobHeader;
class JsonCppHeaderConverter;
class orstream;

namespace MAUS {

/** @class OutputCppRoot
 *  
 *  OutputCppRoot writes the data structure out as a ROOT tree
 */

class OutputCppRoot : public OutputBase<std::string> {
 public:
  /** OutputCppRoot constructor - initialise to NULL
   */
  OutputCppRoot();

  /** OutputCppRoot destructor - calls death()
   */
  ~OutputCppRoot() {death();}

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

  /** Store a spill in the ROOT tree
   *
   * OutputBase provides public interface
   *
   *  @returns True on success, False on failure
   */
  bool _save_spill(std::string json_spill_document);

  /** Store job header in the ROOT tree, overwriting current job header
   *
   * OutputBase provides public interface
   *
   *  @returns True on success, False on failure
   */
  bool _save_job_header(std::string json_header);

  orstream* _outfile;
  orstream* _outfile_2;

  Data* _spill;
  JsonCppConverter* _jsonCppConverter;

  JobHeader* _header;
  JsonCppHeaderConverter* _jsonCppHeaderConverter;

  std::string _fname;
};
}

#endif


