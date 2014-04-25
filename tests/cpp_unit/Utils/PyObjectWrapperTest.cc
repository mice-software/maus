/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

#include "gtest/gtest.h"
#include "json/json.h"
/*
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"

namespace MAUS {
// force the compiler to build conversions from all supported types...
// This does nothing, just checks we can compile the code
TEST(TestPyObjectWrapperTest, TestNullObject) {
  PyObject* args = NULL;
  EXPECT_THROW(PyObjectWrapper::unwrap_pyobject<Data>(args), Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap_pyobject<std::string>(args), Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap_pyobject<PyObject>(args), Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap_pyobject<Json::Value>(args), Exception);
}

}
*/
