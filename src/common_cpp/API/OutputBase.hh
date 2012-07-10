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
#ifndef _MAUS_API_OUTPUT_BASE_H
#define _MAUS_API_OUTPUT_BASE_H
#include <string>
#include "API/IOutput.hh"
#include "API/ModuleBase.hh"

namespace MAUS {

  /*!
   * \class OutputBase
   *
   * \brief Abstract base class for all outputters
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template <typename T>
  class OutputBase : public virtual IOutput<T>, public ModuleBase {

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
    /*!\brief Save data
     *
     * Implementation of the interface. Wraps the _save function
     * providing additional control/checking.
     * \param T* Pointer to the input data
     * \return boolean save status
     */
    bool save(T* t);

  private:
    /*!\brief Save data
     *
     * Pure virtual private function to be implemented by the
     * derived outputter author to correctly save the input data type.
     * \param T* Pointer to the input data
     * \return boolean save status
     */
    virtual bool _save(T* t) = 0;
  };

}// end of namespace

#include "API/OutputBase-inl.hh"
#endif
