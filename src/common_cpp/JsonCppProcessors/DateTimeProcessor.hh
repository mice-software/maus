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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_DATETIMEPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_DATETIMEPROCESSOR_

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"

#include "src/common_cpp/DataStructure/DateTime.hh"

namespace MAUS {

/** @class DateTimeProcessor Conversions for DateTime between C++ and Json 
 */

class DateTimeProcessor : public ObjectProcessor<DateTime> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    DateTimeProcessor();

  private:
    StringProcessor _string_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_DATETIMEPROCESSOR_
