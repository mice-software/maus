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
  // import existing tracks from global event (here Tracker0)
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
    // Check that track is from Tracker0
    if (imported_track->HasDetector(MAUS::DataStructure::Global::kTracker0)) {
      scifi_track_array->push_back(imported_track);
    }
  }
  // Import TrackPoints from TOF0/1 and create spacepoints from them
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
    // Create track points from space points
    MAUS::DataStructure::Global::TrackPoint* tp0;
    MAUS::DataStructure::Global::TrackPoint* tp1;
    if (sp->get_detector() == MAUS::DataStructure::Global::kTOF0) {
      //~ Squeak::mout(Squeak::error) << "Position: " << sp->get_position().T() << ", " << sp->get_position().X() << ", " << sp->get_position().Y() << ", " << sp->get_position().Z() << std::endl;
      tp0 = new MAUS::DataStructure::Global::TrackPoint(sp);
      tp0->set_mapper_name(mapper_name);
      TOF0_tp.push_back(tp0);
    } else if (sp->get_detector() == MAUS::DataStructure::Global::kTOF1) {
      //~ Squeak::mout(Squeak::error) << "Position: " << sp->get_position().T() << ", " << sp->get_position().X() << ", " << sp->get_position().Y() << ", " << sp->get_position().Z() << std::endl;
      tp1 = new MAUS::DataStructure::Global::TrackPoint(sp);
      tp1->set_mapper_name(mapper_name);
      TOF1_tp.push_back(tp1);
    } else {
      continue;
    }
  }

  // Load the magnetic field for RK4 propagation
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  MAUS::DataStructure::Global::TrackPArray::iterator scifi_track_iter;
  // Iterate over all Tracker0 Tracks (typically 1)
  for (scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    MAUS::DataStructure::Global::Track* tracker0_track =
        (*scifi_track_iter);
    // Create primary chain with tracker Track as primary Track
    // TODO primary chain once DS is fixed
    //~ MAUS::DataStructure::Global::PrimaryChain* upstream_primary_chain =
        //~ new MAUS::DataStructure::Global::PrimaryChain(mapper_name);
    //~ upstream_primary_chain->AddPrimaryTrack(tracker0_track);
    // Extract four-position and momentum from first track point (i.e. most
    // upstream) and get the charge hypothesis
    TLorentzVector position = tracker0_track->GetTrackPoints()[4]->get_position();
    TLorentzVector momentum = tracker0_track->GetTrackPoints()[4]->get_momentum();
    int charge_hypothesis = tracker0_track->get_charge();
    // Create the list of PIDs for which we want to create hypothesis tracks
    // If charge hypothesis is given by tracker recon, 3, else, 6
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
    //~ std::cerr << pids.size() << "PIDS\n";
    // Iterate over all possible PIDs and create an hypothesis track for each
    for (size_t i = 0; i < pids.size(); i++) {
      double mass = Particle::GetInstance().GetMass(pids[i]);
      int charge = Particle::GetInstance().GetCharge(pids[i]);
      MAUS::DataStructure::Global::Track* hypothesis_track =
          new MAUS::DataStructure::Global::Track();
      hypothesis_track->set_mapper_name("MapCppGlobalTrackMatching-US");
      hypothesis_track->set_pid(pids[i]);
      // Check whether a hit in a given detector is compatible with the tracker
      // plane hit and particle hypothesis and if yes add it to the track.
      // For consistency we want trackpoints to be z-ordered so we start at
      // TOF0
      // Momentum, energy, and charge are inverted to propagate backwards
      double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
      double x_in_TOF0[] = {0., position.X(), position.Y(), position.Z(),
                       -energy, -momentum.X(), -momentum.Y(), -momentum.Z()};
      BTTracker::integrate(2773.0, x_in_TOF0, field, BTTracker::z, -10.0, -charge);
      for (size_t j = 4; j < 8; ++j) x_in_TOF0[j] *= -1;
      //~ Squeak::mout(Squeak::error) << x_in_TOF0[1] << "\t" <<  x_in_TOF0[2] << "\t" <<  x_in_TOF0[3] << "\t" << x_in_TOF0[5] << "\t" <<  x_in_TOF0[6] << "\t" <<  x_in_TOF0[7] << "\n";
      for (size_t j = 0; j < TOF0_tp.size(); j++) {
        // We don't need a whole slab width as tolerance, should never be much
        // more than half a slab out from the center of the slab
        // TODO Get from covariance matrix once full physics processes implemented
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
        // TODO tolerance via covariance matrix
        //~ Squeak::mout(Squeak::error) << x_in_TOF1[1] << " " << TOF1_tp[j]->get_position().X() << "\n"
                                    //~ << x_in_TOF1[2] << " " << TOF1_tp[j]->get_position().Y() << "\n";
        if (almostEquals(x_in_TOF1[1], TOF1_tp[j]->get_position().X(), 40) and
            almostEquals(x_in_TOF1[2], TOF1_tp[j]->get_position().Y(), 40)) {
          hypothesis_track->AddTrackPoint(TOF1_tp[j]);
          //~ Squeak::mout(Squeak::error) << "TOF1 Match\n";
        }
      }
      // Now we fill the track with trackpoints from the tracker with energy
      // calculated from p and m, trackpoints are cloned as we want everything
      // in the hypothesis track to be "fresh"
      std::vector<const MAUS::DataStructure::Global::TrackPoint*>
          tracker0_trackpoints = tracker0_track->GetTrackPoints();
      for (size_t j = 0; j < tracker0_trackpoints.size(); j++) {
        MAUS::DataStructure::Global::TrackPoint* tracker0_tp =
            tracker0_trackpoints[j]->Clone();
        tracker0_tp->set_mapper_name(mapper_name);
        TLorentzVector momentum = tracker0_tp->get_momentum();
        double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
        momentum.SetE(energy);
        tracker0_tp->set_momentum(momentum);
        hypothesis_track->AddTrackPoint(tracker0_tp);
      }
      //~ upstream_primary_chain->AddTrack(hypothesis_track, tracker0_track);
      //~ std::cerr << "Mapper: " << hypothesis_track->get_mapper_name() << " " << hypothesis_track->get_pid() << "\n";
      global_event->add_track_recursive(hypothesis_track);
    }
    //~ global_event->add_primary_chain(upstream_primary_chain);
    global_event->add_track_recursive(tracker0_track);
  }
}


