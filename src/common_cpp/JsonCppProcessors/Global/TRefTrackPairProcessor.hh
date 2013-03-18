/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TREFTRACKPAIRPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TREFTRACKPAIRPROCESSOR_HH_

#include "src/common_cpp/JsonCppProcessors/PrimaryProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

#include "DataStructure/Global/TRefTrackPair.hh"

namespace MAUS {
namespace Processor {
namespace Global {

/** @class TRefTrackPairProcessor for converting json <-> cpp data 
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
 */
class TRefTrackPairProcessor
    : public ObjectProcessor<MAUS::DataStructure::Global::TRefTrackPair> {
 public:
    /** Constructor - registers the branch structure */
    TRefTrackPairProcessor();
};
} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS

#endif

