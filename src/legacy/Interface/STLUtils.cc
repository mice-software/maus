// MAUS WARNING: THIS IS LEGACY CODE.
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

#include "Interface/STLUtils.hh"
#include "Utils/Exception.hh"

std::string STLUtils::ReplaceVariables(std::string fileName) {
  unsigned int pos = 0;
  std::string fullName;

  while (pos < fileName.size()) {
      if (fileName[pos] == '$') {
          ++pos;
          if (fileName[pos] == '{') {
              ++pos;
          } else {
              throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                "Error - environment variable must be written like ${VARIABLE}"+
                std::string(" in file "+fileName),
                "STLUtils::ReplaceVariables"));
          }
          int end = pos +1;
          while (fileName[end] != '}' &&
                 end < static_cast<int>(fileName.size())) {
              ++end;
          }
          if (end == static_cast<int>(fileName.size())) {
              throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                "Error - environment variable must be written like ${VARIABLE}"+
                std::string(" in file "+fileName),
                "STLUtils::ReplaceVariables"));
          }
          std::string variable;
          for (int vpos = pos; vpos < end; ++vpos) {
              variable += fileName[vpos];
          }
          if (getenv(variable.c_str()) == NULL) {
              throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                    "Error - "+variable+" environment variable was not defined",
                    "STLUtils::ReplaceVariables"));
          }
          fullName += std::string(getenv(variable.c_str()));
          pos = end + 1;
      } else {
          fullName += fileName[pos];
          ++pos;
      }
  }
  return fullName;
}


