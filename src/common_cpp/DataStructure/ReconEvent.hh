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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_RECONEVENTDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_RECONEVENTDATA_HH_

#include <vector>

#include "Rtypes.h" // ROOT

namespace MAUS {

class TofEvent {};

class TrackerEvent {};

class CkovEvent {};

class KLEvent {};

class EMREvent {};

class TriggerEvent {};

class GlobalEvent {};

class ReconEvent {
  public:
    ReconEvent();

    ReconEvent(const ReconEvent& md);
         
    ReconEvent& operator=(const ReconEvent& md);
    
    virtual ~ReconEvent();

    TofEvent* GetTofEvent() const;
    void SetTofEvent(TofEvent* event);

    TrackerEvent* GetTrackerEvent() const;
    void SetTrackerEvent(TrackerEvent* event);

    CkovEvent* GetCkovEvent() const;
    void SetCkovEvent(CkovEvent* event);

    KLEvent* GetKLEvent() const;
    void SetKLEvent(KLEvent* event);

    EMREvent* GetEMREvent() const;
    void SetEMREvent(EMREvent* event);

    TriggerEvent* GetTriggerEvent() const;
    void SetTriggerEvent(TriggerEvent* event);

    GlobalEvent* GetGlobalEvent() const;
    void SetGlobalEvent(GlobalEvent* event);

  private:

    TofEvent* _tof_event;
    TrackerEvent* _tracker_event;
    CkovEvent* _ckov_event;
    KLEvent* _kl_event;
    EMREvent* _emr_event;
    TriggerEvent* _trigger_event;
    GlobalEvent* _global_event;

  ClassDef(ReconEvent, 1)
};

typedef std::vector<ReconEvent*> ReconEventArray;


}

#endif
