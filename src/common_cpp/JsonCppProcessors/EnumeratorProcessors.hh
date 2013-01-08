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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPENUMERATORPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPENUMERATORPROCESSOR_HH_

#include <string>

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/DataStructure/GlobalReconEnums.hh"

namespace MAUS {

/** Processor to convert between MAUS::recon::global::DetectorPoint
 *  and Json::numericValue
 */
class EnumDetectorPointProcessor :
      public ProcessorBase<MAUS::recon::global::DetectorPoint> {
  public:
    /** Convert from Json::numericValue to
     *  MAUS::recon::global::DetectorPoint
     *
     *  If json_double cannot be converted to a
     *  MAUS::recon::global::DetectorPoint, throw a Squeal
     */
    virtual MAUS::recon::global::DetectorPoint*
    JsonToCpp(const Json::Value& json_int);
  
    /** Convert from MAUS::recon::global::DetectorPoint to
     * Json::numericValue
     */
    virtual Json::Value* CppToJson(const MAUS::recon::global::DetectorPoint& cpp_enum);

    /** Convert from MAUS::recon::global::DetectorPoint to
     * Json::numericValue passing path
     */
    virtual Json::Value* CppToJson(const MAUS::recon::global::DetectorPoint& cpp_enum,
                                   std::string path);
};

/** Processor to convert between MAUS::recon::global::PID and Json::numericValue
 */
class EnumPIDProcessor : public ProcessorBase<MAUS::recon::global::PID> {
  public:
    /** Convert from Json::numericValue to MAUS::recon::global::PID
     *
     *  If json_double cannot be converted to a
     *  MAUS::recon::global::PID, throw a Squeal
     */
    virtual MAUS::recon::global::PID* JsonToCpp(const Json::Value& json_double);

    /** Convert from MAUS::recon::global::PID to Json::numericValue
     */
    virtual Json::Value* CppToJson(const MAUS::recon::global::PID& cpp_enum);

    /** Convert from MAUS::recon::global::PID to Json::numericValue passing path
     */
    virtual Json::Value* CppToJson(const MAUS::recon::global::PID& cpp_enum,
                                   std::string path);
};

}

#endif

