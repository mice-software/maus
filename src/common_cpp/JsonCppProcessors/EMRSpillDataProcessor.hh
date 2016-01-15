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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMRSPILLDATAPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMRSPILLDATAPROCESSOR_HH_



#include "DataStructure/EMRSpillData.hh"
#include "JsonCppProcessors/ObjectProcessor.hh"
#include "JsonCppProcessors/ArrayProcessors.hh"
#include "JsonCppProcessors/EMRBarHitProcessor.hh"
#include "JsonCppProcessors/EMREventTrackProcessor.hh"

namespace MAUS {

/** @class EMRSpillData processor for converting EMR spill level data */
class EMRSpillDataProcessor : public ObjectProcessor<EMRSpillData>  {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    EMRSpillDataProcessor();

  private:
    ValueArrayProcessor<EMRBarHit> _bar_hit_array_proc;
    PointerArrayProcessor<EMREventTrack> _event_track_array_proc;
};
} // namespace MAUS

#endif // # define _SRC_COMMON_CPP_JSONCPPPROCESSORS_EMRSPILLDATAPROCESSOR_HH_
