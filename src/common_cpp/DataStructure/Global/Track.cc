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

#include "TRef.h"
#include "TRefArray.h"

#include "Utils/Exception.hh"
#include "DataStructure/Global/Track.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

/// Global function to sort MAUS::DataStructure::Global::TrackPoint's based on
/// their Z coordinate.
bool SortByZ(const MAUS::DataStructure::Global::TrackPoint* tp1,
             const MAUS::DataStructure::Global::TrackPoint* tp2) {
  return (tp1->get_position().Z() < tp2->get_position().Z());
}

// Default constructor
Track::Track()
    : _mapper_name(""),
      _pid(MAUS::DataStructure::Global::kNoPID),
      _charge(0),
      _detectorpoints(0),
      _pid_logL_values(0),
      _emr_range_primary(0.),
      _emr_range_secondary(0.),
      _emr_plane_density(0.),
      _goodness_of_fit(0.) {
  _track_points = new TRefArray();
  _constituent_tracks = new TRefArray();
}

// Copy contructor
Track::Track(const Track &track)
    : _mapper_name(track.get_mapper_name()),
      _pid(track.get_pid()),
      _charge(track.get_charge()),
      _track_points(track.get_track_points()),
      _detectorpoints(track.get_detectorpoints()),
      _geometry_paths(track.get_geometry_paths()),
      _pid_logL_values(track.get_pid_logL_values()),
      _constituent_tracks(track.get_constituent_tracks()),
      _emr_range_primary(track.get_emr_range_primary()),
      _emr_range_secondary(track.get_emr_range_secondary()),
      _emr_plane_density(track.get_emr_plane_density()),
      _goodness_of_fit(track.get_goodness_of_fit()) {
  _track_points = new TRefArray(*track.get_track_points());
  _constituent_tracks = new TRefArray(*track.get_constituent_tracks());
}

// Destructor
Track::~Track() {
  delete _track_points;
  delete _constituent_tracks;
}

// Assignment operator
Track& Track::operator=(const Track &track) {
  if (this == &track) {
    return *this;
  }
  _mapper_name         = track.get_mapper_name();
  _pid                 = track.get_pid();
  _charge              = track.get_charge();
  _track_points = new TRefArray(*track.get_track_points());
  _constituent_tracks  = new TRefArray(*track.get_constituent_tracks());
  _detectorpoints      = track.get_detectorpoints();
  _geometry_paths      = track.get_geometry_paths();
  _pid_logL_values     = track.get_pid_logL_values();
  _emr_range_primary   = track.get_emr_range_primary();
  _emr_range_secondary = track.get_emr_range_secondary();
  _emr_plane_density   = track.get_emr_plane_density();
  _goodness_of_fit     = track.get_goodness_of_fit();

  return *this;
}

// Create a new Track, identical to the original, but separate.  All
// TrackPoints are also cloned.
Track* Track::Clone() const {
  MAUS::DataStructure::Global::Track* trackNew =
      new MAUS::DataStructure::Global::Track();

  trackNew->set_mapper_name(get_mapper_name());
  trackNew->set_pid(get_pid());
  trackNew->set_charge(get_charge());
  trackNew->set_pid_logL_values(get_pid_logL_values());
  trackNew->set_emr_range_primary(get_emr_range_primary());
  trackNew->set_emr_range_secondary(get_emr_range_secondary());
  trackNew->set_emr_plane_density(get_emr_plane_density());

  // Track points may be edited, so we clone the original points
  MAUS::DataStructure::Global::TrackPoint* tp = NULL;
  for (int i = 0; i < _track_points->GetLast()+1; ++i) {
    tp = (MAUS::DataStructure::Global::TrackPoint*) _track_points->At(i);
    if (!tp) continue;
    trackNew->PushBackTrackPoint(tp->Clone());
  }

  trackNew->set_detectorpoints(this->get_detectorpoints());
  trackNew->set_geometry_paths(this->get_geometry_paths());

  // This is just book-keeping, so we copy the TRefArray whole.
  MAUS::DataStructure::Global::Track* t = NULL;
  for (int i = 0; i < _constituent_tracks->GetLast()+1; ++i) {
    t = (MAUS::DataStructure::Global::Track*) _constituent_tracks->At(i);
    if (!t) continue;
    trackNew->AddTrack(t);
  }

  trackNew->set_goodness_of_fit(this->get_goodness_of_fit());

  return trackNew;
}

void Track::set_mapper_name(std::string mapper_name) {
  _mapper_name = mapper_name;
}

std::string Track::get_mapper_name() const {
  return _mapper_name;
}

void Track::set_pid(PID pid) {
  _pid = pid;
}

PID Track::get_pid() const {
  return _pid;
}

void Track::set_charge(int charge) {
  _charge = charge;
}

int Track::get_charge() const {
  return _charge;
}

void Track::set_emr_range_primary(double range) {
  _emr_range_primary = range;
}

double Track::get_emr_range_primary() const {
  return _emr_range_primary;
}

void Track::set_emr_range_secondary(double range) {
  _emr_range_secondary = range;
}

double Track::get_emr_range_secondary() const {
  return _emr_range_secondary;
}

void Track::set_emr_plane_density(double density) {
  _emr_plane_density = density;
}

double Track::get_emr_plane_density() const {
  return _emr_plane_density;
}

// Trackpoint methods

void Track::AddTrackPoint(MAUS::DataStructure::Global::TrackPoint* track_point) {
  if (!track_point) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to add a NULL TrackPoint",
                 "DataStructure::Global::Track::AddTrackPoint()"));
  }
  if (track_point->get_detector() == MAUS::DataStructure::Global::kVirtual) {
    AddGeometryPath(track_point->get_geometry_path());
  }
  SetDetector(track_point->get_detector());
  PushBackTrackPoint(track_point);
}

void Track::PushBackTrackPoint(
    MAUS::DataStructure::Global::TrackPoint* track_point) {
  _track_points->Add(track_point);
}

// Correctly remove the track_point, unsetting the detector bit and/or
// removing the geometry path if appropriate.
void Track::RemoveTrackPoint(
    MAUS::DataStructure::Global::TrackPoint* track_point) {
  if (!track_point) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "No matching TrackPoint: pointer is NULL",
                 "DataStructure::Global::Track::RemoveTrackPoint()"));
  }

  // Remove track_point from TRefArray
  TObject* result = _track_points->FindObject(track_point);
  if (!result) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "No matching TrackPoint ",
                 "DataStructure::Global::Track::RemoveTrackPoint()"));
  } else {
    _track_points->Remove(result);
    _track_points->Compress();
  }

  // Check if track_point detector point should still be set.
  MAUS::DataStructure::Global::DetectorPoint targetDP =
      track_point->get_detector();
  MAUS::DataStructure::Global::TrackPoint *eachTP;
  bool stillNeeded = false;
  for (int i = 0; i < _track_points->GetLast()+1; ++i) {
    eachTP = (MAUS::DataStructure::Global::TrackPoint*) _track_points->At(i);
    if (!eachTP) continue;
    if (eachTP->get_detector() == targetDP) {
      stillNeeded = true;
      break;
    }
  }
  if (!stillNeeded) {
    RemoveDetector(targetDP);
  }

  // If we have an associated geometry path, remove that as well
  if (targetDP == MAUS::DataStructure::Global::kVirtual) {
    RemoveGeometryPath(track_point->get_geometry_path());
  }
}

void Track::SortTrackPointsByZ() {
  std::vector<MAUS::DataStructure::Global::TrackPoint*> temp_track_points;
  MAUS::DataStructure::Global::TrackPoint* tp = NULL;
  for (int i = 0; i < _track_points->GetLast()+1; ++i) {
    tp = (MAUS::DataStructure::Global::TrackPoint*) _track_points->At(i);
    if (!tp) continue;
    temp_track_points.push_back(tp);
  }

  std::sort(temp_track_points.begin(), temp_track_points.end(), SortByZ);

  _track_points->Clear();
  _track_points->Expand(temp_track_points.size()); // Can be used to shrink too
  for (size_t i = 0; i < temp_track_points.size(); ++i) {
    _track_points->AddAt(temp_track_points.at(i), i);
  }
}

