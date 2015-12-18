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

/** @class MapCppDataSelection
 *  Select only those recon events which meet certain criteria for
 *  further processing in MAUS e.g. 1 spacepoint in TOF1 and TOF2
 */

#ifndef _SRC_MAP_MAPCPPDATASELECTION_H_
#define _SRC_MAP_MAPCPPDATASELECTION_H_

#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"

namespace MAUS {

class MapCppDataSelection : public MapBase<Data> {
  public:
    /** Constructor */
    MapCppDataSelection();
    
    /** Destructor */
    ~MapCppDataSelection();

  private:
    /** Sets up the worker
    *
    *  \param argJsonConfigDocument a JSON document with
    *         the configuration.
    */
    void _birth(const std::string& argJsonConfigDocument);

    /** Shutdowns the worker
    *
    *  This takes no arguments and does nothing
    */
    void _death();

    /** Process MAUS data object
    *
    *  Receive a data object with digits (either MC or real) and then call the higher level
    *  reconstruction algorithms
    *
    * \param document a line/spill from the JSON input
    */
    void _process(Data* data) const;
      
}; // ~ class MapCppDataSelection
} // ~namespace MAUS

#endif
