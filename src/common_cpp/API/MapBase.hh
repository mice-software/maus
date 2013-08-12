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
template <typename INPUT, typename OUTPUT>
class MapBase : public virtual IMap<INPUT, OUTPUT>, public ModuleBase {

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

 public:
  /*!\brief Process data
   *
   * Implementation of the interface. Wraps the _process function
   * providing additional control/checking.
   * \param INPUT* The input data to be processed
   * \return The processed data
   */
  OUTPUT* process(INPUT* i) const;

  /*!\brief Process data
   *
   * Templated function that uses the converter suite to automatically
   * attempt to convert the data (OTHER*) into the expected (INPUT*) type
   * that the mapper expects. If this succeeds then the untemplated version
   * is called on the conversion output to perform the necessary processing. 
   * \param OTHER* The input data to be converted and processed
   * \return The processed data
   */
  template <typename OTHER> OUTPUT* process(OTHER* o) const;

  /*!\brief Process data
   *
   * Templated function that calls the OUTPUT* process(OTHER* o)
   * function, and then creates a Python Object to hold the result.
   * \param OTHER* The input data to be processed
   * \return A python container for the result
   */
  template <typename OTHER> PyObject* process_pyobj(OTHER* o) const;

  /*!\brief Set input format
   */
  void set_input(std::string);

  /*!\brief Get input format
   */
  std::string get_input();

  /*!\brief Get output format
   */
  std::string get_output();

 private:
  /*!\brief Process data
   *
   * Pure virtual private function to be implemented by the
   * derived map author to correctly process the input data
   * \param INPUT* The input data to be processed
   * \return The processed data
   */
  virtual OUTPUT* _process(INPUT* i) const = 0;

  /// Variable setting the input format the Mapper has been told to receive.
  std::string _input_format;
};

}// end of namespace

#include "src/common_cpp/API/MapBase-inl.hh"
#endif

