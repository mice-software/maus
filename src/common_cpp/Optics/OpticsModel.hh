/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#ifndef SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH

#include "src/common_cpp/Optics/TransferMap.hh"

namespace Json {
  class Value;
}

namespace MAUS {

class OpticsModel {
 public:
  OpticsModel(const Json::Value & configuration) { }
  virtual ~OpticsModel() { }

  virtual void Build() = 0;

  /* @brief Dynamically allocate a new TransferMap between two z-axis.
   *
   * The user of this function takes ownership of the dynamically allocated
   * memory and is responsible for deallocating it.
   */
  virtual const TransferMap * transfer_map(const int station_id) const = 0;
 protected:
};
}

#endif
