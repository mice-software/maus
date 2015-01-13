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
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/legacy/BeamTools/BTTracker.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"

#include <algorithm>

#include "Interface/Squeak.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

void TrackMatching::USTrack(MAUS::GlobalEvent* global_event,
                            std::string mapper_name) {
  // import tracks already created by tracker recon
  MAUS::DataStructure::Global::TrackPArray *imported_tracks =
    global_event->get_tracks();
  MAUS::DataStructure::Global::TrackPArray::iterator imported_track_iter;
  MAUS::DataStructure::Global::TrackPArray *scifi_track_array =
      new MAUS::DataStructure::Global::TrackPArray();
  for (imported_track_iter = imported_tracks->begin();
       imported_track_iter != imported_tracks->end();
       ++imported_track_iter) {
    MAUS::DataStructure::Global::Track* imported_track =
        (*imported_track_iter);
    if (imported_track->HasDetector(MAUS::DataStructure::Global::kTracker0)) {
      scifi_track_array->push_back(imported_track);
    }
  }
  // Now TOF0 & 1
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF0_tp;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF1_tp;
  // Loop over spacepoints in event and assign to correct detectors
  std::vector<MAUS::DataStructure::Global::SpacePoint*>
    *global_spacepoint_array = global_event->get_space_points();
  for (unsigned int i = 0; i < global_spacepoint_array->size(); ++i) {
    MAUS::DataStructure::Global::SpacePoint* sp = global_spacepoint_array->at(i);
    if (!sp) {
      continue;
    }
    MAUS::DataStructure::Global::TrackPoint* tp0;
    MAUS::DataStructure::Global::TrackPoint* tp1;
    if (sp->get_detector() == MAUS::DataStructure::Global::kTOF0) {
      //~ Squeak::mout(Squeak::error) << "Position: " << sp->get_position().T() << ", " << sp->get_position().X() << ", " << sp->get_position().Y() << ", " << sp->get_position().Z() << std::endl;
      //~ Squeak::mout(Squeak::error) << "Momentum: " << sp->get_momentum().T() << ", " << sp->get_momentum().X() << ", " << sp->get_momentum().Y() << ", " << sp->get_momentum().Z() << std::endl;
      tp0 = new MAUS::DataStructure::Global::TrackPoint(sp);
      TOF0_tp.push_back(tp0);
    } else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF1) {
      //~ Squeak::mout(Squeak::error) << "Position: " << sp->get_position().T() << ", " << sp->get_position().X() << ", " << sp->get_position().Y() << ", " << sp->get_position().Z() << std::endl;
      //~ Squeak::mout(Squeak::error) << "Momentum: " << sp->get_momentum().T() << ", " << sp->get_momentum().X() << ", " << sp->get_momentum().Y() << ", " << sp->get_momentum().Z() << std::endl;
      tp1 = new MAUS::DataStructure::Global::TrackPoint(sp);
      TOF1_tp.push_back(tp1);
    } else {
      continue;
    }
  }
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  MAUS::DataStructure::Global::TrackPArray::iterator scifi_track_iter;
  // Iterate over all Tracker 0 Tracks (typically 1)
  for (scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    MAUS::DataStructure::Global::Track* tracker0_track =
        (*scifi_track_iter);
    // Create primary chain with tracker Track as primary Track
    MAUS::DataStructure::Global::PrimaryChain* upstream_primary_chain =
        new MAUS::DataStructure::Global::PrimaryChain(mapper_name);
    upstream_primary_chain->AddPrimaryTrack(tracker0_track);
    // Extract four-position and momentum from first track point (i.e. most
    // upstream) and get the charge hypothesis
    TLorentzVector position = tracker0_track->GetTrackPoints()[4]->get_position();
    TLorentzVector momentum = tracker0_track->GetTrackPoints()[4]->get_momentum();
    int charge_hypothesis = tracker0_track->get_charge();
    // Create the list of PIDs for which we want to create hypothesis tracks
    std::vector<MAUS::DataStructure::Global::PID> pids;
    if (charge_hypothesis != -1) {
      pids.push_back(MAUS::DataStructure::Global::kEPlus);
      pids.push_back(MAUS::DataStructure::Global::kMuPlus);
      pids.push_back(MAUS::DataStructure::Global::kPiPlus);
    }
    if (charge_hypothesis != 1) {
      pids.push_back(MAUS::DataStructure::Global::kEMinus);
      pids.push_back(MAUS::DataStructure::Global::kMuMinus);
      pids.push_back(MAUS::DataStructure::Global::kPiMinus);
    }
    // Iterate over all possible PIDs and create an hypothesis track for each
    for (size_t i = 0; i < pids.size(); i++) {
      double mass = Particle::GetInstance().GetMass(pids[i]);
      int charge = Particle::GetInstance().GetCharge(pids[i]);
      //~ Squeak::mout(Squeak::error) << Particle::GetInstance().GetName(pids[i]) << " "
                                  //~ << Particle::GetInstance().GetMass(pids[i]) << " "
                                  //~ << Particle::GetInstance().GetCharge(pids[i]) << "\n";
      MAUS::DataStructure::Global::Track* hypothesis_track =
          new MAUS::DataStructure::Global::Track();
      hypothesis_track->set_mapper_name(mapper_name);
      hypothesis_track->set_pid(pids[i]);
      // Check whether a hit in a given detector is compatible with the tracker
      // plane hit and particle hypothesis and if yes add it to the track.
      // For consistency we want trackpoints to be z-ordered so we start at
      // TOF0
      // Momentum, energy, and charge are inverted
      double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
      Squeak::mout(Squeak::error) << energy << "#\n";
      double x_in_TOF0[] = {0., position.X(), position.Y(), position.Z(),
                       -energy, -momentum.X(), -momentum.Y(), -momentum.Z()};
      BTTracker::integrate(2773.0, x_in_TOF0, field, BTTracker::z, -10.0, -charge);
      for (size_t j = 4; j < 8; ++j) x_in_TOF0[j] *= -1;
      //~ Squeak::mout(Squeak::error) << x_in_TOF0[1] << "\t" <<  x_in_TOF0[2] << "\t" <<  x_in_TOF0[3] << "\t" << x_in_TOF0[5] << "\t" <<  x_in_TOF0[6] << "\t" <<  x_in_TOF0[7] << "\n";
      for (size_t j = 0; j < TOF0_tp.size(); j++) {
        // We don't need a whole slab width as tolerance, should never be much
        // more than half a slab out from the center of the slab
        //~ Squeak::mout(Squeak::error) << x_in_TOF0[1] << " " << TOF0_tp[j]->get_position().X() << "\n"
                                    //~ << x_in_TOF0[2] << " " << TOF0_tp[j]->get_position().Y() << "\n";
        if (almostEquals(x_in_TOF0[1], TOF0_tp[j]->get_position().X(), 30) and
            almostEquals(x_in_TOF0[2], TOF0_tp[j]->get_position().Y(), 30)) {
          hypothesis_track->AddTrackPoint(TOF0_tp[j]);
          //~ Squeak::mout(Squeak::error) << "TOF0 Match\n";
        }
      }
      // TOF1
      double x_in_TOF1[] = {0., position.X(), position.Y(), position.Z(),
                       -energy, -momentum.X(), -momentum.Y(), -momentum.Z()};
      //~ Squeak::mout(Squeak::error) << "TR\n" << x_in_TOF1[1] << "\t" <<  x_in_TOF1[2] << "\t" <<  x_in_TOF1[3] << "\t" << x_in_TOF1[5] << "\t" <<  x_in_TOF1[6] << "\t" <<  x_in_TOF1[7] << "\n";
      BTTracker::integrate(10572.0, x_in_TOF1, field, BTTracker::z, -10.0, -charge);
      for (size_t j = 4; j < 8; ++j) x_in_TOF1[j] *= -1;
      //~ Squeak::mout(Squeak::error) << "RK\n" << x_in_TOF1[1] << "\t" <<  x_in_TOF1[2] << "\t" <<  x_in_TOF1[3] << "\t" << x_in_TOF1[5] << "\t" <<  x_in_TOF1[6] << "\t" <<  x_in_TOF1[7] << " " << pids[1] << "\n";
      for (size_t j = 0; j < TOF1_tp.size(); j++) {
        // We don't need a whole slab width as tolerance, should never be much
        // more than half a slab out from the center of the slab
        //~ Squeak::mout(Squeak::error) << x_in_TOF1[1] << " " << TOF1_tp[j]->get_position().X() << "\n"
                                    //~ << x_in_TOF1[2] << " " << TOF1_tp[j]->get_position().Y() << "\n";
        if (almostEquals(x_in_TOF1[1], TOF1_tp[j]->get_position().X(), 30) and
            almostEquals(x_in_TOF1[2], TOF1_tp[j]->get_position().Y(), 30)) {
          hypothesis_track->AddTrackPoint(TOF1_tp[j]);
          Squeak::mout(Squeak::error) << "TOF1 Match\n";
        }
      }

      // Now we fill the track with trackpoints from the tracker with energy
      // calculated from p and m
      std::vector<const MAUS::DataStructure::Global::TrackPoint*>
          tracker0_trackpoints = tracker0_track->GetTrackPoints();

      for (size_t j = 0; j < tracker0_trackpoints.size(); j++) {
        MAUS::DataStructure::Global::TrackPoint* tracker0_tp =
            tracker0_trackpoints[j]->Clone();
        TLorentzVector momentum = tracker0_tp->get_momentum();
        double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
        momentum.SetE(energy);
        tracker0_tp->set_momentum(momentum);
        hypothesis_track->AddTrackPoint(tracker0_tp);
      }
      //~ std::vector<const MAUS::DataStructure::Global::TrackPoint*> temp =
      //~ Squeak::mout(Squeak::error) << hypothesis_track->GetTrackPoints().size() << "\n";
      upstream_primary_chain->AddTrack(hypothesis_track, tracker0_track);
    }
    global_event->add_primary_chain_recursive(upstream_primary_chain);
  }
}


