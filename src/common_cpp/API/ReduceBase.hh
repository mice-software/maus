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
#ifndef _MAUS_API_REDUCE_BASE_H
#define _MAUS_API_REDUCE_BASE_H
#include <string>
#include "API/IReduce.hh"
#include "API/ModuleBase.hh"

namespace MAUS {

  template <typename T>
  class ReduceBase : public virtual IReduce<T>, public ModuleBase {

  public:
    explicit ReduceBase(const std::string&);
    ReduceBase(const ReduceBase&);
    virtual ~ReduceBase();

  public:
    T* process(T* t);

  private:
    virtual T* _process(T* t) = 0;
  };

}// end of namespace

#include "API/ReduceBase-inl.hh"
#endif

