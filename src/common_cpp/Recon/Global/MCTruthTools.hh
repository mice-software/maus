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

/** 
 *  @file MCTruthTools.hh
 *  @brief A collection of helper functions for using MC Truth data
 *  @author Jan Greis, University of Warwick
 *  @date 2015/03/04
 */

#ifndef _SRC_COMMON_CPP_RECON_MCTRUTHTOOLS_HH_
#define _SRC_COMMON_CPP_RECON_MCTRUTHTOOLS_HH_

#include <map>
#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {
namespace MCTruthTools {

/**
 * @brief Returns a map of all DetectorPoint enums with a boolean flag
 * indicating whether the MC event contains a hit in the corresponding
 * detector.
 * 
 * @param mc_event The MC event that should be checked
 */
std::map<DataStructure::Global::DetectorPoint, bool>
    GetMCDetectors(MAUS::MCEvent* mc_event);


} // namespace MCTruthTools
} // namespace MAUS

#endif
