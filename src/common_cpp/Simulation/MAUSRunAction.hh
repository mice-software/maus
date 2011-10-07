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

#ifndef SRC_COMMON_CPP_SIMULATION_MAUSRunAction_H
#define SRC_COMMON_CPP_SIMULATION_MAUSRunAction_H

#include "globals.hh"
#include "G4UserRunAction.hh"


namespace MAUS {

/** @class MAUSRunAction
 *
 *  @brief Actions to run at start and end of every event
 *
 *  At the moment just does some visualisation refresh
 *
 *  Nb: nothing to test here so I don't have a unit test
 */
class MAUSRunAction : public G4UserRunAction
{
  public:
    /** Constructor does nothing */
    MAUSRunAction() {;}
    /** Destructor does nothing */
    virtual ~MAUSRunAction() {;}

    /** Flushes the visualisation scene */
    virtual void BeginOfRunAction(const G4Run* aRun);

    /** Does nothing */
    virtual void EndOfRunAction(const G4Run* aRun) {}

};

}
#endif

