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
#include "src/common_cpp/Recon/Global/GlobalTools.hh"

#include <algorithm>

#include "Interface/Squeak.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

TrackMatching::TrackMatching(MAUS::GlobalEvent* global_event,
                             std::string mapper_name,
                             std::string pid_hypothesis_string,
                             std::map<std::string, std::pair<double, double> >
                             matching_tolerances, double max_step_size,
                             bool energy_loss) {
  _global_event = global_event;
  _mapper_name = mapper_name;
  _pid_hypothesis_string = pid_hypothesis_string;
  _matching_tolerances = matching_tolerances;
  _max_step_size = max_step_size;
  _energy_loss = energy_loss;
}

void TrackMatching::USTrack() {
  // Get all Tracker 0 tracks
  MAUS::DataStructure::Global::TrackPArray *scifi_track_array =
      GetDetectorTrackArray(MAUS::DataStructure::Global::kTracker0);
  // Get Spacepoints for TOF0/1 and convert them to TrackPoints
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF0_tp =
      GetDetectorTrackPoints(MAUS::DataStructure::Global::kTOF0, _mapper_name);
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF1_tp =
      GetDetectorTrackPoints(MAUS::DataStructure::Global::kTOF1, _mapper_name);

  // Load the magnetic field for RK4 propagation
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  // Iterate over all Tracker0 Tracks (typically 1)
  MAUS::DataStructure::Global::TrackPArray::iterator scifi_track_iter;
  for (scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    MAUS::DataStructure::Global::Track* tracker0_track =
        (*scifi_track_iter);
    // Extract four-position and momentum from first track point (i.e. most
    // upstream)
    MAUS::DataStructure::Global::TrackPoint* first_tracker_tp =
        GlobalTools::GetNearestZTrackPoint(tracker0_track, 0);
    TLorentzVector position = first_tracker_tp->get_position();
    TLorentzVector momentum = first_tracker_tp->get_momentum();
    delete first_tracker_tp;
    // Create the list of PIDs for which we want to create hypothesis tracks
    int charge_hypothesis = tracker0_track->get_charge();
    std::vector<MAUS::DataStructure::Global::PID> pids = PIDHypotheses(
        charge_hypothesis, _pid_hypothesis_string);
    // Iterate over all possible PIDs and create an hypothesis track for each
    for (size_t i = 0; i < pids.size(); i++) {
      MAUS::DataStructure::Global::Track* hypothesis_track =
          new MAUS::DataStructure::Global::Track();
      hypothesis_track->set_mapper_name("MapCppGlobalTrackMatching-US");
      hypothesis_track->set_pid(pids[i]);
      MatchTrackPoint(position, momentum, TOF0_tp, pids[i], field, "TOF0",
                      hypothesis_track);
      MatchTrackPoint(position, momentum, TOF1_tp, pids[i], field, "TOF1",
                      hypothesis_track);

      // Now we fill the track with trackpoints from the tracker with energy
      // calculated from p and m, trackpoints are cloned as we want everything
      // in the hypothesis track to be "fresh"
      double mass = Particle::GetInstance().GetMass(pids[i]);
      AddTrackerTrackPoints(tracker0_track, "MapCppGlobalTrackMatching-US",
                            mass, hypothesis_track);

      _global_event->add_track_recursive(hypothesis_track);
    }
    _global_event->add_track_recursive(tracker0_track);
  }
}

