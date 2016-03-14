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

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/KLDaq.hh"
#include "DataStructure/Trigger.hh"
#include "DataStructure/TOFDaq.hh"
#include "DataStructure/V830.hh"
#include "DataStructure/CkovDaq.hh"
#include "DataStructure/EMRDaq.hh"
#include "DataStructure/Unknown.hh"
#include "DataStructure/TriggerRequest.hh"
#include "DataStructure/Tag.hh"
#include "DataStructure/TrackerDaq.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<KLDaq*> KLArray;
typedef std::vector<CkovDaq*> CkovArray;
typedef std::vector<TOFDaq*> TOF0DaqArray;
typedef std::vector<Trigger*> TriggerArray;
typedef std::vector<TOFDaq*> TOF1DaqArray;
typedef std::vector<TOFDaq*> TOF2DaqArray;
typedef std::vector<TriggerRequest*> TriggerRequestArray;
typedef std::vector<Unknown*> UnknownArray;
typedef std::vector<Tag*> TagArray;
typedef std::vector<TrackerDaq*> Tracker0DaqArray;
typedef std::vector<TrackerDaq*> Tracker1DaqArray;

/** @class DAQData comment
 *
 *  @var V830  <--description-->
 *  @var trigger_request  <--description-->
 *  @var tof1  <--description-->
 *  @var ckov  <--description-->
 *  @var tof2  <--description-->
 *  @var unknown  <--description-->
 *  @var kl  <--description-->
 *  @var tag  <--description-->
 *  @var tof0  <--description-->
 *  @var trigger  <--description-->
 *  @var emr  <--description-->
 */

class DAQData {
  public:
    /** Default constructor - initialises to 0/NULL */
    DAQData();

    /** Copy constructor - any pointers are deep copied */
    DAQData(const DAQData& daqdata);

    /** Equality operator - any pointers are deep copied */
    DAQData& operator=(const DAQData& daqdata);

    /** Destructor - any member pointers are deleted */
    virtual ~DAQData();

    /** Gets Event size*/
    unsigned int GetEventSize() const;

    /** Sets Event size*/
    void SetEventSize(unsigned int size);

    /** Returns V830 */
    V830 GetV830() const;

    /** Sets V830 */
    void SetV830(V830 V830);

    /** Returns TriggerRequestArray */
    TriggerRequestArray GetTriggerRequestArray() const;

    /** Returns TriggerRequestArray pointer*/
    TriggerRequestArray* GetTriggerRequestArrayPtr();

    /** Get an element from TriggerRequestArray (needed for PyROOT) */
    TriggerRequest* GetTriggerRequestArrayElement(size_t index) const;

    /** Get size of TriggerRequestArray (needed for PyROOT) */
    size_t GetTriggerRequestArraySize() const;

    /** Sets TriggerRequestArray */
    void SetTriggerRequestArray(TriggerRequestArray trigger_request);

    /** Returns TOF1DaqArray */
    TOF1DaqArray GetTOF1DaqArray() const;

    /** Returns TOF1DaqArray pointer*/
    TOF1DaqArray* GetTOF1DaqArrayPtr();

    /** Get an element from TOF1DaqArray (needed for PyROOT) */
    TOFDaq* GetTOF1DaqArrayElement(size_t index) const;

    /** Get size of TOF1DaqArray (needed for PyROOT) */
    size_t GetTOF1DaqArraySize() const;

    /** Sets TOF1DaqArray */
    void SetTOF1DaqArray(TOF1DaqArray tof1);

    /** Returns CkovArray */
    CkovArray GetCkovArray() const;

    /** Returns CkovArray pointer*/
    CkovArray* GetCkovArrayPtr();

    /** Get an element from CkovArray (needed for PyROOT) */
    CkovDaq* GetCkovArrayElement(size_t index) const;

    /** Get size of CkovArray (needed for PyROOT) */
    size_t GetCkovArraySize() const;

    /** Sets CkovArray */
    void SetCkovArray(CkovArray ckov);

    /** Returns TOF2DaqArray */
    TOF2DaqArray GetTOF2DaqArray() const;

    /** Returns TOF2DaqArray pointer*/
    TOF2DaqArray* GetTOF2DaqArrayPtr();

    /** Get an element from TOF2DaqArray (needed for PyROOT) */
    TOFDaq* GetTOF2DaqArrayElement(size_t index) const;

    /** Get size of TOF2DaqArray (needed for PyROOT) */
    size_t GetTOF2DaqArraySize() const;

    /** Sets TOF2DaqArray */
    void SetTOF2DaqArray(TOF2DaqArray tof2);

    Tracker0DaqArray GetTracker0DaqArray() const;

