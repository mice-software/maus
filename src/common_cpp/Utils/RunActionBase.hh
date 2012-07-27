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

#ifndef _SRC_COMMON_CPP_GLOBALSHANDLING_RUNACTIONBASE_HH_
#define _SRC_COMMON_CPP_GLOBALSHANDLING_RUNACTIONBASE_HH_

namespace MAUS {

class RunHeader;
class RunFooter;

/** Base class for interface to RunActionManager
 *
 *  Provides an interface for performing set up tasks needed at the start of run
 *  e.g. setting up field maps, getting calibrations.
 *
 *  Note that the StartOfRun object will be written to the output once per
 *  process in multiprocessing mode.
 */
class RunActionBase {
  public:

    /** Constructor (does nothing) */
    RunActionBase() {}

    /** Copy Constructor (does nothing) */
    RunActionBase(const RunActionBase& datum) {}

    /** Destructor (does nothing) */
    virtual ~RunActionBase() {}

    /** Clone method is like a copy cosntructor but copies the child object */
    virtual RunActionBase* Clone() = 0;

    /** Do something at the start of run
     *
     *  Perform some operation at the start of the run, like updating a field
     *  map or getting a calibration
     *
     *  Note RunHeader is specifically non-const here. It would be nice to give
     *  some output in the datastructure so user can check e.g. what field map
     *  was used in a given reconstruction, what calibrations were in place, etc
     *  so in general RunAction should edit the run_header.
     *
     *  One RunHeader is made per process in multiprocessing mode.
     */
    virtual void StartOfRun(RunHeader* run_header) = 0;

    /** Do something at the end of run
     *
     *  Perform some operation at the end of the run, like clearing a buffer.
     */
    virtual void EndOfRun(RunFooter* run_footer) = 0;

  private:
};
}

#endif
