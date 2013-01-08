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

#include "DataStructure/GlobalTrack.hh"

#include <algorithm>

#include "Interface/Squeak.hh"
#include "DataStructure/GlobalTrackPoint.hh"

namespace MAUS {
namespace recon {
namespace global {

/// Global function to sort MAUS::recon::global::TrackPoint's based on
/// their Z coordinate.
bool SortByZ(const MAUS::GlobalTrackPoint* tp1,
             const MAUS::GlobalTrackPoint* tp2) {
  return (tp1->get_position().Z() < tp2->get_position().Z());
}

// Default constructor
Track::Track()
    : _mapper_name(""),
      _pid(MAUS::recon::global::kNoPID),
      _charge(0),
      _detectorpoints(0),
      _goodness_of_fit(0.) {
  _trackpoints =
      new std::vector<MAUS::GlobalTrackPoint*>();
  _constituent_tracks =
      new std::vector<MAUS::GlobalTrack*>();
}

// Copy contructor
Track::Track(const Track &track)
    : _mapper_name(track.get_mapper_name()),
      _pid(track.get_pid()),
      _charge(track.get_charge()),
      _trackpoints(track.get_trackpoints()),
      _detectorpoints(track.get_detectorpoints()),
      _geometry_paths(track.get_geometry_paths()),
      _constituent_tracks(track.get_constituent_tracks()),
      _goodness_of_fit(track.get_goodness_of_fit()) {}

// Destructor
Track::~Track() {}

// Assignment operator
Track& Track::operator=(const Track &track) {
  if (this == &track) {
    return *this;
  }
  _mapper_name        = track.get_mapper_name();
  _pid                = track.get_pid();
  _charge             = track.get_charge();
  _trackpoints        = track.get_trackpoints();
  _detectorpoints     = track.get_detectorpoints();
  _geometry_paths     = track.get_geometry_paths();
  _constituent_tracks = track.get_constituent_tracks();
  _goodness_of_fit    = track.get_goodness_of_fit();
  
  return *this;
}

// Create a new Track, identical to the original, but separate.  All
// TrackPoints are also cloned.
Track* Track::Clone() const {
  MAUS::GlobalTrack* trackNew =
      new MAUS::GlobalTrack();

  trackNew->set_mapper_name(get_mapper_name());
  trackNew->set_pid(get_pid());
  trackNew->set_charge(get_charge());

  // Track points may be edited, so we clone the original points
  std::vector<MAUS::GlobalTrackPoint*>::const_iterator tp;
  for(tp = _trackpoints->begin();
      tp != _trackpoints->end(); ++tp) {
    if(*tp){
      trackNew->PushBackTrackPoint((*tp)->Clone());
    } else {
      Squeak::mout(Squeak::error) << "Invalid input TP" << std::endl;
      trackNew->PushBackTrackPoint(NULL);
    }
  }
  
  trackNew->set_detectorpoints(this->get_detectorpoints());
  trackNew->set_geometry_paths(this->get_geometry_paths());

  // This is just book-keeping, so we copy the vector whole.
  trackNew->set_constituent_tracks(this->get_constituent_tracks());
  
  trackNew->set_goodness_of_fit(this->get_goodness_of_fit());
  
  return trackNew;
}

// Trackpoint methods

void Track::AddTrackPoint(MAUS::GlobalTrackPoint* trackpoint) {
  if(!trackpoint){
    Squeak::mout(Squeak::error) << "Adding NULL Track Point" << std::endl;
    return;
  }
  if(trackpoint->get_detector() == MAUS::recon::global::kVirtual) {
    AddGeometryPath(trackpoint->get_geometry_path());
  }
  SetDetector(trackpoint->get_detector());
  PushBackTrackPoint(trackpoint);
}
  
void Track::PushBackTrackPoint(MAUS::GlobalTrackPoint* trackpoint) {
  if(trackpoint)
    _trackpoints->push_back(trackpoint);
  else
    Squeak::mout(Squeak::error)
        << "recon::global::track - Attempting to add a NULL trackpoint pointer"
        << std::endl;
}

// Correctly remove the trackpoint, unsetting the detector bit and/or
// removing the geometry path if appropriate.
void Track::RemoveTrackPoint(MAUS::GlobalTrackPoint* trackpoint) {
  if(!trackpoint) {
    Squeak::mout(Squeak::error)
        << "recon::global::track - "
        << "Attempting to remove a NULL trackpoint pointer"
        << std::endl;
    return;
  }

  // Remove trackpoint from vector
  MAUS::GlobalTrackPointPArray::iterator result =
      find(_trackpoints->begin(), _trackpoints->end(), trackpoint);
  EraseTrackPoint(result);

  // Check if trackpoint detector point should still be set.
  MAUS::recon::global::DetectorPoint targetDP = trackpoint->get_detector();
  MAUS::GlobalTrackPointPArray::iterator eachTP;
  for(eachTP = _trackpoints->begin(); eachTP != _trackpoints->end(); ++eachTP) {
    if((*eachTP)->get_detector() == targetDP) break;
  }
  if(eachTP == _trackpoints->end()){
    // Must have passed all remaining points, unset the bit
    RemoveDetector(targetDP);
  }

  // If we have an associated geometry path, remove that as well
  if(targetDP == MAUS::recon::global::kVirtual) {
    RemoveGeometryPath(trackpoint->get_geometry_path());
  }
}

void Track::EraseTrackPoint(
    MAUS::GlobalTrackPointPArray::iterator trackpointIter) {
  if(trackpointIter < _trackpoints->begin() ||
     trackpointIter >= _trackpoints->end()) {
    Squeak::mout(Squeak::debug)
        << "recon::global::track - "
        << "Trying to remove a trackpoint not stored in track"
        << std::endl;
  } else {
    _trackpoints->erase(trackpointIter);
  }
}

void Track::SortTrackPointsByZ() {
  std::sort(_trackpoints->begin(), _trackpoints->end(), SortByZ);
}

// Detector Point Methods
void Track::SetDetector(MAUS::recon::global::DetectorPoint detector) {
  // Set the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  _detectorpoints |= (1u << detector);
}

void Track::RemoveDetector(MAUS::recon::global::DetectorPoint detector) {
  // Clear the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  _detectorpoints &= ~(1u << detector);
}

bool Track::HasDetector(MAUS::recon::global::DetectorPoint detector) {
  // Return the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  return (_detectorpoints & (1u << detector));
}

void Track::ClearDetectors() {
  // Set all bits to 0, i.e. set int 0.
  _detectorpoints = 0;
}

std::vector<MAUS::recon::global::DetectorPoint> Track::GetDetectorPoints() {
  std::vector<MAUS::recon::global::DetectorPoint> result;
  MAUS::recon::global::DetectorPoint test;
  for (int i = 0; i < MAUS::recon::global::kDetectorPointSize; ++i) {
    test = static_cast<MAUS::recon::global::DetectorPoint>(i);
    if (HasDetector(test)) result.push_back(test);
  }

  return result;
}

// Geometry Path methods
void Track::AddGeometryPath(std::string geometry_path) {
  _geometry_paths.push_back(geometry_path);
}

void Track::RemoveGeometryPath(std::string geometry_path) {
  std::vector<std::string>::iterator result =
      find(_geometry_paths.begin(), _geometry_paths.end(), geometry_path);

  if(result == _geometry_paths.end()) {
    Squeak::mout(Squeak::debug)
        << "recon::global::track - "
        << "Trying to remove a geometry_path not stored in track"
        << std::endl;
  } else {
    _geometry_paths.erase(result);
  }
}

bool Track::HasGeometryPath(std::string geometry_path) {
  std::vector<std::string>::iterator result =
      find(_geometry_paths.begin(), _geometry_paths.end(), geometry_path);

  return (result != _geometry_paths.end());
}

void Track::ClearGeometryPaths() {
  _geometry_paths.clear();
}

// Constituent Tracks methods
void Track::AddTrack(MAUS::GlobalTrack* track) {
  _constituent_tracks->push_back(track);
}

void Track::RemoveTrack(MAUS::GlobalTrack* track) {
  MAUS::GlobalTrackPArray::iterator result =
      find(_constituent_tracks->begin(), _constituent_tracks->end(), track);

  if(result == _constituent_tracks->end()) {
    Squeak::mout(Squeak::debug)
        << "recon::global::track - "
        << "Trying to remove a constituent track not stored in track"
        << std::endl;
  } else {
    _constituent_tracks->erase(result);
  }
}

bool Track::HasTrack(MAUS::GlobalTrack* track) {
  MAUS::GlobalTrackPArray::iterator result =
      find(_constituent_tracks->begin(), _constituent_tracks->end(), track);

  return (result != _constituent_tracks->end());
}

void Track::ClearTracks() {
  _constituent_tracks->clear();
}

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
