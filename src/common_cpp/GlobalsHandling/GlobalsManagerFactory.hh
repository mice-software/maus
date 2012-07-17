/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#ifndef _SRC_COMMON_CPP_GLOBALSMANAGERFACTORY_HH_
#define _SRC_COMMON_CPP_GLOBALSMANAGERFACTORY_HH_

#include <string>

namespace MAUS {

/** GlobalsManagerFactory initialises the GlobalsManager
 *
 *  I want to keep it so that the GlobalsManager is quite a low level object -
 *  just handing pointers around. But some objects, e.g. MAUSGeant4Manager, are
 *  quite high level objects. The only time I need to know about them is during
 *  instantiation, so I make a high level Factory class to instantiate them.
 *
 *  Could probably be a namespace, but I make it a class just in case I need in
 *  the future some member data (occasionally happens).
 *
 *  Friend of GlobalsManager.
 *
 *  Tested with GlobalsManager.
 */

class GlobalsManagerFactory {
  public:
    /** Initialise the GlobalsManager.
     *
     *  This initialises the singleton instance or throws an exception if it is
     *  already initialised (as we can initialise with the wrong configuration).
     *
     *  Nb: this should be initialised in Go (somewhere early in the MAUS
     *  setup)
     */
    static void InitialiseGlobalsManager(std::string json_datacards);


    /** Deallocate memory and set singleton instance to NULL
     *
     *  It is possible to delete the GlobalsManager and then reinitialise
     *  with different datacards - but devs should never need to do this (please
     *  don't unless you know what you are doing).
     */
    static void DeleteGlobalsManager();

  private:
    // disallow copy and assign
    GlobalsManagerFactory();
    ~GlobalsManagerFactory();
    GlobalsManagerFactory(const GlobalsManagerFactory& glob);
    GlobalsManagerFactory& operator=(const GlobalsManagerFactory& glob);

};
}

#endif //  _SRC_COMMON_CPP_GLOBALSMANAGERFACTORY_HH_

