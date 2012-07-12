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
#ifndef _MAUS_CONVERTER_FACTORY_H
#define _MAUS_CONVERTER_FACTORY_H
#include <iostream>
#include "Converter/IConverter.hh"
#include "Converter/ConverterExceptions.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"
#include "Converter/DataConverters/JsonCppConverter.hh"
#include "Converter/DataConverters/CppJsonConverter.hh"

namespace MAUS {

  class ConverterFactory {
  public:
    template <typename INPUT, typename OUTPUT>
    IConverter<INPUT, OUTPUT>* getConverter() const throw(ConverterNotFoundException);

    template <typename INPUT, typename OUTPUT>
    OUTPUT* convert(INPUT* i) const throw(NullInputException, UnhandledException);
  };

} // end of namespace

#include "Converter/ConverterFactory-inl.hh"
#endif
