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
 * \file IMap.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the interface class for all maps.
 *
 */
#ifndef _MAUS_API_IMAP_H
#define _MAUS_API_IMAP_H
#include "API/IModule.hh"

namespace MAUS {

  /*!
   * \class IMap
   *
   * \brief Interface class for all maps
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template <typename INPUT, typename OUTPUT>
  class IMap : public virtual IModule {
  public:
    // ! Destructor
    virtual ~IMap() {}

  public:
    /*!\brief Process data
     *
     * Pure virtual function to process the input data
     * and return the results
     * \param INPUT* Pointer to the input data
     * \return OUTPUT* Pointer to the output data
     */
    virtual OUTPUT* process(INPUT* i) const = 0;
  };

}// end of namespace
#endif
