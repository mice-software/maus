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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_V1290_
#define _SRC_COMMON_CPP_DATASTRUCTURE_V1290_

#include <string>

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class V1290 comment
 *
 *  @var bunch_id  <--description-->
 *  @var ldc_id  <--description-->
 *  @var equip_type  <--description-->
 *  @var phys_event_number  <--description-->
 *  @var trailing_time  <--description-->
 *  @var channel_key  <--description-->
 *  @var leading_time  <--description-->
 *  @var trigger_time_tag  <--description-->
 *  @var time_stamp  <--description-->
 *  @var detector  <--description-->
 *  @var part_event_number  <--description-->
 *  @var geo  <--description-->
 *  @var channel  <--description-->
 */

class V1290 {
  public:
    /** Default constructor - initialises to 0/NULL */
    V1290();

    /** Copy constructor - any pointers are deep copied */
    V1290(const V1290& _v1290);

    /** Equality operator - any pointers are deep copied */
    V1290& operator=(const V1290& _v1290);

    /** Destructor - any member pointers are deleted */
    ~V1290();


    /** Returns BunchId */
    int GetBunchId() const;

    /** Sets BunchId */
    void SetBunchId(int bunch_id);

    /** Returns LdcId */
    int GetLdcId() const;

    /** Sets LdcId */
    void SetLdcId(int ldc_id);

    /** Returns EquipType */
    int GetEquipType() const;

    /** Sets EquipType */
    void SetEquipType(int equip_type);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns TrailingTime */
    int GetTrailingTime() const;

    /** Sets TrailingTime */
    void SetTrailingTime(int trailing_time);

    /** Returns ChannelKey */
    std::string GetChannelKey() const;

    /** Sets ChannelKey */
    void SetChannelKey(std::string channel_key);

    /** Returns LeadingTime */
    int GetLeadingTime() const;

    /** Sets LeadingTime */
    void SetLeadingTime(int leading_time);

    /** Returns TriggerTimeTag */
    int GetTriggerTimeTag() const;

    /** Sets TriggerTimeTag */
    void SetTriggerTimeTag(int trigger_time_tag);

    /** Returns TimeStamp */
    int GetTimeStamp() const;

    /** Sets TimeStamp */
    void SetTimeStamp(int time_stamp);

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

    /** Returns Channel */
    int GetChannel() const;

    /** Sets Channel */
    void SetChannel(int channel);

  private:
    int _bunch_id;
    int _ldc_id;
    int _equip_type;
    int _phys_event_number;
    int _trailing_time;
    std::string _channel_key;
    int _leading_time;
    int _trigger_time_tag;
    int _time_stamp;
    std::string _detector;
    int _part_event_number;
    int _geo;
    int _channel;

    ClassDef(V1290, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_V1290_

