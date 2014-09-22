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

/** @class SciFiAnalyserBase
 *
 *  Abstract base class for other SciFi analysis classes
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIANALYSERBASE_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIANALYSERBASE_

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"


namespace MAUS {

class SciFiAnalyserBase  {
  public:

    /** Default constructor */
    SciFiAnalyserMomentum() {};

    /** Destructor */
    virtual ~SciFiAnalyserMomentum() {};

    /** Pure virtual function hence must be overidden in daughter classes.
     *  Takes in the data, one spill at a time
     */
    virtual void accumulate(Spill* spill) = 0;

  protected:

  private:

};

} // ~namespace MAUS

#endif
