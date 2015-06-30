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
 *
 */

#ifndef _SRC_COMMON_CPP_SIMULATION_MAUSEVENTACTION_HH_
#define _SRC_COMMON_CPP_SIMULATION_MAUSEVENTACTION_HH_

#include "Geant4/G4UserEventAction.hh"

#include "json/value.h"

class MICEDetectorConstruction;

namespace MAUS {
class MAUSGeant4Manager;
class VirtualPlaneManager;
class MAUSTrackingAction;
class MAUSSteppingAction;
class MCEvent;
namespace Simulation {
class DetectorConstruction;
}

/** @class MAUSEventAction
 *
 *  @brief Holds the event buffer for the spill and updates after each event.
 *
 *  MAUSEventAction is responsible for maintaining the event buffer for the
 *  spill. Several objects hold data buffer for each event. VirtualPlaneManager
 *  holds a buffer of virtual plane hits, tracking holds a buffer of tracks (and
 *  steps), MICEDetectorConstruction holds a buffer of sensitive detector hits.
 *
 *  At the beginning of every event, MAUSEventAction clears each of these
 *  buffers ready for tracking. At the end of every event, MAUSEventAction gets
 *  data from each of these objects and then clears their buffers ready for the
 *  next event.
 */

class MAUSEventAction : public G4UserEventAction {
 public:
    /** Constructor - Initialises everything to 0
     */
    MAUSEventAction();

    /** Nothing to do as I don't allocate memory
     */
    ~MAUSEventAction() {}

    /** @brief Clear per-event buffers ready for the new event.
     *
     *  (I do the clear here so that any other folks who want access to this
     *  data are still able to get it).
     */
    void BeginOfEventAction(const G4Event *anEvent);

    /** @brief Move per-event buffers into the _events json array.
     *
     *   Increment the _primary array so that we now point at the next item in
     *   _events.
     */
    void EndOfEventAction(const G4Event *anEvent);

    /** Set event buffer, set _primary counter to 0 and reassign pointers
     *  @param events: vector of events in the spill. Will append
     *                 hits, virtual_hits, tracks to each event after
     *                 geant4 is done tracking. MAUSEventAction takes ownership
     *                 of this memory.
     *  Also refreshes pointers to objects holding per-event buffers from the
     *  geant4 manager. We support dynamic reallocation of these pointers (even
     *  if geant4 doesnt in some instances). This means, for example, one can
     *  dynamically change the VirtualPlane setup per spill (e.g. to run
     *  reference particles). 
     */
    void SetEvents(std::vector<MCEvent*>* events);

    /** Returns the event buffer. 
     */
    std::vector<MCEvent*>* GetEvents() {return _events;}

 private:
    std::vector<MCEvent*>* _events;
    unsigned int _primary;
    // All borrowed references owned by _g4manager (don't delete!)
    MAUSGeant4Manager* _g4manager;
    VirtualPlaneManager* _virtPlanes;
    MAUSTrackingAction* _tracking;
    MAUSSteppingAction* _stepping;
    Simulation::DetectorConstruction* _geometry;
};
}

#endif

