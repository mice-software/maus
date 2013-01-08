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

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TLorentzVectorProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EnumeratorProcessors.hh"

#include "src/common_cpp/DataStructure/GlobalSpacePoint.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALSPACEPOINTPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALSPACEPOINTPROCESSOR_HH_

namespace MAUS {

/** @class GlobalSpacePointProcessor processor for
 *  MAUS::recon::global::SpacePoint */
class GlobalSpacePointProcessor :
      public ObjectProcessor<MAUS::recon::global::SpacePoint> {
 public:
  /** Constructor - registers the branch structure */
  GlobalSpacePointProcessor();

 private:
  EnumDetectorPointProcessor _detector_enum_proc;
  DoubleProcessor _double_proc;
  TLorentzVectorProcessor _tlorentz_vec_proc;
  StringProcessor _string_proc;
};
} // ~namespace MAUS

#endif
