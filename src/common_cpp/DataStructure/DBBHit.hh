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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_DBBHIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_DBBHIT_

#include <iostream>

#include "Utils/VersionNumber.hh"

namespace MAUS {

class DBBHit {
  public:
    /** Default constructor - initialises to 0/NULL */
    DBBHit();

    /** Copy constructor - any pointers are deep copied */
    DBBHit(const DBBHit& _dbbhit);

    /** Equality operator - any pointers are deep copied */
    DBBHit& operator=(const DBBHit& _dbbhit);

    /** Destructor - any member pointers are deleted */
    virtual ~DBBHit();

    /** Returns LDC */
    int GetLDC() const;

    /** Sets Channel */
    void SetLDC(int ch);

    /** Returns GEO */
    int GetGEO() const;

    /** Sets Channel */
    void SetGEO(int ch);

    /** Returns Channel */
    int GetChannel() const;

    /** Sets Channel */
    void SetChannel(int ch);

    /** Returns leading edge time */
    unsigned int GetLTime() const;

    /** Returns trailing edge time */
    unsigned int GetTTime() const;

    /** Sets leading edge times */
    void SetLTime(unsigned int t);

    /** Sets trailing edge times */
    void SetTTime(unsigned int t);

    void print() {
      std::cout << "*** Hit DBB" << _geo << " ***\n  ch: " <<_channel << "\n  le: " << GetLTime()
                << "\n  te: " << GetTTime() << std::endl;
    }
  private:

    int _channel;
    int _geo;
    int _ldc;

    unsigned int _leading_edge_time;
    unsigned int _trailing_edge_time;

    MAUS_VERSIONED_CLASS_DEF(DBBHit)
};
}

#endif   //  _SRC_COMMON_CPP_DATASTRUCTURE_DBBHIT_

