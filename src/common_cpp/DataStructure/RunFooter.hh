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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_RUNFOOTER_
#define _SRC_COMMON_CPP_DATASTRUCTURE_RUNFOOTER_

#include "src/common_cpp/Utils/VersionNumber.hh"


namespace MAUS {

/** @class RunFooter holds footer information for a given data run
 *
 *  At the moment just holds the run number...
 */

class RunFooter {
  public:
    /** Default constructor - initialises to 0/NULL */
    RunFooter();

    /** Copy constructor - any pointers are deep copied */
    RunFooter(const RunFooter& _run_footer);

    /** Equality operator - any pointers are deep copied */
    RunFooter& operator=(const RunFooter& _run_footer);

    /** Destructor - any member pointers are deleted */
    virtual ~RunFooter();

    /** Returns the run number */
    int GetRunNumber() const;

    /** Sets the run number */
    void SetRunNumber(int run_number);

  private:
    int _run_number;

    MAUS_VERSIONED_CLASS_DEF(RunFooter)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_RUNFOOTER_

