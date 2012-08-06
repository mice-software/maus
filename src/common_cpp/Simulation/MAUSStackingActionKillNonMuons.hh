/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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


/** @class  MAUSStackingActionKillNonMuons
 *  Geant4 can calls this class at the start of each track so we kill non-muons
 *
 *  This class is optionally included within the MAUSGeant4Manager and will kill
 *  any particle that isn't a muon in order to not simulate electron/gamma
 *  showers.
 */

#ifndef _SRC_CPP_CORE_SIMULATION_MAUSSTACKINGACTION_HH_
#define _SRC_CPP_CORE_SIMULATION_MAUSSTACKINGACTION_HH_

#include "Geant4/G4UserStackingAction.hh"
#include "Geant4/G4Track.hh"

class G4Track;

class MAUSStackingActionKillNonMuons : public G4UserStackingAction {
  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
};

#endif
