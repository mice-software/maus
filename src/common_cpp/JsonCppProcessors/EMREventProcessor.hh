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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMREVENTPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMREVENTPROCESSOR_

#include "DataStructure/EMREvent.hh"
#include "JsonCppProcessors/ObjectProcessor.hh"
#include "JsonCppProcessors/EMREventTrackProcessor.hh"

namespace MAUS {

/** @class EMREventProcessor Conversions for EMREvent between C++ and Json 
 *
 */

class EMREventProcessor : public ObjectProcessor<EMREvent> {
 public:
  /** Set up processors and register branches
   *
   *  Everything else is handled by the base class
   */
  EMREventProcessor();

 private:
  PointerArrayProcessor<EMREventTrack> _event_track_array_proc;
  ThreeVectorProcessor _threevector_proc;
  DoubleProcessor _double_proc;
};
}  // namespace MAUS

#endif // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMREVENTPROCESSOR_
