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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_RECONEVENTPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_RECONEVENTPROCESSOR_HH_

#include "json/value.h"

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"

#include "src/common_cpp/JsonCppProcessors/TOFEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/CkovEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/KLEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMREventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TriggerEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/GlobalEventProcessor.hh"

#include "src/common_cpp/DataStructure/ReconEvent.hh"

namespace MAUS {

/** @class ReconEventProcessor processes the data for ReconEvent class
 *  
 *  Stubs in place for processors for each of the detector types (but
 *  expect them to be implemented later)
 */
class ReconEventProcessor : public ObjectProcessor<ReconEvent> {
  public:
    /** Constructor - registers the branch structure
     */
    ReconEventProcessor();

  private:
    IntProcessor _int_proc;
    TOFEventProcessor _tof_proc;
    SciFiEventProcessor _sci_fi_proc;
    CkovEventProcessor _ckov_proc;
    KLEventProcessor _kl_proc;
    EMREventProcessor _emr_proc;
    TriggerEventProcessor _trigger_proc;
    GlobalEventProcessor _global_proc;
};
}

#endif

