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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_V830_
#define _SRC_COMMON_CPP_DATASTRUCTURE_V830_

#include "Rtypes.h"  // ROOT
#include "src/common_cpp/DataStructure/Channels.hh"

namespace MAUS {

/** @class V830 comment
 *
 *  @var ldc_id  <--description-->
 *  @var equip_type  <--description-->
 *  @var channels  <--description-->
 *  @var phys_event_number  <--description-->
 *  @var time_stamp  <--description-->
 *  @var geo  <--description-->
 */

class V830 {
  public:
    /** Default constructor - initialises to 0/NULL */
    V830();

    /** Copy constructor - any pointers are deep copied */
    V830(const V830& _v830);

    /** Equality operator - any pointers are deep copied */
    V830& operator=(const V830& _v830);

    /** Destructor - any member pointers are deleted */
    ~V830();


    /** Returns LdcId */
    int GetLdcId() const;

    /** Sets LdcId */
    void SetLdcId(int ldc_id);

    /** Returns EquipType */
    int GetEquipType() const;

    /** Sets EquipType */
    void SetEquipType(int equip_type);

    /** Returns Channels */
    Channels GetChannels() const;

    /** Sets Channels */
    void SetChannels(Channels channels);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns TimeStamp */
    int GetTimeStamp() const;

    /** Sets TimeStamp */
    void SetTimeStamp(int time_stamp);

    /** Returns Geo */
    int GetGeo() const;

    /** Sets Geo */
    void SetGeo(int geo);

  private:
    int _ldc_id;
    int _equip_type;
    Channels _channels;
    int _phys_event_number;
    int _time_stamp;
    int _geo;

    ClassDef(V830, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_V830_

