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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_DAQDATA_
#define _SRC_COMMON_CPP_DATASTRUCTURE_DAQDATA_

#include "src/common_cpp/DataStructure/KLDaq.hh"
#include "src/common_cpp/DataStructure/Trigger.hh"
#include "src/common_cpp/DataStructure/TOFDaq.hh"
#include "src/common_cpp/DataStructure/V830.hh"
#include "src/common_cpp/DataStructure/CkovDaq.hh"
#include "src/common_cpp/DataStructure/Unknown.hh"
#include "src/common_cpp/DataStructure/TriggerRequest.hh"
#include "Rtypes.h"  // ROOT
#include "src/common_cpp/DataStructure/Tag.hh"
#include <vector>

namespace MAUS {
// Needed for ROOT
typedef std::vector<KLDaq> KLArray;
typedef std::vector<TOFDaq> TOF2Array;
typedef std::vector<TOFDaq> TOF1Array;
typedef std::vector<Trigger> TriggerArray;
typedef std::vector<CkovDaq> CkovArray;
typedef std::vector<TriggerRequest> TriggerRequestArray;
typedef std::vector<Unknown> UnknownArray;
typedef std::vector<TOFDaq> TOF0Array;
typedef std::vector<Tag> TagArray;

/** @class DAQData comment
 *
 *  @var V830  <--description-->
 *  @var trigger_request  <--description-->
 *  @var tof1  <--description-->
 *  @var ckov  <--description-->
 *  @var tof2  <--description-->
 *  @var tof0  <--description-->
 *  @var kl  <--description-->
 *  @var tag  <--description-->
 *  @var unknown  <--description-->
 *  @var trigger  <--description-->
 */

class DAQData {
  public:
    /** Default constructor - initialises to 0/NULL */
    DAQData();

    /** Copy constructor - any pointers are deep copied */
    DAQData(const DAQData& _daqdata);

    /** Equality operator - any pointers are deep copied */
    DAQData& operator=(const DAQData& _daqdata);

    /** Destructor - any member pointers are deleted */
    ~DAQData();


    /** Returns V830 */
    V830 GetV830() const;

    /** Sets V830 */
    void SetV830(V830 V830);

    /** Returns TriggerRequestArray */
    TriggerRequestArray GetTriggerRequestArray() const;

    /** Get an element from TriggerRequestArray (needed for PyROOT) */
    TriggerRequest GetTriggerRequestArrayElement(size_t index) const;

    /** Get size of TriggerRequestArray (needed for PyROOT) */
    size_t GetTriggerRequestArraySize() const;

    /** Sets TriggerRequestArray */
    void SetTriggerRequestArray(TriggerRequestArray trigger_request);

    /** Returns TOF1Array */
    TOF1Array GetTOF1Array() const;

    /** Get an element from TOF1Array (needed for PyROOT) */
    TOFDaq GetTOF1ArrayElement(size_t index) const;

    /** Get size of TOF1Array (needed for PyROOT) */
    size_t GetTOF1ArraySize() const;

    /** Sets TOF1Array */
    void SetTOF1Array(TOF1Array tof1);

    /** Returns CkovArray */
    CkovArray GetCkovArray() const;

    /** Get an element from CkovArray (needed for PyROOT) */
    CkovDaq GetCkovArrayElement(size_t index) const;

    /** Get size of CkovArray (needed for PyROOT) */
    size_t GetCkovArraySize() const;

    /** Sets CkovArray */
    void SetCkovArray(CkovArray ckov);

    /** Returns TOF2Array */
    TOF2Array GetTOF2Array() const;

    /** Get an element from TOF2Array (needed for PyROOT) */
    TOFDaq GetTOF2ArrayElement(size_t index) const;

    /** Get size of TOF2Array (needed for PyROOT) */
    size_t GetTOF2ArraySize() const;

    /** Sets TOF2Array */
    void SetTOF2Array(TOF2Array tof2);

    /** Returns TOF0Array */
    TOF0Array GetTOF0Array() const;

    /** Get an element from TOF0Array (needed for PyROOT) */
    TOFDaq GetTOF0ArrayElement(size_t index) const;

    /** Get size of TOF0Array (needed for PyROOT) */
    size_t GetTOF0ArraySize() const;

    /** Sets TOF0Array */
    void SetTOF0Array(TOF0Array tof0);

    /** Returns KLArray */
    KLArray GetKLArray() const;

    /** Get an element from KLArray (needed for PyROOT) */
    KLDaq GetKLArrayElement(size_t index) const;

    /** Get size of KLArray (needed for PyROOT) */
    size_t GetKLArraySize() const;

    /** Sets KLArray */
    void SetKLArray(KLArray kl);

    /** Returns TagArray */
    TagArray GetTagArray() const;

    /** Get an element from TagArray (needed for PyROOT) */
    Tag GetTagArrayElement(size_t index) const;

    /** Get size of TagArray (needed for PyROOT) */
    size_t GetTagArraySize() const;

    /** Sets TagArray */
    void SetTagArray(TagArray tag);

    /** Returns UnknownArray */
    UnknownArray GetUnknownArray() const;

    /** Get an element from UnknownArray (needed for PyROOT) */
    Unknown GetUnknownArrayElement(size_t index) const;

    /** Get size of UnknownArray (needed for PyROOT) */
    size_t GetUnknownArraySize() const;

    /** Sets UnknownArray */
    void SetUnknownArray(UnknownArray unknown);

    /** Returns TriggerArray */
    TriggerArray GetTriggerArray() const;

    /** Get an element from TriggerArray (needed for PyROOT) */
    Trigger GetTriggerArrayElement(size_t index) const;

    /** Get size of TriggerArray (needed for PyROOT) */
    size_t GetTriggerArraySize() const;

    /** Sets TriggerArray */
    void SetTriggerArray(TriggerArray trigger);

  private:
    V830 _V830;
    TriggerRequestArray _trigger_request;
    TOF1Array _tof1;
    CkovArray _ckov;
    TOF2Array _tof2;
    TOF0Array _tof0;
    KLArray _kl;
    TagArray _tag;
    UnknownArray _unknown;
    TriggerArray _trigger;

    ClassDef(DAQData, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_DAQDATA_

