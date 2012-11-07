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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_DATETIME_
#define _SRC_COMMON_CPP_DATASTRUCTURE_DATETIME_

#include <string>

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class DateTime holds time stamp information
 *
 *  Timestamps are stored in a string formatted like
 *
 *     YYYY-MM-DDTHH:MM:SS.mmmmmm
 *
 *  and DateTime checks this formatting at runtime. T is a user-defined
 *  separator character. (Chosen to match python datetime library).
 */

class DateTime {
  public:
    /** Default constructor - initialises to 0/NULL */
    DateTime();

    /** Copy constructor - any pointers are deep copied */
    DateTime(const DateTime& date_time);

    /** Equality operator - any pointers are deep copied */
    DateTime& operator=(const DateTime& date_time);

    /** Destructor - any member pointers are deleted */
    virtual ~DateTime();

    /** Get the string formatted date time */
    std::string GetDateTime() const;

    /** Set the string formatted date time
     *
     *  \param datetime time stamp stored in a string formatted like\n
     *     YYYY-MM-DDTHH:MM:SS.mmmmmm\n
     *  where T is a user defined separator.
     *
     *  Throws a Squeal if formatting is not correct
     */
    void SetDateTime(std::string datetime);

    /** Return true if the datetime stamp is correctly formatted
     *
     *  Returns true if format is like\n
     *     YYYY-MM-DDTHH:MM:SS.mmmmmm\n
     *  where T is any character. Checks months, days, etc; does not check
     *  leapyears and leapseconds - so allow 61 seconds in a minute and 29 days
     *  in February.
     */
    static bool CheckFormat(std::string datetime);

  private:
    std::string _date_time;

    // dupe of code in STLUtils
    static int ToInt(std::string int_as_string);

    static bool Check(std::string number, int minimum, int maximum_plus_one);
    static bool CheckDay(std::string day, std::string month);
    static bool CheckSeparators(std::string datetime);
    ClassDef(DateTime, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_DATETIME_

