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

/** @class MAUS::DataStructure::Global::TrackPoint
 *  @ingroup globalrecon
 *  @brief TrackPoint object for the global reconstruction.
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
 *
 *  A reconstructed track point, produced by the global
 *  reconstruction.  It represents a single point in space through
 *  which the reconstruction has determined the
 *  MAUS::DataStructure::Global::Track passed, defined by a position and
 *  momentum measurement, with associated errors.

 *  Most TrackPoints will correspond to a detector measurement (with a
 *  MAUS::DataStructure::Global::SpacePoint object), but some will be virtual
 *  (MAUS::DataStructure::Global::DetectorPoint kVirtual), matching a
 *  projected point.
 *
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACKPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACKPOINT_HH_


// C++ headers
#include <string>
#include <vector>
#include <iostream>

// ROOT headers
#include "Rtypes.h"
#include "TLorentzVector.h"
#include "TRef.h"

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/DataStructure/Global/BasePoint.hh"
#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Only elements in the DataStructure should inherit from TObject.
class TrackPoint : public BasePoint {
 public:

  /// Default constructor - initialises to 0/NULL
  TrackPoint();

  /// Copy constructor - any pointers are deep copied
  TrackPoint(const TrackPoint &track_point);

  /// Trackpoint from spacepoint
  explicit TrackPoint(SpacePoint *space_point);

  /// Destructor
  virtual ~TrackPoint();

  /// Assignment operator - any pointers are deep copied
  TrackPoint& operator=(const TrackPoint &track_point);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  TrackPoint* Clone() const;

  // Getters and Setters for the member variables

  /// Set the index of the particle event that created this track point.
  void set_particle_event(const int particle_event);

  /// Get the name for the mapper which produced the track, #_mapper_name.
  int get_particle_event() const;

  /// Set the name for the mapper which produced the track, #_mapper_name.
  void set_mapper_name(std::string mapper_name);

  /// Get the name for the mapper which produced the track, #_mapper_name.
  std::string get_mapper_name() const;

  /// Set the calibrated 'charge' or energy deposit of the track at
  /// this point, based on detector measurements.  This will be
  /// undefined for MAUS::DataStructure::Global::kVirtual points.
  void set_charge(double charge);

  /// Get the calibrated 'charge' or energy deposit at this point,
  /// #_charge.
  double get_charge() const;

  /// Set the 4-momentum of the reconstructed
  /// MAUS::DataStructure::Global::Track at this point.
  void set_momentum(TLorentzVector momentum);

  /// Get the 4-momentum of the reconstructed
  /// MAUS::DataStructure::Global::Track at this point.
  TLorentzVector get_momentum() const;

  /// Set the error on the 4-momentum of the reconstructed
  /// MAUS::DataStructure::Global::Track at this point.
  void set_momentum_error(TLorentzVector momentum_error);

  /// Get the error on the 4-momentum of the reconstructed
  /// MAUS::DataStructure::Global::Track at this point.
  TLorentzVector get_momentum_error() const;

  /// Set the component MAUS::DataStructure::Global::SpacePoint, the detector
  /// measurement matching this reconstructed position.
  void set_space_point_tref(TObject *space_point);

  /// Get the component MAUS::DataStructure::Global::SpacePoint, the detector
  /// measurement matching this reconstructed position.
  TObject* get_space_point_tref() const;

  /// Set the component MAUS::DataStructure::Global::SpacePoint, the detector
  /// measurement matching this reconstructed position.
  void set_space_point(MAUS::DataStructure::Global::SpacePoint* space_point);

  /// Get the component MAUS::DataStructure::Global::SpacePoint, the detector
  /// measurement matching this reconstructed position.
  MAUS::DataStructure::Global::SpacePoint* get_space_point() const;

 private:

  /// The index of the particle event that produced this track point. If
  /// creation of the TrackPoint was not due to a particle event the value
  /// should remain at the default value of -1.
  int _particle_event;

  /// The name of the mapper which produced this track.
  std::string    _mapper_name;

  /// The calibrated 'charge' or energy deposit of this point.
  ///
  /// \todo It is possible that this should simply be the matching
  /// SpacePoint's charge, in which case it may be removed the and
  /// Set/Get functions changed to access the underlieing SpacePoint.
  double         _charge;

  /// The 4-momentum of the reconstructed Track at this point.
  TLorentzVector  _momentum;
  /// The error on the 4-momentum of the reconstructed Track at this
  /// point.
  TLorentzVector  _momentum_error;

  /// The reconstruction object defining the matching detector
  /// measurement.
  TRef _space_point;

  MAUS_VERSIONED_CLASS_DEF(TrackPoint);
}; // ~class TrackPoint

typedef std::vector<MAUS::DataStructure::Global::TrackPoint> TrackPointArray;
typedef std::vector<MAUS::DataStructure::Global::TrackPoint *>
  TrackPointPArray;
typedef std::vector<const MAUS::DataStructure::Global::TrackPoint *>
  TrackPointCPArray;
typedef std::vector<const MAUS::DataStructure::Global::TrackPoint *>
  ConstTrackPointPArray;
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
