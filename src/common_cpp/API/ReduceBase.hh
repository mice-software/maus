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
 * \file ReduceBase.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the abstract base class for all reducers.
 *
 */
#ifndef _SRC_COMMON_CPP_API_REDUCEBASE_
#define _SRC_COMMON_CPP_API_REDUCEBASE_

// Python include statement insists on being first
#include <Python.h>

#include <string>
#include "src/common_cpp/API/IReduce.hh"
#include "src/common_cpp/API/ModuleBase.hh"

namespace MAUS {

  /*!
   * \class ReduceBase
   *
   * \brief Abstract base class for all reducers
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template <typename T_IN, typename T_OUT>
  class ReduceBase : public virtual IReduce<T_IN, T_OUT>, public ModuleBase {

  public:
    /*!\brief Constructor
     * \param std::string& The name of the reducer.
     */
    explicit ReduceBase(const std::string&);
    /*!\brief Copy Constructor
     * \param ReduceBase& An reducer to copy from.
     */
    ReduceBase(const ReduceBase&);
    // ! Destructor
    virtual ~ReduceBase();

  public:
    /*!\brief Reduce data
     *
     * Implementation of the interface. Wraps the _process function
     * providing additional control/checking.
     * \param T* The input data to be reduced
     * \return The reduced data
     */
    T_OUT* process(T_IN* t);

    PyObject* process_pyobj(PyObject* py_input);

  private:
    /*!\brief Reduce data
     *
     * Pure virtual private function to be implemented by the
     * derived reducer author to correctly apply the specific reduction 
     * process.
     * \param T* The input data to be reduced
     * \return The reduced data
     */
    virtual T_OUT* _process(T_IN* t) = 0;
  };

}// end of namespace

#include "src/common_cpp/API/ReduceBase-inl.hh"
#endif