void TrackMatching::DSTrack(MAUS::GlobalEvent* global_event,
                            std::string mapper_name) {
  // import existing tracks from global event (here Tracker1 & EMR)
  MAUS::DataStructure::Global::TrackPArray *imported_tracks =
    global_event->get_tracks();
  MAUS::DataStructure::Global::TrackPArray::iterator imported_track_iter;
  MAUS::DataStructure::Global::TrackPArray *scifi_track_array =
      new MAUS::DataStructure::Global::TrackPArray();
  MAUS::DataStructure::Global::TrackPArray *emr_track_array =
      new MAUS::DataStructure::Global::TrackPArray();
  for (imported_track_iter = imported_tracks->begin();
       imported_track_iter != imported_tracks->end();
       ++imported_track_iter) {
    MAUS::DataStructure::Global::Track* imported_track =
        (*imported_track_iter);
    // Sort tracks by whether they are Tracker1 or EMR
    if (imported_track->HasDetector(MAUS::DataStructure::Global::kTracker1)) {
      scifi_track_array->push_back(imported_track);
    }
    if (imported_track->HasDetector(MAUS::DataStructure::Global::kEMR) and
        imported_track->get_emr_range_secondary() < 0.001) {
      emr_track_array->push_back(imported_track);
      //~ std::vector<const MAUS::DataStructure::Global::TrackPoint*> EMR_tp =
          //~ imported_track->GetTrackPoints();
      //~ for (size_t n = 0; n < EMR_tp.size(); ++n) {
        //~ std::cerr << EMR_tp[n]->get_position().X() << " " << EMR_tp[n]->get_position().Y() << " " << EMR_tp[n]->get_position().Z() << "\n";
      //~ }
      //~ std::cerr << "\n";
    }
  }
  // TOF2 & KL
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF2_tp;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> KL_tp;
  // Loop over spacepoints in event and assign to correct detectors
  std::vector<MAUS::DataStructure::Global::SpacePoint*>
    *global_spacepoint_array = global_event->get_space_points();
  for (unsigned int i = 0; i < global_spacepoint_array->size(); ++i) {
    MAUS::DataStructure::Global::SpacePoint* sp = global_spacepoint_array->at(i);
    if (!sp) {
      continue;
    }
    // Create track points from space points
    MAUS::DataStructure::Global::TrackPoint* tp2;
    MAUS::DataStructure::Global::TrackPoint* tp_kl;
    if (sp->get_detector() == MAUS::DataStructure::Global::kTOF2) {
      tp2 = new MAUS::DataStructure::Global::TrackPoint(sp);
      tp2->set_mapper_name(mapper_name);
      TOF2_tp.push_back(tp2);
    } else if (sp->get_detector() ==
          MAUS::DataStructure::Global::kCalorimeter) {
      //~ Squeak::mout(Squeak::error) << "Position: " << sp->get_position().T() << ", " << sp->get_position().X() << ", " << sp->get_position().Y() << ", " << sp->get_position().Z() << std::endl;
      tp_kl = new MAUS::DataStructure::Global::TrackPoint(sp);
      tp_kl->set_mapper_name(mapper_name);
      KL_tp.push_back(tp_kl);
 
    } else {
      continue;
    }
  }

  // Load the magnetic field for RK4 propagation
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  MAUS::DataStructure::Global::TrackPArray::iterator scifi_track_iter;
  // Iterate over all Tracker1 Tracks (typically 1)
  for (scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    MAUS::DataStructure::Global::Track* tracker1_track =
        (*scifi_track_iter);
    // Create primary chain with tracker Track as primary Track
    // TODO primary chain once DS is fixed
    //~ MAUS::DataStructure::Global::PrimaryChain* downstream_primary_chain =
        //~ new MAUS::DataStructure::Global::PrimaryChain(mapper_name);
    //~ downstream_primary_chain->AddPrimaryTrack(tracker1_track);
    // Extract four-position and momentum from last track point (i.e. most
    // downstream
    // TODO Will need to do this in a cleaner way later, check by station or sth.
    TLorentzVector position = tracker1_track->GetTrackPoints()[4]->get_position();
    TLorentzVector momentum = tracker1_track->GetTrackPoints()[4]->get_momentum();
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
      hypothesis_track->set_mapper_name("MapCppGlobalTrackMatching-DS");
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
        tracker1_tp->set_mapper_name(mapper_name);
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
        // TODO Get from covariance matrix once full physics processes implemented
        //~ Squeak::mout(Squeak::error) << "TOF2:\n" << x_in_TOF2[1] << " " << TOF2_tp[j]->get_position().X() << "\n"
                                    //~ << x_in_TOF2[2] << " " << TOF2_tp[j]->get_position().Y() << "\n";
        if (almostEquals(x_in_TOF2[1], TOF2_tp[j]->get_position().X(), 40) and
            almostEquals(x_in_TOF2[2], TOF2_tp[j]->get_position().Y(), 40)) {
          hypothesis_track->AddTrackPoint(TOF2_tp[j]);
          //~ Squeak::mout(Squeak::error) << "TOF2 Match\n";
        }
      }

      // KL
      double x_in_KL[] = {0., position.X(), position.Y(), position.Z(),
                          energy, momentum.X(), momentum.Y(), momentum.Z()};
      BTTracker::integrate(18053.0, x_in_KL, field, BTTracker::z, 10.0, charge);
      for (size_t j = 0; j < KL_tp.size(); j++) {
        // We don't need a whole slab width as tolerance, should never be much
        // more than half a slab out from the center of the slab
        //~ Squeak::mout(Squeak::error) << "KL:\n" <<  x_in_KL[2] << " " << KL_tp[j]->get_position().Y() << "\n";
                                    //~ << x_in_KL[2] << " " << KL_tp[j]->get_position().Y() << "\n";
        // For KL we can only match one dimension as no x info is given
        if (almostEquals(x_in_KL[2], KL_tp[j]->get_position().Y(), 32)) {
          hypothesis_track->AddTrackPoint(KL_tp[j]);
          //~ Squeak::mout(Squeak::error) << "KL Match\n";
        }
      }

      // EMR
      ////~ std::cerr << emr_track_array->size() << "# of EMR Tracks\n";
      MAUS::DataStructure::Global::TrackPArray::iterator emr_track_iter;
      for (emr_track_iter = emr_track_array->begin();
           emr_track_iter != emr_track_array->end();
           ++emr_track_iter) {
        std::vector<const MAUS::DataStructure::Global::TrackPoint*>
            emr_trackpoints = (*emr_track_iter)->GetTrackPoints();
        ////~ std::cerr << emr_trackpoints.size() << "# of EMR Hits\n";
        double x_in_EMR[] = {0., position.X(), position.Y(), position.Z(),
                            energy, momentum.X(), momentum.Y(), momentum.Z()};
        TLorentzVector first_hit_pos = emr_trackpoints[0]->get_position();
        TLorentzVector first_hit_pos_error =
            emr_trackpoints[0]->get_position_error();
        double z = first_hit_pos.Z();
        BTTracker::integrate(z, x_in_EMR, field, BTTracker::z, 10.0, charge);
        //~ Squeak::mout(Squeak::error) << "EMR\n" << x_in_EMR[1] << " " << first_hit_pos.X() << "\n"
                                    //~ << x_in_EMR[2] << " " << first_hit_pos.Y() << "\n";
        if (almostEquals(x_in_EMR[1], first_hit_pos.X(),
                first_hit_pos_error.X()*3) and
            almostEquals(x_in_EMR[2], first_hit_pos.Y(),
                first_hit_pos_error.Y()*3)) {
          //~ Squeak::mout(Squeak::error) << "EMR Match\n";
          for (size_t j = 0; j < emr_trackpoints.size(); j++) {
            MAUS::DataStructure::Global::TrackPoint* emr_tp =
                emr_trackpoints[j]->Clone();
            emr_tp->set_mapper_name(mapper_name);
            TLorentzVector momentum = emr_tp->get_momentum();
            double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
            momentum.SetE(energy);
            emr_tp->set_momentum(momentum);
            hypothesis_track->AddTrackPoint(emr_tp);
          }
        }
      }
      //~ downstream_primary_chain->AddTrack(hypothesis_track, tracker1_track);
      global_event->add_track_recursive(hypothesis_track);
    }
    //~ global_event->add_primary_chain_recursive(downstream_primary_chain);
  }
}

