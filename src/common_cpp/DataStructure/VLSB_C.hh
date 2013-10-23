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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_VLSB_C_
#define _SRC_COMMON_CPP_DATASTRUCTURE_VLSB_C_

#include <string>

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** @class VLSB_C comment
 *
 *  @var detector
 *  @var equip_type
 *  @var time_stamp
 *  @var phys_event_number
 *  @var bank_id
 *  @var adc
 *  @var part_event_number
 *  @var channel
 *  @var tdc
 *  @var discriminator
 *  @var ldc_id
 *  @var geo
 */

class VLSB_C {
  public:
    /** Default constructor - initialises to 0/NULL */
    VLSB_C();

    /** Copy constructor - any pointers are deep copied */
    VLSB_C(const VLSB_C& VLSB_C);

    /** Equality operator - any pointers are deep copied */
    VLSB_C& operator=(const VLSB_C& VLSB_C);

    /** Destructor - any member pointers are deleted */
    virtual ~VLSB_C();

    /** Returns EquipType */
    int GetEquipType() const;

    /** Sets EquipType */
    void SetEquipType(int equip_type);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

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

    /** Returns Channel */
    int GetChannel() const;

    /** Sets Channel */
    void SetChannel(int channel);

    int GetBankID() const;

    void SetBankID(int bank);

    int GetADC() const;

    void SetADC(int adc);

    int GetTDC() const;

    void SetTDC(int tdc);

    int GetDiscriminator() const;

    void SetDiscriminator(int discriminator);

    int GetLdcId() const;

    void SetLdcId(int ldc_id);

    int GetGeo() const;

    void SetGeo(int geo);

  private:
    std::string _detector;
    int _equip_type;
    int _time_stamp;
    int _phys_event_number;
    int _bank_id;
    int _adc;
    int _part_event_number;
    int _channel;
    int _tdc;
    int _discriminator;
    int _ldc_id;
    int _geo;

    MAUS_VERSIONED_CLASS_DEF(VLSB_C)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_VLSB_C_

