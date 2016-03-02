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

// C++ headers
#include <map>

// ROOT headers
#include "TLorentzVector.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
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
    GetMCDetectors(MCEvent* mc_event);

/**
 * @brief Returns array of all TOF hits in a given TOF from the MC event
 * 
 * @param mc_event The MC event
 * @param detector The detector for the selection, must correspond to a TOF
 */
TOFHitArray* GetTOFHits(MCEvent* mc_event,
    DataStructure::Global::DetectorPoint detector);

/**
 * @brief Returns array of all Tracker hits in a given Tracker or Tracker
 * station from the MC event
 * 
 * @param mc_event The MC event
 * @param detector The detector for the selection, must correspond to a Tracker
 *        or Tracker station
 */
SciFiHitArray* GetTrackerHits(MCEvent* mc_event,
    DataStructure::Global::DetectorPoint detector);

/**
 * @brief Returns the first mc hit found for a tracker station as determined
 * by 3 integers
 * 
 * @param mc_event The MC event
 * @param tracker number of the tracker, 0 or 1
 * @param station tracker station number, 0 to 4 counting outward from the
 *        cooling channel
 * @param plane tracker plane number within a station, 0 to 2 counting outward
 *        from the cooling channel
 */
SciFiHit* GetTrackerPlaneHit(MCEvent* mc_event,
    int tracker, int station, int plane);

/**
 * @brief Returns the TOF mc hit nearest to the position TLorentzVector provided
 * 
 * @param hits Array of TOF hits
 * @param position TLorentzVector of the position at which to find a hit
 */
TOFHit GetNearestZHit(TOFHitArray* hits, TLorentzVector position);

/**
 * @brief Returns the KL mc hit nearest to the position TLorentzVector provided
 * 
 * @param hits Array of KL hits
 * @param position TLorentzVector of the position at which to find a hit
 */
KLHit GetNearestZHit(KLHitArray* hits, TLorentzVector position);

/**
 * @brief Returns the Tracker mc hit nearest to the position TLorentzVector
 * provided
 * 
 * @param hits Array of Tracker hits
 * @param position TLorentzVector of the position at which to find a hit
 */
EMRHit GetNearestZHit(EMRHitArray* hits, TLorentzVector position);

} // namespace MCTruthTools
} // namespace MAUS

#endif