void TrackMatching::throughTrack(MAUS::GlobalEvent* global_event,
                            std::string mapper_name) {
  // import tracks already created by tracker recon
  MAUS::DataStructure::Global::TrackPArray* global_tracks =
    global_event->get_tracks();

  //~ MAUS::DataStructure::Global::TrackPArray* through_tracks;
  MAUS::DataStructure::Global::TrackPArray::iterator global_track_iter;
  // This is for no fields, so tracker won't be able to give charge hypothesis
  // --> we need to go through all PID hypotheses
  std::vector<MAUS::DataStructure::Global::PID> pids;
  pids.push_back(MAUS::DataStructure::Global::kEPlus);
  pids.push_back(MAUS::DataStructure::Global::kMuPlus);
  pids.push_back(MAUS::DataStructure::Global::kPiPlus);
  pids.push_back(MAUS::DataStructure::Global::kEMinus);
  pids.push_back(MAUS::DataStructure::Global::kMuMinus);
  pids.push_back(MAUS::DataStructure::Global::kPiMinus);
  // Iterate over all PIDs
  for (size_t i = 0; i < pids.size(); i++) {
    MAUS::DataStructure::Global::TrackPArray* us_tracks =
        new MAUS::DataStructure::Global::TrackPArray();
    MAUS::DataStructure::Global::TrackPArray* ds_tracks =
        new MAUS::DataStructure::Global::TrackPArray();
    MAUS::DataStructure::Global::TrackPArray::iterator us_track_iter;
    MAUS::DataStructure::Global::TrackPArray::iterator ds_track_iter;
    // Loop over all global tracks and sort into US and DS tracks by mapper name
    // Also make sure that TOF1 & 2 hits exist, otherwise we can't match by dT
    // and that the PID is correct
    for (global_track_iter = global_tracks->begin();
         global_track_iter != global_tracks->end();
         ++global_track_iter) {
      if (((*global_track_iter)->get_mapper_name() == "MapCppGlobalTrackMatching-US") and
          ((*global_track_iter)->HasDetector(MAUS::DataStructure::Global::kTOF1)) and
          ((*global_track_iter)->get_pid() == pids[i])) {
        us_tracks->push_back(*global_track_iter);
      } else if (((*global_track_iter)->get_mapper_name() == "MapCppGlobalTrackMatching-DS") and
                 ((*global_track_iter)->HasDetector(MAUS::DataStructure::Global::kTOF2)) and
                 ((*global_track_iter)->get_pid() == pids[i])) {
        ds_tracks->push_back(*global_track_iter);
      }
    }

    // Do we have both US and DS tracks in the event?
    if ((us_tracks->size() > 0) and (ds_tracks->size() > 0)) {
      // Iterate over all possible combinations of US and DS tracks
      for (us_track_iter = us_tracks->begin(); us_track_iter != us_tracks->end();
           ++us_track_iter) {
        for (ds_track_iter = ds_tracks->begin(); ds_track_iter != ds_tracks->end();
             ++ds_track_iter) {
          // Going to assume that if several TrackPoints for the same TOF are in
          // the track that they have been checked to be sensible (TODO above)
          MAUS::DataStructure::Global::TrackPointCPArray us_trackpoints =
              (*us_track_iter)->GetTrackPoints();
          MAUS::DataStructure::Global::TrackPointCPArray ds_trackpoints =
              (*ds_track_iter)->GetTrackPoints();
          if ((us_trackpoints.size() > 0) and (ds_trackpoints.size() > 0)) {
            MAUS::DataStructure::Global::TrackPointCPArray::iterator us_trackpoint_iter;
            MAUS::DataStructure::Global::TrackPointCPArray::iterator ds_trackpoint_iter;
            // Pick out the first TOF1/2 trackpoints in the respective tracks
            for (us_trackpoint_iter = us_trackpoints.begin();
                 us_trackpoint_iter != us_trackpoints.end();
                 ++us_trackpoint_iter) {
              if ((*us_trackpoint_iter)->get_detector() ==
                  MAUS::DataStructure::Global::kTOF1) {
                break;
              }
            }
            for (ds_trackpoint_iter = ds_trackpoints.begin();
                 ds_trackpoint_iter != ds_trackpoints.end();
                 ++ds_trackpoint_iter) {
               if ((*ds_trackpoint_iter)->get_detector() ==
                  MAUS::DataStructure::Global::kTOF2) {
                break;
              }
            }
            // Matching criterion is a sensible dT between TOF1/2
            double TOFdT = (*ds_trackpoint_iter)->get_position().T() -
                (*us_trackpoint_iter)->get_position().T();
            if ((TOFdT > 24.0) and (TOFdT < 40.0)) {
              MAUS::DataStructure::Global::Track* through_track =
                  new MAUS::DataStructure::Global::Track();
              through_track->set_mapper_name("MapCppGlobalTrackMatching-Through");
              through_track->set_pid(pids[i]);
              std::cerr << "US & DS Matched\n";
              for (us_trackpoint_iter = us_trackpoints.begin();
                   us_trackpoint_iter != us_trackpoints.end();
                   ++us_trackpoint_iter) {
                through_track->AddTrackPoint(
                    const_cast<MAUS::DataStructure::Global::TrackPoint*>
                    (*us_trackpoint_iter));
              }
              for (ds_trackpoint_iter = ds_trackpoints.begin();
                   ds_trackpoint_iter != ds_trackpoints.end();
                   ++ds_trackpoint_iter) {
                through_track->AddTrackPoint(
                    const_cast<MAUS::DataStructure::Global::TrackPoint*>
                    (*ds_trackpoint_iter));
              }
              //~ through_track->AddTrack(*us_track_iter);
              //~ through_track->AddTrack(*ds_track_iter);
              //~ std::cerr << through_track << " " << *us_track_iter << " " << *ds_track_iter << "\n";
              //~ global_event->add_track_recursive(*us_track_iter);
              //~ global_event->add_track_recursive(*ds_track_iter);
              global_event->add_track(through_track);
            } else {
              std::cerr << "###" << TOFdT << "\n";
            }
          }
        }
      }
    }
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
