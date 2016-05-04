
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
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRCHANNELID_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRCHANNELID_HH_

#include "Rtypes.h" // ROOT

namespace MAUS {

/** @class EMRChannelId
 *  @brief Identifier for the channel where a hit was recorded in the EMR (for MC)
 *  @var bar	<-- Global bar identifier (0->2831: 48 planes, 59 bars per plane) -->
 */
class EMRChannelId {
  public:
    /** @brief Constructor - allocate to 0 */
    EMRChannelId()
      :_bar(-1) {}

    /** @brief Copy Constructor - copies data from channel*/
    EMRChannelId(const EMRChannelId& channel);

    /** @brief Equality operator - copies data from channel */
    EMRChannelId& operator=(const EMRChannelId& channel);

    /** @brief Destructor (does nothing)*/
    virtual ~EMRChannelId() {}

    /** @brief Returns the bar that was hit */
    int GetBar() const  {return _bar;}

    /** @brief Sets the bar that was hit */
    void SetBar(int bar) {_bar = bar;}

  private:
    int _bar;

    ClassDef(EMRChannelId, 1)
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMRCHANNELID_HH_
