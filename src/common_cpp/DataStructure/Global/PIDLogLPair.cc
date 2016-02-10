/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 *
 */

#include "src/common_cpp/DataStructure/Global/PIDLogLPair.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

PIDLogLPair::PIDLogLPair() {
  this->first = 0;
  this->second = 0.0;
};

PIDLogLPair::PIDLogLPair(int pid, double logL) {
  this->first = pid;
  this->second = logL;
};

// Destructor
PIDLogLPair::~PIDLogLPair() {}

// Setter for first element, for PIDLogLPairProcessor
void PIDLogLPair::set_PID(int PID) {
  this->first = PID;
}

// Getter for first element, for PIDLogLPairProcessor
int PIDLogLPair::get_PID() const {
  return this->first;
}

// Setter for second element, for PIDLogLPairProcessor
void PIDLogLPair::set_logL(double logL) {
  this->second = logL;
}

// Getter for second element, for PIDLogLPairProcessor
double PIDLogLPair::get_logL() const {
  return this->second;
}
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
