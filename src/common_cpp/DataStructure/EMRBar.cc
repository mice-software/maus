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

#include "DataStructure/EMRBar.hh"

namespace MAUS {

EMRBar::EMRBar()
    : _bar(0), _emrbarhitarray() {
}

EMRBar::EMRBar(const EMRBar& _emrbar)
    : _bar(0), _emrbarhitarray() {
    *this = _emrbar;
}

EMRBar& EMRBar::operator=(const EMRBar& _emrbar) {
    if (this == &_emrbar) {
        return *this;
    }

    SetBar(_emrbar._bar);

    SetBar(_emrbar._bar);
    SetEMRBarHitArray(_emrbar._emrbarhitarray);

    return *this;
}

EMRBar::~EMRBar() {
}

int EMRBar::GetBar() const {
    return _bar;
}

void EMRBar::SetBar(int bar) {
    _bar = bar;
}

int EMRBar::GetNHits() const {
    return _emrbarhitarray.size();
}

EMRBarHitArray EMRBar::GetEMRBarHitArray() const {
    return _emrbarhitarray;
}

void EMRBar::SetEMRBarHitArray(EMRBarHitArray emrbarhitarray) {
    _emrbarhitarray = emrbarhitarray;
}

void EMRBar::AddBarHit(EMRBarHit bHit) {
  _emrbarhitarray.push_back(bHit);
}
}

