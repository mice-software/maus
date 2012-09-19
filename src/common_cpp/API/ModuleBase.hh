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
 * \file ModuleBase.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the top level abstract base class for all modules.
 *
 */
#ifndef _SRC_COMMON_CPP_API_MODULEBASE_
#define _SRC_COMMON_CPP_API_MODULEBASE_
#include <string>
#include "src/common_cpp/API/IModule.hh"

namespace MAUS {

  /*!
   * \class ModuleBase
   *
   * \brief Top level abstract base class for all module types
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  class ModuleBase : public virtual IModule {

  public:
    /*!\brief Constructor
     * \param std::string& The name of the module.
     */
    explicit ModuleBase(const std::string&);
    /*!\brief Copy Constructor
     * \param ModuleBase& A Module to copy from.
     */
    ModuleBase(const ModuleBase& mb);
    // ! Destructor
    virtual ~ModuleBase();

  public:
    /*!\brief Module initialisation
     *
     * Implementation of the interface. Wraps the _birth function
     * providing additional control/checking.
     * \param std::string& the configuration string
     */
    void birth(const std::string&);
    /*!\brief Module finalisation
     *
     * Implementation of the interface. Wraps the _birth function
     * providing additional control/checking.
     */
    void death();

  protected:
    /*!\var std::string _classname
     * \brief The name of the derived module class
     */
    std::string _classname;

    /** event types that we can read */
    enum event_type {_job_header_tp, _spill_tp};

  private:
    /*!\brief Module initialisation
     *
     * Pure virtual private function to be implemented by the
     * derived module author to correctly initialise the module.
     * \param std::string& the configuration string
     */
    virtual void _birth(const std::string&) = 0;
    /*!\brief Module finalisation
     *
     * Pure virtual private function to be implemented by the
     * derived module author to correctly finalise the module.
     */
    virtual void _death()                   = 0;
  };

}// end of namespace
#endif
