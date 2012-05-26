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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTDIGITPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTDIGITPROCESSOR_

#include "src/common_cpp/DataStructure/TOFEventDigit.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFDigitProcessor.hh"

namespace MAUS {

/** @class TOFEventDigitProcessor Conversions for TOFEventDigit between C++ and Json 
 *
 *  @var _tof1_proc_proc Processor for _tof1
 *  @var _tof0_proc_proc Processor for _tof0
 *  @var _tof2_proc_proc Processor for _tof2
 */

class TOFEventDigitProcessor : public ObjectProcessor<TOFEventDigit> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    TOFEventDigitProcessor();

  private:
    ValueArrayProcessor<TOFDigit> _tof1_proc;
    ValueArrayProcessor<TOFDigit> _tof0_proc;
    ValueArrayProcessor<TOFDigit> _tof2_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TOFEVENTDIGITPROCESSOR_

