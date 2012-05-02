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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_V1724_
#define _SRC_COMMON_CPP_DATASTRUCTURE_V1724_

#include "Rtypes.h"  // ROOT
#include <string>

namespace MAUS {

/** @class V1724 comment
 *
 *  @var ldc_id  <--description-->
 *  @var charge_mm  <--description-->
 *  @var equip_type  <--description-->
 *  @var channel_key  <--description-->
 *  @var charge_pm  <--description-->
 *  @var phys_event_number  <--description-->
 *  @var position_max  <--description-->
 *  @var time_stamp  <--description-->
 *  @var trigger_time_tag  <--description-->
 *  @var detector  <--description-->
 *  @var part_event_number  <--description-->
 *  @var geo  <--description-->
 *  @var pedestal  <--description-->
 *  @var channel  <--description-->
 */

class V1724 {
  public:
    /** Default constructor - initialises to 0/NULL */
    V1724();

    /** Copy constructor - any pointers are deep copied */
    V1724(const V1724& _v1724);

    /** Equality operator - any pointers are deep copied */
    V1724& operator=(const V1724& _v1724);

    /** Destructor - any member pointers are deleted */
    ~V1724();


    /** Returns LdcId */
    int GetLdcId() const;

    /** Sets LdcId */
    void SetLdcId(int ldc_id);

    /** Returns ChargeMm */
    int GetChargeMm() const;

    /** Sets ChargeMm */
    void SetChargeMm(int charge_mm);

    /** Returns EquipType */
    int GetEquipType() const;

    /** Sets EquipType */
    void SetEquipType(int equip_type);

    /** Returns ChannelKey */
    std::string GetChannelKey() const;

    /** Sets ChannelKey */
    void SetChannelKey(std::string channel_key);

    /** Returns ChargePm */
    int GetChargePm() const;

    /** Sets ChargePm */
    void SetChargePm(int charge_pm);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns PositionMax */
    int GetPositionMax() const;

    /** Sets PositionMax */
    void SetPositionMax(int position_max);

    /** Returns TimeStamp */
    int GetTimeStamp() const;

    /** Sets TimeStamp */
    void SetTimeStamp(int time_stamp);

    /** Returns TriggerTimeTag */
    int GetTriggerTimeTag() const;

    /** Sets TriggerTimeTag */
    void SetTriggerTimeTag(int trigger_time_tag);

    /** Returns Detector */
    std::string GetDetector() const;

    /** Sets Detector */
    void SetDetector(std::string detector);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);

    /** Returns Geo */
    int GetGeo() const;

    /** Sets Geo */
    void SetGeo(int geo);

    /** Returns Pedestal */
    int GetPedestal() const;

    /** Sets Pedestal */
    void SetPedestal(int pedestal);

    /** Returns Channel */
    int GetChannel() const;

    /** Sets Channel */
    void SetChannel(int channel);

  private:
    int _ldc_id;
    int _charge_mm;
    int _equip_type;
    std::string _channel_key;
    int _charge_pm;
    int _phys_event_number;
    int _position_max;
    int _time_stamp;
    int _trigger_time_tag;
    std::string _detector;
    int _part_event_number;
    int _geo;
    int _pedestal;
    int _channel;

    ClassDef(V1724, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_V1724_

