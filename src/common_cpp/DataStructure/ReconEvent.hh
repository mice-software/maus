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

class TOFEvent {};  // placeholder for the Tof Event

class SciFiEvent {};  // placeholder for the SciFi Event

class CkovEvent {};  // placeholder for the Ckov Event

class KLEvent {};  // placeholder for the KL Event

class EMREvent {};  // placeholder for the EMR Event

class TriggerEvent {};  // placeholder for the Trigger Event

class GlobalEvent {};  // placeholder for the Global recon event

/** @class ReconEvent contains reconstruction data pertaining to a DAQ particle
 *         event
 *
 *  ReconEvent holds the data for a single DAQ particle event (i.e. trigger
 *  condition). Holds reconstruction data for each detector (TOF, Ckov, SciFi,
 *  KL, EMR) as well as the trigger and the global event (global tracks).
 */
class ReconEvent {
  public:
    /** Initialise to NULL */
    ReconEvent();

    /** Copy constructor - deep copy */
    ReconEvent(const ReconEvent& md);

    /** Equality operator - deep copy */
    ReconEvent& operator=(const ReconEvent& md);

    /** Destructor */
    virtual ~ReconEvent();

    /** Get the TOF detector event */
    TOFEvent* GetTOFEvent() const;

    /** Set the TOF detector event */
    void SetTOFEvent(TOFEvent* event);

    /** Get the SciFi detector event */
    SciFiEvent* GetSciFiEvent() const;

    /** Set the SciFi detector event */
    void SetSciFiEvent(SciFiEvent* event);

    /** Get the Cerenkov detector event */
    CkovEvent* GetCkovEvent() const;

    /** Set the Cerenkov detector event */
    void SetCkovEvent(CkovEvent* event);

    /** Get the KL detector event */
    KLEvent* GetKLEvent() const;

    /** Set the KL detector event */
    void SetKLEvent(KLEvent* event);

    /** Get the EMR detector event */
    EMREvent* GetEMREvent() const;

    /** Set the EMR detector event */
    void SetEMREvent(EMREvent* event);

    /** Get the trigger detector event */
    TriggerEvent* GetTriggerEvent() const;

    /** Set the trigger detector event */
    void SetTriggerEvent(TriggerEvent* event);

    /** Get the global track */
    GlobalEvent* GetGlobalEvent() const;

    /** Set the global track */
    void SetGlobalEvent(GlobalEvent* event);

  private:

    TOFEvent* _tof_event;
    SciFiEvent* _scifi_event;
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