void TrackMatching::DSTrack() {
  // Get all Tracker 1 tracks
  MAUS::DataStructure::Global::TrackPArray *scifi_track_array =
      GetDetectorTrackArray(MAUS::DataStructure::Global::kTracker1);
  // Get all EMR tracks
  MAUS::DataStructure::Global::TrackPArray *emr_track_array =
      GetDetectorTrackArray(MAUS::DataStructure::Global::kEMR);
  // Get Spacepoints for TOF0/1 and convert them to TrackPoints
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF2_tp =
      GetDetectorTrackPoints(MAUS::DataStructure::Global::kTOF2, _mapper_name);
  std::vector<MAUS::DataStructure::Global::TrackPoint*> KL_tp =
      GetDetectorTrackPoints(MAUS::DataStructure::Global::kCalorimeter,
                             _mapper_name);
  // Load the magnetic field for RK4 propagation
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  // Iterate over all Tracker1 Tracks (typically 1)
  MAUS::DataStructure::Global::TrackPArray::iterator scifi_track_iter;
  for (scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    MAUS::DataStructure::Global::Track* tracker1_track =
        (*scifi_track_iter);
    // Extract four-position and momentum from last track point (i.e. most
    // downstream
    MAUS::DataStructure::Global::TrackPoint* last_tracker_tp =
        GlobalTools::GetNearestZTrackPoint(tracker1_track, 100000);
    TLorentzVector position = last_tracker_tp->get_position();
    TLorentzVector momentum = last_tracker_tp->get_momentum();
    // Create the list of PIDs for which we want to create hypothesis tracks
    int charge_hypothesis = tracker1_track->get_charge();
    std::vector<MAUS::DataStructure::Global::PID> pids = PIDHypotheses(
        charge_hypothesis, _pid_hypothesis_string);
    // Iterate over all possible PIDs and create an hypothesis track for each
    for (size_t i = 0; i < pids.size(); i++) {
      MAUS::DataStructure::Global::Track* hypothesis_track =
          new MAUS::DataStructure::Global::Track();
      hypothesis_track->set_mapper_name("MapCppGlobalTrackMatching-DS");
      hypothesis_track->set_pid(pids[i]);

      // This time we add in the tracker trackpoints first
      double mass = Particle::GetInstance().GetMass(pids[i]);
      AddTrackerTrackPoints(tracker1_track, "MapCppGlobalTrackMatching-DS",
                            mass, hypothesis_track);
      // TOF2
      MatchTrackPoint(position, momentum, TOF2_tp, pids[i], field, "TOF2",
                      hypothesis_track);
      // KL
      MatchTrackPoint(position, momentum, KL_tp, pids[i], field, "KL",
                      hypothesis_track);
      // EMR
      MatchEMRTrack(position, momentum, emr_track_array, pids[i], field,
                    hypothesis_track);
      _global_event->add_track_recursive(hypothesis_track);
    }
  }
}

void TrackMatching::throughTrack() {
  // import tracks already created by tracker recon
  MAUS::DataStructure::Global::TrackPArray* global_tracks =
    _global_event->get_tracks();
  // Typically used for no fields so we can't discriminate by charge hypothesis
  std::vector<MAUS::DataStructure::Global::PID> pids = PIDHypotheses(
        0, _pid_hypothesis_string);
  // Iterate over all PIDs
  for (size_t i = 0; i < pids.size(); i++) {
    MAUS::DataStructure::Global::TrackPArray* us_tracks =
        new MAUS::DataStructure::Global::TrackPArray();
    MAUS::DataStructure::Global::TrackPArray* ds_tracks =
        new MAUS::DataStructure::Global::TrackPArray();

    // Loop over all global tracks and sort into US and DS tracks by mapper name
    // provided they have the correct PID and hits in TOF1/2 to match by dT
    USDSTracks(global_tracks, pids[i], us_tracks, ds_tracks);
    
    MAUS::DataStructure::Global::TrackPArray::iterator us_track_iter;
    MAUS::DataStructure::Global::TrackPArray::iterator ds_track_iter;
    // Do we have both US and DS tracks in the event?
    if ((us_tracks->size() > 0) and (ds_tracks->size() > 0)) {
      // Iterate over all possible combinations of US and DS tracks
      for (us_track_iter = us_tracks->begin();
           us_track_iter != us_tracks->end();
           ++us_track_iter) {
        for (ds_track_iter = ds_tracks->begin();
             ds_track_iter != ds_tracks->end();
             ++ds_track_iter) {
          // Going to assume that if several TrackPoints for the same TOF are in
          // the track that they have been checked to be sensible (TODO above)
          MAUS::DataStructure::Global::TrackPointCPArray us_trackpoints =
              (*us_track_iter)->GetTrackPoints();
          MAUS::DataStructure::Global::TrackPointCPArray ds_trackpoints =
              (*ds_track_iter)->GetTrackPoints();
          double emr_range_primary = (*ds_track_iter)->get_emr_range_primary();
          if ((us_trackpoints.size() > 0) and (ds_trackpoints.size() > 0)) {
            MatchUSDS(us_trackpoints, ds_trackpoints, pids[i],
                      emr_range_primary);
          }
        }
      }
    }
  }
}

