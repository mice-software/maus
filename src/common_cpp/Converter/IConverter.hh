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
 * \file IConverter.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the interface class for all converters.
 *
 */
#ifndef _SRC_COMMON_CPP_CONVERTER_ICONVERTER_
#define _SRC_COMMON_CPP_CONVERTER_ICONVERTER_

namespace MAUS {

  /*!
   * \class IConverter
   *
   * \brief Interface class for all converters
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template <typename INPUT, typename OUTPUT>
  class IConverter {
  public:
    // ! Destructor
    virtual ~IConverter() {}

  public:
    /*!\brief Convert data
     *
     * Pure virtual function to convert the input data
     * and return it as type OUTPUT*
     * \param INPUT* Pointer to the input data
     * \return output data type OUTPUT*
     */
    virtual OUTPUT* operator()(const INPUT* ) const = 0;
    /*!\brief Convert data
     *
     * reverse conversion
     */
    virtual INPUT* operator()(const OUTPUT* ) const = 0;
    /*!\brief Convert data
     *
     * Pure virtual function to convert the input data
     * and return it as type OUTPUT*
     * \param INPUT* Pointer to the input data
     * \return output data type OUTPUT*
     */
    virtual OUTPUT* convert(const INPUT* )    const = 0;
    /*!\brief Convert data
     *
     * reverse conversion
     */
    virtual INPUT* convert(const OUTPUT* )    const = 0;
  };

} // end of namespace
#endif
