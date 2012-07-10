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
#ifndef _MAUS_API_MAP_BASE_H
#define _MAUS_API_MAP_BASE_H
#include <string>
#include "API/IMap.hh"
#include "API/ModuleBase.hh"
#include "json/json.h"

namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  class MapBase : public virtual IMap<INPUT, OUTPUT>, public ModuleBase {

  public:
    explicit MapBase(const std::string&);
    MapBase(const MapBase&);
    virtual ~MapBase();

  public:
    OUTPUT* process(INPUT* i) const;
    template <typename OTHER> OUTPUT* process(OTHER* o) const;

  private:
    virtual OUTPUT* _process(INPUT* i) const = 0;
  };

  template <typename OUTPUT>
  class MapBase<Json::Value, OUTPUT> : public virtual IMap<Json::Value, OUTPUT>, public ModuleBase {

  public:
    explicit MapBase(const std::string&);
    MapBase(const MapBase&);
    virtual ~MapBase();

  public:
    OUTPUT* process(Json::Value*) const;
    template <typename OTHER> OUTPUT* process(OTHER* o) const;

  private:
    virtual OUTPUT* _process(Json::Value*) const = 0;
  };

}// end of namespace

#include "API/MapBase-inl.hh"
#endif