MAUS::DataStructure::Global::TrackPArray* TrackMatching::GetDetectorTrackArray(
    MAUS::DataStructure::Global::DetectorPoint detector) {
  MAUS::DataStructure::Global::TrackPArray *imported_tracks =
    _global_event->get_tracks();
  MAUS::DataStructure::Global::TrackPArray::iterator imported_track_iter;
  MAUS::DataStructure::Global::TrackPArray *track_array =
      new MAUS::DataStructure::Global::TrackPArray();
  for (imported_track_iter = imported_tracks->begin();
       imported_track_iter != imported_tracks->end();
       ++imported_track_iter) {
    MAUS::DataStructure::Global::Track* imported_track =
        (*imported_track_iter);
    // Check that track is from correct detector
    if (imported_track->HasDetector(detector)) {
      // Exclude EMR secondary tracks (tracker tracks also have this at 0 by
      // default)
      if (imported_track->get_emr_range_secondary() < 0.001) {
        track_array->push_back(imported_track);
      }
    }
  }
  return track_array;
}

std::vector<MAUS::DataStructure::Global::TrackPoint*>
    TrackMatching::GetDetectorTrackPoints(
    MAUS::DataStructure::Global::DetectorPoint detector,
    std::string mapper_name) {
  std::vector<MAUS::DataStructure::Global::TrackPoint*> track_points;
  std::vector<MAUS::DataStructure::Global::SpacePoint*>
      *global_spacepoint_array = _global_event->get_space_points();
  for (size_t i = 0; i < global_spacepoint_array->size(); i++) {
    MAUS::DataStructure::Global::SpacePoint* space_point =
        global_spacepoint_array->at(i);
    if (!space_point) {
      continue;
    }
    if (space_point->get_detector() == detector) {
      MAUS::DataStructure::Global::TrackPoint* track_point =
          new MAUS::DataStructure::Global::TrackPoint(space_point);
      track_point->set_mapper_name(mapper_name);
      track_points.push_back(track_point);
    }
  }
  return track_points;
}

std::vector<MAUS::DataStructure::Global::PID> TrackMatching::PIDHypotheses(
    int charge_hypothesis, std::string pid_hypothesis_string) {
  std::vector<MAUS::DataStructure::Global::PID> pids;
  // First check if a specific PID has been forced by the configuration
  if (pid_hypothesis_string == "kEPlus") {
      pids.push_back(MAUS::DataStructure::Global::kEPlus);
  } else if (pid_hypothesis_string == "kEMinus") {
      pids.push_back(MAUS::DataStructure::Global::kEMinus);
  } else if (pid_hypothesis_string == "kMuPlus") {
      pids.push_back(MAUS::DataStructure::Global::kMuPlus);
  } else if (pid_hypothesis_string == "kMuMinus") {
      pids.push_back(MAUS::DataStructure::Global::kMuMinus);
  } else if (pid_hypothesis_string == "kPiPlus") {
      pids.push_back(MAUS::DataStructure::Global::kPiPlus);
  } else if (pid_hypothesis_string == "kPiMinus") {
      pids.push_back(MAUS::DataStructure::Global::kPiMinus);
  } else {
    // No specific PID forced, hence we assign by charge hypothesis
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
  }
  return pids;
}

