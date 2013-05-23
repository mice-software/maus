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

#include "src/common_cpp/JsonCppProcessors/Global/TrackPointProcessor.hh"

namespace MAUS {
namespace Processor {
namespace Global {

TrackPointProcessor::TrackPointProcessor() {
  RegisterValueBranch(
      "particle_event", &_int_proc,
      &MAUS::DataStructure::Global::TrackPoint::get_particle_event,
      &MAUS::DataStructure::Global::TrackPoint::set_particle_event,
      true);

  RegisterValueBranch(
      "mapper_name", &_string_proc,
      &MAUS::DataStructure::Global::TrackPoint::get_mapper_name,
      &MAUS::DataStructure::Global::TrackPoint::set_mapper_name,
      true);

  RegisterValueBranch(
      "charge", &_double_proc,
      &MAUS::DataStructure::Global::TrackPoint::get_charge,
      &MAUS::DataStructure::Global::TrackPoint::set_charge, true);

  RegisterValueBranch(
      "momentum", &_tlorentz_vec_proc,
      &MAUS::DataStructure::Global::TrackPoint::get_momentum,
      &MAUS::DataStructure::Global::TrackPoint::set_momentum, true);

  RegisterValueBranch(
      "momentum_error", &_tlorentz_vec_proc,
      &MAUS::DataStructure::Global::TrackPoint::get_momentum_error,
      &MAUS::DataStructure::Global::TrackPoint::set_momentum_error,
      true);

  RegisterTRef(
      "space_point",
      &MAUS::DataStructure::Global::TrackPoint::get_space_point_tref,
      &MAUS::DataStructure::Global::TrackPoint::set_space_point_tref,
      false);

  RegisterBaseClass(
      "basepoint", &_base_proc,
      &MAUS::DataStructure::Global::BasePoint::set_base, true);
}

} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
