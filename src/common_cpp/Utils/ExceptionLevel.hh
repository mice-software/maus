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

#ifndef SRC_COMMON_CPP_UTILS_EXCEPTIONLEVEL_HH
#define SRC_COMMON_CPP_UTILS_EXCEPTIONLEVEL_HH

namespace MAUS {
namespace Exceptions {

  /** ExceptionHandling::ExceptionLevel enumerates the severity of the exception.
   *
   *  I use an enumeration to distinguish between different error levels.
   *  - nonRecoverable means the internal state of the programme is no longer
   *    well-defined i.e. some memory problem or so.
   *  - recoverable means that in principle we could keep on running, although
   *    most of the time this results in end of run (usually indicates typo in
   *    an input file).
   *  If we start doing GUIs, then the distinction becomes important. Most stuff
   *  should be recoverable.
   */
  enum exceptionLevel {recoverable, nonRecoverable};
}
}

#endif
