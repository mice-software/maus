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

#include "CLHEP/Units/PhysicalConstants.h"
#include "Geant4/G4NistManager.hh"

#include "src/common_cpp/Recon/Global/GlobalTools.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/Exception.hh"

#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "Utils/Squeak.hh"

#include "src/common_cpp/Recon/Global/TrackMatching.hh"

namespace MAUS {
namespace recon {
namespace global {

TrackMatching::TrackMatching(GlobalEvent* global_event, std::string mapper_name,
    std::string pid_hypothesis_string, int beamline_polarity,
    std::map<std::string, std::pair<double, double> > matching_tolerances,
    double max_step_size, std::pair<bool, bool> no_check_settings,
    bool energy_loss, bool residuals, TrackMatching::geometry_algorithm geo_algo,
    std::vector<double> extra_z_planes, TrackMatching::through_track_algorithm through_algo) {
  _global_event = global_event;
  _mapper_name = mapper_name;
  _pid_hypothesis_string = pid_hypothesis_string;
  _beamline_polarity = beamline_polarity;
  _matching_tolerances = matching_tolerances;
  _max_step_size = max_step_size;
  _energy_loss = energy_loss;
  _no_check_settings = no_check_settings;
  _residuals = residuals;
  _geo_algo = geo_algo;
  _extra_z_planes = extra_z_planes;
  _through_algo = through_algo;
}

void TrackMatching::USTrack() {
  // Get all Tracker 0 tracks
  DataStructure::Global::TrackPArray *scifi_track_array =
      GetDetectorTrackArray(DataStructure::Global::kTracker0);
  // Get Spacepoints for TOF0/1 and convert them to TrackPoints
  std::vector<DataStructure::Global::SpacePoint*> TOF0_sp =
      GetDetectorSpacePoints(DataStructure::Global::kTOF0);
  std::vector<DataStructure::Global::SpacePoint*> TOF1_sp =
      GetDetectorSpacePoints(DataStructure::Global::kTOF1);
  // Ckov Spacepoints
  std::vector<DataStructure::Global::SpacePoint*> CkovA_sp =
      GetDetectorSpacePoints(DataStructure::Global::kCherenkovA);
  std::vector<DataStructure::Global::SpacePoint*> CkovB_sp =
      GetDetectorSpacePoints(DataStructure::Global::kCherenkovB);
  // Here we check whether we actually want to use propagation for TrackMatching
  // or if we can just assume that all hits are from the same particle
  bool no_check = false;
  if (_no_check_settings.first) {
    if (scifi_track_array->size() == 1 and TOF0_sp.size() < 2 and TOF1_sp.size() < 2 and
        CkovA_sp.size() < 2 and CkovB_sp.size() < 2) {
      no_check = true;
    }
  }
  BTFieldConstructor* field = NULL;
  if (!no_check) {
    // Load the magnetic field for RK4 propagation
    field = Globals::GetMCFieldConstructor();
    _field = field;
  }
  // Iterate over all Tracker0 Tracks (typically 1)
  for (auto scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    DataStructure::Global::Track* tracker0_track = (*scifi_track_iter);
    // Create the list of PIDs for which we want to create hypothesis tracks
    int charge_hypothesis = tracker0_track->get_charge();
    if (charge_hypothesis == 0) {
      charge_hypothesis = _beamline_polarity;
    }
    std::vector<DataStructure::Global::PID> pids = PIDHypotheses(
        charge_hypothesis, _pid_hypothesis_string);
    // Create Primary Chain for this tracker track, declare as orphan initially,
    // change later if matched
    DataStructure::Global::PrimaryChain* us_primary_chain =
        new DataStructure::Global::PrimaryChain("MapCppGlobalTrackMatching",
            DataStructure::Global::kUSOrphan);
    // Iterate over all possible PIDs and create an hypothesis track for each
    for (size_t i = 0; i < pids.size(); i++) {
      DataStructure::Global::Track* hypothesis_track =
          new DataStructure::Global::Track();
      hypothesis_track->set_mapper_name("MapCppGlobalTrackMatching");
      hypothesis_track->set_pid(pids[i]);
      hypothesis_track->set_p_value(tracker0_track->get_p_value());
      hypothesis_track->set_tracker_clusters(tracker0_track->get_tracker_clusters());
      // No matching criterion for Cherenkov hits, so if they exist, we add them
      if (CkovA_sp.size() > 0) {
        Squeak::mout(Squeak::debug) << "TrackMatching: CkovA Added" << std::endl;
        DataStructure::Global::TrackPoint* CkovA_tp =
            new DataStructure::Global::TrackPoint(CkovA_sp.at(0));
        hypothesis_track->AddTrackPoint(CkovA_tp);
      }
      if (CkovB_sp.size() > 0) {
        Squeak::mout(Squeak::debug) << "TrackMatching: CkovB Added" << std::endl;
        DataStructure::Global::TrackPoint* CkovB_tp =
            new DataStructure::Global::TrackPoint(CkovB_sp.at(0));
        hypothesis_track->AddTrackPoint(CkovB_tp);
      }
      // Now match TOF1 and then TOF0
      if (!no_check) {
        // Extract four-position and momentum from first track point (i.e. most
        // upstream)
        TLorentzVector position;
        TLorentzVector momentum;
        DataStructure::Global::TrackPoint* first_tracker_tp =
            GlobalTools::GetNearestZTrackPoint(tracker0_track, 0);
        position = first_tracker_tp->get_position();
        momentum = first_tracker_tp->get_momentum();
        delete first_tracker_tp;
        MatchTrackPoint(position, momentum, TOF1_sp, pids[i], field, "TOF1",
                        hypothesis_track);

        std::vector<const MAUS::DataStructure::Global::TrackPoint*> ht_tof1_tps =
            hypothesis_track->GetTrackPoints(DataStructure::Global::kTOF1);
        if (ht_tof1_tps.size() > 0) {
          double tof1_z = ht_tof1_tps[0]->get_position().Z();
          double tof1_t = ht_tof1_tps[0]->get_position().T();
          MatchTOF0(position, momentum, tof1_z, tof1_t, TOF0_sp, pids[i], field, hypothesis_track);
        }
      } else {
        if (TOF0_sp.size() == 1) {
          Squeak::mout(Squeak::debug) << "TrackMatching: TOF 0 Added (No Check)" << std::endl;
          DataStructure::Global::TrackPoint* TOF0_tp =
              new DataStructure::Global::TrackPoint(TOF0_sp.at(0));
          hypothesis_track->AddTrackPoint(TOF0_tp);
        }
        if (TOF1_sp.size() == 1) {
          Squeak::mout(Squeak::debug) << "TrackMatching: TOF 1 Added (No Check)" << std::endl;
          DataStructure::Global::TrackPoint* TOF1_tp =
              new DataStructure::Global::TrackPoint(TOF1_sp.at(0));
          hypothesis_track->AddTrackPoint(TOF1_tp);
        }
      }

      // We had to add TOF0 before TOF1, so the order isn't right
      hypothesis_track->SortTrackPointsByZ();

      // Now we fill the track with trackpoints from the tracker with energy
      // calculated from p and m, trackpoints are cloned as we want everything
      // in the hypothesis track to be "fresh"
      double mass = Particle::GetInstance().GetMass(pids[i]);
      AddTrackerTrackPoints(tracker0_track, "MapCppGlobalTrackMatching",
                            mass, hypothesis_track);
      // Add reference to the tracker track to the hypothesis track
      hypothesis_track->AddTrack(tracker0_track);
      us_primary_chain->AddMatchedTrack(hypothesis_track);
      _global_event->add_track_recursive(hypothesis_track);
    }
    // Only add to global event if we have at least one matched track
    if (us_primary_chain->GetMatchedTracks().size() > 0) {
      _global_event->add_primary_chain(us_primary_chain);
    }
  }
  delete scifi_track_array;
}

void TrackMatching::DSTrack() {
  // Get all Tracker 1 tracks
  DataStructure::Global::TrackPArray *scifi_track_array =
      GetDetectorTrackArray(DataStructure::Global::kTracker1);
  // Get all EMR tracks
  DataStructure::Global::TrackPArray *emr_track_array =
      GetDetectorTrackArray(DataStructure::Global::kEMR);
  // Get Spacepoints for TOF0/1 and convert them to TrackPoints
  std::vector<DataStructure::Global::SpacePoint*> TOF2_sp =
      GetDetectorSpacePoints(DataStructure::Global::kTOF2);
  std::vector<DataStructure::Global::SpacePoint*> KL_sp =
      GetDetectorSpacePoints(DataStructure::Global::kCalorimeter);
  // Here we check whether we actually want to use propagation for TrackMatching
  // or if we can just assume that all hits are from the same particle
  bool no_check = false;
  if (_no_check_settings.second) {
    if (scifi_track_array->size() == 1 and TOF2_sp.size() < 2 and KL_sp.size() < 2 and
        emr_track_array->size() < 2) {
      no_check = true;
    }
  }
  BTFieldConstructor* field = NULL;
  if (!no_check) {
    // Load the magnetic field for RK4 propagation
    field = Globals::GetMCFieldConstructor();
  }
  // Iterate over all Tracker1 Tracks (typically 1)
  for (auto scifi_track_iter = scifi_track_array->begin();
       scifi_track_iter != scifi_track_array->end();
       ++scifi_track_iter) {
    // Pull out the track so we're not making a mess with ressources
    DataStructure::Global::Track* tracker1_track = (*scifi_track_iter);
    // Create the list of PIDs for which we want to create hypothesis tracks
    int charge_hypothesis = tracker1_track->get_charge();
    if (charge_hypothesis == 0) {
      charge_hypothesis = _beamline_polarity;
    }
    std::vector<DataStructure::Global::PID> pids = PIDHypotheses(
        charge_hypothesis, _pid_hypothesis_string);
    // Create Primary Chain for this tracker track, declare as orphan initially,
    // change later if matched
    DataStructure::Global::PrimaryChain* ds_primary_chain =
        new DataStructure::Global::PrimaryChain("MapCppGlobalTrackMatching",
            DataStructure::Global::kDSOrphan);
    // Iterate over all possible PIDs and create an hypothesis track for each
    for (size_t i = 0; i < pids.size(); i++) {
      DataStructure::Global::Track* hypothesis_track =
          new DataStructure::Global::Track();
      hypothesis_track->set_mapper_name("MapCppGlobalTrackMatching");
      hypothesis_track->set_pid(pids[i]);
      hypothesis_track->set_p_value(tracker1_track->get_p_value());
      hypothesis_track->set_tracker_clusters(tracker1_track->get_tracker_clusters());
      // This time we add in the tracker trackpoints first
      double mass = Particle::GetInstance().GetMass(pids[i]);
      AddTrackerTrackPoints(tracker1_track, "MapCppGlobalTrackMatching",
                            mass, hypothesis_track);
      if (!no_check) {
        // Extract four-position and momentum from last track point (i.e. most
        // downstream
        TLorentzVector position;
        TLorentzVector momentum;
        DataStructure::Global::TrackPoint* last_tracker_tp =
            GlobalTools::GetNearestZTrackPoint(tracker1_track, 100000);
        position = last_tracker_tp->get_position();
        momentum = last_tracker_tp->get_momentum();
        delete last_tracker_tp;
        // TOF2
        MatchTrackPoint(position, momentum, TOF2_sp, pids[i], field, "TOF2",
                        hypothesis_track);
        // KL
        MatchTrackPoint(position, momentum, KL_sp, pids[i], field, "KL",
                        hypothesis_track);
        // EMR
        MatchEMRTrack(position, momentum, emr_track_array, pids[i], field,
                      hypothesis_track);
      } else {
        if (TOF2_sp.size() == 1) {
          Squeak::mout(Squeak::debug) << "TrackMatching: TOF 2 Added (No Check)" << std::endl;
          DataStructure::Global::TrackPoint* TOF2_tp =
              new DataStructure::Global::TrackPoint(TOF2_sp.at(0));
          hypothesis_track->AddTrackPoint(TOF2_tp);
        }
        if (KL_sp.size() == 1) {
          Squeak::mout(Squeak::debug) << "TrackMatching: KL Added (No Check)" << std::endl;
          DataStructure::Global::TrackPoint* KL_tp =
              new DataStructure::Global::TrackPoint(KL_sp.at(0));
          hypothesis_track->AddTrackPoint(KL_tp);
        }
        if (emr_track_array->size() == 1) {
          hypothesis_track->set_emr_range_primary(emr_track_array->at(0)->get_emr_range_primary());
          hypothesis_track->set_emr_plane_density(emr_track_array->at(0)->get_emr_plane_density());
          std::vector<const DataStructure::Global::TrackPoint*> emr_trackpoints =
              emr_track_array->at(0)->GetTrackPoints();
          for (size_t i = 0; i < emr_trackpoints.size(); i++) {
            DataStructure::Global::TrackPoint* emr_tp =
                emr_trackpoints[i]->Clone();
            emr_tp->set_mapper_name("MapCppGlobalTrackMatching_DS");
            TLorentzVector momentum = emr_tp->get_momentum();
            double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
            momentum.SetE(energy);
            emr_tp->set_momentum(momentum);
            hypothesis_track->AddTrackPoint(emr_tp);
          }
          Squeak::mout(Squeak::debug) << "TrackMatching: EMR Added (No Check)" << std::endl;
        }
      }
      // Add reference to the tracker track to the hypothesis track
      hypothesis_track->AddTrack(tracker1_track);
      ds_primary_chain->AddMatchedTrack(hypothesis_track);
      _global_event->add_track_recursive(hypothesis_track);
    }
    // Only add to global event if we have at least one matched track
    if (ds_primary_chain->GetMatchedTracks().size() > 0) {
      _global_event->add_primary_chain(ds_primary_chain);
    }
  }
  delete scifi_track_array;
  delete emr_track_array;
}

std::ostream& operator<<(std::ostream& out, const TLorentzVector& vec) {
    out << vec.X() << " " << vec.Y() << " " << vec.Z() << "; " << vec.T();
    return out;
}

bool TrackMatching::throughMatchPropagate(
                            DataStructure::Global::Track* us_track,
                            DataStructure::Global::Track* ds_track,
                            DataStructure::Global::PID pid,
                            DataStructure::Global::Track* through_track) {
    if (!us_track or !us_track->HasDetector(DataStructure::Global::kTracker0_1)) {
      Squeak::mout(Squeak::debug) << "ThroughMatch (Propagate) missing TKU track" << std::endl;
      return false;
    }
    if (through_track == NULL) {
      return false;
    }
    const MAUS::DataStructure::Global::TrackPoint* us_tp =
            us_track->GetTrackPoints(DataStructure::Global::kTracker0_1).back();

    TLorentzVector pos_us = us_tp->get_position();
    TLorentzVector mom_us = us_tp->get_momentum();

    through_track->set_mapper_name("MapCppGlobalTrackMatching");
    through_track->set_pid(pid);
    std::vector<const DataStructure::Global::TrackPoint*> tp_vecs =
                                                     ds_track->GetTrackPoints();
    double mass = Particle::GetInstance().GetMass(pid);
    double energy = ::sqrt(mom_us.Rho()*mom_us.Rho() + mass*mass);
    double x_in[] = {pos_us.T(), pos_us.X(), pos_us.Y(), pos_us.Z(),
                     energy, mom_us.X(), mom_us.Y(), mom_us.Z()};
    ClearVirtualTrackPoints();
    for (size_t i = 0; i < tp_vecs.size(); ++i) {
        if (tp_vecs[i]->get_detector() ==  DataStructure::Global::kVirtual) {
            // we propagate to real detectors only; we should pick up
            // virtuals anyway by the "AddVirtualTrackPoints" mechanism
            continue;
        }
        double target_z = tp_vecs[i]->get_position().Z();
        try {
            Propagate(x_in, _field, pid, target_z);
        } catch (std::exception& exc) {
            break;
        }
        if (fabs(x_in[3] - target_z) > 1e-9) {
            break;
        }
        TLorentzVector pos_ds(x_in[1], x_in[2], x_in[3], x_in[0]); // x, y, z, t
        TLorentzVector mom_ds(x_in[5], x_in[6], x_in[7], x_in[4]); // px, py, pz, E
        MAUS::DataStructure::Global::TrackPoint* tp =
                                  new MAUS::DataStructure::Global::TrackPoint();
        tp->set_position(pos_ds);
        tp->set_momentum(mom_ds);
        tp->set_charge(through_track->get_charge());
        tp->set_mapper_name(through_track->get_mapper_name());
        tp->set_space_point(tp_vecs[i]->get_space_point());
        tp->set_detector(tp_vecs[i]->get_detector());
        tp->set_particle_event(tp_vecs[i]->get_particle_event());
        through_track->AddTrackPoint(tp);
    }
    if (not through_track->HasDetector(DataStructure::Global::kTracker1_1)) {
       Squeak::mout(Squeak::debug) << "ThroughMatch (Propagate) missing TKD track" << std::endl;
        return false; // tracking failed
    }
    double pos_tolerance = _matching_tolerances.at("TKD").first;
    double mom_tolerance = _matching_tolerances.at("TKD").second;
    const MAUS::DataStructure::Global::TrackPoint* ds_recon =
          ds_track->GetTrackPoints(DataStructure::Global::kTracker1_1)[0];
    const MAUS::DataStructure::Global::TrackPoint* ds_through =
          through_track->GetTrackPoints(DataStructure::Global::kTracker1_1)[0];
    TVector3 dpos = ds_recon->get_position().Vect() -  ds_through->get_position().Vect();
    TVector3 dmom = ds_recon->get_momentum().Vect() -  ds_through->get_momentum().Vect();
    Squeak::mout(Squeak::debug) << "ThroughMatch (Propagate) "
                                << "checking tolerances " << std::endl;
    Squeak::mout(Squeak::debug) << "    Recon Pos: " << ds_recon->get_position()
                               << " Mom: " <<ds_recon->get_momentum() << std::endl;
    Squeak::mout(Squeak::debug) << "    Thru  Pos: " << ds_through->get_position()
                               << " Mom: " <<ds_through->get_momentum() << std::endl;
    Squeak::mout(Squeak::debug) << "    Deltas r: " << dpos.Mag()
                               << " p: " << dmom.Mag() << std::endl;
    Squeak::mout(Squeak::debug) << "    Tolerances r: " << pos_tolerance
                               << " p: " << mom_tolerance << std::endl;
    dpos.SetZ(0.);
    if (dpos.Mag() < pos_tolerance and dmom.Mag() < mom_tolerance) {
      Squeak::mout(Squeak::debug) << "    Accepted" << std::endl;
      // Assemble through track from trackpoints from the matched US and DS tracks
      Squeak::mout(Squeak::debug) << "TrackMatching: US & DS Matched" << std::endl;
      addTrackRecursive(through_track, us_track, false);
      AddVirtualTrackPoints(through_track);
      through_track->SortTrackPointsByZ();
      through_track->set_emr_range_primary(ds_track->get_emr_range_primary());
      through_track->set_emr_plane_density(ds_track->get_emr_plane_density());
      _global_event->add_track_recursive(through_track);
      tp_vecs = through_track->GetTrackPoints();
      return true;
    } else {
      // There may be a small memory leak here because the US and DS tracks don't get
      // deleted, deleting them here manually causes a segfault. TODO: Investigate
      Squeak::mout(Squeak::debug) << "    Rejected" << std::endl;
      return false;
    }
}

bool TrackMatching::throughMatchTOF(
                            DataStructure::Global::Track* us_track,
                            DataStructure::Global::Track* ds_track,
                            DataStructure::Global::PID pid,
                            DataStructure::Global::Track* through_track) {
    if (!us_track or !ds_track) {
        return false;
    }
    ofstream throughfile;
    if (_residuals) {
      throughfile.open("match_through.csv", std::ios::out | std::ios::app);
    }
    if (!us_track->HasDetector(DataStructure::Global::kTOF1) or
        !ds_track->HasDetector(DataStructure::Global::kTOF2)) {
      return false;
    }
    // Get TOF1&2 times to calculate effective particle speed between detectors
    DataStructure::Global::TrackPointCPArray us_trackpoints = us_track->GetTrackPoints();
    DataStructure::Global::TrackPointCPArray ds_trackpoints = ds_track->GetTrackPoints();
    double TOF1_time = TOFTimeFromTrackPoints(us_trackpoints,
                                              DataStructure::Global::kTOF1);
    double TOF2_time = TOFTimeFromTrackPoints(ds_trackpoints,
                                              DataStructure::Global::kTOF2);
    double TOFdT = TOF2_time - TOF1_time;
    if (_residuals) {
      throughfile << TOFdT/_matching_tolerances.at("TOF12dT").second << "\n";
    }
    if ((TOFdT > _matching_tolerances.at("TOF12dT").first) and
        (TOFdT < _matching_tolerances.at("TOF12dT").second)) {
      Squeak::mout(Squeak::debug) << "TrackMatching (TOF12): US & DS Matched" << std::endl;
      through_track->set_mapper_name("MapCppGlobalTrackMatching");
      through_track->set_pid(pid);
      addTrackRecursive(through_track, us_track, false);
      addTrackRecursive(through_track, ds_track, true);
      _global_event->add_track(through_track);
      return true;
    } else {
      return false;
    }
}

void TrackMatching::addTrackRecursive(DataStructure::Global::Track* parent,
                                      DataStructure::Global::Track* child,
                                      bool doEMR) {
    parent->AddTrack(child);
    DataStructure::Global::TrackPointCPArray tps = child->GetTrackPoints();
    for (auto trackpoint_iter = tps.begin();
         trackpoint_iter != tps.end();
         ++trackpoint_iter) {
        parent->AddTrackPoint(
              const_cast<DataStructure::Global::TrackPoint*>(*trackpoint_iter));
    }
    if (doEMR) {
        parent->set_emr_range_primary(child->get_emr_range_primary());
        parent->set_emr_plane_density(child->get_emr_plane_density());
    }
}

void TrackMatching::throughTrack() {
  if (_through_algo == kNoThroughMatching) {
      return;
  }
  std::vector<DataStructure::Global::PrimaryChain*> us_chains =
      _global_event->GetUSPrimaryChainOrphans();
  std::vector<DataStructure::Global::PrimaryChain*> ds_chains =
      _global_event->GetDSPrimaryChainOrphans();
  for (auto us_chain_iter = us_chains.begin();
       us_chain_iter != us_chains.end(); ++us_chain_iter) {
    for (auto ds_chain_iter = ds_chains.begin();
         ds_chain_iter != ds_chains.end(); ++ds_chain_iter) {
      DataStructure::Global::PrimaryChain* through_primary_chain =
          new DataStructure::Global::PrimaryChain("MapCppGlobalTrackMatching",
          MAUS::DataStructure::Global::kThrough);
      std::vector<DataStructure::Global::PID> pids = PIDHypotheses(0, _pid_hypothesis_string);
      for (size_t i = 0; i < pids.size(); i++) {
        DataStructure::Global::Track* us_track =
                                    (*us_chain_iter)->GetMatchedTrack(pids[i]);
        DataStructure::Global::Track* ds_track =
                                    (*ds_chain_iter)->GetMatchedTrack(pids[i]);
        DataStructure::Global::Track* propagate_track =
                                             new DataStructure::Global::Track();
        DataStructure::Global::Track* tof12_track =
                                             new DataStructure::Global::Track();
        bool tof12_match = false;
        bool propagate_match = false;
        if (_through_algo == kTOF12 ||
            _through_algo == kPropagateRequiringTOF12 ||
            _through_algo == kPropagateAndTOF12) {
            tof12_match = throughMatchTOF(us_track, ds_track, pids[i], tof12_track);
        }
        if ((_through_algo == kPropagateRequiringTOF12 && tof12_match) ||
            _through_algo == kPropagate ||
            _through_algo == kPropagateAndTOF12) {
            propagate_match = throughMatchPropagate(us_track, ds_track, pids[i], propagate_track);
        }
        if (propagate_match) {
            through_primary_chain->AddMatchedTrack(propagate_track);
            delete tof12_track;
        } else if (tof12_match) {
            through_primary_chain->AddMatchedTrack(tof12_track);
            delete propagate_track;
        } else {
            delete tof12_track;
            delete propagate_track;
        }
      }

      if (through_primary_chain->GetMatchedTracks().size() > 0) {
        (*us_chain_iter)->set_chain_type(DataStructure::Global::kUS);
        (*ds_chain_iter)->set_chain_type(DataStructure::Global::kDS);
        through_primary_chain->SetUSDaughter(*us_chain_iter);
        through_primary_chain->SetDSDaughter(*ds_chain_iter);
        _global_event->add_primary_chain(through_primary_chain);
      }
    }
  }
  CheckChainMultiplicity();
}

DataStructure::Global::TrackPArray* TrackMatching::GetDetectorTrackArray(
    DataStructure::Global::DetectorPoint detector) {
  DataStructure::Global::TrackPArray *imported_tracks =
      _global_event->get_tracks();
  DataStructure::Global::TrackPArray *track_array =
      new DataStructure::Global::TrackPArray();
  for (auto imported_track_iter = imported_tracks->begin();
       imported_track_iter != imported_tracks->end();
       ++imported_track_iter) {
    DataStructure::Global::Track* imported_track = (*imported_track_iter);
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

std::vector<DataStructure::Global::SpacePoint*>
    TrackMatching::GetDetectorSpacePoints(
    DataStructure::Global::DetectorPoint detector) {
  std::vector<DataStructure::Global::SpacePoint*> space_points;
  std::vector<DataStructure::Global::SpacePoint*> *global_spacepoint_array =
      _global_event->get_space_points();
  for (size_t i = 0; i < global_spacepoint_array->size(); i++) {
    if (global_spacepoint_array->at(i) &&
        global_spacepoint_array->at(i)->get_detector() == detector) {
      space_points.push_back(global_spacepoint_array->at(i));
    }
  }
  return space_points;
}

std::vector<DataStructure::Global::PID> TrackMatching::PIDHypotheses(
    int charge_hypothesis, std::string pid_hypothesis_string) {
  std::vector<DataStructure::Global::PID> pids;
  // First check if a specific PID has been forced by the configuration
  if (pid_hypothesis_string == "kEPlus") {
      pids.push_back(DataStructure::Global::kEPlus);
  } else if (pid_hypothesis_string == "kEMinus") {
      pids.push_back(DataStructure::Global::kEMinus);
  } else if (pid_hypothesis_string == "kMuPlus") {
      pids.push_back(DataStructure::Global::kMuPlus);
  } else if (pid_hypothesis_string == "kMuMinus") {
      pids.push_back(DataStructure::Global::kMuMinus);
  } else if (pid_hypothesis_string == "kPiPlus") {
      pids.push_back(DataStructure::Global::kPiPlus);
  } else if (pid_hypothesis_string == "kPiMinus") {
      pids.push_back(DataStructure::Global::kPiMinus);
  } else {
    // No specific PID forced, hence we assign by charge hypothesis
    if (charge_hypothesis != -1) {
      pids.push_back(DataStructure::Global::kEPlus);
      pids.push_back(DataStructure::Global::kMuPlus);
      pids.push_back(DataStructure::Global::kPiPlus);
    }
    if (charge_hypothesis != 1) {
      pids.push_back(DataStructure::Global::kEMinus);
      pids.push_back(DataStructure::Global::kMuMinus);
      pids.push_back(DataStructure::Global::kPiMinus);
    }
  }
  return pids;
}

void TrackMatching::MatchTrackPoint(
    TLorentzVector &position, TLorentzVector &momentum,
    const std::vector<DataStructure::Global::SpacePoint*> &spacepoints,
    DataStructure::Global::PID pid, BTFieldConstructor* field,
    std::string detector_name, DataStructure::Global::Track* hypothesis_track) {
  double mass = Particle::GetInstance().GetMass(pid);
  double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
  double x_in[] = {0., position.X(), position.Y(), position.Z(),
                   energy, momentum.X(), momentum.Y(), momentum.Z()};
  ofstream tof1file;
  ofstream tof2file;
  ofstream klfile;
  if (_residuals) {
    tof1file.open("match_tof1.csv", std::ios::out | std::ios::app);
    tof2file.open("match_tof2.csv", std::ios::out | std::ios::app);
    klfile.open("match_kl.csv", std::ios::out | std::ios::app);
  }
  if (spacepoints.size() > 0) {
    double target_z = spacepoints.at(0)->get_position().Z();
    try {
      ClearVirtualTrackPoints();
      Propagate(x_in, field, pid, target_z);
      // To avoid doing the same propagation again for the same point, store the propagated
      // values back in the TLorentzVectors
      position.SetX(x_in[1]);
      position.SetY(x_in[2]);
      position.SetZ(x_in[3]);
      momentum.SetX(x_in[5]);
      momentum.SetY(x_in[6]);
      momentum.SetZ(x_in[7]);
      // Temporary container for trackpoints for checking if multiple matches are compatible
      std::vector<DataStructure::Global::SpacePoint*> temp_spacepoints;
      for (size_t i = 0; i < spacepoints.size(); i++) {
        if (_residuals) {
          if (detector_name == "TOF1") {
            tof1file << spacepoints.at(i)->get_position().X() - x_in[1] << " "
                     << spacepoints.at(i)->get_position().Y() - x_in[2] << "\n";
          } else if (detector_name == "TOF2") {
            tof2file << spacepoints.at(i)->get_position().X() - x_in[1] << " "
                     << spacepoints.at(i)->get_position().Y() - x_in[2] << "\n";
          } else if (detector_name == "KL") {
            klfile << spacepoints.at(i)->get_position().Y() - x_in[2] << " " << x_in[4] << "\n";
          }
        }
        // Check if TrackPoints match and if yes, collect them to later check for consistency
        if (GlobalTools::approx(x_in[1], spacepoints.at(i)->get_position().X(),
                _matching_tolerances.at(detector_name).first) and
            GlobalTools::approx(x_in[2], spacepoints.at(i)->get_position().Y(),
                _matching_tolerances.at(detector_name).second)) {
          temp_spacepoints.push_back(spacepoints.at(i));
          Squeak::mout(Squeak::debug) << "TrackMatching: "
                                      << detector_name << " Match" << std::endl;
        } else {
          Squeak::mout(Squeak::debug) << "TrackMatching: " << detector_name
              << " Mismatch, dx, dy are:\n"
              << x_in[1] - spacepoints.at(i)->get_position().X() << " "
              << x_in[2] - spacepoints.at(i)->get_position().Y() << std::endl;
        }
      }
      // If there are multiple matches, this checks if they could have been from the same particle
      AddIfConsistent(temp_spacepoints, hypothesis_track);
    } catch (Exceptions::Exception exc) {
      Squeak::mout(Squeak::debug) << exc.what() << std::endl;
    }
  }
}

void TrackMatching::MatchTOF0(
    const TLorentzVector &position, const TLorentzVector &momentum,
    double tof1_z, double tof1_t,
    const std::vector<DataStructure::Global::SpacePoint*> &spacepoints,
    DataStructure::Global::PID pid, BTFieldConstructor* field,
    DataStructure::Global::Track* hypothesis_track) {
  ofstream tof0file;
  if (_residuals) {
    tof0file.open("match_tof0.csv", std::ios::out | std::ios::app);
  }
  double mass = Particle::GetInstance().GetMass(pid);
  double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
  if (spacepoints.size() > 0) {
    double x_in[] = {0., position.X(), position.Y(), position.Z(),
                     energy, momentum.X(), momentum.Y(), momentum.Z()};
    // First propagate to just upstream of TOF1 to get the energy during TOF0-1 transit
    try {
      ClearVirtualTrackPoints();
      Propagate(x_in, field, pid, tof1_z - 25.0);
    } catch (Exceptions::Exception exc) {
      Squeak::mout(Squeak::debug) << exc.what() << std::endl;
    }
    // Calculate the distance to TOF0 and the approximate distance the particle travelled
    double z_distance = tof1_z - spacepoints.at(0)->get_position().Z();
    double approx_travel_distance =
        z_distance*std::sqrt(x_in[5]*x_in[5] + x_in[6]*x_in[6] + x_in[7]*x_in[7])/x_in[7];
    // To account for energy loss in air, calculate approximate energy loss for half the distance
    // travelled (to obtain ~average velocity)
    G4NistManager* manager = G4NistManager::Instance();
    G4Material* air = manager->FindOrBuildMaterial("G4_AIR");
    double air_E_loss = GlobalTools::dEdx(air, energy, mass)*approx_travel_distance*0.5;
    GlobalTools::changeEnergy(x_in, -air_E_loss, mass);
    double velocity = (x_in[7] / x_in[4]) * CLHEP::c_light;


    // Compare reconstructed delta T with expected travel time
    // Change later to be set by datacards
    double deltaTMin = (z_distance/velocity) - _matching_tolerances.at("TOF0").first;
    double deltaTMax = (z_distance/velocity) + _matching_tolerances.at("TOF0").first;
    // Temporary container for trackpoints for checking if multiple matches are compatible
    std::vector<DataStructure::Global::SpacePoint*> temp_spacepoints;
    for (size_t i = 0; i < spacepoints.size(); i++) {
      double deltaT = tof1_t - spacepoints.at(i)->get_position().T();
      if (_residuals) {
        tof0file << deltaT - (z_distance/velocity) << " " << deltaT + x_in[0] << "\n";
      }
      if (deltaT > deltaTMin and deltaT < deltaTMax) {
        temp_spacepoints.push_back(spacepoints.at(i));
        Squeak::mout(Squeak::debug) << "TrackMatching: TOF0 Match"
                                    << std::endl;
      } else {
        Squeak::mout(Squeak::debug) << "TrackMatching: TOF0 Mismatch, "
            << "dT is " << deltaT << " when expected between " << deltaTMin
            << " and " << deltaTMax << std::endl;
      }
    }
    AddIfConsistent(temp_spacepoints, hypothesis_track);
  }
}

void TrackMatching::MatchEMRTrack(
    const TLorentzVector &position, const TLorentzVector &momentum,
    DataStructure::Global::TrackPArray* emr_track_array,
    DataStructure::Global::PID pid, BTFieldConstructor* field,
    DataStructure::Global::Track* hypothesis_track) {
  double mass = Particle::GetInstance().GetMass(pid);
  double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
  // Here we need to iterate over the EMR tracks first, as they might have
  // different starting z. Given expected particle rates at the EMR and the
  // way the EMR would handle multiple particles, we don't need to take great
  // care with multiple matches and will just stop iterating after the first
  // match
  bool matched = false;
  ofstream emrfile;
  if (_residuals) {
    emrfile.open("match_emr.csv", std::ios::out | std::ios::app);
  }
  for (auto emr_track_iter = emr_track_array->begin();
       emr_track_iter != emr_track_array->end();
       ++emr_track_iter) {
    std::vector<const DataStructure::Global::TrackPoint*> emr_trackpoints =
        (*emr_track_iter)->GetTrackPoints();
    double x_in[] = {0., position.X(), position.Y(), position.Z(),
                     energy, momentum.X(), momentum.Y(), momentum.Z()};
    TLorentzVector first_hit_pos = emr_trackpoints[0]->get_position();
    TLorentzVector first_hit_pos_err = emr_trackpoints[0]->get_position_error();
    double target_z = first_hit_pos.Z();
    try {
      ClearVirtualTrackPoints();
      Propagate(x_in, field, pid, target_z);
      if (_residuals) {
        emrfile << first_hit_pos.X() - x_in[1] << " " << first_hit_pos.Y() - x_in[2] << "\n";
      }
      if (GlobalTools::approx(x_in[1], first_hit_pos.X(), _matching_tolerances.at("EMR").first) and
          GlobalTools::approx(x_in[2], first_hit_pos.Y(), _matching_tolerances.at("EMR").second)) {
        matched = true;
        Squeak::mout(Squeak::debug) << "TrackMatching: EMR Match" << std::endl;
        hypothesis_track->set_emr_range_primary(
            (*emr_track_iter)->get_emr_range_primary());
        hypothesis_track->set_emr_plane_density(
            (*emr_track_iter)->get_emr_plane_density());
        for (size_t i = 0; i < emr_trackpoints.size(); i++) {
          DataStructure::Global::TrackPoint* emr_tp =
              emr_trackpoints[i]->Clone();
          emr_tp->set_mapper_name("MapCppGlobalTrackMatching_DS");
          TLorentzVector momentum = emr_tp->get_momentum();
          double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
          momentum.SetE(energy);
          emr_tp->set_momentum(momentum);
          hypothesis_track->AddTrackPoint(emr_tp);
        }
        AddVirtualTrackPoints(hypothesis_track);
      } else {
        Squeak::mout(Squeak::debug)
            << "TrackMatching: EMR Mismatch, dx, dy are:\n"
            << x_in[1] - first_hit_pos.X() << " "
            << x_in[2] - first_hit_pos.Y() << std::endl;
      }
    } catch (Exceptions::Exception exc) {
      Squeak::mout(Squeak::debug) << exc.what() << std::endl;
    }
    if (matched) {
      break;
    }
  }
}

void TrackMatching::AddTrackerTrackPoints(
    DataStructure::Global::Track* tracker_track, std::string mapper_name,
    double mass, DataStructure::Global::Track* hypothesis_track) {
  std::vector<const DataStructure::Global::TrackPoint*> tracker_trackpoints =
      tracker_track->GetTrackPoints();
  std::sort(tracker_trackpoints.begin(), tracker_trackpoints.end(),
            GlobalTools::TrackPointSort);
  for (size_t i = 0; i < tracker_trackpoints.size(); i++) {
    DataStructure::Global::TrackPoint* tracker_tp =
        tracker_trackpoints[i]->Clone();
    tracker_tp->set_mapper_name(mapper_name);
    TLorentzVector momentum = tracker_tp->get_momentum();
    double energy = ::sqrt(momentum.Rho()*momentum.Rho() + mass*mass);
    momentum.SetE(energy);
    tracker_tp->set_momentum(momentum);
    hypothesis_track->AddTrackPoint(tracker_tp);
  }
  AddVirtualTrackPoints(hypothesis_track);
}

void TrackMatching::Propagate(double* x_in,
                              BTFieldConstructor* field,
                              DataStructure::Global::PID pid,
                              double target_z) {
    std::vector<double>::iterator start, end;
    if (target_z > x_in[3]) {
        start = std::lower_bound(_extra_z_planes.begin(),
                                 _extra_z_planes.end(), x_in[3]);
        end = std::upper_bound(_extra_z_planes.begin(),
                               _extra_z_planes.end(), target_z);
    } else {
        start = std::lower_bound(_extra_z_planes.begin(),
                                 _extra_z_planes.end(), target_z);
        end = std::upper_bound(_extra_z_planes.begin(),
                               _extra_z_planes.end(), x_in[3]);
    }
    for (/*start defined above*/; start < end && start < _extra_z_planes.end(); ++start) {
        GlobalTools::propagate(x_in, *start, field, _max_step_size, pid,
                           _energy_loss, static_cast<int>(_geo_algo));
        DataStructure::Global::TrackPoint tp;
        tp.set_position(TLorentzVector(x_in[1], x_in[2], x_in[3], x_in[0]));
        tp.set_momentum(TLorentzVector(x_in[5], x_in[6], x_in[7], x_in[4]));
        _virtual_track_points.push_back(tp);
    }
    GlobalTools::propagate(x_in, target_z, field, _max_step_size, pid,
                       _energy_loss, static_cast<int>(_geo_algo));
}

void TrackMatching::AddVirtualTrackPoints(DataStructure::Global::Track* hypothesis_track) {
    int particle_event = -1;
    if (hypothesis_track->GetTrackPoints().size() > 0) {
        particle_event = hypothesis_track->GetTrackPoints()[0]->get_particle_event();
    }
    for (size_t i = 0; i < _virtual_track_points.size(); ++i) {
        DataStructure::Global::SpacePoint* sp =
                                        new DataStructure::Global::SpacePoint();
        sp->set_detector(DataStructure::Global::kVirtual);
        sp->set_charge(hypothesis_track->get_charge());
        sp->set_mapper_name(_mapper_name);
        DataStructure::Global::TrackPoint* tp =
                new DataStructure::Global::TrackPoint(_virtual_track_points[i]);
        tp->set_detector(DataStructure::Global::kVirtual);
        tp->set_charge(hypothesis_track->get_charge());
        tp->set_mapper_name(_mapper_name);
        tp->set_space_point(sp);
        tp->set_particle_event(particle_event);
        hypothesis_track->AddTrackPoint(tp);
    }
}

void TrackMatching::ClearVirtualTrackPoints() {
    _virtual_track_points = std::vector<DataStructure::Global::TrackPoint>();
}
double TrackMatching::TOFTimeFromTrackPoints(
    DataStructure::Global::TrackPointCPArray trackpoints,
    DataStructure::Global::DetectorPoint detector) {
  double TOF_time = 0.0;
  for (auto trackpoint_iter = trackpoints.begin();
       trackpoint_iter != trackpoints.end();
       ++trackpoint_iter) {
    if ((*trackpoint_iter)->get_detector() == detector) {
      TOF_time = (*trackpoint_iter)->get_position().T();
    }
  }
  return TOF_time;
}

void TrackMatching::AddIfConsistent(std::vector<DataStructure::Global::SpacePoint*> spacepoints,
                     DataStructure::Global::Track* hypothesis_track) {
  bool consistent = true;
  // If there are no trackpoints, we don't have to add anything, if there is
  // exactly one, we leave consistent at true so that it gets added in the end
  if (spacepoints.size() < 1) {
    consistent = false;
  } else if (spacepoints.size() > 1) {
    // For the KL, we can only compare by y position
    if (spacepoints.at(0)->get_detector() == DataStructure::Global::kCalorimeter) {
      for (size_t i = 0; i < spacepoints.size() - 1; i++) {
        for (size_t j = i + 1; j < spacepoints.size(); j++) {
          if (!(GlobalTools::approx(spacepoints.at(i)->get_position().Y(),
              spacepoints.at(j)->get_position().Y(), 50.0))) {
            consistent = false;
          }
        }
      }
    } else {
      // Though the detector granularity is different for TOF0, we can use the same
      // x and y allowance for all TOFs, 70mm is above one slab and below two slabs
      // for all TOFs
      for (size_t i = 0; i < spacepoints.size() - 1; i++) {
        for (size_t j = i + 1; j < spacepoints.size(); j++) {
          if (!(GlobalTools::approx(spacepoints.at(i)->get_position().X(),
              spacepoints.at(j)->get_position().X(), 70.0))) {
            consistent = false;
          }
          if (!(GlobalTools::approx(spacepoints.at(i)->get_position().Y(),
              spacepoints.at(j)->get_position().Y(), 70.0))) {
            consistent = false;
          }
          if (!(GlobalTools::approx(spacepoints.at(i)->get_position().T(),
              spacepoints.at(j)->get_position().T(), 0.5))) {
            consistent = false;
          }
        }
      }
    }
  }
  // If we only have one point or multiple consistent ones, we add everything to
  // the hypothesis track, else we don't add anything as we can't tell which
  // trackpoint actually belongs to the track
  if (consistent) {
    for (size_t i = 0; i < spacepoints.size(); i++) {
      DataStructure::Global::TrackPoint* trackpoint =
          new DataStructure::Global::TrackPoint(spacepoints.at(i));
      trackpoint->set_mapper_name(_mapper_name);
      hypothesis_track->AddTrackPoint(trackpoint);
    }
    AddVirtualTrackPoints(hypothesis_track);
  }
  return;
}

void TrackMatching::CheckChainMultiplicity() {
  std::vector<DataStructure::Global::PrimaryChain*> through_chains =
      _global_event->GetThroughPrimaryChains();
  std::vector<std::pair<bool, bool> > multiplicities
      (through_chains.size(), std::make_pair(false, false));
  // Last execution won't reach the inner loop but is necessary for assignment of enums at the end
  for (size_t i = 0; i < through_chains.size(); i++) {
    for (size_t j = i + 1; j < through_chains.size(); j++) {
      if (through_chains.at(i)->GetUSDaughter() == through_chains.at(j)->GetUSDaughter()) {
        multiplicities.at(i).first = true;
        multiplicities.at(j).first = true;
      }
      if (through_chains.at(i)->GetDSDaughter() == through_chains.at(j)->GetDSDaughter()) {
        multiplicities.at(i).second = true;
        multiplicities.at(j).second = true;
      }
    }
    if (multiplicities.at(i).first and multiplicities.at(i).second) {
      through_chains.at(i)->set_multiplicity(DataStructure::Global::kMultipleBoth);
    } else if (multiplicities.at(i).first) {
      through_chains.at(i)->set_multiplicity(DataStructure::Global::kMultipleUS);
    } else if (multiplicities.at(i).second) {
      through_chains.at(i)->set_multiplicity(DataStructure::Global::kMultipleDS);
    }
  }
}

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
