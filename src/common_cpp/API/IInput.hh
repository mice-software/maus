/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 * \file IInput.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the interface class for all inputters.
 *
 */
#ifndef _SRC_COMMON_CPP_API_IINPUT_
#define _SRC_COMMON_CPP_API_IINPUT_
#include "src/common_cpp/API/IModule.hh"

namespace MAUS {

  /*!
   * \class IInput
   *
   * \brief Interface class for all inputters
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template<typename T>
  class IInput : public virtual IModule {
  public:
    // ! Destructor
    virtual ~IInput() {}

  public:
    /*!\brief Generate data
     *
     * Pure virtual function to generate data
     * \return The data generated
     */
    virtual T emitter_cpp() = 0;

    /*!\brief Load the job header
     *
     * Pure virtual function to generate data
     * \return The data generated
     */
    virtual T load_job_header() = 0;
  };

}// end of namespace
#endif
