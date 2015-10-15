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

#include <iostream>

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/reduce/ReduceCppGlobalReconEfficiency/ReduceCppGlobalReconEfficiency.hh"

#include "src/common_cpp/Recon/Global/MCTruthTools.hh"
#include "src/common_cpp/Recon/Global/GlobalTools.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Primary.hh"

namespace MAUS {

void ReduceCppGlobalReconEfficiency::_birth(const std::string& json_config) {
  Json::Value config = JsonWrapper::StringToJson(json_config);
  for (size_t i = 0; i < 9; i++) {
    _detector_matches[i] = 0;
    _detector_matches_expected[i] = 0;
    _detector_false_matches[i] = 0;
    _detector_lr_failed[i] = 0;
  }
}

ImageData* ReduceCppGlobalReconEfficiency::_process(Data* data) {
  _spill = data->GetSpill();
  //~ std::cerr << "EMR FAIL: " << _detector_lr_failed[4] << "\n";
  // Upstream
  std::vector<std::pair<MAUS::DataStructure::Global::Track*, MAUS::MCEvent*> >
      us_track_mc_pairs = matchTrackerReconWithMC(
      "MapCppGlobalTrackMatching-US", MAUS::DataStructure::Global::kMuPlus);
  for (size_t i = 0; i < us_track_mc_pairs.size(); i++) {
    MAUS::DataStructure::Global::Track* track = us_track_mc_pairs[i].first;
    MAUS::MCEvent* mc_event = us_track_mc_pairs[i].second;
    double p_time = mc_event->GetPrimary()->GetTime();
    TOFEfficiency(0, MAUS::DataStructure::Global::kTOF0, track, mc_event,
                  p_time, 30, 30, 2);
    TOFEfficiency(1, MAUS::DataStructure::Global::kTOF1, track, mc_event,
                  p_time, 40, 40, 2);
  }
  // Downstream
  std::vector<std::pair<MAUS::DataStructure::Global::Track*, MAUS::MCEvent*> >
      ds_track_mc_pairs = matchTrackerReconWithMC(
      "MapCppGlobalTrackMatching-DS", MAUS::DataStructure::Global::kMuPlus);
  for (size_t i = 0; i < ds_track_mc_pairs.size(); i++) {
    MAUS::DataStructure::Global::Track* track = ds_track_mc_pairs[i].first;
    MAUS::MCEvent* mc_event = ds_track_mc_pairs[i].second;
    double p_time = mc_event->GetPrimary()->GetTime();
    TOFEfficiency(2, MAUS::DataStructure::Global::kTOF2, track, mc_event,
                  p_time, 40, 40, 2);
    KLEfficiency(track, mc_event, 30);
    EMREfficiency(track, mc_event);
  }

  // Through-Tracks
  std::vector<std::pair<MAUS::DataStructure::Global::Track*, MAUS::MCEvent*> >
      through_track_mc_pairs = matchTrackerReconWithMC(
      "MapCppGlobalTrackMatching-Through", MAUS::DataStructure::Global::kMuPlus);
  for (size_t i = 0; i < through_track_mc_pairs.size(); i++) {
    MAUS::DataStructure::Global::Track* track = through_track_mc_pairs[i].first;
    MAUS::MCEvent* mc_event = through_track_mc_pairs[i].second;
    double p_time = mc_event->GetPrimary()->GetTime();
    throughEfficiency(track, mc_event, p_time, 40, 40, 40, 40, 2, 2);
  }
  ImageData * image_data = new ImageData();
  image_data->SetImage(new Image());
  return image_data; 
}

void ReduceCppGlobalReconEfficiency::_death()  {
  // US Aggregate
  _detector_matches[5] = _detector_matches[0] + _detector_matches[1];
  _detector_matches_expected[5] = _detector_matches_expected[0] +
      _detector_matches_expected[1];
  _detector_false_matches[5] = _detector_false_matches[0] +
      _detector_false_matches[1];
  _detector_lr_failed[5] = _detector_lr_failed[0] + _detector_lr_failed[1];

  // DS Aggregate
  _detector_matches[6] = _detector_matches[2] + _detector_matches[3] +
      _detector_matches[4];
  _detector_matches_expected[6] = _detector_matches_expected[2] +
      _detector_matches_expected[3] + _detector_matches_expected[4];
  _detector_false_matches[6] = _detector_false_matches[2] +
      _detector_false_matches[3] + _detector_false_matches[4];
  _detector_lr_failed[6] = _detector_lr_failed[2] + _detector_lr_failed[3] +
      _detector_lr_failed[4];

  // Total Aggregate
  _detector_matches[7] = _detector_matches[5] + _detector_matches[6];
  _detector_matches_expected[7] = _detector_matches_expected[5] +
      _detector_matches_expected[6];
  _detector_false_matches[7] = _detector_false_matches[5] +
      _detector_false_matches[6];
  _detector_lr_failed[7] = _detector_lr_failed[5] + _detector_lr_failed[6];

  double detector_efficiency[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double detector_total_efficiency[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double detector_purity[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  
  std::vector<std::string> detector_labels;
  detector_labels.push_back("TOF0");
  detector_labels.push_back("TOF1");
  detector_labels.push_back("TOF2");
  detector_labels.push_back("KL  ");
  detector_labels.push_back("EMR ");
  detector_labels.push_back("US  ");
  detector_labels.push_back("DS  ");
  detector_labels.push_back("Tot.");
  detector_labels.push_back("Thru");
  std::cerr
      << "     ┏━━━━━━━┳━━━━━━━┳━━━━━━━┳━━━━━━━┳━━━━━━━┳━━━━━━━┳━━━━━━━┓\n"
      << "     ┃Matches┃Expect ┃False  ┃LR Fail┃Efficie┃Effici*┃Purity ┃\n"
      << "┏━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━┫\n";
  for (size_t i = 0; i < 9; i++) {
    if (_detector_matches_expected[i] > 0) {
      detector_efficiency[i] = (_detector_matches[i] + 0.0)*100 /
          _detector_matches_expected[i];
    }
    if (_detector_matches_expected[i] > 0 or _detector_lr_failed[i] > 0) {
      detector_total_efficiency[i] = ((_detector_matches[i] + 0.0)*100) /
          (_detector_matches_expected[i] + _detector_lr_failed[i]);
    }
    if (_detector_matches[i] > 0) {
      detector_purity[i] = 100 - ((_detector_false_matches[i] + 0.0)*100 /
          _detector_matches[i]);
    }
    std::cerr << "┃" << detector_labels[i];
    printf("┃%7u┃%7u┃%7u┃%7u┃%6.2f%%┃%6.2f%%┃%6.2f%%┃\n",
           _detector_matches[i], _detector_matches_expected[i],
           _detector_false_matches[i], _detector_lr_failed[i],
           detector_efficiency[i], detector_total_efficiency[i],
           detector_purity[i]);
    if (i == 1 or i == 4 or i == 6 or i == 7) {
      std::cerr
          << "┣━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━╋━━━━━━━┫\n";
    }
  }
  std::cerr
    << "┗━━━━┻━━━━━━━┻━━━━━━━┻━━━━━━━┻━━━━━━━┻━━━━━━━┻━━━━━━━┻━━━━━━━┛\n";

  Json::Value *json = Globals::GetConfigurationCards();
  const char* eff_filename = (*json)["efficiency_filename"].asString().c_str();
  ofstream eff_file;
  eff_file.open(eff_filename);
  for (size_t i = 0; i < 5; i++) {
  eff_file << _detector_matches[i] << "\t" << _detector_matches_expected[i]
      << "\t" << _detector_false_matches[i] << "\t" << _detector_lr_failed[i]
      << "\n";
  }
  eff_file << _detector_matches[8] << "\t" << _detector_matches_expected[8]
      << "\t" << _detector_false_matches[8] << "\t" << _detector_lr_failed[8]
      << "\n";
  eff_file.close();
}

bool ReduceCppGlobalReconEfficiency::checkDetector(const
    MAUS::DataStructure::Global::TrackPoint* track_point, int min, int max) {
  int enum_key = static_cast<int>(track_point->get_detector());
  if ((enum_key >= min) and (enum_key <= max)) {
    return true;
  } else {
    return false;
  }
}

bool ReduceCppGlobalReconEfficiency::checkDetector(const
    MAUS::DataStructure::Global::SpacePoint* space_point, int min, int max) {
  int enum_key = static_cast<int>(space_point->get_detector());
  if ((enum_key >= min) and (enum_key <= max)) {
    return true;
  } else {
    return false;
  }
}

bool ReduceCppGlobalReconEfficiency::TOFReconMatchesMC(
    TLorentzVector recon_position, TOFHit tof_hit, double primary_time,
    double dX_max, double dY_max, double dT_max) {
  ThreeVector mc_position = tof_hit.GetPosition();
  double mc_time = tof_hit.GetTime();
  double dX = std::abs(mc_position.X() - recon_position.X());
  double dY = std::abs(mc_position.Y() - recon_position.Y());
  double dT = std::abs(mc_time - primary_time - recon_position.T());
  if (dX < dX_max and dY < dY_max and dT < dT_max) {
    return true;
  } else {
    //~ std::cerr << "TOF dX,dY,dT: " << dX << " " << dY << " " << dT << "\n";
    return false;
  }
}

bool ReduceCppGlobalReconEfficiency::KLReconMatchesMC(
    TLorentzVector recon_position, KLHit kl_hit, double dY_max) {
  ThreeVector mc_position = kl_hit.GetPosition();
  double dY = std::abs(mc_position.Y() - recon_position.Y());
  if (dY < dY_max) {
    return true;
  } else {
    //~ std::cerr << "KL dY: " << dY << "\n";
    return false;
  }
}

bool ReduceCppGlobalReconEfficiency::EMRReconMatchesMC(
    TLorentzVector recon_position, EMRHit emr_hit,
    double dX_max, double dY_max) {
  ThreeVector mc_position = emr_hit.GetPosition();
  double dX = std::abs(mc_position.X() - recon_position.X());
  double dY = std::abs(mc_position.Y() - recon_position.Y());
  if (dX < dX_max and dY < dY_max) {
    return true;
  } else {
    std::cerr << "EMR dX,dY: " << dX << "/" << dX_max << " (" << dX*100/dX_max - 100 << ") - " << dY << "/" << dY_max << " (" << dY*100/dY_max - 100 << ")\n";
    return false;
  }
}

void ReduceCppGlobalReconEfficiency::TOFEfficiency(size_t detector_number,
    MAUS::DataStructure::Global::DetectorPoint detector,
    MAUS::DataStructure::Global::Track* track, MAUS::MCEvent* mc_event,
    double p_time, double dX_max, double dY_max, double dT_max) {
  TLorentzVector tp_position;
  MAUS::TOFHitArray* tof_hits = MCTruthTools::GetTOFHits(mc_event, detector);
  // Do we have MC hits for the TOF
  if (tof_hits) {
    //~ std::cerr << detector << "\n";
    // First check if there is a corresponding spacepoint in the spill i.e. if
    // we would expect a match to occur
    std::vector<MAUS::DataStructure::Global::SpacePoint*>*
          tof_sps = GlobalTools::GetSpillSpacePoints(_spill, detector);
    if (tof_sps) {
      //~ std::cerr << "TOF SPs\n";
      for (size_t i = 0; i < tof_sps->size(); i++) {
        TLorentzVector sp_position = tof_sps->at(i)->get_position();
        TLorentzVector sp_position_error = tof_sps->at(i)->get_position_error();
        //~ std::cerr << "TOF ERROR xy: " << sp_position_error.X() << " " << sp_position_error.Y() << "\n";
        TOFHit tof_hit = MCTruthTools::GetNearestZHit(tof_hits, sp_position);
        if (TOFReconMatchesMC(sp_position, tof_hit, p_time,
                              dX_max, dY_max, dT_max)) {
          _detector_matches_expected[detector_number]++;
          // It's possible that multiple matches occur depending on the data
          // meaning we might get false positives, but definitely shouldn't
          // count twice
          break;
        }
        // We haven't found any matches
        //~ std::cerr << "TOF " << detector_number + 1 << " FAILED\n";
        _detector_lr_failed[detector_number]++;
      }
    } else {
      //~ std::cerr << "No TOF SPs\n";
      _detector_lr_failed[detector_number]++;
    }
    MAUS::DataStructure::Global::TrackPointCPArray tof_tps =
        track->GetTrackPoints(detector);
    if (tof_tps.size() > 0) {
      // Usually there will be only one hit, if not, pick out the last one
      tp_position = tof_tps[tof_tps.size() -1]->get_position();
      TOFHit tof_hit = MCTruthTools::GetNearestZHit(tof_hits, tp_position);
      // Check if hits agree, if yes, that's one point for efficiency
      if (TOFReconMatchesMC(tp_position, tof_hit, p_time,
                            dX_max, dY_max, dT_max)) {
        _detector_matches[detector_number]++;
      } else {
        // We have a trackpoint for the detector but it doesn't match the MC,
        // hence it's a false match
        //~ std::cerr << "TOF " << detector_number + 1 << " FALSE\n";
        _detector_false_matches[detector_number]++;
      }
    }
  }
}

void ReduceCppGlobalReconEfficiency::KLEfficiency(
    MAUS::DataStructure::Global::Track* track, MAUS::MCEvent* mc_event,
    double dY_max) {
  TLorentzVector tp_position;
  MAUS::DataStructure::Global::DetectorPoint detector =
      MAUS::DataStructure::Global::kCalorimeter;
  size_t detector_number = 3;
  MAUS::KLHitArray* kl_hits = mc_event->GetKLHits();
  // Do we have MC hits for the KL
  if (kl_hits) {
    // First check if there is a corresponding spacepoint in the spill i.e. if
    // we would expect a match to occur
    std::vector<MAUS::DataStructure::Global::SpacePoint*>*
        kl_sps = GlobalTools::GetSpillSpacePoints(_spill, detector);
    if (kl_sps) {
      for (size_t i = 0; i < kl_sps->size(); i++) {
        TLorentzVector sp_position = kl_sps->at(i)->get_position();
        //~ TLorentzVector sp_position_error = kl_sps->at(i)->get_position_error();
        //~ std::cerr << "KL ERROR xy: " << sp_position_error.X() << " " << sp_position_error.Y() << "\n";
        KLHit kl_hit = MCTruthTools::GetNearestZHit(kl_hits, sp_position);
        if (KLReconMatchesMC(sp_position, kl_hit, dY_max)) {
          _detector_matches_expected[detector_number]++;
          // It's possible that multiple matches occur depending on the data
          // meaning we might get false positives, but definitely shouldn't
          // count twice
          break;
        }
        // We haven't found any matches
        //~ std::cerr "KL: " << detector number << "\n";
        _detector_lr_failed[detector_number]++;
      }
    } else {
      //~ std::cerr "KL: " << detector number << "\n";
      _detector_lr_failed[detector_number]++;
    }
    MAUS::DataStructure::Global::TrackPointCPArray kl_tps =
        track->GetTrackPoints(detector);
    if (kl_tps.size() > 0) {
      // Usually there will be only one hit, if not, pick out the last one
      tp_position = kl_tps[kl_tps.size() -1]->get_position();
      KLHit kl_hit = MCTruthTools::GetNearestZHit(kl_hits, tp_position);
      // Check if hits agree, if yes, that's one point for efficiency
      if (KLReconMatchesMC(tp_position, kl_hit, dY_max)) {
        _detector_matches[detector_number]++;
      } else {
        // We have a trackpoint for the detector but it doesn't match the MC,
        // hence it's a false match
        _detector_false_matches[detector_number]++;
      }
    }
  }
}


void ReduceCppGlobalReconEfficiency::EMREfficiency(
    MAUS::DataStructure::Global::Track* track, MAUS::MCEvent* mc_event) {
  TLorentzVector tp_position;
  MAUS::DataStructure::Global::DetectorPoint detector =
      MAUS::DataStructure::Global::kEMR;
  size_t detector_number = 4;
  MAUS::EMRHitArray* emr_hits = mc_event->GetEMRHits();
  // Do we have MC hits for the EMR
  if (emr_hits) {
    // Load the EMR Recon tracks
    std::vector<MAUS::DataStructure::Global::Track*>* emr_tracks =
        GlobalTools::GetSpillDetectorTracks(_spill,
        MAUS::DataStructure::Global::kEMR, "MapCppGlobalReconImport");
    if (emr_tracks->size() > 0) {
      for (size_t i = 0; i < emr_tracks->size(); i++) {
        TLorentzVector emr_position = emr_tracks->at(i)->GetTrackPoints(
            detector)[0]->get_position();
        TLorentzVector emr_position_error = emr_tracks->at(i)->GetTrackPoints(
            detector)[0]->get_position_error();
        //~ std::cerr << "EMR ERROR xy: " << emr_position_error.X() << " " << emr_position_error.Y() << "\n";
        EMRHit emr_hit = MCTruthTools::GetNearestZHit(emr_hits, emr_position);
        if (EMRReconMatchesMC(emr_position, emr_hit, emr_position_error.X()*2.575, emr_position_error.Y()*2.575)) {
          _detector_matches_expected[detector_number]++;
          // It's possible that multiple matches occur depending on the data
          // meaning we might get false positives, but definitely shouldn't
          // count twice
          break;
        }
        // We haven't found any matches
        
        _detector_lr_failed[detector_number]++;
        //~ std::cerr << "EMR FAILED" << _detector_lr_failed[detector_number] << "\n";
      }
    } else {
      _detector_lr_failed[detector_number]++;
      //~ std::cerr << "EMR NO SPs" << _detector_lr_failed[detector_number] << "\n";
    }
    MAUS::DataStructure::Global::TrackPointCPArray emr_tps =
        track->GetTrackPoints(detector);
    if (emr_tps.size() > 0) {
      // Here we need to pick the first one
      tp_position = emr_tps[0]->get_position();
      TLorentzVector tp_position_error = emr_tps[0]->get_position_error();
      EMRHit emr_hit = MCTruthTools::GetNearestZHit(emr_hits, tp_position);
      // Check if hits agree, if yes, that's one point for efficiency
      if (EMRReconMatchesMC(tp_position, emr_hit, tp_position_error.X()*2.575, tp_position_error.Y()*2.575)) {
        _detector_matches[detector_number]++;
      } else {
        // We have a trackpoint for the detector but it doesn't match the MC,
        // hence it's a false match
        std::cerr << "EMR FALSE\n";
        _detector_false_matches[detector_number]++;
      }
    }
  }
}

void ReduceCppGlobalReconEfficiency::throughEfficiency(
    MAUS::DataStructure::Global::Track* track, MAUS::MCEvent* mc_event,
    double p_time, double dX_max1, double dX_max2,
    double dY_max1, double dY_max2, double dT_max1, double dT_max2) {
  TLorentzVector tp1_position;
  TLorentzVector tp2_position;
  MAUS::TOFHitArray* tof1_hits =
      MCTruthTools::GetTOFHits(mc_event, MAUS::DataStructure::Global::kTOF1);
  MAUS::TOFHitArray* tof2_hits =
      MCTruthTools::GetTOFHits(mc_event, MAUS::DataStructure::Global::kTOF2);
  // Do we have MC hits for the TOF
  if (tof1_hits and tof2_hits) {
    // First check if there are corresponding spacepoints in the spill i.e. if
    // we would expect a match to occur
    std::vector<MAUS::DataStructure::Global::SpacePoint*>* tof1_sps =
        GlobalTools::GetSpillSpacePoints(_spill,
                                         MAUS::DataStructure::Global::kTOF1);
    std::vector<MAUS::DataStructure::Global::SpacePoint*>* tof2_sps =
        GlobalTools::GetSpillSpacePoints(_spill,
                                         MAUS::DataStructure::Global::kTOF2);
    if (tof1_sps and tof2_sps) {
      for (size_t i = 0; i < tof1_sps->size(); i++) {
        for (size_t j = 0; j < tof2_sps->size(); j++) {
          TLorentzVector tof1_position = tof1_sps->at(i)->get_position();
          TLorentzVector tof2_position = tof2_sps->at(j)->get_position();
          TOFHit tof1_hit =
              MCTruthTools::GetNearestZHit(tof1_hits, tof1_position);
          TOFHit tof2_hit =
              MCTruthTools::GetNearestZHit(tof2_hits, tof2_position);
          if (TOFReconMatchesMC(tof1_position, tof1_hit, p_time,
                                dX_max1, dY_max1, dT_max1) and
              TOFReconMatchesMC(tof2_position, tof2_hit, p_time,
                                dX_max2, dY_max2, dT_max2)) {
            _detector_matches_expected[8]++;
            // It's possible that multiple matches occur depending on the data
            // meaning we might get false positives, but definitely shouldn't
            // count twice
            break;
          }
          // We haven't found any matches
          _detector_lr_failed[8]++;
        }
      }
    } else {
      _detector_lr_failed[8]++;
    }
    MAUS::DataStructure::Global::TrackPointCPArray tof1_tps =
        track->GetTrackPoints(MAUS::DataStructure::Global::kTOF1);
    MAUS::DataStructure::Global::TrackPointCPArray tof2_tps =
        track->GetTrackPoints(MAUS::DataStructure::Global::kTOF2);
    if (tof1_tps.size() > 0 and tof2_tps.size() > 0) {
      // Usually there will be only one hit, if not, pick out the last one
      tp1_position = tof1_tps[tof1_tps.size() -1]->get_position();
      tp2_position = tof2_tps[tof2_tps.size() -1]->get_position();
      TOFHit tof1_hit = MCTruthTools::GetNearestZHit(tof1_hits, tp1_position);
      TOFHit tof2_hit = MCTruthTools::GetNearestZHit(tof2_hits, tp2_position);
      // Check if hits agree, if yes, that's one point for efficiency
      if (TOFReconMatchesMC(tp1_position, tof1_hit, p_time,
                            dX_max1, dY_max1, dT_max1) and
          TOFReconMatchesMC(tp2_position, tof2_hit, p_time,
                            dX_max2, dY_max2, dT_max2)) {
        _detector_matches[8]++;
      } else {
        _detector_false_matches[8]++;
      }
    }
  }
}


std::vector<std::pair<MAUS::DataStructure::Global::Track*, MAUS::MCEvent*> >
    ReduceCppGlobalReconEfficiency::matchTrackerReconWithMC(
    std::string mapper_name, MAUS::DataStructure::Global::PID pid) {
  std::vector<std::pair<MAUS::DataStructure::Global::Track*, MAUS::MCEvent*> >
      track_mc_pairs;
  ReconEventPArray* recon_events = _spill->GetReconEvents();
  MCEventPArray* mc_events = _spill->GetMCEvents();
  if (recon_events and mc_events) {
    ReconEventPArray::iterator recon_event_iter;
    // Loop over recon events and pull out global event
    for (recon_event_iter = recon_events->begin();
         recon_event_iter != recon_events->end();
         ++recon_event_iter) {
      GlobalEvent* global_event = (*recon_event_iter)->GetGlobalEvent();
      if (!global_event) continue;
      // Pick out the matched tracks and loop over them
      std::vector<DataStructure::Global::Track*>* tracks =
          GlobalTools::GetTracksByMapperName(global_event, mapper_name, pid);
      //~ std::cerr << "Number of tracks: " << tracks->size() << "\n";
      std::vector<MAUS::DataStructure::Global::Track*>::iterator
          track_iter;
      for (track_iter = tracks->begin();
           track_iter != tracks->end();
           ++track_iter) {
        //~ std::cerr << "                      EKTF2Track1Track0TF1CkTF0VU\n";
        //~ std::cerr << "Detector Points: " << std::bitset<32>((*track_iter)->get_detectorpoints()) << "\n";
        std::vector<const MAUS::DataStructure::Global::TrackPoint*>
                track_points = (*track_iter)->GetTrackPoints();
        MCEventPArray::iterator mc_event_iter;
        for (mc_event_iter = mc_events->begin();
             mc_event_iter != mc_events->end(); ++mc_event_iter) {
          size_t matched_tps = 0;
          size_t total_tps = 0;
          std::vector<const MAUS::DataStructure::Global::TrackPoint*>::iterator
              track_point_iter;
          for (track_point_iter = track_points.begin();
               track_point_iter != track_points.end();
               ++track_point_iter) {
            TLorentzVector position = (*track_point_iter)->get_position();
            TLorentzVector momentum = (*track_point_iter)->get_momentum();
            std::vector<int> tracker_plane = GlobalTools::GetTrackerPlane(*track_point_iter);
            SciFiHit* tracker_mc_hit = MCTruthTools::GetTrackerPlaneHit(
                *mc_event_iter, tracker_plane[0], tracker_plane[1],
                tracker_plane[2]);
            if (tracker_mc_hit) {
              double dX = std::abs(tracker_mc_hit->GetPosition().X() - position.X());
              double dY = std::abs(tracker_mc_hit->GetPosition().Y() - position.Y());
              double dZ = std::abs(tracker_mc_hit->GetPosition().Z() - position.Z());
              double dPx = std::abs(tracker_mc_hit->GetMomentum().X() - momentum.X());
              double dPy = std::abs(tracker_mc_hit->GetMomentum().Y() - momentum.Y());
              double dPz = std::abs(tracker_mc_hit->GetMomentum().Z() - momentum.Z());
              if ((dX < 1.0) and (dY < 1.0) and (dZ < 0.3) and (dPx < 3.0) and (dPy < 3.0)) {
                matched_tps++;
              } else {
                if ((dX > 1.0) or (dY > 1.0)) {
                //~ std::cerr << dX << " " << dY << " " << tracker_plane[0] << tracker_plane[1] << tracker_plane[2] << "\n";
                //~ std::cerr << tracker_plane[0] << tracker_plane[1] << tracker_plane[2] << "\n";
                //~ std::cerr << dX << " " << dY << " " << dZ << " " << dPx << " " << dPy << " " << dPz << " " << tracker_plane[0] << tracker_plane[1] << tracker_plane[2] << "\n";
                }
              }
              total_tps++;
            }
            //~ //std::cerr << "Pos: " << tracker_mc_hit->GetPosition().X() << " " << tracker_mc_hit->GetPosition().Y() << " " << tracker_mc_hit->GetPosition().Z() << " " << tracker_mc_hit->GetMomentum().X() << " " << tracker_mc_hit->GetMomentum().Y() << " " << tracker_mc_hit->GetMomentum().Z() <<  "\n";
            //~ std::cerr << tracker_mc_hit->GetPosition().Z() << " " << tracker_mc_hit->GetChannelId()->GetTrackerNumber() << " " << tracker_mc_hit->GetChannelId()->GetStationNumber() << " " << tracker_mc_hit->GetChannelId()->GetPlaneNumber() << "\n";
          }
          //~ std::cerr << matched_tps << "/" << total_tps << "\n";
          if (matched_tps > 10) {
            //~ std::cerr << "###" << matched_tps << "/" << total_tps << "\n";
            track_mc_pairs.push_back(std::make_pair(*track_iter,*mc_event_iter));
          }
          //~ std::cerr << "Pos: " << position.Z() <<  " " << tracker_plane[0] << " " << tracker_plane[1] << " " << tracker_plane[2] << "\n#\n";
          //~ std::cerr << "Pos: " << position.X() << " " << position.Y() << " " << position.Z() << " " << momentum.X() << " " << momentum.Y() << " " << momentum.Z() <<  "\n";
        }
      }
    }
  }
  return track_mc_pairs;
}

PyMODINIT_FUNC init_ReduceCppGlobalReconEfficiency(void) {
  PyWrapReduceBase<ReduceCppGlobalReconEfficiency>::PyWrapReduceBaseModInit
                              ("ReduceCppGlobalReconEfficiency",
                               "","","","");
}


} // ~namespace MAUS
