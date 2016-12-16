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
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

/*!
 * \file EnumeratorProcessors.hh
 *
 * \author Ian Taylore, University of Warwick
 * \date 2013/03/01 - First added to trunk
 *
 * Contains Processors for all Enumrators used by the global
 * reconstruction.
 */

namespace MAUS {
namespace Processor {
namespace Global {

/** Processor to convert between MAUS::DataStructure::Global::DetectorPoint
 *  and Json::numericValue
 *  @date
 */
class EnumDetectorPointProcessor
    : public ProcessorBase<MAUS::DataStructure::Global::DetectorPoint> {
  public:
    /** Convert from Json::numericValue to
     *  MAUS::DataStructure::Global::DetectorPoint
     *
     *  If json_double cannot be converted to a
     *  MAUS::DataStructure::Global::DetectorPoint, throw a Exception
     */
    virtual MAUS::DataStructure::Global::DetectorPoint*
    JsonToCpp(const Json::Value& json_int);

    /** Convert from MAUS::DataStructure::Global::DetectorPoint to
     * Json::numericValue
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::DetectorPoint& cpp_enum);

    /** Convert from MAUS::DataStructure::Global::DetectorPoint to
     * Json::numericValue passing path
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::DetectorPoint& cpp_enum,
        std::string path);
};

/** Processor to convert between MAUS::DataStructure::Global::PID and
 *  Json::numericValue
 */
class EnumPIDProcessor
    : public ProcessorBase<MAUS::DataStructure::Global::PID> {
  public:
    /** Convert from Json::numericValue to MAUS::DataStructure::Global::PID
     *
     *  If json_double cannot be converted to a
     *  MAUS::DataStructure::Global::PID, throw a Exception
     */
    virtual MAUS::DataStructure::Global::PID* JsonToCpp(
        const Json::Value& json_double);

    /** Convert from MAUS::DataStructure::Global::PID to Json::numericValue
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::PID& cpp_enum);

    /** Convert from MAUS::DataStructure::Global::PID to
     *  Json::numericValue passing path
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::PID& cpp_enum,
        std::string path);
};

/** Processor to convert between MAUS::DataStructure::Global::ChainType and
 *  Json::numericValue
 */
class EnumChainTypeProcessor
    : public ProcessorBase<MAUS::DataStructure::Global::ChainType> {
  public:
    /** Convert from Json::numericValue to MAUS::DataStructure::Global::ChainType
     *
     *  If json_double cannot be converted to a
     *  MAUS::DataStructure::Global::ChainType, throw a Exception
     */
    virtual MAUS::DataStructure::Global::ChainType* JsonToCpp(
        const Json::Value& json_double);

    /** Convert from MAUS::DataStructure::Global::ChainType to Json::numericValue
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::ChainType& cpp_enum);

    /** Convert from MAUS::DataStructure::Global::ChainType to
     *  Json::numericValue passing path
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::ChainType& cpp_enum,
        std::string path);
};

/** Processor to convert between MAUS::DataStructure::Global::ChainChildMultiplicity and
 *  Json::numericValue
 */
class EnumChainChildMultiplicityProcessor
    : public ProcessorBase<MAUS::DataStructure::Global::ChainChildMultiplicity> {
  public:
    /** Convert from Json::numericValue to MAUS::DataStructure::Global::ChainChildMultiplicity
     *
     *  If json_double cannot be converted to a
     *  MAUS::DataStructure::Global::ChainChildMultiplicity, throw a Exception
     */
    virtual MAUS::DataStructure::Global::ChainChildMultiplicity* JsonToCpp(
        const Json::Value& json_double);

    /** Convert from MAUS::DataStructure::Global::ChainChildMultiplicity to Json::numericValue
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::ChainChildMultiplicity& cpp_enum);

    /** Convert from MAUS::DataStructure::Global::ChainChildMultiplicity to
     *  Json::numericValue passing path
     */
    virtual Json::Value* CppToJson(
        const MAUS::DataStructure::Global::ChainChildMultiplicity& cpp_enum,
        std::string path);
};

} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS

#endif
