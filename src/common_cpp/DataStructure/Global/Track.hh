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

/** @class MAUS::DataStructure::Global::Track
 *  @ingroup globalrecon
 *  @brief Track object for the global reconstruction.
 *
 *  A reconstructed track, produced by the global reconstruction.
 *  Primarily consists of a series of
 *  MAUS::DataStructure::Global::TrackPoint's, as well as the PID/charge
 *  assumption underwhich the track was reconstructed.  Constituent
 *  tracks can be stored, for book-keeping purposes.
 *
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACK_HH_

// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "TObject.h"
#include "TRefArray.h"
#include "Rtypes.h"

// MAUS headers
#include "DataStructure/Global/ReconEnums.hh"
#include "DataStructure/Global/TrackPoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Only elements in the DataStructure should inherit from TObject.
class Track : public TObject {
 public:

  /// Default constructor - initialises to 0/NULL
  Track();

  /// Copy constructor - any pointers are deep copied
  Track(const Track &track);

  /// Destructor
  virtual ~Track();

  /// Assignment operator - any pointers are deep copied
  Track& operator=(const Track &track);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  Track* Clone() const;

  // Getters and Setters for the member variables
  
  /// Set the name for the mapper which produced the track, #_mapper_name.
  void set_mapper_name(std::string mapper_name) {
    _mapper_name = mapper_name;
  }
  /// Get the name for the mapper which produced the track, #_mapper_name.
  std::string get_mapper_name() const {
    return _mapper_name;
  }

  /// Set the PID hypothesis, #_pid
  void set_pid(PID pid) {
    _pid = pid;
  }
  /// Get the PID hypothesis, #_pid
  PID get_pid() const {
    return _pid;
  }

  /// Set the particle charge hypothesis, #_charge.
  void set_charge(int charge) {
    _charge = charge;
  }
  /// Get the particle charge hypothesis, #_charge.
  int get_charge() const {
    return _charge;
  }

  /// Add a MAUS::DataStructure::Global::TrackPoint, filling #_geometry_paths
  /// and #_detectorpoints as required.
  void AddTrackPoint(MAUS::DataStructure::Global::TrackPoint* trackpoint);

  /// Remove a MAUS::DataStructure::Global::TrackPoint, unsetting
  /// #_detectorpoints bist and removing #_geometry_paths entries if
  /// required.
  void RemoveTrackPoint(MAUS::DataStructure::Global::TrackPoint* trackpoint);

  /// Sort the track points based purely on the Z coordinate. The
  /// order of #_geometry_paths is unchanged, as I don't believe it
  /// will matter.
  void SortTrackPointsByZ();

  /// Set the list of associated TrackPoints, #_trackpoints, from a TRefArray
  void set_trackpoints(TRefArray* trackpoints) {
    _trackpoints = trackpoints;
  }

  /// Directly access the MAUS::DataStructure::Global::TrackPoint pointers
  /// stored in the track, #_trackpoints.
  TRefArray* get_trackpoints() const {
    return _trackpoints;
  }

  /// Set the corresponding bit in #_detectorpoints.
  void SetDetector(MAUS::DataStructure::Global::DetectorPoint detector);

  /// Unsetting the bit in #_detectorpoints.
  void RemoveDetector(MAUS::DataStructure::Global::DetectorPoint detector);

  /// Check whether the input bit is set in #_detectorpoints (via
  /// bitmask and enumerator value).
  bool HasDetector(MAUS::DataStructure::Global::DetectorPoint detector);

  /// Set #_detectorpoints = 0, clearing all detector bits.
  void ClearDetectors();

  /// Provides a list of detector points associated with the track
  /// object.
  std::vector<MAUS::DataStructure::Global::DetectorPoint> GetDetectorPoints();

  /// Directly set the #_detectorpoints value.
  void set_detectorpoints(unsigned int detectorpoints) {
    _detectorpoints = detectorpoints;
  }

  /// Direct access to the #_detectorpoints value.
  unsigned int get_detectorpoints() const {
    return _detectorpoints;
  }

  /// Add a geometry path to #_geometry_paths, mostly used by
  /// AddTrackPoint(MAUS::DataStructure::Global::TrackPoint*).
  void AddGeometryPath(std::string geometry_path);

  /// Remove a geometry path from #_geometry_paths, mostly used by
  /// RemoveTrackPoint(MAUS::DataStructure::Global::TrackPoint*).
  void RemoveGeometryPath(std::string geometry_path);

  /// Search for a geometry path in #_geometry_paths.
  bool HasGeometryPath(std::string geometry_path);

  /// Clear the std::vector #_geometry_paths.
  void ClearGeometryPaths();

  /// Directly set the #_geometry_paths vector.
  void set_geometry_paths(std::vector<std::string> geometry_paths) {
    _geometry_paths = geometry_paths;
  }
  /// Direct access to the #_geometry_paths vector.
  std::vector<std::string> get_geometry_paths() const {
    return _geometry_paths;
  }

  /// Add a constituent track (for book-keeping purposes), stored in
  /// the #_constituent_tracks vector.
  void AddTrack(Track* track);

  /// Remove a constituent track (for book-keeping purposes), from the
  /// #_constituent_tracks vector.
  void RemoveTrack(Track* track);

  /// Check for a historical track in the #_constituent_tracks TRefArray.
  bool HasTrack(Track* track);

  /// Empty the book-keeping track TRefArray, #_constituent_tracks.
  void ClearTracks() {
    _constituent_tracks->Clear();
  }

  /// Directly set the #_constituent_tracks TRefArray.
  void set_constituent_tracks(TRefArray* constituent_tracks){
    _constituent_tracks = constituent_tracks;
  }

  /// Direct access to the #_constituent_tracks vector.
  TRefArray* get_constituent_tracks() const {
    return _constituent_tracks;
  }

  /// Set the goodness of fit varialbe, #_goodness_of_fit.
  void set_goodness_of_fit(double goodness_of_fit) {
    _goodness_of_fit = goodness_of_fit;
  }
  /// Get the goodness of fit varialbe, #_goodness_of_fit.
  double get_goodness_of_fit() const {
    return _goodness_of_fit;
  }

 private:

  /// Push a MAUS::DataStructure::Global::TrackPoint* into the #_trackpoints
  /// TRefArray.  This method is private, as
  /// MAUS::DataStructure::Global::TrackPoint's should be added through the
  /// AddTrackPoint(MAUS::DataStructure::Global::TrackPoint*) method.
  void PushBackTrackPoint(MAUS::DataStructure::Global::TrackPoint* trackpoint);

  /// The name of the mapper which produced this Track. This can be
  /// used to separate different stages of the global reconstruction
  /// if required.
  std::string _mapper_name;

  /// The particle hypothesis for the Track.
  PID _pid;

  /// Charge (+1/-1) of particle hypothesis.  0 for undecided.
  int _charge;

  /// The associated MAUS::DataStructure::Global::TrackPoint's, which define
  /// the path of the reconstructed track.
  TRefArray* _trackpoints;

  /// A bitmask for the set detector points, and a vector of strings
  /// for the vairable geometry paths.
  unsigned int _detectorpoints;

  /// A vector of paths for the virtual
  /// MAUS::DataStructure::Global::TrackPoint objects, which don't match a
  /// DetectorPoint.
  std::vector<std::string> _geometry_paths;

  /// These tracks aren't owned by this track, they are just for
  /// book-keeping.  Therefore, we use a TRefArray.
  TRefArray* _constituent_tracks;

  /// A currently undefined 'Goodness of Fit' variable. This will be
  /// used to help analysers in interpreting the results, and will
  /// depend on the Track's conformity to the hypothesised particle
  /// and trajectory.
  double _goodness_of_fit;

  ClassDef(Track, 1)
}; // ~class Track
  
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
