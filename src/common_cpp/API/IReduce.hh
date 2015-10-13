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
 * \file IReduce.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This is the interface class for all reducers.
 *
 */
#ifndef _SRC_COMMON_CPP_API_IREDUCE_
#define _SRC_COMMON_CPP_API_IREDUCE_
#include "src/common_cpp/API/IModule.hh"

namespace MAUS {

  /*!
   * \class IReduce
   *
   * \brief Interface class for all reducers
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   */
  template<typename T_IN, typename T_OUT>
  class IReduce : public virtual IModule {
  public:
    // ! Destructor
    virtual ~IReduce() {}

  public:
    /*!\brief Process data
     *
     * Pure virtual function to process the input data
     * and return the results
     * \param T* Pointer to the input data
     * \return T* Pointer to the output data
     */
    virtual void process(T_IN* t) = 0;
  };

}// end of namespace
#endif
