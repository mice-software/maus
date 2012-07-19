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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_RUNHEADER_
#define _SRC_COMMON_CPP_DATASTRUCTURE_RUNHEADER_

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class RunHeader holds header information for a given data run
 *
 *  At the moment just holds the run number...
 */

class RunHeader {
  public:
    /** Default constructor - initialises to 0/NULL */
    RunHeader();

    /** Copy constructor - any pointers are deep copied */
    RunHeader(const RunHeader& _run_header);

    /** Equality operator - any pointers are deep copied */
    RunHeader& operator=(const RunHeader& _run_header);

    /** Destructor - any member pointers are deleted */
    virtual ~RunHeader();

    /** Returns the run number */
    int GetRunNumber() const;

    /** Sets the run number */
    void SetRunNumber(int run_number);

  private:
    int _run_number;

    ClassDef(RunHeader, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_RUNHEADER_

