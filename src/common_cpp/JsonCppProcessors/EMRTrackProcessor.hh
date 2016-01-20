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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMRTRACKPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMRTRACKPROCESSOR_

#include "DataStructure/EMRTrack.hh"
#include "JsonCppProcessors/ObjectProcessor.hh"
#include "JsonCppProcessors/PrimitivesProcessors.hh"
#include "JsonCppProcessors/ArrayProcessors.hh"
#include "JsonCppProcessors/ThreeVectorProcessor.hh"
#include "JsonCppProcessors/EMRTrackPointProcessor.hh"

namespace MAUS {

/** @class EMRTrackProcessor Conversions for EMRTrack between C++ and Json 
 *
 */

class EMRTrackProcessor : public ObjectProcessor<EMRTrack> {
 public:
  /** Set up processors and register branches
  *
  *  Everything else is handled by the base class
  */
  EMRTrackProcessor();

 private:
  ValueArrayProcessor<EMRTrackPoint> _trackpoint_array_proc;
  ValueArrayProcessor<double> _double_array_proc;
  ThreeVectorProcessor _threevector_proc;
  DoubleProcessor _double_proc;
};
}  // namespace MAUS

#endif // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMRTRACKPROCESSOR_
