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