std::vector<const MAUS::DataStructure::Global::TrackPoint*>
Track::GetTrackPoints() const {
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> temp_track_points;
  if (!_track_points) return temp_track_points;
  const MAUS::DataStructure::Global::TrackPoint* tp = NULL;
  for (int i = 0; i < _track_points->GetSize(); ++i) {
    tp = (const MAUS::DataStructure::Global::TrackPoint*) _track_points->At(i);
    if (!tp) continue;
    temp_track_points.push_back(tp);
  }
  return temp_track_points;
}

std::vector<const MAUS::DataStructure::Global::TrackPoint*>
Track::GetTrackPoints(DetectorPoint detector) const {
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> temp_track_points;
  if (!_track_points) return temp_track_points;
  const MAUS::DataStructure::Global::TrackPoint* tp = NULL;
  for (int i = 0; i < _track_points->GetSize(); ++i) {
    tp = (const MAUS::DataStructure::Global::TrackPoint*) _track_points->At(i);
    if (!tp) continue;
    // Need to make sure that requests for the main detector (e.g. Tracker0)
    // return points that are tagged with subdetectors as well
    DetectorPoint tp_main_detector = tp->get_detector();
    if ((tp_main_detector == kTOF0_1) or
        (tp_main_detector == kTOF0_2)) {
      tp_main_detector = kTOF0;
    } else if ((tp_main_detector == kTOF1_1) or
               (tp_main_detector == kTOF1_2)) {
      tp_main_detector = kTOF1;
    } else if ((tp_main_detector == kTOF2_1) or
               (tp_main_detector == kTOF2_2)) {
      tp_main_detector = kTOF2;
    } else if ((tp_main_detector == kTracker0_1) or
               (tp_main_detector == kTracker0_2) or
               (tp_main_detector == kTracker0_3) or
               (tp_main_detector == kTracker0_4) or
               (tp_main_detector == kTracker0_5)) {
      tp_main_detector = kTracker0;
    } else if ((tp_main_detector == kTracker1_1) or
               (tp_main_detector == kTracker1_2) or
               (tp_main_detector == kTracker1_3) or
               (tp_main_detector == kTracker1_4) or
               (tp_main_detector == kTracker1_5)) {
      tp_main_detector = kTracker1;
    }
    if ((tp->get_detector() == detector) or tp_main_detector == detector) {
      temp_track_points.push_back(tp);
    }
  }
  return temp_track_points;
}

void Track::set_track_points(TRefArray* track_points) {
  if (_track_points != NULL) {
    delete _track_points;
  }
  _track_points = track_points;
}

TRefArray* Track::get_track_points() const {
  return _track_points;
}

// Detector Point Methods
void Track::SetDetector(MAUS::DataStructure::Global::DetectorPoint detector) {
  // Set the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  _detectorpoints |= (1u << detector);
}

void Track::RemoveDetector(
    MAUS::DataStructure::Global::DetectorPoint detector) {
  // Clear the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  _detectorpoints &= ~(1u << detector);
}

bool Track::HasDetector(MAUS::DataStructure::Global::DetectorPoint detector)
    const {
  // Return the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  return (_detectorpoints & (1u << detector));
}

void Track::ClearDetectors() {
  // Set all bits to 0, i.e. set int 0.
  _detectorpoints = 0;
}

std::vector<MAUS::DataStructure::Global::DetectorPoint>
Track::GetDetectorPoints() const {
  std::vector<MAUS::DataStructure::Global::DetectorPoint> result;
  MAUS::DataStructure::Global::DetectorPoint test;
  for (int i = 0; i < MAUS::DataStructure::Global::kDetectorPointSize; ++i) {
    test = static_cast<MAUS::DataStructure::Global::DetectorPoint>(i);
    if (HasDetector(test)) result.push_back(test);
  }

  return result;
}

void Track::set_detectorpoints(unsigned int detectorpoints) {
  _detectorpoints = detectorpoints;
}

unsigned int Track::get_detectorpoints() const {
  return _detectorpoints;
}

