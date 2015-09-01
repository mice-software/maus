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
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
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
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "DataStructure/Global/PIDLogLPair.hh"

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
  void set_mapper_name(std::string mapper_name);

  /// Get the name for the mapper which produced the track, #_mapper_name.
  std::string get_mapper_name() const;

  /// Set the PID hypothesis, #_pid
  void set_pid(PID pid);

  /// Get the PID hypothesis, #_pid
  PID get_pid() const;

  /// Set the particle charge hypothesis, #_charge.
  void set_charge(int charge);

  /// Get the particle charge hypothesis, #_charge.
  int get_charge() const;

  /// Set the range of the particles primary track in the emr, #_emr_range_primary
  void set_emr_range_primary(double range);

  /// Get the range of the particles primary track in the emr, #_emr_range_primary
  double get_emr_range_primary() const;

  /// Set the range of the particles secondary track in the emr, #_emr_range_secondary
  void set_emr_range_secondary(double range);

  /// Get the range of the particles secondary track in the emr, #_emr_range_secondary
  double get_emr_range_secondary() const;

  /// Set the plane density of the track in the emr, #_emr_plane_density
  void set_emr_plane_density(double density);

  /// Get the plane density of the particles secondary track in the emr, #_emr_plane_density
  double get_emr_plane_density() const;

  /// Add a MAUS::DataStructure::Global::TrackPoint, filling #_geometry_paths
  /// and #_detectorpoints as required.
  void AddTrackPoint(MAUS::DataStructure::Global::TrackPoint* track_point);

  /// Remove a MAUS::DataStructure::Global::TrackPoint, unsetting
  /// #_detectorpoints bist and removing #_geometry_paths entries if
  /// required.
  void RemoveTrackPoint(MAUS::DataStructure::Global::TrackPoint* track_point);

  /// Sort the track points based purely on the Z coordinate. The
  /// order of #_geometry_paths is unchanged, as I don't believe it
  /// will matter.
  void SortTrackPointsByZ();

  /// Get the associated track points, as a vector of const pointers,
  /// ensuring they are unchanged.
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> GetTrackPoints()
      const;

  /// Set the list of associated TrackPoints, #_track_points, from a
  /// TRefArray.  The Track takes ownership of this pointer, deleting
  /// the current TRefArray if necessary.  This is mostly for the
  /// Json/Cpp Processor, users are encouraged to use the Add/Remove
  /// TrackPoint methods.
  void set_track_points(TRefArray* track_points);

  /// Directly access the MAUS::DataStructure::Global::TrackPoint
  /// pointers stored in the track, #_track_points.  This is mostly for
  /// the Json/Cpp Processor, users are encouraged to use the
  /// GetTrackPoints method.
  TRefArray* get_track_points() const;

  /// Set the corresponding bit in #_detectorpoints.
  void SetDetector(MAUS::DataStructure::Global::DetectorPoint detector);

  /// Unsetting the bit in #_detectorpoints.
  void RemoveDetector(MAUS::DataStructure::Global::DetectorPoint detector);

  /// Check whether the input bit is set in #_detectorpoints (via
  /// bitmask and enumerator value).
  bool HasDetector(MAUS::DataStructure::Global::DetectorPoint detector) const;

  /// Set #_detectorpoints = 0, clearing all detector bits.
  void ClearDetectors();

  /// Provides a list of detector points associated with the track
  /// object.
  std::vector<MAUS::DataStructure::Global::DetectorPoint> GetDetectorPoints()
      const;

  /// Directly set the #_detectorpoints value.
  void set_detectorpoints(unsigned int detectorpoints);

  /// Direct access to the #_detectorpoints value.
  unsigned int get_detectorpoints() const;

  /// Add a geometry path to #_geometry_paths, mostly used by
  /// AddTrackPoint(MAUS::DataStructure::Global::TrackPoint*).
  void AddGeometryPath(std::string geometry_path);

  /// Remove a geometry path from #_geometry_paths, mostly used by
  /// RemoveTrackPoint(MAUS::DataStructure::Global::TrackPoint*).
  void RemoveGeometryPath(std::string geometry_path);

  /// Search for a geometry path in #_geometry_paths.
  bool HasGeometryPath(std::string geometry_path) const;

  /// Clear the std::vector #_geometry_paths.
  void ClearGeometryPaths();

  /// Directly set the #_geometry_paths vector.
  void set_geometry_paths(std::vector<std::string> geometry_paths);

  /// Direct access to the #_geometry_paths vector.
  std::vector<std::string> get_geometry_paths() const;

  /// Set the #_pid_logL_values vector.
  void set_pid_logL_values(std::vector<MAUS::DataStructure::Global::PIDLogLPair> pid_logL_values);

  /// Direct access to the #_pid_logL_values vector.
  std::vector<MAUS::DataStructure::Global::PIDLogLPair> get_pid_logL_values() const;

  /// Add a pid/log-likelihood pair to the _pid_logL_values vector.
  void AddPIDLogLValues(MAUS::DataStructure::Global::PIDLogLPair pid_logL);

  /// Add a constituent track (for book-keeping purposes), stored in
  /// the #_constituent_tracks vector.
  void AddTrack(Track* track);

  /// Remove a constituent track (for book-keeping purposes), from the
  /// #_constituent_tracks vector.
  void RemoveTrack(Track* track);

  /// Check for a historical track in the #_constituent_tracks TRefArray.
  bool HasTrack(Track* track) const;

  /// Empty the book-keeping track TRefArray, #_constituent_tracks.
  void ClearTracks();

  /// User method for accessing the constituent tracks.  These are
  /// returned as const, so that they can't be changed.
  std::vector<const MAUS::DataStructure::Global::Track*> GetConstituentTracks()
      const;

  /// Directly set the #_constituent_tracks TRefArray.  This passes
  /// ownership of the pointer to the Track, and is mostly for use by
  /// the Json/Cpp Processor.  Users should use Add/Remove Track
  /// methods.
  void set_constituent_tracks(TRefArray* constituent_tracks);

  /// Direct access to the #_constituent_tracks vector, for the
  /// Json/Cpp Processor.  Users should probably access the
  /// constituent tracks through: GetConstituentTracks(), which
  /// returns a vector of Track pointers.
  TRefArray* get_constituent_tracks() const;

  /// Set the goodness of fit varialbe, #_goodness_of_fit.
  void set_goodness_of_fit(double goodness_of_fit);

  /// Get the goodness of fit variable, #_goodness_of_fit.
  double get_goodness_of_fit() const;

 private:

  /// Push a MAUS::DataStructure::Global::TrackPoint* into the #_track_points
  /// TRefArray.  This method is private, as
  /// MAUS::DataStructure::Global::TrackPoint's should be added through the
  /// AddTrackPoint(MAUS::DataStructure::Global::TrackPoint*) method.
  void PushBackTrackPoint(MAUS::DataStructure::Global::TrackPoint* track_point);

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
  TRefArray* _track_points;

  /// A bitmask for the set detector points, and a vector of strings
  /// for the vairable geometry paths.
  unsigned int _detectorpoints;

  /// A vector of paths for the virtual
  /// MAUS::DataStructure::Global::TrackPoint objects, which don't match a
  /// DetectorPoint.
  std::vector<std::string> _geometry_paths;

  /// A vector of pairs to hold a possible pid hypothesis (given by it's PDG
  /// code, as an int) and the log-likelihood of the track being that pid.
  /// Can be used during analysis to view the likelihoods assigned to all
  /// possible pids, not just the final one assigned to the track.
  std::vector<MAUS::DataStructure::Global::PIDLogLPair> _pid_logL_values;

  /// These tracks aren't owned by this track, they are just for
  /// book-keeping.  Therefore, we use a TRefArray.
  TRefArray* _constituent_tracks;

  /// The range of a primary track from the EMR
  double _emr_range_primary;

  /// The range of a secondary track from the EMR
  double _emr_range_secondary;

  /// The plane density for an event in the EMR
  double _emr_plane_density;

  /// A currently undefined 'Goodness of Fit' variable. This will be
  /// used to help analysers in interpreting the results, and will
  /// depend on the Track's conformity to the hypothesised particle
  /// and trajectory.
  double _goodness_of_fit;

  MAUS_VERSIONED_CLASS_DEF(Track);
}; // ~class Track

typedef std::vector<MAUS::DataStructure::Global::Track> TrackArray;
typedef std::vector<MAUS::DataStructure::Global::Track *> TrackPArray;
typedef
std::vector<const MAUS::DataStructure::Global::Track *> ConstTrackPArray;
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
