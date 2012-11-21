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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_JOBFOOTERPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JOBFOOTERPROCESSOR_

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DateTimeProcessor.hh"

#include "src/common_cpp/DataStructure/JobFooter.hh"

namespace MAUS {

/** @class JobFooterProcessor Just handles DateTime conversions 
 */
class JobFooterProcessor : public ObjectProcessor<JobFooter> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    JobFooterProcessor();

  private:
    DateTimeProcessor _date_time_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JOBFOOTERPROCESSOR_
