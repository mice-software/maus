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

#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"

#include "src/common_cpp/DataStructure/ReconEvent.hh"

namespace MAUS {

class TOFEventProcessor : public ObjectProcessor<TOFEvent> {};

class SciFiEventProcessor : public ObjectProcessor<SciFiEvent> {};

class CkovEventProcessor : public ObjectProcessor<CkovEvent> {};

class KLEventProcessor : public ObjectProcessor<KLEvent> {};

class EMREventProcessor : public ObjectProcessor<EMREvent> {};

class TriggerEventProcessor : public ObjectProcessor<TriggerEvent> {};

class GlobalEventProcessor : public ObjectProcessor<GlobalEvent> {};

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
    TOFEventProcessor _tof_proc;
    SciFiEventProcessor _scifi_proc;
    CkovEventProcessor _ckov_proc;
    KLEventProcessor _kl_proc;
    EMREventProcessor _emr_proc;
    TriggerEventProcessor _trigger_proc;
    GlobalEventProcessor _global_proc;
};
}

#endif

