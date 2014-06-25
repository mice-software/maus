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

#include "src/common_cpp/JsonCppProcessors/Global/BasePointProcessor.hh"

namespace MAUS {
namespace Processor {
namespace Global {

BasePointProcessor::BasePointProcessor() {
  RegisterValueBranch(
      "position", &_tlorentz_vec_proc,
      &MAUS::DataStructure::Global::BasePoint::get_position,
      &MAUS::DataStructure::Global::BasePoint::set_position, true);

  RegisterValueBranch(
      "position_error", &_tlorentz_vec_proc,
      &MAUS::DataStructure::Global::BasePoint::get_position_error,
      &MAUS::DataStructure::Global::BasePoint::set_position_error,
      true);

  RegisterValueBranch(
      "detector", &_detector_enum_proc,
      &MAUS::DataStructure::Global::BasePoint::get_detector,
      &MAUS::DataStructure::Global::BasePoint::set_detector, true);

  RegisterValueBranch(
      "geometry_path", &_string_proc,
      &MAUS::DataStructure::Global::BasePoint::get_geometry_path,
      &MAUS::DataStructure::Global::BasePoint::set_geometry_path,
      true);

  RegisterValueBranch(
      "num_photoelectrons", &_double_proc,
      &MAUS::DataStructure::Global::BasePoint::get_num_photoelectrons,
      &MAUS::DataStructure::Global::BasePoint::set_num_photoelectrons,
      true);

  RegisterValueBranch(
      "ADC_charge", &_int_proc,
      &MAUS::DataStructure::Global::BasePoint::get_ADC_charge,
      &MAUS::DataStructure::Global::BasePoint::set_ADC_charge,
      true);

  RegisterValueBranch(
      "ADC_charge_product", &_int_proc,
      &MAUS::DataStructure::Global::BasePoint::get_ADC_charge_product,
      &MAUS::DataStructure::Global::BasePoint::set_ADC_charge_product,
      true);
}

} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