void TrackMatching::DSTrack(MAUS::GlobalEvent* global_event,
                            std::string mapper_name) {
  // import tracks already created by tracker recon
  MAUS::DataStructure::Global::TrackPArray *imported_tracks =
    global_event->get_tracks();
  MAUS::DataStructure::Global::TrackPArray::iterator imported_track_iter;
  MAUS::DataStructure::Global::TrackPArray *scifi_track_array =
      new MAUS::DataStructure::Global::TrackPArray();
  for (imported_track_iter = imported_tracks->begin();
       imported_track_iter != imported_tracks->end();
       ++imported_track_iter) {
    MAUS::DataStructure::Global::Track* imported_track =
        (*imported_track_iter);
    if (imported_track->HasDetector(MAUS::DataStructure::Global::kTracker1)) {
      scifi_track_array->push_back(imported_track);
    }
  }
  // TOF2
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF2_tp;
  // Loop over spacepoints in event and assign to correct detectors
  std::vector<MAUS::DataStructure::Global::SpacePoint*>
    *global_spacepoint_array = global_event->get_space_points();
  for (unsigned int i = 0; i < global_spacepoint_array->size(); ++i) {
    MAUS::DataStructure::Global::SpacePoint* sp = global_spacepoint_array->at(i);
    if (!sp) {
      continue;
    }
    MAUS::DataStructure::Global::TrackPoint* tp2;
    if (sp->get_detector() == MAUS::DataStructure::Global::kTOF2) {
      tp2 = new MAUS::DataStructure::Global::TrackPoint(sp);
      TOF2_tp.push_back(tp2);
    } else {
      continue;
    }
  }
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  MAUS::DataStructure::Global::TrackPArray::iterator scifi_track_iter;
  // Iterate over all Tracker 1 Tracks (typically 1)
  for (scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    MAUS::DataStructure::Global::Track* tracker1_track =
        (*scifi_track_iter);
    // Create primary chain with tracker Track as primary Track
    MAUS::DataStructure::Global::PrimaryChain* downstream_primary_chain =
        new MAUS::DataStructure::Global::PrimaryChain(mapper_name);
    downstream_primary_chain->AddPrimaryTrack(tracker1_track);
    // Extract four-position and momentum from last track point (i.e. most
    // downstream
    // ### Will need to do this in a cleaner way later, check by station or sth.
    TLorentzVector position = tracker1_track->GetTrackPoints()[tracker1_track->GetTrackPoints().size()-1]->get_position();
    TLorentzVector momentum = tracker1_track->GetTrackPoints()[tracker1_track->GetTrackPoints().size()-1]->get_momentum();
    Squeak::mout(Squeak::error) << position.X() << " " << momentum.X() << "\n";
    int charge_hypothesis = tracker1_track->get_charge();
    // Create the list of PIDs for which we want to create hypothesis tracks
    std::vector<MAUS::DataStructure::Global::PID> pids;
    if (charge_hypothesis != -1) {
      pids.push_back(MAUS::DataStructure::Global::kEPlus);
      pids.push_back(MAUS::DataStructure::Global::kMuPlus);
      pids.push_back(MAUS::DataStructure::Global::kPiPlus);
    }
    if (charge_hypothesis != 1) {
      pids.push_back(MAUS::DataStructure::Global::kEMinus);
      pids.push_back(MAUS::DataStructure::Global::kMuMinus);
      pids.push_back(MAUS::DataStructure::Global::kPiMinus);
    }
    // Iterate over all possible PIDs and create an hypothesis track for each
    for (size_t i = 0; i < pids.size(); i++) {
      double mass = Particle::GetInstance().GetMass(pids[i]);
      int charge = Particle::GetInstance().GetCharge(pids[i]);
      MAUS::DataStructure::Global::Track* hypothesis_track =
          new MAUS::DataStructure::Global::Track();
      hypothesis_track->set_mapper_name(mapper_name);
      hypothesis_track->set_pid(pids[i]);
      // Check whether a hit in a given detector is compatible with the tracker
      // plane hit and particle hypothesis and if yes add it to the track.
      // This time we start at the Tracker
      // Now we fill the track with trackpoints from the tracker with energy
      // calculated from p and m
      std::vector<const MAUS::DataStructure::Global::TrackPoint*>
          tracker1_trackpoints = tracker1_track->GetTrackPoints();

      for (size_t j = 0; j < tracker1_trackpoints.size(); j++) {
        MAUS::DataStructure::Global::TrackPoint* tracker1_tp =
            tracker1_trackpoints[j]->Clone();
        TLorentzVector momentum = tracker1_tp->get_momentum();
        double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
        momentum.SetE(energy);
        tracker1_tp->set_momentum(momentum);
        hypothesis_track->AddTrackPoint(tracker1_tp);
      }
      // TOF2
      double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
      double x_in_TOF2[] = {0., position.X(), position.Y(), position.Z(),
                            energy, momentum.X(), momentum.Y(), momentum.Z()};
      BTTracker::integrate(17931.0, x_in_TOF2, field, BTTracker::z, 10.0, charge);
      for (size_t j = 0; j < TOF2_tp.size(); j++) {
        // We don't need a whole slab width as tolerance, should never be much
        // more than half a slab out from the center of the slab
        //~ Squeak::mout(Squeak::error) << x_in_TOF2[1] << " " << TOF2_tp[j]->get_position().X() << "\n"
                                    //~ << x_in_TOF2[2] << " " << TOF2_tp[j]->get_position().Y() << "\n";
        if (almostEquals(x_in_TOF2[1], TOF2_tp[j]->get_position().X(), 45) and
            almostEquals(x_in_TOF2[2], TOF2_tp[j]->get_position().Y(), 45)) {
          hypothesis_track->AddTrackPoint(TOF2_tp[j]);
          Squeak::mout(Squeak::error) << "TOF2 Match\n";
        }
      }

      //~ std::vector<const MAUS::DataStructure::Global::TrackPoint*> temp =
      //~ Squeak::mout(Squeak::error) << hypothesis_track->GetTrackPoints().size() << "\n";
      downstream_primary_chain->AddTrack(hypothesis_track, tracker1_track);
    }
    global_event->add_primary_chain_recursive(downstream_primary_chain);
  }
}