void TrackMatching::MatchTrackPoint(
    const TLorentzVector &position, const TLorentzVector &momentum,
    const std::vector<MAUS::DataStructure::Global::TrackPoint*> &trackpoints,
    MAUS::DataStructure::Global::PID pid, BTFieldConstructor* field,
    std::string detector_name, MAUS::DataStructure::Global::Track*
    hypothesis_track) {
  double mass = Particle::GetInstance().GetMass(pid);
  double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
  double x_in[] = {0., position.X(), position.Y(), position.Z(),
                   energy, momentum.X(), momentum.Y(), momentum.Z()};
  if (trackpoints.size() > 0) {
    double target_z = trackpoints.at(0)->get_position().Z();
    try {
      GlobalTools::propagate(x_in, target_z, field, _max_step_size, pid,
                             _energy_loss);
      for (size_t i = 0; i < trackpoints.size(); i++) {
        if (GlobalTools::approx(x_in[1], trackpoints.at(i)->get_position().X(),
                _matching_tolerances.at(detector_name).first) and
            GlobalTools::approx(x_in[2], trackpoints.at(i)->get_position().Y(),
                _matching_tolerances.at(detector_name).second)) {
          hypothesis_track->AddTrackPoint(trackpoints.at(i));
          std::cerr << detector_name << " Match\n";
        } else {
          std::cerr << x_in[1] - trackpoints.at(i)->get_position().X() << " "
                    << x_in[2] - trackpoints.at(i)->get_position().Y() << "\n";
        }
      }
    } catch (Exception exc) {
      std::cerr << exc.what() << std::endl;
    }
  }
}

void TrackMatching::MatchEMRTrack(
    const TLorentzVector &position, const TLorentzVector &momentum,
    MAUS::DataStructure::Global::TrackPArray* emr_track_array,
    MAUS::DataStructure::Global::PID pid, BTFieldConstructor* field,
    MAUS::DataStructure::Global::Track* hypothesis_track) {
  double mass = Particle::GetInstance().GetMass(pid);
  double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
  // Here we need to iterate over the EMR tracks first, as they might have
  // different starting z
  MAUS::DataStructure::Global::TrackPArray::iterator emr_track_iter;
  for (emr_track_iter = emr_track_array->begin();
       emr_track_iter != emr_track_array->end();
       ++emr_track_iter) {
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
        emr_trackpoints = (*emr_track_iter)->GetTrackPoints();
    double x_in[] = {0., position.X(), position.Y(), position.Z(),
                     energy, momentum.X(), momentum.Y(), momentum.Z()};
    TLorentzVector first_hit_pos = emr_trackpoints[0]->get_position();
    TLorentzVector first_hit_pos_err = emr_trackpoints[0]->get_position_error();
    double target_z = first_hit_pos.Z();
    try {
      GlobalTools::propagate(x_in, target_z, field, _max_step_size, pid,
                             _energy_loss);
      if (GlobalTools::approx(x_in[1], first_hit_pos.X(),
                              first_hit_pos_err.X()*::sqrt(12)) and
          GlobalTools::approx(x_in[2], first_hit_pos.Y(),
                              first_hit_pos_err.Y()*::sqrt(12))) {
        std::cerr << "EMR Match\n";
        hypothesis_track->set_emr_range_primary(
            (*emr_track_iter)->get_emr_range_primary());
        for (size_t i = 0; i < emr_trackpoints.size(); i++) {
          MAUS::DataStructure::Global::TrackPoint* emr_tp =
              emr_trackpoints[i]->Clone();
          emr_tp->set_mapper_name("MapCppGlobalTrackMatching-DS");
          TLorentzVector momentum = emr_tp->get_momentum();
          double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
          momentum.SetE(energy);
          emr_tp->set_momentum(momentum);
          hypothesis_track->AddTrackPoint(emr_tp);
        }
      }
    } catch (Exception exc) {
      std::cerr << exc.what() << std::endl;
    }
  }
}

void TrackMatching::AddTrackerTrackPoints(
    MAUS::DataStructure::Global::Track* tracker_track,
    std::string mapper_name, double mass,
    MAUS::DataStructure::Global::Track* hypothesis_track) {
  std::vector<const MAUS::DataStructure::Global::TrackPoint*>
  tracker_trackpoints = tracker_track->GetTrackPoints();
  std::sort(tracker_trackpoints.begin(), tracker_trackpoints.end(),
            GlobalTools::TrackPointSort);
  for (size_t i = 0; i < tracker_trackpoints.size(); i++) {
    MAUS::DataStructure::Global::TrackPoint* tracker_tp =
        tracker_trackpoints[i]->Clone();
    tracker_tp->set_mapper_name(mapper_name);
    TLorentzVector momentum = tracker_tp->get_momentum();
    double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
    momentum.SetE(energy);
    tracker_tp->set_momentum(momentum);
    hypothesis_track->AddTrackPoint(tracker_tp);
  }
}

