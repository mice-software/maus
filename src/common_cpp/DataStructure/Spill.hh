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

#include "Rtypes.h"

#include "src/common_cpp/DataStructure/DAQData.hh"
#include "src/common_cpp/DataStructure/EMRSpillData.hh"
#include "src/common_cpp/DataStructure/Scalars.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

namespace MAUS {
typedef std::vector<ReconEvent*> ReconEventArray;
typedef std::vector<MCEvent*> MCEventArray;

/** @class Spill stores data for all items in a given spill
 *
 *  The Spill is the fundamental "event" type for MAUS. Branches for spill-level
 *  objects; Scalars, DAQ, EMR, plus Recon event and MC Event
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

  /** Set the Recon events */
  void SetReconEvents(ReconEventArray* ReconEvent);

  /** Get the Recon events */
  ReconEventArray* GetReconEvents() const;

  /** Set the spill number */
  void SetSpillNumber(int spill);

  /** Get the spill number */
  int GetSpillNumber() const;

 private:
  DAQData* _daq;
  Scalars* _scalars;
  EMRSpillData* _emr;
  MCEventArray* _mc;
  ReconEventArray* _recon;
  int _spill_number;
  ClassDef(Spill, 1)
};
}

#endif

