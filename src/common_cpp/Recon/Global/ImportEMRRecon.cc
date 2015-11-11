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

#include "Interface/Squeak.hh"

#include "Recon/Global/ImportEMRRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportEMRRecon::process(const MAUS::EMREvent &emr_event,
			       MAUS::GlobalEvent* global_event,
			       std::string mapper_name, double x_ref,
			       double y_ref, double z_ref) {

    double x;
    double y;
    double z;
    double t = -1000000;
    double x_err;
    double y_err;
    double z_err;
    double t_err = -1000000;

    // Check that EMR event has a primary track
    if (emr_event.GetHasPrimary()) {
      MAUS::DataStructure::Global::Track* PrimaryEMRTrack =
	new MAUS::DataStructure::Global::Track();
      PrimaryEMRTrack->set_emr_range_primary(emr_event.GetRangePrimary());
      PrimaryEMRTrack->set_emr_plane_density(emr_event.GetPlaneDensity());
      PrimaryEMRTrack->SetDetector(MAUS::DataStructure::Global::kEMR);
      PrimaryEMRTrack->set_mapper_name("MapCppGlobalReconImport");
      global_event->add_track(PrimaryEMRTrack);
      MAUS::DataStructure::Global::Track* SecondaryEMRTrack =
	new MAUS::DataStructure::Global::Track();
      PrimaryEMRTrack->set_emr_range_primary(emr_event.GetRangePrimary());
      PrimaryEMRTrack->set_emr_plane_density(emr_event.GetPlaneDensity());
      // Plane hit array
      MAUS::EMRPlaneHitArray plane_hit_array = emr_event.GetEMRPlaneHitArray();
      if (plane_hit_array.size() > 0) {
	MAUS::EMRPlaneHitArray::iterator plane_hit_iter;
	for (plane_hit_iter = plane_hit_array.begin();
	     plane_hit_iter != plane_hit_array.end();
	     ++plane_hit_iter) {
	  // Plane hit (sets z)
	  //MAUS::EMRPlaneHit* plane_hit = (*plane_hit_iter);
	  // Primary Bar array
	  MAUS::EMRBarArray primary_bar_array =  (*plane_hit_iter)->GetEMRBarArrayPrimary();
	  MAUS::EMRBarArray::iterator primary_bar_iter;
	  for (primary_bar_iter = primary_bar_array.begin();
	       primary_bar_iter != primary_bar_array.end();
	       ++primary_bar_iter) {
	    //MAUS::EMRBar* emr_bar = (*primary_bar_iter);
	    // Bar hit array
	    MAUS::EMRBarHitArray bar_hit_array =  (*primary_bar_iter)->GetEMRBarHitArray();
	    MAUS::EMRBarHitArray::iterator bar_hit_iter;
	    for (bar_hit_iter = bar_hit_array.begin();
		 bar_hit_iter != bar_hit_array.end();
		 ++bar_hit_iter) {
	      // Bar hit (get x and y)
	      //MAUS::EMRBarHit bar_hit = (*bar_hit_iter);
	      // EMR coordinate system should now match MICE coordinate system
	      x = (*bar_hit_iter).GetX() + x_ref;
	      y = (*bar_hit_iter).GetY() + y_ref;
	      z = (*bar_hit_iter).GetZ() + z_ref;
	      TLorentzVector pos(x, y, z, t);
	      x_err = (*bar_hit_iter).GetErrorX();
	      y_err = (*bar_hit_iter).GetErrorY();
	      z_err = (*bar_hit_iter).GetErrorZ();
	      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
	      MAUS::DataStructure::Global::TrackPoint* tpoint =
		new MAUS::DataStructure::Global::TrackPoint();
	      MAUS::DataStructure::Global::SpacePoint* spoint =
		new MAUS::DataStructure::Global::SpacePoint();
	      spoint->set_detector(MAUS::DataStructure::Global::kEMR);
	      tpoint->set_detector(MAUS::DataStructure::Global::kEMR);
	      spoint->set_position(pos);
	      tpoint->set_position(pos);
	      spoint->set_position_error(pos_err);
	      tpoint->set_position_error(pos_err);
	      tpoint->set_space_point(spoint);
	      tpoint->set_mapper_name("MapCppGlobalReconImport");
	      PrimaryEMRTrack->AddTrackPoint(tpoint);
	      PrimaryEMRTrack->SetDetector(MAUS::DataStructure::Global::kEMR);
	      PrimaryEMRTrack->set_mapper_name("MapCppGlobalReconImport");
    
	    }
	    // Secondary Bar array
	    if (emr_event.GetHasSecondary()) {
	      SecondaryEMRTrack->set_emr_range_secondary(emr_event.GetRangeSecondary());
	      MAUS::EMRBarArray secondary_bar_array =  (*plane_hit_iter)->GetEMRBarArraySecondary();
	      MAUS::EMRBarArray::iterator secondary_bar_iter;
	      for (secondary_bar_iter = secondary_bar_array.begin();
		   secondary_bar_iter != secondary_bar_array.end();
		   ++secondary_bar_iter) {
		//MAUS::EMRBar* emr_bar = (*secondary_bar_iter);
		// Bar hit array
		MAUS::EMRBarHitArray bar_hit_array =  (*secondary_bar_iter)->GetEMRBarHitArray();
		MAUS::EMRBarHitArray::iterator bar_hit_iter;
		for (bar_hit_iter = bar_hit_array.begin();
		     bar_hit_iter != bar_hit_array.end();
		     ++bar_hit_iter) {
		  // Bar hit (get x and y)
		  //MAUS::EMRBarHit bar_hit = (*bar_hit_iter);
		  // EMR coordinate system should now match MICE coordinate system
		  x = (*bar_hit_iter).GetX() + x_ref;
		  y = (*bar_hit_iter).GetY() + y_ref;
		  z = (*bar_hit_iter).GetZ() + z_ref;
		  TLorentzVector pos(x, y, z, t);
		  x_err = (*bar_hit_iter).GetErrorX();
		  y_err = (*bar_hit_iter).GetErrorY();
		  z_err = (*bar_hit_iter).GetErrorZ();
		  TLorentzVector pos_err(x_err, y_err, z_err, t_err);
		  MAUS::DataStructure::Global::TrackPoint* tpoint =
		    new MAUS::DataStructure::Global::TrackPoint();
		  MAUS::DataStructure::Global::SpacePoint* spoint =
		    new MAUS::DataStructure::Global::SpacePoint();
		  spoint->set_detector(MAUS::DataStructure::Global::kEMR);
		  tpoint->set_detector(MAUS::DataStructure::Global::kEMR);
		  spoint->set_position(pos);
		  tpoint->set_position(pos);
		  spoint->set_position_error(pos_err);
		  tpoint->set_position_error(pos_err);
		  tpoint->set_space_point(spoint);
		  tpoint->set_mapper_name("MapCppGlobalReconImport");
		  SecondaryEMRTrack->AddTrackPoint(tpoint);
		  SecondaryEMRTrack->set_mapper_name("MapCppGlobalReconImport");
		}
	      }
	    } else {
	      delete SecondaryEMRTrack;
	      SecondaryEMRTrack = NULL;
	    }
	  }
	  global_event->add_track(PrimaryEMRTrack);
	  if (SecondaryEMRTrack != NULL) {
	    global_event->add_track_recursive(SecondaryEMRTrack);
	  }
	}
      }
    }
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
