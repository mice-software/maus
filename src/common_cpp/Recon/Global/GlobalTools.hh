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
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {
namespace GlobalTools {

std::map<MAUS::DataStructure::Global::DetectorPoint, bool>
    GetMCDetectors(MAUS::GlobalEvent* global_event);


/**
 * @brief Returns a vector of all Tracks in the Spill (i.e. across recon events
 * if applicable) that have the given mapper name and include TrackPoints in
 * the given detector
 * 
 * @param spill The spill
 * @param detector Detector enum for the selection
 * @param mapper_name Mapper name for the selection
 */
std::vector<MAUS::DataStructure::Global::Track*>* GetSpillDetectorTracks(
    MAUS::Spill* spill, MAUS::DataStructure::Global::DetectorPoint detector,
     std::string mapper_name);

/**
 * @brief Returns a vector of all Tracks in in the global event with the given
 * mapper name
 * 
 * @param global_even The global event
 * @param mapper_name Mapper name for the selection
 */
std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name);

/**
 * @brief Returns a vector of all Tracks in in the global event with the given
 * mapper name and the given PID
 * 
 * @param global_even The global event
 * @param mapper_name Mapper name for the selection
 * @param pid PID for the selection
 */
std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name,
    MAUS::DataStructure::Global::PID pid);

/**
 * @brief Returns a vector of ints denoting the tracker plane of a TrackPoint.
 * The first number indicates the tracker, the second the tracker station, the
 * third the tracker plane
 * 
 * @param track_point The trackpoint for which to determine the tracker plane
 */
std::vector<int> GetTrackerPlane(const MAUS::DataStructure::Global::TrackPoint*
    track_point);

/**
 * @brief Returns a vector of all SpacePoints in a spill (i.e. across recon
 * events) for a given detector
 * 
 * @param spill The spill
 * @param detector Detector enum for the selection
 */
std::vector<MAUS::DataStructure::Global::SpacePoint*>* GetSpillSpacePoints(
    Spill* spill, MAUS::DataStructure::Global::DetectorPoint detector);

/**
 * @brief Checks whether two numbers are the same to within a given tolerance
 */
bool approx(double a, double b, double tolerance);


} // namespace GlobalTools
} // namespace MAUS

#endif
