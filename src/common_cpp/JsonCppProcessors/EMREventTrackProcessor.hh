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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMREVENTTRACKPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMREVENTTRACKPROCESSOR_

#include "DataStructure/EMREvent.hh"
#include "JsonCppProcessors/ObjectProcessor.hh"
#include "JsonCppProcessors/EMRPlaneHitProcessor.hh"
#include "JsonCppProcessors/EMRSpacePointProcessor.hh"
#include "JsonCppProcessors/EMRTrackProcessor.hh"

namespace MAUS {

/** @class EMREventTrackProcessor Conversions for EMREventTrack between C++ and Json 
 *
 */

class EMREventTrackProcessor : public ObjectProcessor<EMREventTrack> {
 public:
  /** Set up processors and register branches
   *
   *  Everything else is handled by the base class
   */
  EMREventTrackProcessor();

 private:
  PointerArrayProcessor<EMRPlaneHit> _plane_hit_array_proc;
  PointerArrayProcessor<EMRSpacePoint> _space_point_array_proc;
  EMRTrackProcessor _track_proc;
  StringProcessor _string_proc;
  IntProcessor _int_proc;
  DoubleProcessor _double_proc;
};
}  // namespace MAUS

#endif // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMREVENTTRACKPROCESSOR_
