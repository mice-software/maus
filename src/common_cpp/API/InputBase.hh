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
 * \file InputBase.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the abstract base class for all inputters.
 *
 */
#ifndef _MAUS_API_INPUT_BASE_H
#define _MAUS_API_INPUT_BASE_H
#include <string>
#include "API/IInput.hh"
#include "API/ModuleBase.hh"

namespace MAUS {

  /*!
   * \class InputBase
   *
   * \brief Abstract base class for all inputters
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template <typename T>
  class InputBase : public virtual IInput<T>, public ModuleBase {

  public:
    /*!\brief Constructor
     * \param std::string& The name of the inputter.
     */
    explicit InputBase(const std::string&);
    /*!\brief Copy Constructor
     * \param InputBase& An inputter to copy from.
     */
    InputBase(const InputBase&);
    // ! Destructor
    virtual ~InputBase();

  public:
    /*!\brief Generate data
     *
     * Implementation of the interface. Wraps the _emitter function
     * providing additional control/checking.
     * \return The data generated
     */
    T* emitter();

  private:
    /*!\brief Generate data
     *
     * Pure virtual private function to be implemented by the
     * derived inputter author to correctly generate the input data.
     * \return The data generated
     */
    virtual T* _emitter() = 0;
  };

}// end of namespace

#include "API/InputBase-inl.hh"
#endif

