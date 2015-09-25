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


/** @class MapCppMCReconSetup
 *  Setup dummy recon events to hold detector recon events
 *
 */

#ifndef _MAP_MAPCPPMCReconSetup_H
#define _MAP_MAPCPPMCReconSetup_H_
// C headers
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// C++ headers
#include <string>
#include <iostream>
#include <sstream>
#include "API/MapBase.hh"

namespace MAUS {

class MapCppMCReconSetup : public MapBase<MAUS::Data> {

 public:
  MapCppMCReconSetup();

 private:
 /** @brief Sets up the worker
  *  @param argJsonConfigDocument a JSON document with
  *         the configuration.
  */
  void _birth(const std::string& argJsonConfigDocument);

 /** @brief Shutdowns the worker
  *  This takes no arguments and does nothing
  */
  void _death();

  /** @brief process the data object
 *
 *  @param
 */
  void _process(MAUS::Data *data) const;
};
}

#endif

