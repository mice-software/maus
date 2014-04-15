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


/** @class MapCppEmrMCDigitization
 * Digitization of the EMR MC data.
 *
 */

#ifndef _MAP_MAPCPPEMRMCDIGITIZATION_H_
#define _MAP_MAPCPPEMRMCDIGITIZATION_H_

// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <string>

// MAUS
#include "DataStructure/Data.hh"

namespace MAUS {

class MapCppEmrMCDigitization {
 public:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** @brief Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool death();

  /** @brief process JSON document
   *
   *  Receive a document with MC hits and return
   *  a document with digits
   */
  std::string process(std::string document);

  /** @brief process the data object
 *
 *  @param
 */
  void process(MAUS::Data *data);

 private:

  std::string _classname;
};  // Don't forget this trailing colon!!!!
} // ~namespace MAUS

#endif

