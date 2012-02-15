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

namespace MAUS {

class ScalarsData;
class EMRData;
class MCEventData;
class ReconEventData;
typedef std::vector<MCEventData> MCEventArray;
typedef std::vector<ReconEventData> ReconEventArray;

class MausSpill {
 public:
  MausSpill();

  MausSpill(const MausSpill& md);
       
  MausSpill& operator=(const MausSpill& md);
  
  virtual ~MausSpill();
  
  void SetScalarsData(ScalarsData* scalars);
  ScalarsData* GetScalarsData() const;

  void SetEMRData(EMRData* emr);
  EMRData* GetEMRData() const;

  void SetMCEventArray(MCEventArray* MCEvent);
  MCEventArray* GetMCEventArray() const;

  void SetReconEventArray(ReconEventArray* ReconEvent);
  ReconEventArray* GetReconEventArray() const;

  ClassDef(MausSpill, 1)

 private:
  ScalarsData* _scalars;
  EMRData* _emr;
  MCEventArray* _mc;
  ReconEventArray* _recon;
};

}



#endif
