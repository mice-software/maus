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

#include "DataStructure/DBBHit.hh"

namespace MAUS {

DBBHit::DBBHit() {
}

DBBHit::DBBHit(const DBBHit& _dbbhit) {
  *this = _dbbhit;
}

DBBHit& DBBHit::operator=(const DBBHit& _dbbhit) {
  if (this == &_dbbhit) {
    return *this;
  }

  SetLDC(_dbbhit._ldc);
  SetGEO(_dbbhit._geo);
  SetChannel(_dbbhit._channel);
  SetLTime(_dbbhit._leading_edge_time);
  SetTTime(_dbbhit._trailing_edge_time);
  return *this;
}

DBBHit::~DBBHit() {
}

int DBBHit::GetLDC() const {
  return _ldc;
}

void DBBHit::SetLDC(int ldc) {
  _ldc = ldc;
}

int DBBHit::GetGEO() const {
  return _geo;
}

void DBBHit::SetGEO(int geo) {
  _geo = geo;
}

int DBBHit::GetChannel() const {
  return _channel;
}

void DBBHit::SetChannel(int ch) {
  _channel = ch;
}

unsigned int DBBHit::GetLTime() const {
  return _leading_edge_time;
}

unsigned int DBBHit::GetTTime() const {
  return _trailing_edge_time;
}

void DBBHit::SetLTime(unsigned int t) {
  _leading_edge_time = t;
}

void DBBHit::SetTTime(unsigned int t) {
  _trailing_edge_time = t;
}
}


