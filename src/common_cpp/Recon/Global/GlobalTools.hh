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
 *  @file GlobalTools.hh
 *  @brief A collection of helper functions for GlobalEvent objects
 *  @author Jan Greis, University of Warwick
 *  @date 2015/04/17
 */

#ifndef _SRC_COMMON_CPP_RECON_GLOBALTOOLS_HH_
#define _SRC_COMMON_CPP_RECON_GLOBALTOOLS_HH_

#include <vector>
#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {
namespace GlobalTools {

std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name,
    MAUS::DataStructure::Global::PID pid);

std::vector<int> GetTrackerPlane(const MAUS::DataStructure::Global::TrackPoint*
    track_point);

bool approx(double a, double b, double tolerance);

/**
 * @brief Returns a map of all DetectorPoint enums with a boolean flag
 * indicating whether the MC event contains a hit in the corresponding
 * detector.
 * 
 * @param mc_event The MC event that should be checked
 */


} // namespace GlobalTools
} // namespace MAUS

#endif
