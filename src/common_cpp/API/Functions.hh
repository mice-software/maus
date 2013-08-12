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

#ifndef _SRC_COMMON_CPP_API_FUNCTIONS_HH_
#define _SRC_COMMON_CPP_API_FUNCTIONS_HH_

#include <string>
#include "json/json.h"
#include "src/common_cpp/DataStructure/Data.hh"

// Inline functions to return strings for the known OUTPUT types, or
// undefined for an unknown type.
template <typename T>
inline std::string format_output() { return "Undefined"; }

template <>
inline std::string format_output<MAUS::Data>() { return "MAUS::Data"; }

template <>
inline std::string format_output<Json::Value>() { return "Json::Value"; }

#endif
