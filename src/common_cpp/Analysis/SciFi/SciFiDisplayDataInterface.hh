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

/** @class SciFiDisplayDataInterface
 *
 *  Class template allowing SciFiDisplay classes easier access to
 *  SciFiData classes.
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYDATAINTERFACE_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYDATAINTERFACE_

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDataBase.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayBase.hh"

namespace MAUS {

template <class T>
class SciFiDisplayDataInterface : public SciFiDisplayBase {
  public:
    /** Default constructor */
    SciFiDisplayDataInterface() {}

    /** Destructor */
    virtual ~SciFiDisplayDataInterface() {}

    /** Return a pointer to the SciFiData object associated with the display */
    virtual T* GetDataTemplate();

    /** Create a new SciFiData object of the correct derived type */
    virtual T* MakeDataObjectTemplate();

    /** Set the SciFiData object associated with the display */
    SciFiDataBase* SetDataTemplate(SciFiDataBase* data);
};

template <class T>
T* SciFiDisplayDataInterface<T>::GetDataTemplate() {
  return dynamic_cast<T*>(mSpillData);
}

template <class T>
T* SciFiDisplayDataInterface<T>::MakeDataObjectTemplate() {
  T* lData = new T();
  return lData;
}

template <class T>
SciFiDataBase* SciFiDisplayDataInterface<T>::SetDataTemplate(SciFiDataBase* data) {
  if ( dynamic_cast<T*>(data) ) {
    mSpillData = data;
    return mSpillData;
  } else {
    return NULL;
  }
}
}

#endif
