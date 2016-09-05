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

#include <algorithm>

#include "Utils/Squeak.hh"

#include "Recon/Global/ImportCkovRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportCkovRecon::process(const MAUS::CkovEvent &ckov_event,
			       MAUS::GlobalEvent* global_event,
			       std::string mapper_name) {

    for (size_t i = 0; i < ckov_event.GetCkovDigitArraySize(); ++i) {
      CkovDigit ckov_digit = ckov_event.GetCkovDigitArrayElement(i);
      CkovA ckovA = ckov_digit.GetCkovA();
      MAUS::DataStructure::Global::SpacePoint* GlobalCkovASpacePoint =
	new MAUS::DataStructure::Global::SpacePoint();
      GlobalCkovASpacePoint->
	set_detector(MAUS::DataStructure::Global::kCherenkovA);
      GlobalCkovASpacePoint->set_num_photoelectrons(ckovA.GetNumberOfPes());
      CkovB ckovB = ckov_digit.GetCkovB();
      MAUS::DataStructure::Global::SpacePoint* GlobalCkovBSpacePoint =
	new MAUS::DataStructure::Global::SpacePoint();
      GlobalCkovBSpacePoint->
	set_detector(MAUS::DataStructure::Global::kCherenkovB);
      GlobalCkovBSpacePoint->set_num_photoelectrons(ckovB.GetNumberOfPes());

      global_event->add_space_point(GlobalCkovASpacePoint);
      global_event->add_space_point(GlobalCkovBSpacePoint);
    }
  }

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
