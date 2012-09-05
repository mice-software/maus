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
 * \file IOutput.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the interface class for all outputters.
 *
 */
#ifndef _SRC_COMMON_CPP_API_IOUTPUT_
#define _SRC_COMMON_CPP_API_IOUTPUT_
#include "src/common_cpp/API/IModule.hh"

namespace MAUS {

  /*!
   * \class IOutput
   *
   * \brief Interface class for all outputters
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template<typename T>
  class IOutput : public virtual IModule {
  public:
    // ! Destructor
    virtual ~IOutput() {}

  public:
    /*!\brief Save data
     *
     * Pure virtual function to save the input spill data
     * and return the boolean status
     * \param T* Pointer to the input data
     * \return boolean save status
     */
    virtual bool save_spill(T t) = 0;

    /*!\brief Save data
     *
     * Pure virtual function to save the input header
     * \param T* Pointer to the input data
     * \return boolean save status - true if successful save
     */
    virtual bool save_job_header(T t) = 0;
  };
}// end of namespace
#endif
