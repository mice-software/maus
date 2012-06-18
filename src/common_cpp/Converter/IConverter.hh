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
#ifndef _MAUS_ICONVERTER_H
#define _MAUS_ICONVERTER_H

namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  class IConverter {
  public:
    virtual ~IConverter() {}

  public:
    virtual OUTPUT* operator()(const INPUT* ) const = 0;
//     virtual INPUT*  operator()(const OUTPUT*) const = 0;
    virtual OUTPUT* convert(const INPUT* )    const = 0;
//     virtual INPUT*  convert   (const OUTPUT*) const = 0;
  };

} // end of namespace
#endif