void TrackMatching::FormTracks(MAUS::GlobalEvent* global_event, std::string
    mapper_name) {
  if (!global_event) {
    throw(Exception(Exception::recoverable,
      "Trying to import an empty global event.",
      "MapCppGlobalPID::TrackMatching"));
  }
//~ Squeak::mout(Squeak::error) << "#1#\n";
  MAUS::DataStructure::Global::TrackPArray *ImportedTracks =
    global_event->get_tracks();
  MAUS::DataStructure::Global::TrackPArray::iterator ImportedTrackIterator;
  MAUS::DataStructure::Global::TrackPArray *SciFiTrackArray =
    new MAUS::DataStructure::Global::TrackPArray();
  for (ImportedTrackIterator = ImportedTracks->begin();
       ImportedTrackIterator != ImportedTracks->end();
       ++ImportedTrackIterator) {
    MAUS::DataStructure::Global::Track* ImportedTrack =
        (*ImportedTrackIterator);
    if (ImportedTrack->HasDetector(MAUS::DataStructure::Global::kTracker0)) {
      SciFiTrackArray->push_back(ImportedTrack);
    }
  }
//~ Squeak::mout(Squeak::error) << "#2#\n";
  std::vector<MAUS::DataStructure::Global::SpacePoint*>
    *GlobalSpacePointArray = global_event->get_space_points();
  MAUS::DataStructure::Global::TrackPArray TOFTrackArray;
  MakeTOFTracks(global_event, GlobalSpacePointArray, TOFTrackArray);
//~ Squeak::mout(Squeak::error) << "#3#\n";
  // Adding global tracks for case where global event contains both SciFi and TOF tracks
  if (!SciFiTrackArray->empty() && !TOFTrackArray.empty()) {
    for (unsigned int i = 0; i < SciFiTrackArray->size(); i++) {
      for (unsigned int j = 0; j < TOFTrackArray.size(); j++) {
        MAUS::DataStructure::Global::Track* GlobalTrack =
            new MAUS::DataStructure::Global::Track();
        GlobalTrack->set_mapper_name(mapper_name);
        MAUS::DataStructure::Global::Track* tempSciFiTrack =
            SciFiTrackArray->at(i);
        MAUS::DataStructure::Global::Track* tempTOFTrack = TOFTrackArray[j];
        std::vector<const MAUS::DataStructure::Global::TrackPoint*>
            tempSciFiTrackPointArray = tempSciFiTrack->GetTrackPoints();
        std::vector<const MAUS::DataStructure::Global::TrackPoint*>
            tempTOFTrackPointArray = tempTOFTrack->GetTrackPoints();
        for (unsigned int k = 0; k < tempTOFTrackPointArray.size(); k++) {
          MAUS::DataStructure::Global::TrackPoint* tempTOFTrackPoint =
              const_cast<MAUS::DataStructure::Global::TrackPoint*>
              (tempTOFTrackPointArray[k]);
          tempTOFTrackPoint->set_mapper_name(mapper_name);
          GlobalTrack->AddTrackPoint(tempTOFTrackPoint);
        }
        for (unsigned int k = 0; k < tempSciFiTrackPointArray.size(); k++) {
          MAUS::DataStructure::Global::TrackPoint* tempSciFiTrackPoint =
              const_cast<MAUS::DataStructure::Global::TrackPoint*>
              (tempSciFiTrackPointArray[k]);
          tempSciFiTrackPoint->set_mapper_name(mapper_name);
          GlobalTrack->AddTrackPoint(tempSciFiTrackPoint);
        }
        global_event->add_track_recursive(GlobalTrack);
      }
    }
  }
//~ Squeak::mout(Squeak::error) << "#4#\n";
  // Adding global tracks for case where global event contains only TOF tracks
  if (SciFiTrackArray->empty() && !TOFTrackArray.empty()) {
    for (unsigned int i = 0; i < TOFTrackArray.size(); i++) {
      MAUS::DataStructure::Global::Track* GlobalTrack =
          new MAUS::DataStructure::Global::Track();
      GlobalTrack->set_mapper_name(mapper_name);
      MAUS::DataStructure::Global::Track* tempTOFTrack = TOFTrackArray[i];
      std::vector<const MAUS::DataStructure::Global::TrackPoint*>
          tempTOFTrackPointArray = tempTOFTrack->GetTrackPoints();
      for (unsigned int j = 0; j < tempTOFTrackPointArray.size(); j++) {
        MAUS::DataStructure::Global::TrackPoint* tempTOFTrackPoint =
            const_cast<MAUS::DataStructure::Global::TrackPoint*>
            (tempTOFTrackPointArray[j]);
        GlobalTrack->AddTrackPoint(tempTOFTrackPoint);
      }
      global_event->add_track_recursive(GlobalTrack);
    }
  }

  // Adding global tracks for case where global event contains only SciFi tracks
  if (!SciFiTrackArray->empty() && TOFTrackArray.empty()) {
    for (unsigned int i = 0; i < SciFiTrackArray->size(); i++) {
      MAUS::DataStructure::Global::Track* GlobalTrack =
          new MAUS::DataStructure::Global::Track();
      GlobalTrack->set_mapper_name(mapper_name);
      MAUS::DataStructure::Global::Track* tempSciFiTrack =
          SciFiTrackArray->at(i);
      std::vector<const MAUS::DataStructure::Global::TrackPoint*>
          tempSciFiTrackPointArray = tempSciFiTrack->GetTrackPoints();
      for (unsigned int j = 0; j < tempSciFiTrackPointArray.size(); j++) {
        MAUS::DataStructure::Global::TrackPoint* tempSciFiTrackPoint =
            const_cast<MAUS::DataStructure::Global::TrackPoint*>
            (tempSciFiTrackPointArray[j]);
        tempSciFiTrackPoint->set_mapper_name(mapper_name);
        GlobalTrack->AddTrackPoint(tempSciFiTrackPoint);
      }
      global_event->add_track_recursive(GlobalTrack);
    }
  }
}

