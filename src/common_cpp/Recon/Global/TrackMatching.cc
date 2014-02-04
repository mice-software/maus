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

#include "Recon/Global/TrackMatching.hh"

#include <algorithm>

#include "Interface/Squeak.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  void TrackMatching::FormTracks(MAUS::GlobalEvent* global_event, std::string mapper_name) {
    std::cerr << "TrackMatching is happening" << std::endl;

    if (!global_event) {
      throw(Exception(Exception::recoverable,
		   "Trying to import an empty global event.",
		   "MapCppGlobalPID::TrackMatching"));
    }

    std::cerr << "checked for global event" << std::endl;

    std::vector<MAUS::DataStructure::Global::SpacePoint*> *TOFspacepointarray =
      global_event->get_space_points();

    std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF0tp;
    std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF1tp;
    std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF2tp;

    std::vector<MAUS::DataStructure::Global::TrackPoint*> tempTOF0tp;
    std::vector<MAUS::DataStructure::Global::TrackPoint*> tempTOF2tp;


    double TOF01offset = 30;
    double TOF12offset = 35;
    double allowance = 7.5;
    std::cerr << "TOFspacepointarray->size() : " << TOFspacepointarray->size() << std::endl;
    for (unsigned int i = 0; i < TOFspacepointarray->size(); ++i) {
      std::cerr << "entered tm for loop" << std::endl;
      MAUS::DataStructure::Global::SpacePoint* sp = TOFspacepointarray->at(i);

      MAUS::DataStructure::Global::TrackPoint* tp0;
      MAUS::DataStructure::Global::TrackPoint* tp1;
      MAUS::DataStructure::Global::TrackPoint* tp2;
      if (sp->get_detector() == MAUS::DataStructure::Global::kTOF0) {
        std::cerr<< "kTOF0" << std::endl;
	tp0 = new MAUS::DataStructure::Global::TrackPoint(sp);
	TOF0tp.push_back(tp0);
      } else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF1) {
        std::cerr<< "kTOF1" << std::endl;
	tp1 = new MAUS::DataStructure::Global::TrackPoint(sp);
	TOF1tp.push_back(tp1);
      } else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF2) {
        std::cerr<< "kTOF2" << std::endl;
	tp2 = new MAUS::DataStructure::Global::TrackPoint(sp);
	TOF2tp.push_back(tp2);
      }
    }

    for (unsigned int i = 0; i < TOF1tp.size(); ++i) {
      MAUS::DataStructure::Global::Track* TOFtrack =
	new MAUS::DataStructure::Global::Track();
      for (unsigned int j = 0; j < TOF0tp.size(); ++j) {
	if ((TOF1tp[i]->get_position().T() - TOF0tp[j]->get_position().T()) <=
	    (TOF01offset + allowance) &&
	    (TOF1tp[i]->get_position().T() - TOF0tp[j]->get_position().T()) >=
	    (TOF01offset - allowance)) {
	  tempTOF0tp.push_back(TOF0tp[j]);
	}
      }
      for (unsigned int k = 0; k < TOF2tp.size(); ++k) {
	if ((TOF2tp[k]->get_position().T() - TOF1tp[i]->get_position().T()) <=
	    (TOF12offset + allowance) &&
	    (TOF2tp[k]->get_position().T() - TOF1tp[i]->get_position().T()) >=
	    (TOF12offset - allowance)) {
	  tempTOF2tp.push_back(TOF2tp[k]);
	}
      }
      if (tempTOF0tp.size() < 2 && tempTOF2tp.size() < 2) {
	TOFtrack->AddTrackPoint(TOF1tp[i]);
	global_event->add_track_point_recursive(TOF1tp[i]);
	if (tempTOF0tp.size() == 1) {
	  TOFtrack->AddTrackPoint(tempTOF0tp[0]);
	  global_event->add_track_point_recursive(tempTOF0tp[0]);
	}
	if (tempTOF2tp.size() == 1) {
	  TOFtrack->AddTrackPoint(tempTOF2tp[0]);
	  global_event->add_track_point_recursive(tempTOF2tp[0]);
	}
  std::cerr << "towards the end of TrackMatching" << std::endl;
  TOFtrack->set_mapper_name(mapper_name);
	global_event->add_track_recursive(TOFtrack);
      } else {
	Squeak::mout(Squeak::error) << "Global event returned multiple potential"
				    << " TOF0 and/or TOF2 space points that "
	                            << "could not be separated into tracks."
				    << std::endl;
      }

      tempTOF0tp.clear();
      tempTOF2tp.clear();
    }
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
