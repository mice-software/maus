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

#include "Recon/Global/ImportEMRRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportEMRRecon::process(MAUS::EMREvent &emr_event,
			       MAUS::GlobalEvent* global_event,
			       std::string mapper_name) {

    double x;
    double y;
    double z;
    double t = -1000000;
    double x_err;
    double y_err;
    double z_err;
    double t_err = -1000000;

    // Mother track
    if (emr_event.GetMotherPtr() != NULL) {
      MAUS::EMRTrack* mother_track =
	emr_event.GetMotherPtr()->GetEMRTrackPtr();
      if (mother_track->GetEMRTrackPointArray().size() > 0) {
	MAUS::DataStructure::Global::Track* PrimaryEMRTrack =
	  new MAUS::DataStructure::Global::Track();
	PrimaryEMRTrack->set_emr_range_primary(mother_track->GetRange());
	PrimaryEMRTrack->set_emr_plane_density(emr_event.GetMotherPtr()->
					       GetPlaneDensityMA());
	// TrackPoint array
	MAUS::EMRTrackPointArray tp_array = mother_track->GetEMRTrackPointArray();
	if (tp_array.size() > 0) {
	  MAUS::EMRTrackPointArray::iterator tp_iter;
	  for (tp_iter = tp_array.begin();
	       tp_iter != tp_array.end();
	       ++tp_iter) {
	    x = (*tp_iter).GetGlobalPosition().X();
	    y = (*tp_iter).GetGlobalPosition().Y();
	    z = (*tp_iter).GetGlobalPosition().Z();
	    TLorentzVector pos(x, y, z, t);
	    x_err = (*tp_iter).GetPositionErrors().X();
	    y_err = (*tp_iter).GetPositionErrors().Y();
	    z_err = (*tp_iter).GetPositionErrors().Z();
	    TLorentzVector pos_err(x_err, y_err, z_err, t_err);
	    MAUS::DataStructure::Global::TrackPoint* tpoint =
	      new MAUS::DataStructure::Global::TrackPoint();
	    MAUS::DataStructure::Global::SpacePoint* spoint =
	      new MAUS::DataStructure::Global::SpacePoint();
	    spoint->set_detector(MAUS::DataStructure::Global::kEMR);
	    spoint->set_position(pos);
	    spoint->set_position_error(pos_err);
	    tpoint->set_space_point(spoint);
	    tpoint->set_detector(MAUS::DataStructure::Global::kEMR);
	    tpoint->set_position(pos);
	    tpoint->set_position_error(pos_err);
	    tpoint->set_mapper_name("MapCppGlobalReconImport");
	    PrimaryEMRTrack->AddTrackPoint(tpoint);
	  }
	  PrimaryEMRTrack->SetDetector(MAUS::DataStructure::Global::kEMR);
	  PrimaryEMRTrack->set_mapper_name("MapCppGlobalReconImport");
	  global_event->add_track_recursive(PrimaryEMRTrack);
	}
      }
    }
    // Daughter track
    if (emr_event.GetDaughterPtr() != NULL) {
      MAUS::EMRTrack* daughter_track =
	emr_event.GetDaughterPtr()->GetEMRTrackPtr();
      if (daughter_track->GetEMRTrackPointArray().size() > 0) {
	MAUS::DataStructure::Global::Track* SecondaryEMRTrack =
	  new MAUS::DataStructure::Global::Track();
	SecondaryEMRTrack->set_emr_range_primary(daughter_track->GetRange());
	SecondaryEMRTrack->set_emr_plane_density(emr_event.GetDaughterPtr()->
						 GetPlaneDensityMA());
	// TrackPoint array
	MAUS::EMRTrackPointArray tp_array = daughter_track->GetEMRTrackPointArray();
	if (tp_array.size() > 0) {
	  MAUS::EMRTrackPointArray::iterator tp_iter;
	  for (tp_iter = tp_array.begin();
	       tp_iter != tp_array.end();
	       ++tp_iter) {
	    x = (*tp_iter).GetGlobalPosition().X();
	    y = (*tp_iter).GetGlobalPosition().Y();
	    z = (*tp_iter).GetGlobalPosition().Z();
	    TLorentzVector pos(x, y, z, t);
	    x_err = (*tp_iter).GetPositionErrors().X();
	    y_err = (*tp_iter).GetPositionErrors().Y();
	    z_err = (*tp_iter).GetPositionErrors().Z();
	    TLorentzVector pos_err(x_err, y_err, z_err, t_err);
	    MAUS::DataStructure::Global::TrackPoint* tpoint =
	      new MAUS::DataStructure::Global::TrackPoint();
	    MAUS::DataStructure::Global::SpacePoint* spoint =
	      new MAUS::DataStructure::Global::SpacePoint();
	    spoint->set_detector(MAUS::DataStructure::Global::kEMR);
	    spoint->set_position(pos);
	    spoint->set_position_error(pos_err);
	    tpoint->set_space_point(spoint);
	    tpoint->set_detector(MAUS::DataStructure::Global::kEMR);
	    tpoint->set_position(pos);
	    tpoint->set_position_error(pos_err);
	    tpoint->set_mapper_name("MapCppGlobalReconImport");
	    SecondaryEMRTrack->AddTrackPoint(tpoint);
	  }
	  SecondaryEMRTrack->SetDetector(MAUS::DataStructure::Global::kEMR);
	  SecondaryEMRTrack->set_mapper_name("MapCppGlobalReconImport");
	  global_event->add_track_recursive(SecondaryEMRTrack);
	}
      }
    }
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