void TrackMatching::USDSTracks(
    MAUS::DataStructure::Global::TrackPArray* global_tracks,
    MAUS::DataStructure::Global::PID pid,
    MAUS::DataStructure::Global::TrackPArray* us_tracks,
    MAUS::DataStructure::Global::TrackPArray* ds_tracks) {
  MAUS::DataStructure::Global::TrackPArray::iterator global_track_iter;
  for (global_track_iter = global_tracks->begin();
       global_track_iter != global_tracks->end();
       ++global_track_iter) {
    if (((*global_track_iter)->get_mapper_name() ==
            "MapCppGlobalTrackMatching-US") and
        ((*global_track_iter)->HasDetector(MAUS::DataStructure::Global::kTOF1))
        and ((*global_track_iter)->get_pid() == pid)) {
      us_tracks->push_back(*global_track_iter);
    } else if (((*global_track_iter)->get_mapper_name() ==
                   "MapCppGlobalTrackMatching-DS") and
               ((*global_track_iter)->HasDetector(
                  MAUS::DataStructure::Global::kTOF2)) and
               ((*global_track_iter)->get_pid() == pid)) {
      ds_tracks->push_back(*global_track_iter);
    }
  }
}

void TrackMatching::MatchUSDS(
    MAUS::DataStructure::Global::TrackPointCPArray us_trackpoints,
    MAUS::DataStructure::Global::TrackPointCPArray ds_trackpoints,
    MAUS::DataStructure::Global::PID pid,
    double emr_range_primary) {
  double TOF1_time = TOFTimeFromTrackPoints(us_trackpoints,
                                            MAUS::DataStructure::Global::kTOF1);
  double TOF2_time = TOFTimeFromTrackPoints(ds_trackpoints,
                                            MAUS::DataStructure::Global::kTOF2);
  double TOFdT = TOF2_time - TOF1_time;
  if ((TOFdT > _matching_tolerances.at("TOF12dT").first) and
      (TOFdT < _matching_tolerances.at("TOF12dT").second)) {
    MAUS::DataStructure::Global::Track* through_track =
        new MAUS::DataStructure::Global::Track();
    through_track->set_mapper_name("MapCppGlobalTrackMatching-Through");
    through_track->set_pid(pid);
    std::cerr << "US & DS Matched\n";
    MAUS::DataStructure::Global::TrackPointCPArray::iterator trackpoint_iter;
    for (trackpoint_iter = us_trackpoints.begin();
         trackpoint_iter != us_trackpoints.end();
         ++trackpoint_iter) {
      through_track->AddTrackPoint(
          const_cast<MAUS::DataStructure::Global::TrackPoint*>
          (*trackpoint_iter));
    }
    for (trackpoint_iter = ds_trackpoints.begin();
         trackpoint_iter != ds_trackpoints.end();
         ++trackpoint_iter) {
      through_track->AddTrackPoint(
          const_cast<MAUS::DataStructure::Global::TrackPoint*>
          (*trackpoint_iter));
    }
    through_track->set_emr_range_primary(emr_range_primary);
    _global_event->add_track(through_track);
  }
}

double TrackMatching::TOFTimeFromTrackPoints(
    MAUS::DataStructure::Global::TrackPointCPArray trackpoints,
    MAUS::DataStructure::Global::DetectorPoint detector) {
  MAUS::DataStructure::Global::TrackPointCPArray::iterator trackpoint_iter;
  double TOF_time = 0.0;
  for (trackpoint_iter = trackpoints.begin();
       trackpoint_iter != trackpoints.end();
       ++trackpoint_iter) {
    if ((*trackpoint_iter)->get_detector() == detector) {
      TOF_time = (*trackpoint_iter)->get_position().T();
    }
  }
  return TOF_time;
}

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
