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

#include "Rtypes.h"

#include <vector>

#include "src/common_cpp/DataStructure/DAQData.hh"
#include "src/common_cpp/DataStructure/EMRSpillData.hh"
#include "src/common_cpp/DataStructure/Scalars.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

namespace MAUS {
typedef std::vector<ReconEvent*> ReconEventArray;
typedef std::vector<MCEvent*> MCEventArray;

class Spill {
 public:
  Spill();

  Spill(const Spill& md);
       
  Spill& operator=(const Spill& md);
  
  virtual ~Spill();
  
  void SetScalars(Scalars* scalars);
  Scalars* GetScalars() const;

  void SetEMRSpillData(EMRSpillData* emr);
  EMRSpillData* GetEMRSpillData() const;

  void SetDAQData(DAQData* daq);
  DAQData* GetDAQData() const;

  void SetMCEvents(MCEventArray* events);
  MCEventArray* GetMCEvents() const;

  void SetReconEvents(ReconEventArray* ReconEvent);
  ReconEventArray* GetReconEvents() const;

  void SetSpillNumber(int spill);
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

// Dec 2012 step iv construction complete
// Dec 2018 step vi construction complete

