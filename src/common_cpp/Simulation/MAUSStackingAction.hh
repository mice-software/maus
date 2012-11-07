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


#ifndef _SRC_COMMON_CPP_SIMULATION_MAUSSTACKINGACTION_HH_
#define _SRC_COMMON_CPP_SIMULATION_MAUSSTACKINGACTION_HH_

#include <map>
#include <string>
#include "Geant4/G4UserStackingAction.hh"

class G4Track;

namespace MAUS {

/** @class MAUSStackingAction
 *  MAUSStackingAction is used to control how Geant4 invokes new tracks
 *
 *  G4UsetStackingAction controls how Geant4 invokes new tracks. Geant4 has a
 *  few "Stacks" of events; urgentStack and waitingStack. urgentStack is tracked
 *  first, then waitingStack. postponeStack is tracked in the next Event. Also
 *  if tracks are classified by fKill they are killed immediately.
 *
 *  In MAUS instantiation, we cut (fKill) incoming particles; any particle
 *  with energy less than a kinetic_energy_threshold or pdg_pid_keep_kill dict
 *  set to False will be killed. If default_keep_or_kill is False, particles not
 *  in pdg_pid_keep_kill are also killed.
 */

class MAUSStackingAction : public G4UserStackingAction {
  public:
    /** Set the cuts - kinetic energy cut and pid cuts are set here
     *
     *  convert datacards to C++ objects. Datacards invoked:
     *    - "default_keep_or_kill" as bool
     *    - "keep_or_kill_particles" as std::map<std::string, bool>
     *    - "kinetic_energy_threshold" as bool
     */
    MAUSStackingAction();

    /** Kill the particles
     *
     *  Kills if:
     *    - pdg_pid_keep_kill[particle_name] is set to false
     *    - particle_name is not in pdg_pid_keep_kill and _defalt is set to
     *      false
     *    - kinetic energy is below _kinetic_energy_threshold
     */ 
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);

    /** Return kinetic energy threshold */
    inline double GetKineticEnergyThreshold() const;

    /** Set kinetic energy threshold */
    inline void SetKineticEnergyThreshold(double ke_threshold);

    /** Return pdg_pid_keep_or_kill */
    inline std::map<std::string, bool> GetPdgPidKeepKill() const;

    /** Set pdg_pid_keep_or_kill */
    inline void SetPdgPidKeepKill(std::map<std::string, bool> keep_kill);

    /** Return default_keep_or_kill */
    inline bool GetDefaultKeepOrKill() const;

    /** Set default_keep_or_kill */
    inline void SetDefaultKeepOrKill(bool keep_kill);

  private:
    bool _default;
    std::map<std::string, bool> _pdg_pid_keep_kill;
    double _kinetic_energy_threshold;
};

double MAUSStackingAction::GetKineticEnergyThreshold() const {
    return _kinetic_energy_threshold;
}

void MAUSStackingAction::SetKineticEnergyThreshold(double ke_threshold) {
    _kinetic_energy_threshold = ke_threshold;
}

std::map<std::string, bool> MAUSStackingAction::GetPdgPidKeepKill() const {
    return _pdg_pid_keep_kill;
}

void MAUSStackingAction::SetPdgPidKeepKill(std::map<std::string, bool> keep_kill) {
    _pdg_pid_keep_kill = keep_kill;
}

bool MAUSStackingAction::GetDefaultKeepOrKill() const {
    return _default;
}

void MAUSStackingAction::SetDefaultKeepOrKill(bool keep_kill) {
    _default = keep_kill;
}
}
#endif
