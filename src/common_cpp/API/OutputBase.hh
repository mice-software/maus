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
 * \file OutputBase.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the abstract base class for all outputters.
 *
 */
#ifndef _SRC_COMMON_CPP_API_OUTPUTBASE_
#define _SRC_COMMON_CPP_API_OUTPUTBASE_

#include <Python.h>

#include <string>
#include "src/common_cpp/API/IOutput.hh"
#include "src/common_cpp/API/ModuleBase.hh"

namespace MAUS {

/*!
 * \class OutputBase
 *
 * \brief Abstract base class for all outputters
 */
class OutputBase : public ModuleBase {
  public:
    /*!\brief Constructor
     * \param std::string& The name of the outputter.
     */
    explicit OutputBase(const std::string&);
    /*!\brief Copy Constructor
     * \param OutputBase& An outputter to copy from.
     */
    OutputBase(const OutputBase&);
    // ! Destructor
    virtual ~OutputBase();

  public:
    /** Save PyObject data */
    PyObject* save_pyobj(PyObject* data_in);

  private:
    /*!\brief Save data
     *
     * Pure virtual private function to be implemented by the
     * derived outputter author to correctly save the input spill data type.
     * \param PyObject* data_in to the input data
     * As we don't know the type of data_in, we have to unwrap the PyObject in
     * the derived object. Makes it a bit more complicated for user, compared to
     * other modules; but we need to handle e.g. JobHeader, Spill, Run Header, 
     * etc ... 
     * \return boolean save status
     */
    virtual bool _save(PyObject* data_in) = 0;
};
}// end of namespace

#include "src/common_cpp/API/OutputBase-inl.hh"
#endif
