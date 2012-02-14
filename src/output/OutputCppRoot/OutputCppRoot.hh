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

// TODO (Rogers): looks okay - but throws a segmentation fault if we forget to
//                call death(); needs proper error handler calling

class MausData;
class JsonCppConverter;
class orstream;

namespace MAUS {

/** @class OutputCppRoot
 *  
 *  OutputCppRoot writes the data structure out as a ROOT tree
 */

class OutputCppRoot {
 public:
  /** OutputCppRoot constructor - initialise to NULL
   */
  OutputCppRoot() : _outfile(NULL), _md(NULL), _jsonCppConverter(NULL) {;}

  /** OutputCppRoot destructor - calls death()
   */
  ~OutputCppRoot() {death();}

  /** Initialise member data using datacards
   *  @param json_datacards Json document containing datacards. Only card used
   *         is root_output_file [string], the root file name
   *
   *  @returns True on success, False on failure
   */
  bool birth(std::string json_datacards);

  /** Store a spill in the ROOT tree
   *
   *  @returns True on success, False on failure
   */
  bool save(std::string json_spill_document);

  /** Delete member data
   *
   *  @returns True on success, False on failure
   */
  bool death();

 private:
  orstream* _outfile;
  MC* _mc;
  Digits* _d;
  MausData* _md;
  JsonCppConverter* _jsonCppConverter;
};
}

#endif


