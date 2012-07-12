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

/*!
 * \file ConverterBase.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the top level abstract base class for all converters.
 *
 */
#ifndef _MAUS_CONVERTER_BASE_H
#define _MAUS_CONVERTER_BASE_H
#include <string>
#include "Converter/IConverter.hh"

namespace MAUS {

  /*!
   * \class ConverterBase
   *
   * \brief Top level abstract base class for all converters
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   *
   * Templated base class taking INPUT and OUTPUT types.
   */
  template <typename INPUT, typename OUTPUT>
  class ConverterBase : public IConverter<INPUT, OUTPUT> {

  public:
    /*!\brief ConstructorBase
     * \param std::string& The name of the converter.
     */
    explicit ConverterBase(const std::string&);
    /*!\brief Copy Constructor
     * \param ConverterBase& A converter to copy from.
     */
    ConverterBase(const ConverterBase& cb);
    // ! Destructor
    virtual ~ConverterBase();

  public:
    /*!\brief Convert data
     *
     * Implementation of the interface. Wraps the convert function
     * \param INPUT* Pointer to the input data
     * \return output data type OUTPUT*
     */
    OUTPUT* operator()(const INPUT* ) const;
    /*!\brief Convert data
     *
     * Implementation of the interface. Wraps the _convert function
     * \param INPUT* Pointer to the input data
     * \return output data type OUTPUT*
     */
    OUTPUT* convert(const INPUT* )    const;

  protected:
    /*!\var std::string _classname
     * \brief The name of the derived converter class
     */
    std::string _classname;

  private:
    /*!\brief Convert data
     *
     * Pure virtual private function to be implemented by the
     * derived converter author to correctly convert the data from
     * type INPUT* to type OUTPUT*.
     * \param INPUT* Pointer to the input data
     * \return output data type OUTPUT*
     */
    virtual OUTPUT* _convert(const INPUT* ) const = 0;
  };

}// end of namespace

#include "Converter/ConverterBase-inl.hh"
#endif
