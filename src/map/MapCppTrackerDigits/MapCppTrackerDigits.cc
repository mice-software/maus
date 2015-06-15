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

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/map/MapCppTrackerDigits/MapCppTrackerDigits.hh"

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTrackerDigits(void) {
  PyWrapMapBase<MAUS::MapCppTrackerDigits>::PyWrapMapBaseModInit
                                        ("MapCppTrackerDigits", "", "", "", "");
}

MapCppTrackerDigits::MapCppTrackerDigits()
    : MapBase<Data>("MapCppTrackerDigits"), real(0) {
}

MapCppTrackerDigits::~MapCppTrackerDigits() {
  if (real) delete real;
}

void MapCppTrackerDigits::_birth(const std::string& argJsonConfigDocument) {
  real = new RealDataDigitization();
  real->initialise();
}

void MapCppTrackerDigits::_death() {}

void MapCppTrackerDigits::_process(Data* data) const {
//   RealDataDigitization real;
//   real.initialise();
  Spill *spill = data->GetSpill();
  real->process(spill);
}

} // ~namespace MAUS

