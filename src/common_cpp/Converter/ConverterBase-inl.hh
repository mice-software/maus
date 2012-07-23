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
#ifndef _SRC_COMMON_CPP_CONVERTER_CONVERTERBASE_INL_
#define _SRC_COMMON_CPP_CONVERTER_CONVERTERBASE_INL_
#include <string>
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"
#include "API/APIExceptions.hh"


namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  ConverterBase<INPUT, OUTPUT>::ConverterBase(const std::string& name) :
    IConverter<INPUT, OUTPUT>(), _classname(name) {}

  template <typename INPUT, typename OUTPUT>
  ConverterBase<INPUT, OUTPUT>::ConverterBase(const ConverterBase& cb) :
    IConverter<INPUT, OUTPUT>(), _classname(cb._classname) {}

  template <typename INPUT, typename OUTPUT>
  ConverterBase<INPUT, OUTPUT>::~ConverterBase() {}

  template <typename INPUT, typename OUTPUT>
  OUTPUT* ConverterBase<INPUT, OUTPUT>::operator()(const INPUT* i) const {
    return convert(i);
  }

  template <typename INPUT, typename OUTPUT>
  OUTPUT* ConverterBase<INPUT, OUTPUT>::convert(const INPUT* i) const {
    if (!i) { throw NullInputException(_classname); }
    OUTPUT* o = 0;
    o =  _convert(i);
    return o;
  }


}// end of namespace
#endif
