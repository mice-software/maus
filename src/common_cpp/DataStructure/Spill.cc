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

#include <iostream>

#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

Spill::Spill() {
    std::cerr << "Spill Ctor " << this <<  std::endl;
}

Spill::Spill(const Spill& md) {
    std::cerr << "Spill Copy Ctor MC: " << &_mc << std::endl;
}
       
Spill& Spill::operator=(const Spill& md) {
    if (this == &md) {
        return *this;
    }
    std::cerr << "Spill Eq Operator MC: " << &_mc << std::endl; 
    return *this;
}
  
Spill::~Spill() {
}

void Spill::SetScalars(Scalars scalars) {
  _scalars = scalars;
}

Scalars Spill::GetScalars() const {
  return _scalars;
}

void Spill::SetEMRSpillData(EMRSpillData emr) {
  _emr = emr;
}

EMRSpillData Spill::GetEMRSpillData() const {
  return _emr;
}

void Spill::SetMCEventArray(MCEventArray mc) {
  _mc = mc;
  std::cerr << "SET MC:" << &_mc << std::endl;
}

MCEventArray Spill::GetMCEventArray() const {
  std::cerr << "GET MC:" << &_mc << std::endl;
  return _mc;
}

void Spill::SetReconEventArray(ReconEventArray recon) {
  _recon = recon;
}

ReconEventArray Spill::GetReconEventArray() const {
  return _recon;
}

void Spill::SetDAQData(DAQData daq) {
  _daq = daq;
}

DAQData Spill::GetDAQData() const {
  return _daq;
}

int Spill::GetMyInt() const { 
    return my_int;
}

void Spill::SetMyInt(int _int) {
    my_int = _int;
}

}