// Geometry Path methods
void Track::AddGeometryPath(std::string geometry_path) {
  _geometry_paths.push_back(geometry_path);
}

void Track::RemoveGeometryPath(std::string geometry_path) {
  std::vector<std::string>::iterator result =
      find(_geometry_paths.begin(), _geometry_paths.end(), geometry_path);

  if (result == _geometry_paths.end()) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to remove a geometry path not stored in Track",
                 "DataStructure::Global::Track::RemoveGeometryPath()"));
  } else {
    _geometry_paths.erase(result);
  }
}

bool Track::HasGeometryPath(std::string geometry_path) const {
  std::vector<std::string>::const_iterator result =
      find(_geometry_paths.begin(), _geometry_paths.end(), geometry_path);

  return (result != _geometry_paths.end());
}

void Track::ClearGeometryPaths() {
  _geometry_paths.clear();
}

void Track::set_geometry_paths(std::vector<std::string> geometry_paths) {
  _geometry_paths = geometry_paths;
}

std::vector<std::string> Track::get_geometry_paths() const {
  return _geometry_paths;
}

// Object to hold pid hypotheses and the log-likelihood that they are the
// correct hypothesis
void Track::set_pid_logL_values(std::vector<MAUS::DataStructure::Global::PIDLogLPair>
				pid_logL_values) {
  _pid_logL_values = pid_logL_values;
}

std::vector<MAUS::DataStructure::Global::PIDLogLPair> Track::get_pid_logL_values() const {
  return _pid_logL_values;
}

// Method to fill pid_logL_values
void Track::AddPIDLogLValues(MAUS::DataStructure::Global::PIDLogLPair pid_logL) {
  _pid_logL_values.push_back(pid_logL);
}

// Constituent Tracks methods
void Track::AddTrack(MAUS::DataStructure::Global::Track* track) {
  _constituent_tracks->Add(track);
}

void Track::RemoveTrack(MAUS::DataStructure::Global::Track* track) {
  TObject *result = _constituent_tracks->FindObject(track);
  if (!result) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to remove a constituent track not stored in Track",
                 "DataStructure::Global::Track::RemoveTrack()"));
  } else {
    _constituent_tracks->Remove(track);
  }
}

bool Track::HasTrack(MAUS::DataStructure::Global::Track* track) const {
  TObject *result = _constituent_tracks->FindObject(track);

  return (result != NULL);
}

std::vector<const MAUS::DataStructure::Global::Track*>
    Track::GetConstituentTracks() const {
  std::vector<const MAUS::DataStructure::Global::Track*> temp_tracks;
  const MAUS::DataStructure::Global::Track* t = NULL;
  for (int i = 0; i < _constituent_tracks->GetLast()+1; ++i) {
    t = (const MAUS::DataStructure::Global::Track*) _constituent_tracks->At(i);
    if (!t) continue;
    temp_tracks.push_back(t);
  }
  return temp_tracks;
}

std::vector<const MAUS::DataStructure::Global::Track*>
    Track::GetConstituentTracks(MAUS::DataStructure::Global::DetectorPoint detector) const {
  std::vector<const MAUS::DataStructure::Global::Track*> temp_tracks;
  const MAUS::DataStructure::Global::Track* t = NULL;
  for (int i = 0; i < _constituent_tracks->GetLast()+1; ++i) {
    t = (const MAUS::DataStructure::Global::Track*) _constituent_tracks->At(i);
    if (!t) continue;
    if (t->HasDetector(detector)) {
      temp_tracks.push_back(t);
    }
  }
  return temp_tracks;
}

void Track::ClearTracks() {
  _constituent_tracks->Clear();
}

void Track::set_constituent_tracks(TRefArray* constituent_tracks) {
  _constituent_tracks = constituent_tracks;
}

TRefArray* Track::get_constituent_tracks() const {
  return _constituent_tracks;
}

void Track::set_goodness_of_fit(double goodness_of_fit) {
  _goodness_of_fit = goodness_of_fit;
}

double Track::get_goodness_of_fit() const {
  return _goodness_of_fit;
}

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