// Make a track out of TOF hits
void TrackMatching::MakeTOFTracks(
    MAUS::GlobalEvent* global_event,
    std::vector<MAUS::DataStructure::Global::SpacePoint*>
        *GlobalSpacePointArray,
    MAUS::DataStructure::Global::TrackPArray& TOFTrackArray) {
//~ Squeak::mout(Squeak::error) << "#MakeTOFTracks#\n";
  std::string local_mapper_name = "GlobalTOFTrack";
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF0tp;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF1tp;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF2tp;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> tempTOF0tp;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> tempTOF2tp;
  // Expected time difference between TOFs
  double TOF01offset = 30;
  double TOF12offset = 35;
  double allowance = 8.0;
  // Loop over spacepoints and put them into track point vectors for the
  // corresponding TOF
  for (unsigned int i = 0; i < GlobalSpacePointArray->size(); ++i) {
    MAUS::DataStructure::Global::SpacePoint* sp = GlobalSpacePointArray->at(i);
    if (!sp) {
      continue;
    }
    //~ Squeak::mout(Squeak::error) << "#5#\n";
    MAUS::DataStructure::Global::TrackPoint* tp0;
    MAUS::DataStructure::Global::TrackPoint* tp1;
    MAUS::DataStructure::Global::TrackPoint* tp2;
    if (sp->get_detector() == MAUS::DataStructure::Global::kTOF0) {
      Squeak::mout(Squeak::error) << sp->get_position().X() << " " << sp->get_position().Y() << " " << sp->get_position().Z() << std::endl;
      tp0 = new MAUS::DataStructure::Global::TrackPoint(sp);
      TOF0tp.push_back(tp0);
    } else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF1) {
      Squeak::mout(Squeak::error) << sp->get_position().X() << " " << sp->get_position().Y() << " " << sp->get_position().Z() << std::endl;
      tp1 = new MAUS::DataStructure::Global::TrackPoint(sp);
      TOF1tp.push_back(tp1);
    } else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF2) {
      Squeak::mout(Squeak::error) << sp->get_position().X() << " " << sp->get_position().Y() << " " << sp->get_position().Z() << std::endl;
      tp2 = new MAUS::DataStructure::Global::TrackPoint(sp);
      TOF2tp.push_back(tp2);
    } else {
      continue;
    }
  }
  //~ Squeak::mout(Squeak::error) << "#~#" << TOF1tp.size() << std::endl;
  // Loop over hits in TOF1, then check time difference to TOF0 and TOF2 hits
  for (unsigned int i = 0; i < TOF1tp.size(); ++i) {
    MAUS::DataStructure::Global::Track* TOFtrack =
        new MAUS::DataStructure::Global::Track();
    TOFtrack->set_mapper_name(local_mapper_name);
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
    // Check if more than 1 hit per TOF, in which case this algorithm fails
    if (tempTOF0tp.size() < 2 && tempTOF2tp.size() < 2) {
      TOF1tp[i]->set_mapper_name(local_mapper_name);
      TOFtrack->AddTrackPoint(TOF1tp[i]);
      global_event->add_track_point_recursive(TOF1tp[i]);
      // If there is a hit in TOF0 or 1, it get's added, otherwise the detector
      // is omitted
      if (tempTOF0tp.size() == 1) {
        tempTOF0tp[0]->set_mapper_name(local_mapper_name);
        TOFtrack->AddTrackPoint(tempTOF0tp[0]);
        global_event->add_track_point_recursive(tempTOF0tp[0]);
      }
      if (tempTOF2tp.size() == 1) {
        tempTOF2tp[0]->set_mapper_name(local_mapper_name);
        TOFtrack->AddTrackPoint(tempTOF2tp[0]);
        global_event->add_track_point_recursive(tempTOF2tp[0]);
      }
      TOFTrackArray.push_back(TOFtrack);
    } else {
      Squeak::mout(Squeak::error) << "Global event returned multiple possible"
                                  << " TOF0 and/or TOF2 space points that "
                                  << "could not be separated into tracks."
                                  << std::endl;
    }
    tempTOF0tp.clear();
    tempTOF2tp.clear();
  }
}

bool TrackMatching::almostEquals(double var1, double var2, double tolerance) const {
  if ((var1 < var2 + tolerance) and (var1 > var2 - tolerance)) {
    return true;
  } else {
    return false;
  }
}

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
