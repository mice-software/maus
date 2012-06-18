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
#ifndef _MAUS_API_MODULE_BASE_H
#define _MAUS_API_MODULE_BASE_H
#include <string>
#include "API/IModule.hh"

namespace MAUS {

  class ModuleBase : public virtual IModule {

  public:
    explicit ModuleBase(const std::string&);
    ModuleBase(const ModuleBase& mb);
    virtual ~ModuleBase();

  public:
    void birth(const std::string&);
    void death();

  protected:
    std::string _classname;

  private:
    virtual void _birth(const std::string&) = 0;
    virtual void _death()                   = 0;
  };

}// end of namespace
#endif
