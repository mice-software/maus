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

#ifndef SCIFILOOKUP_HH
#define SCIFILOOKUP_HH

// C++ headers
#include <vector>
#include <map>
#include "stdint.h"

// MAUS headers
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/SciFiNoiseHit.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"

namespace MAUS {

/** @class SciFiLookup
 *  @author A. Dobbs
 *  @brief Class used to link between SciFiDigits and the MC hits
 *         used to make them
 */
class SciFiLookup {
  public:
    /** @brief Default constructor */
    SciFiLookup();

    /** @brief Default destructor, does nothing */
    ~SciFiLookup();

    /** @brief Create the maps between digits and hits */
    bool make_hits_map(MCEvent* evt);

    /** @brief Create the maps between digits and noise hits */
    bool make_noise_map(MCEvent* evt);

    /** @brief Return a vector of SciFiHits used to make a SciFiDigit */
    bool get_hits(const SciFiDigit* dig, std::vector<SciFiHit*> &hits);

    /** @brief Return a vector of SciFiNoiseHits used to make a SciFiDigit */
    bool get_noise(const SciFiDigit* dig, std::vector<SciFiNoiseHit*> &noise);

    /** @brief Return a vector of SciFiNoiseHits used to make a SciFiDigit */
    uint64_t get_digit_id(const SciFiDigit* digit);

    /** @brief Return the digit to hits map */
    std::map<uint64_t, std::vector<SciFiHit*> > get_hits_map() { return _hits_map; }

    /** @brief Return the digit to noise hits map */
    std::map<uint64_t, std::vector<SciFiNoiseHit*> > get_noise_map() { return _noise_map; }

  private:
    std::map<uint64_t, std::vector<SciFiHit*> > _hits_map;
    std::map<uint64_t, std::vector<SciFiNoiseHit*> > _noise_map;
};

} // ~namespace MAUS

#endif
