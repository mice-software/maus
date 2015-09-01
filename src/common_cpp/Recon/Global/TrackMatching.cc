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

    if (!global_event) {
      throw(Exception(Exception::recoverable,
		      "Trying to import an empty global event.",
		      "Recon::Global::TrackMatching"));
    }


    MAUS::DataStructure::Global::TrackPArray *ImportedTracks = global_event->get_tracks();
    MAUS::DataStructure::Global::TrackPArray::iterator ImportedTrackIterator;
    MAUS::DataStructure::Global::Track* ImportedSciFiTrack =
      new MAUS::DataStructure::Global::Track();
    MAUS::DataStructure::Global::Track* EMRTrack =
      new MAUS::DataStructure::Global::Track();
    for (ImportedTrackIterator = ImportedTracks->begin();
	 ImportedTrackIterator != ImportedTracks->end();
	 ++ImportedTrackIterator) {
      MAUS::DataStructure::Global::Track* ImportedTrack =
	(*ImportedTrackIterator);
      if (ImportedTrack->HasDetector(MAUS::DataStructure::Global::kTracker0) ||
	  ImportedTrack->HasDetector(MAUS::DataStructure::Global::kTracker1)) {
	std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	  tempSciFiTrackPointArray = ImportedTrack->GetTrackPoints();
	for (unsigned int i = 0; i < tempSciFiTrackPointArray.size(); i++) {
	  MAUS::DataStructure::Global::TrackPoint* tempSciFiTrackPoint =
	    const_cast<MAUS::DataStructure::Global::TrackPoint*>
	    (tempSciFiTrackPointArray[i]);
	  tempSciFiTrackPoint->set_mapper_name(mapper_name);
	  ImportedSciFiTrack->AddTrackPoint(tempSciFiTrackPoint);
	}
      } else if (ImportedTrack->HasDetector(MAUS::DataStructure::Global::kEMR)) {
	  std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	  tempEMRTrackPointArray = ImportedTrack->GetTrackPoints();
	for (unsigned int i = 0; i < tempEMRTrackPointArray.size(); i++) {
	  MAUS::DataStructure::Global::TrackPoint* tempEMRTrackPoint =
	    const_cast<MAUS::DataStructure::Global::TrackPoint*>
	    (tempEMRTrackPointArray[i]);
	  tempEMRTrackPoint->set_mapper_name(mapper_name);
	  EMRTrack->AddTrackPoint(tempEMRTrackPoint);
	}
	EMRTrack->set_emr_range_primary(ImportedTrack->get_emr_range_primary());
	EMRTrack->set_emr_plane_density(ImportedTrack->get_emr_plane_density());
	EMRTrack->set_emr_range_secondary(ImportedTrack->get_emr_range_secondary());
      }
    }
    if (ImportedSciFiTrack->GetTrackPoints().size() == 0) {
	delete ImportedSciFiTrack;
	ImportedSciFiTrack = NULL;
      }
      if (EMRTrack->GetTrackPoints().size() == 0) {
	delete EMRTrack;
	EMRTrack = NULL;
      }

    std::vector<MAUS::DataStructure::Global::SpacePoint*>
      *GlobalSpacePointArray = global_event->get_space_points();
    MAUS::DataStructure::Global::TrackPArray TOFTrackArray;
    MakeTOFTracks(global_event, GlobalSpacePointArray, TOFTrackArray, mapper_name);
    MAUS::DataStructure::Global::Track* KLTrack =
      new MAUS::DataStructure::Global::Track();
    MakeKLTracks(global_event, GlobalSpacePointArray, KLTrack, mapper_name);
    if (KLTrack->GetTrackPoints().size() == 0) {
      delete KLTrack;
      KLTrack = NULL;
    }

    // Adding global tracks for case where global event contains both SciFi and TOF tracks
    // (And KL track if applicable)
    if (ImportedSciFiTrack != NULL && !TOFTrackArray.empty()) {
      for (unsigned int j = 0; j < TOFTrackArray.size(); j++) {
	MAUS::DataStructure::Global::Track* GlobalTrack = TOFTrackArray[j]->Clone();
	GlobalTrack->set_mapper_name(mapper_name);
	std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	  tempSciFiTrackPointArray = ImportedSciFiTrack->GetTrackPoints();
	for (unsigned int k = 0; k < tempSciFiTrackPointArray.size(); k++) {
	  MAUS::DataStructure::Global::TrackPoint* tempSciFiTrackPoint =
	    const_cast<MAUS::DataStructure::Global::TrackPoint*>
	    (tempSciFiTrackPointArray[k]);
	  tempSciFiTrackPoint->set_mapper_name(mapper_name);
	  GlobalTrack->AddTrackPoint(tempSciFiTrackPoint);
	}
	if (KLTrack != NULL) {
	  std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	    tempKLTrackPointArray = KLTrack->GetTrackPoints();
	  for (unsigned int l = 0; l < tempKLTrackPointArray.size(); l++) {
	    MAUS::DataStructure::Global::TrackPoint* tempKLTrackPoint =
	      const_cast<MAUS::DataStructure::Global::TrackPoint*>
	      (tempKLTrackPointArray[l]);
	    tempKLTrackPoint->set_mapper_name(mapper_name);
	    GlobalTrack->AddTrackPoint(tempKLTrackPoint);
	  }
	}
	if (EMRTrack != NULL) {
	  std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	    tempEMRTrackPointArray = EMRTrack->GetTrackPoints();
	  for (unsigned int m = 0; m < tempEMRTrackPointArray.size(); m++) {
	    MAUS::DataStructure::Global::TrackPoint* tempEMRTrackPoint =
	      const_cast<MAUS::DataStructure::Global::TrackPoint*>
	      (tempEMRTrackPointArray[m]);
	    tempEMRTrackPoint->set_mapper_name(mapper_name);
	    GlobalTrack->AddTrackPoint(tempEMRTrackPoint);
	  }
	  GlobalTrack->set_emr_range_primary(EMRTrack->get_emr_range_primary());
	  GlobalTrack->set_emr_plane_density(EMRTrack->get_emr_plane_density());
	  GlobalTrack->set_emr_range_secondary(EMRTrack->get_emr_range_secondary());
	}
	global_event->add_track_recursive(GlobalTrack);
      }
    }

    // Adding global tracks for case where global event contains only TOF tracks
    // (And KL track if applicable)
    if (ImportedSciFiTrack == NULL && !TOFTrackArray.empty()) {
      for (unsigned int i = 0; i < TOFTrackArray.size(); i++) {
	MAUS::DataStructure::Global::Track* GlobalTrack = TOFTrackArray[i]->Clone();
	GlobalTrack->set_mapper_name(mapper_name);
	if (KLTrack != NULL) {
	  std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	    tempKLTrackPointArray = KLTrack->GetTrackPoints();
	  for (unsigned int l = 0; l < tempKLTrackPointArray.size(); l++) {
	    MAUS::DataStructure::Global::TrackPoint* tempKLTrackPoint =
	      const_cast<MAUS::DataStructure::Global::TrackPoint*>
	      (tempKLTrackPointArray[l]);
	    tempKLTrackPoint->set_mapper_name(mapper_name);
	    GlobalTrack->AddTrackPoint(tempKLTrackPoint);
	  }
	}
	if (EMRTrack != NULL) {
	  std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	    tempEMRTrackPointArray = EMRTrack->GetTrackPoints();
	  for (unsigned int m = 0; m < tempEMRTrackPointArray.size(); m++) {
	    MAUS::DataStructure::Global::TrackPoint* tempEMRTrackPoint =
	      const_cast<MAUS::DataStructure::Global::TrackPoint*>
	      (tempEMRTrackPointArray[m]);
	    tempEMRTrackPoint->set_mapper_name(mapper_name);
	    GlobalTrack->AddTrackPoint(tempEMRTrackPoint);
	  }
	  GlobalTrack->set_emr_range_primary(EMRTrack->get_emr_range_primary());
	  GlobalTrack->set_emr_plane_density(EMRTrack->get_emr_plane_density());
	  GlobalTrack->set_emr_range_secondary(EMRTrack->get_emr_range_secondary());
	}
	global_event->add_track_recursive(GlobalTrack);
      }
    }

    // Adding global tracks for case where global event contains only SciFi tracks
    // (And KL track if applicable)
    if (ImportedSciFiTrack != NULL && TOFTrackArray.empty()) {
      MAUS::DataStructure::Global::Track* GlobalTrack = ImportedSciFiTrack->Clone();
      GlobalTrack->set_mapper_name(mapper_name);
      if (KLTrack != NULL) {
	std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	  tempKLTrackPointArray = KLTrack->GetTrackPoints();
	for (unsigned int l = 0; l < tempKLTrackPointArray.size(); l++) {
	  MAUS::DataStructure::Global::TrackPoint* tempKLTrackPoint =
	    const_cast<MAUS::DataStructure::Global::TrackPoint*>
	    (tempKLTrackPointArray[l]);
	  tempKLTrackPoint->set_mapper_name(mapper_name);
	  GlobalTrack->AddTrackPoint(tempKLTrackPoint);
	}
      }
      if (EMRTrack != NULL) {
	std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	  tempEMRTrackPointArray = EMRTrack->GetTrackPoints();
	for (unsigned int m = 0; m < tempEMRTrackPointArray.size(); m++) {
	  MAUS::DataStructure::Global::TrackPoint* tempEMRTrackPoint =
	    const_cast<MAUS::DataStructure::Global::TrackPoint*>
	    (tempEMRTrackPointArray[m]);
	  tempEMRTrackPoint->set_mapper_name(mapper_name);
	  GlobalTrack->AddTrackPoint(tempEMRTrackPoint);
	}
	GlobalTrack->set_emr_range_primary(EMRTrack->get_emr_range_primary());
	GlobalTrack->set_emr_plane_density(EMRTrack->get_emr_plane_density());
	GlobalTrack->set_emr_range_secondary(EMRTrack->get_emr_range_secondary());
      }
      global_event->add_track_recursive(GlobalTrack);
    }

    // Adding global tracks for case where global event contains only a KL or EMR track
    if (ImportedSciFiTrack == NULL && TOFTrackArray.empty() &&
	(KLTrack != NULL || EMRTrack != NULL)) {
      if (KLTrack != NULL && EMRTrack == NULL) {
	MAUS::DataStructure::Global::Track* GlobalTrack = KLTrack->Clone();
	GlobalTrack->set_mapper_name(mapper_name);
	global_event->add_track_recursive(GlobalTrack);
      } else if (KLTrack == NULL && EMRTrack != NULL) {
	MAUS::DataStructure::Global::Track* GlobalTrack = EMRTrack->Clone();
	GlobalTrack->set_mapper_name(mapper_name);
	GlobalTrack->set_emr_range_primary(EMRTrack->get_emr_range_primary());
	GlobalTrack->set_emr_plane_density(EMRTrack->get_emr_plane_density());
	GlobalTrack->set_emr_range_secondary(EMRTrack->get_emr_range_secondary());
	global_event->add_track_recursive(GlobalTrack);
      } else {
	MAUS::DataStructure::Global::Track* GlobalTrack = KLTrack->Clone();
	std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	  tempEMRTrackPointArray = EMRTrack->GetTrackPoints();
	for (unsigned int m = 0; m < tempEMRTrackPointArray.size(); m++) {
	  MAUS::DataStructure::Global::TrackPoint* tempEMRTrackPoint =
	    const_cast<MAUS::DataStructure::Global::TrackPoint*>
	    (tempEMRTrackPointArray[m]);
	  tempEMRTrackPoint->set_mapper_name(mapper_name);
	  GlobalTrack->AddTrackPoint(tempEMRTrackPoint);
	}
	GlobalTrack->set_emr_range_primary(EMRTrack->get_emr_range_primary());
	GlobalTrack->set_emr_plane_density(EMRTrack->get_emr_plane_density());
	GlobalTrack->set_emr_range_secondary(EMRTrack->get_emr_range_secondary());
	GlobalTrack->set_mapper_name(mapper_name);
	global_event->add_track_recursive(GlobalTrack);
      }
    }
  }

    void TrackMatching::MakeTOFTracks(MAUS::GlobalEvent* global_event,
				      std::vector<MAUS::DataStructure::Global::SpacePoint*>
				      *GlobalSpacePointArray,
				      MAUS::DataStructure::Global::TrackPArray& TOFTrackArray,
				      std::string mapper_name) {

      std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF0tp;
      std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF1tp;
      std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF2tp;

      std::vector<MAUS::DataStructure::Global::TrackPoint*> tempTOF0tp;
      std::vector<MAUS::DataStructure::Global::TrackPoint*> tempTOF2tp;
      double TOF01offset = 30;
      double TOF12offset = 35;
      double allowance = 8.0;
      for (unsigned int i = 0; i < GlobalSpacePointArray->size(); ++i) {
	MAUS::DataStructure::Global::SpacePoint* sp = GlobalSpacePointArray->at(i);
	if (!sp) {
	  continue;
	}
	MAUS::DataStructure::Global::TrackPoint* tp0;
	MAUS::DataStructure::Global::TrackPoint* tp1;
	MAUS::DataStructure::Global::TrackPoint* tp2;
	if (sp->get_detector() == MAUS::DataStructure::Global::kTOF0) {
	  tp0 = new MAUS::DataStructure::Global::TrackPoint(sp);
	  TOF0tp.push_back(tp0);
	} else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF1) {
	  tp1 = new MAUS::DataStructure::Global::TrackPoint(sp);
	  TOF1tp.push_back(tp1);
	} else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF2) {
	  tp2 = new MAUS::DataStructure::Global::TrackPoint(sp);
	  TOF2tp.push_back(tp2);
	} else {
	  continue;
	}
      }

      for (unsigned int i = 0; i < TOF1tp.size(); ++i) {
	MAUS::DataStructure::Global::Track* TOFtrack =
	  new MAUS::DataStructure::Global::Track();
	TOFtrack->set_mapper_name(mapper_name);
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
	  TOF1tp[i]->set_mapper_name(mapper_name);
	  TOFtrack->AddTrackPoint(TOF1tp[i]);
	  global_event->add_track_point_recursive(TOF1tp[i]);
	  if (tempTOF0tp.size() == 1) {
	    tempTOF0tp[0]->set_mapper_name(mapper_name);
	    TOFtrack->AddTrackPoint(tempTOF0tp[0]);
	    global_event->add_track_point_recursive(tempTOF0tp[0]);
	  }
	  if (tempTOF2tp.size() == 1) {
	    tempTOF2tp[0]->set_mapper_name(mapper_name);
	    TOFtrack->AddTrackPoint(tempTOF2tp[0]);
	    global_event->add_track_point_recursive(tempTOF2tp[0]);
	  }
	  TOFTrackArray.push_back(TOFtrack);
	} else {
	  Squeak::mout(Squeak::debug) << "Global event returned multiple possible"
				      << " TOF0 and/or TOF2 space points that "
				      << "could not be separated into tracks."
				      << std::endl;
	}

	tempTOF0tp.clear();
	tempTOF2tp.clear();
      }
    }

  void TrackMatching::MakeKLTracks(MAUS::GlobalEvent* global_event,
				   std::vector<MAUS::DataStructure::Global::SpacePoint*>
				   *GlobalSpacePointArray,
				   MAUS::DataStructure::Global::Track* KLTrack,
				   std::string mapper_name) {

    std::vector<MAUS::DataStructure::Global::TrackPoint*> KLtp;

      for (unsigned int i = 0; i < GlobalSpacePointArray->size(); ++i) {
	MAUS::DataStructure::Global::SpacePoint* sp = GlobalSpacePointArray->at(i);
	if (!sp) {
	  continue;
	}
	MAUS::DataStructure::Global::TrackPoint* tp;
	if (sp->get_detector() == MAUS::DataStructure::Global::kCalorimeter) {
	  tp = new MAUS::DataStructure::Global::TrackPoint(sp);
	  KLtp.push_back(tp);
	} else {
	  continue;
	}
      }

      for (unsigned int i = 0; i < KLtp.size(); ++i) {
        KLTrack->set_mapper_name(mapper_name);
	KLtp[i]->set_mapper_name(mapper_name);
	KLTrack->AddTrackPoint(KLtp[i]);
	global_event->add_track_point_recursive(KLtp[i]);
      }
    }

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

