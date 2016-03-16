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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_MAUS_SPILL_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_MAUS_SPILL_HH_

#include <vector>
#include <map>
#include <string>

#include "Utils/VersionNumber.hh"
#include "DataStructure/DAQData.hh"
#include "DataStructure/Scalars.hh"
#include "DataStructure/MCEvent.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/EMRSpillData.hh"
#include "DataStructure/TestBranch.hh"

namespace MAUS {
typedef std::map<std::string, std::string> ErrorsMap;

/** @class Spill stores data for all items in a given spill
 *
 *  The Spill is the fundamental "event" type for MAUS. Branches for spill-level
 *  objects; Scalars, DAQ, EMR, plus Recon event and MC Event vectors.
 */
class Spill {
 public:
  /** Initialise to NULL/0 */
  Spill();

  /** Copy constructor - deep copy */
  Spill(const Spill& md);

  /** Equals operator - does a deep copy */
  Spill& operator=(const Spill& md);

  /** Destructor - frees memory */
  virtual ~Spill();

  /** Set the scalars information
   *
   *  Spill takes ownership of memory assigned to scalars
   */
  void SetScalars(Scalars *scalars);

  /** Get the scalars information
   *
   *  Spill keeps ownership of this memory
   */
  Scalars* GetScalars() const;


  /** Set the DAQ output
   *
   *  Spill takes ownership of memory assigned to daq
   */
  void SetDAQData(DAQData* daq);

  /** Get the DAQ output
   *
   *  Spill keeps ownership of this memory
   */
  DAQData* GetDAQData() const;

  /** Set the MC events
   *
   *  Spill takes ownership of memory assigned to events
   */
  void SetMCEvents(MCEventPArray* events);

  /** Get the MC events
   *
   *  Spill keeps ownership of this memory
   */
  MCEventPArray* GetMCEvents() const;

  /** Get a single MC event (needed for PyROOT) */
  MCEvent& GetAnMCEvent(size_t i) const {
    return (*(*_mc)[i]);
  }

  /** Get the MC event size (needed for PyROOT)*/
  size_t GetMCEventSize() const {
    if (!_mc) return 0;
    return _mc->size();
  }

  /** Set the Recon events
   *
   *  Spill takes ownership of memory assigned to ReconEvent
   */
  void SetReconEvents(ReconEventPArray* ReconEvent);

  /** Get the Recon events
   *
   *  Spill keeps ownership of this memory
   */
  ReconEventPArray* GetReconEvents() const;

  /** Get a single Recon event (needed for PyROOT) */
  ReconEvent& GetAReconEvent(int i) const {
    return (*(*_recon)[i]);
  }

  /** Get the Recon event size (needed for PyROOT)*/
  size_t GetReconEventSize() const {
    if (!_recon) return 0;
    return _recon->size();
  }

  /** Set the EMR Spill Data
   *
   *  Spill takes ownership of the memory assigned to EMRSpillData
   */
  void SetEMRSpillData(EMRSpillData* emr);

  /** Get the EMR Spill Data
   *
   *  Spill keeps ownership of this memory
   */
  EMRSpillData* GetEMRSpillData() const;

  /** Set the spill number */
  void SetSpillNumber(int spill);

  /** Get the spill number */
  int GetSpillNumber() const;

  /** Set the event type */
  void SetDaqEventType(std::string type);

  /** Get the event type */
  std::string GetDaqEventType() const;

  /** Set the run number */
  void SetRunNumber(int run);

  /** Get the run number */
  int GetRunNumber() const;

  /** Set the event Id */
  void SetEventId(int eid);

  /** Get the event Id */
  int GetEventId() const;

  /** Set the time stamp */
  void SetTimeStamp(int st);

  /** Get the time stamp */
  int GetTimeStamp() const;

  /** Set the errors on this spill */
  void SetErrors(ErrorsMap errors);

  /** Get the errors on this spill */
  ErrorsMap GetErrors() const;

  /** Test branch for testing porpoises only */
  void SetTestBranch(TestBranch* test);

  /** Test branch for testing porpoises only */
  TestBranch* GetTestBranch() const;

 private:

  DAQData* _daq;
  Scalars* _scalars;
  MCEventPArray* _mc;
  ReconEventPArray* _recon;
  EMRSpillData* _emr;
  int _spill_number;
  int _run_number;
  int _event_id;
  int _time_stamp;
  std::string _daq_event_type;
  std::map<std::string, std::string> _errors;
  TestBranch* _test;
  static int reference_count;
  MAUS_VERSIONED_CLASS_DEF(Spill)
};
}

#endif

