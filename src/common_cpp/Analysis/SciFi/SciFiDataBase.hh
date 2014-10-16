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

/** @class SciFiDataBase
 *
 *  Abstract base class for other SciFi reduced data classes
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATABASE_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDATABASE_

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"


namespace MAUS {

class SciFiDataBase  {
  public:

    /** Default constructor */
    SciFiDataBase() {}

    /** Destructor */
    virtual ~SciFiDataBase() {}

    /** Pure virtual function hence must be overidden in daughter classes.
     *  Takes in the data, one spill at a time
     */
    virtual bool Process(Spill* spill) = 0;
};

} // ~namespace MAUS

#endif
