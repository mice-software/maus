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
#ifndef _SRC_COMMON_CPP_CONVERTER_CONVERTEREXCEPTIONS_
#define _SRC_COMMON_CPP_CONVERTER_CONVERTEREXCEPTIONS_

#include <string>
#include "gtest/gtest_prod.h"
#include "API/MAUSExceptionBase.hh"

namespace MAUS {

  class ConverterNotFoundException: public MAUSExceptionBase {
  public:
    explicit ConverterNotFoundException(const std::string& classname) :
      MAUSExceptionBase(classname) {}

    virtual ~ConverterNotFoundException() throw() {}

  private:
    virtual const char* _what() const throw() {
      return "The required converter was not found";
    }
    FRIEND_TEST(ConverterExceptionsTest, TestConverterNotFoundExceptionConstructor);
    FRIEND_TEST(ConverterExceptionsTest, TestConverterNotFoundException_What);
  };

}// end of namespace
#endif
