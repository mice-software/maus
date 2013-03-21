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

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/DAQData.hh"
#include "src/common_cpp/DataStructure/EMRSpillData.hh"
#include "src/common_cpp/DataStructure/Scalars.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/TestBranch.hh"

namespace MAUS {
typedef std::vector<ReconEvent*> ReconEventArray;
typedef std::vector<MCEvent*> MCEventArray;
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

  /** Set the scalars information */
  void SetScalars(Scalars* scalars);

  /** Get the scalars information */
  Scalars* GetScalars() const;

  /** Set the EMR Spill information */
  void SetEMRSpillData(EMRSpillData* emr);

  /** Get the EMR Spill information */
  EMRSpillData* GetEMRSpillData() const;

  /** Set the DAQ output */
  void SetDAQData(DAQData* daq);

  /** Get the DAQ output */
  DAQData* GetDAQData() const;

  /** Set the MC events */
  void SetMCEvents(MCEventArray* events);

  /** Get the MC events */
  MCEventArray* GetMCEvents() const;

  /** Get a single MC event (needed for PyROOT) */
  MCEvent& GetAnMCEvent(size_t i) const {
    return (*(*_mc)[i]);
  }

  /** Get the MC event size (needed for PyROOT)*/
  size_t GetMCEventSize() const {
    if(!_mc) return 0;
    return _mc->size();
  }

  /** Set the Recon events */
  void SetReconEvents(ReconEventArray* ReconEvent);

  /** Get the Recon events */
  ReconEventArray* GetReconEvents() const;

  /** Get a single Recon event (needed for PyROOT) */
  ReconEvent& GetAReconEvent(int i) const {
    return (*(*_recon)[i]);
  }

  /** Get the Recon event size (needed for PyROOT)*/
  size_t GetReconEventSize() const {
    if(!_recon) return 0;
    return _recon->size();
  }

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
  EMRSpillData* _emr;
  MCEventArray* _mc;
  ReconEventArray* _recon;
  int _spill_number;
  int _run_number;
  std::string _daq_event_type;
  std::map<std::string, std::string> _errors;
  TestBranch* _test;

  MAUS_VERSIONED_CLASS_DEF(Spill)
};
}

#endif

