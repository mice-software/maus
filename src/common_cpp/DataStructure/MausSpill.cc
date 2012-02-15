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

#include "src/common_cpp/DataStructure/DAQData.hh"
#include "src/common_cpp/DataStructure/EMRSpillData.hh"
#include "src/common_cpp/DataStructure/Scalars.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/MausSpill.hh"


namespace MAUS {

MausSpill::MausSpill() : _scalars(NULL), _emr(NULL), _mc(NULL), _recon(NULL) {
}

MausSpill::MausSpill(const MausSpill& md) {
    _recon = new ReconEventArray(*md._recon);
    _mc = new MCEventArray(*md._mc);
    _emr = new EMRSpillData(*md._emr);
    _scalars = new Scalars(*md._scalars);
    _daq = new DAQData(*md._daq);
}
       
MausSpill& MausSpill::operator=(const MausSpill& md) {
    if (this == &md) {
        return *this;
    }
    _recon = new ReconEventArray(*md._recon);
    _mc = new MCEventArray(*md._mc);
    _emr = new EMRSpillData(*md._emr);
    _scalars = new Scalars(*md._scalars);
    _daq = new DAQData(*md._daq);
    return *this;
}
  
MausSpill::~MausSpill() {
    if (_scalars != NULL) {
        delete _scalars;
        _scalars = NULL;
    }

    if (_emr != NULL) {
        delete _emr;
        _emr = NULL;
    }

    if (_mc != NULL) {
        delete _mc;
        _mc = NULL;
    }

    if (_recon != NULL) {
        delete _recon;
        _recon = NULL;
    }

    if (_daq != NULL) {
        delete _daq;
        _daq = NULL;
    }

}

void MausSpill::SetScalars(Scalars* scalars) {
  _scalars = scalars;
}

Scalars* MausSpill::GetScalars() const {
  return _scalars;
}

void MausSpill::SetEMRSpillData(EMRSpillData* emr) {
  _emr = emr;
}

EMRSpillData* MausSpill::GetEMRSpillData() const {
  return _emr;
}

void MausSpill::SetMCEventArray(MCEventArray* mc) {
  _mc = mc;
}

MCEventArray* MausSpill::GetMCEventArray() const {
  return _mc;
}

void MausSpill::SetReconEventArray(ReconEventArray* recon) {
  _recon = recon;
}

ReconEventArray* MausSpill::GetReconEventArray() const {
  return _recon;
}

void MausSpill::SetDAQData(DAQData* daq) {
  _daq = daq;
}

DAQData* MausSpill::GetDAQData() const {
  return _daq;
}

}

