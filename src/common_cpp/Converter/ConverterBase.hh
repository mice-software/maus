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
#ifndef _MAUS_CONVERTER_BASE_H
#define _MAUS_CONVERTER_BASE_H
#include <string>
#include "Converter/IConverter.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"
#include "API/APIExceptions.hh"


namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  class ConverterBase : public IConverter<INPUT, OUTPUT> {

  public:
    explicit ConverterBase(const std::string&);
    ConverterBase(const ConverterBase& cb);
    virtual ~ConverterBase();

  public:
    OUTPUT* operator()(const INPUT* ) const;
//     INPUT*  operator()(const OUTPUT*) const;
    OUTPUT* convert(const INPUT* )    const;
//     INPUT*  convert   (const OUTPUT*) const;

  protected:
    std::string _classname;

  private:
    virtual OUTPUT* _convert(const INPUT* ) const = 0;
//     virtual INPUT*  _convert(const OUTPUT*) const = 0;
  };

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

//   template <typename INPUT, typename OUTPUT>
//   INPUT* ConverterBase<INPUT, OUTPUT>::operator()(const OUTPUT* o) const {
//     return convert(o);
//   }

  template <typename INPUT, typename OUTPUT>
  OUTPUT* ConverterBase<INPUT, OUTPUT>::convert(const INPUT* i) const {
    if (!i) { throw NullInputException(_classname); }
    OUTPUT* o = 0;
    try {
      o =  _convert(i);
    }
    catch(Squeal& s) {
      CppErrorHandler::getInstance()->HandleSquealNoJson(s, _classname);
    }
    catch(std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch(...) {
      throw UnhandledException(_classname);
    }
    return o;
  }


//   template <typename INPUT, typename OUTPUT>
//   INPUT* ConverterBase<INPUT, OUTPUT>::convert(const OUTPUT* o) const {
//     if(!o){ throw NullInputException(_classname); }
//     try {
//       return _convert(o);
//     }
//     catch (Squeal& s) {
//       CppErrorHandler::getInstance()->HandleSquealNoJson(s, _classname);
//     }
//     catch (std::exception& e) {
//       CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
//     }
//     catch (...){
//       throw UnhandledException(_classname);
//     }
//   }

}// end of namespace
#endif
