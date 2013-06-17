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

#ifndef SRC_COMMON_CPP_UTILS_CONVERTBINARYTOSTRING_HH_
#define SRC_COMMON_CPP_UTILS_CONVERTBINARYTOSTRING_HH_

#include <string>

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

namespace MAUS {
namespace Utils {

class ConvertBinaryToString {
  public:
    ConvertBinaryToString();

    ~ConvertBinaryToString();

    std::string convert(std::string file_name, bool delete_file);

  private:
    void reset();
    void clear();

    PyObject* _convert_func;
    PyObject* _utils_mod;
    PyObject* _utils_mod_dict;
};

}
}

#endif