    /** Returns UnknownArray pointer*/
    Tracker0DaqArray* GetTracker0DaqArrayPtr();

    TrackerDaq* GetTracker0DaqArrayElement(size_t index) const;

    size_t GetTracker0DaqArraySize() const;

    void SetTracker0DaqArray(Tracker0DaqArray tracker0);

    Tracker1DaqArray GetTracker1DaqArray() const;

    /** Returns UnknownArray pointer*/
    Tracker1DaqArray* GetTracker1DaqArrayPtr();

    TrackerDaq* GetTracker1DaqArrayElement(size_t index) const;

    size_t GetTracker1DaqArraySize() const;

    void SetTracker1DaqArray(Tracker1DaqArray tracker1);

    /** Returns UnknownArray */
    UnknownArray GetUnknownArray() const;

    /** Returns UnknownArray pointer*/
    UnknownArray* GetUnknownArrayPtr();

    /** Get an element from UnknownArray (needed for PyROOT) */
    Unknown* GetUnknownArrayElement(size_t index) const;

    /** Get size of UnknownArray (needed for PyROOT) */
    size_t GetUnknownArraySize() const;

    /** Sets UnknownArray */
    void SetUnknownArray(UnknownArray unknown);

    /** Returns KLArray */
    KLArray GetKLArray() const;

    /** Returns KLArray pointer*/
    KLArray* GetKLArrayPtr();

    /** Get an element from KLArray (needed for PyROOT) */
    KLDaq* GetKLArrayElement(size_t index) const;

    /** Get size of KLArray (needed for PyROOT) */
    size_t GetKLArraySize() const;

    /** Sets KLArray */
    void SetKLArray(KLArray kl);

    /** Returns TagArray */
    TagArray GetTagArray() const;

    /** Returns TagArray pointer*/
    TagArray* GetTagArrayPtr();

    /** Get an element from TagArray (needed for PyROOT) */
    Tag* GetTagArrayElement(size_t index) const;

    /** Get size of TagArray (needed for PyROOT) */
    size_t GetTagArraySize() const;

    /** Sets TagArray */
    void SetTagArray(TagArray tag);

    /** Returns TOF0DaqArray */
    TOF0DaqArray GetTOF0DaqArray() const;

    /** Returns TOF0DaqArray pointer*/
    TOF0DaqArray* GetTOF0DaqArrayPtr();

    /** Get an element from TOF0DaqArray (needed for PyROOT) */
    TOFDaq* GetTOF0DaqArrayElement(size_t index) const;

    /** Get size of TOF0DaqArray (needed for PyROOT) */
    size_t GetTOF0DaqArraySize() const;

    /** Sets TOF0DaqArray */
    void SetTOF0DaqArray(TOF0DaqArray tof0);

    /** Returns TriggerArray */
    TriggerArray GetTriggerArray() const;

    /** Returns TriggerArray pointer*/
    TriggerArray* GetTriggerArrayPtr();

    /** Get an element from TriggerArray (needed for PyROOT) */
    Trigger* GetTriggerArrayElement(size_t index) const;

    /** Get size of TriggerArray (needed for PyROOT) */
    size_t GetTriggerArraySize() const;

    /** Sets TriggerArray */
    void SetTriggerArray(TriggerArray trigger);

    /** Returns EMRDaqArray */
    EMRDaq GetEMRDaq() const;

    /** Returns EMRDaqArray pointer */
    EMRDaq* GetEMRDaqPtr();

    /** Sets EMRDaqArray */
    void SetEMRDaq(EMRDaq emr);

  private:
    unsigned int _event_size;

    V830 _V830;
    TriggerRequestArray  _trigger_request;
    TOF1DaqArray         _tof1;
    CkovArray            _ckov;
    TOF2DaqArray         _tof2;
    UnknownArray         _unknown;
    KLArray              _kl;
    TagArray             _tag;
    TOF0DaqArray         _tof0;
    TriggerArray         _trigger;
    EMRDaq               _emr;
    Tracker0DaqArray     _tracker0;
    Tracker1DaqArray     _tracker1;

    MAUS_VERSIONED_CLASS_DEF(DAQData)
};

// ROOT refused to build DAQData bindings when this was in the DAQData class
// I guess it doesn't like templates? So put it somewhere separate
namespace DAQDataHelper {
template <class T>
std::vector<T*> VectorDeepcopy(std::vector<T*> rhs) {
    std::vector<T*> lhs;
    for (size_t i = 0; i < rhs.size(); ++i) {
        lhs.push_back(new T(*rhs[i]));
    }
    return lhs;
}
}
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_DAQDATA_

