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
 * \file IModule.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the top level interface for all modules.
 *
 */
#ifndef _MAUS_API_IMODULE_H
#define _MAUS_API_IMODULE_H
#include <string>

namespace MAUS {

  /*!
   * \class IModule
   *
   * \brief Top level interface for all modules
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  class IModule {
  public:
    // ! Destructor
    virtual ~IModule() {}

  public:
    /*!\brief Module initialisation
     *
     * Pure virtual function to initialise the module
     * \param std::string& the configuration string
     */
    virtual void birth(const std::string&) = 0;
    /*!\brief Module finalisation
     *
     * Pure virtual function to finalise the module
     */
    virtual void death()                   = 0;
  };

}// end of namespace
#endif
