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
 * \file MapBase.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the abstract base class for all mappers.
 *
 */
#ifndef _SRC_COMMON_CPP_API_MAPBASE_
#define _SRC_COMMON_CPP_API_MAPBASE_

// Python include statement insists on being first
#include <Python.h>

#include <string>
#include "json/json.h"
#include "src/common_cpp/API/IMap.hh"
#include "src/common_cpp/API/ModuleBase.hh"

namespace MAUS {

/*!
 * \class MapBase
 *
 * \brief Abstract base class for all mappers
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 */
template <typename TYPE>
class MapBase : public virtual IMap<TYPE>, public ModuleBase {

 public:
  /*!\brief Constructor
   * \param std::string& The name of the mapper.
   */
  explicit MapBase(const std::string&);
  /*!\brief Copy Constructor
   * \param MapBase& An mapper to copy from.
   */
  MapBase(const MapBase&);
  // ! Destructor
  virtual ~MapBase();

  void process(TYPE* data) const {
    this->_process(data);
  }

  /*!\brief Process data
   *
   *  Uses PyObjectWrapper to convert py_input to TYPE; calls user defined 
   *  _process to process TYPE; wraps output to as a PyObject and returns
   *
   *  \param py_input a borrowed reference to the input data (process_pyobj does
   *         not take any ownership of the memory)
   *  \returns the processed data as a new PyObject
   */
  inline PyObject* process_pyobj(PyObject* py_input) const;

 private:
  /*!\brief Process data
   *
   * Pure virtual private function to be implemented in the child map to process

   * the input data
   * \param data The input data to be processed; this is a borrowed reference -
   *        MapBase retains ownership of memory allocated by data.
   */
  virtual void _process(TYPE* data) const = 0;

  virtual void HandleException(TYPE** data,
                               std::exception* exc,
                               std::string class_name) const;
};
}// end of namespace

#include "src/common_cpp/API/MapBase-inl.hh"
#endif

