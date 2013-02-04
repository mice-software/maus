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

/** @class MAUS::DataStructure::Global::SpacePoint
 *  @ingroup globalrecon
 *  @brief A reconstructed point in 4D space, matching a detector measurement.
 *
 *  The measurement of a single detector element, represented as a 4D
 *  point in space, with an associated set of errors.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALSPACEPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALSPACEPOINT_HH_


// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "Rtypes.h"
#include "TObject.h"
#include "TLorentzVector.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

class SpacePoint : public TObject {
 public:

  /// Default constructor - initialises to 0/NULL
  SpacePoint();

  /// Copy constructor - any pointers are deep copied
  SpacePoint(const SpacePoint &spacepoint);

  /// Destructor
  virtual ~SpacePoint();

  /// Assignment operator - any pointers are deep copied
  SpacePoint& operator=(const SpacePoint &spacepoint);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  SpacePoint* Clone() const;

  // Getters and Setters for the member variables
  /// Set the calibrated 'charge' or energy deposit of the matching
  /// detector measurement.
  void set_charge(double charge) {
    _charge = charge;
  }
  /// Get the calibrated 'charge' or energy deposit of the matching
  /// detector measurement.
  double get_charge() const {
    return _charge;
  }

  /// Set the 4D position of the detector measurement.
  void set_position(TLorentzVector position) {
    _position = position;
  }
  /// Get the 4D position of the detector measurement.
  TLorentzVector get_position() const {
    return _position;
  }

  /// Set the error on the 4D position of the detector measurement.
  void set_position_error(TLorentzVector position_error) {
    _position_error = position_error;
  }
  /// Get the error on the 4D position of the detector measurement.
  TLorentzVector get_position_error() const {
    return _position_error;
  }

  /// Set the MAUS::global::recon::DetectorPoint flag for the
  /// SpacePoint, defining which detector produced the point.
  void set_detector(DetectorPoint detector) {
    _detector = detector;
  }
  /// Get the MAUS::global::recon::DetectorPoint flag, #_detector, for the
  /// SpacePoint, which defines the detector that produced this point.
  DetectorPoint get_detector() const {
    return _detector;
  }

  /// Set the fully qualified geometry path for this point in space.  This
  /// string may not be required in the end...
  void set_geometry_path(std::string geometry_path) {
    _geometry_path = geometry_path;
  }
  /// Get the fully qualified geometry path for this point in space.
  std::string get_geometry_path() const {
    return _geometry_path;
  }
  
 private:

  /// The calibrated 'charge' or energy deposit of the detector
  /// measurement.
  double         _charge;

  /// The central 4D space position for the detector measurement.
  TLorentzVector  _position;
  /// The 4D uncertainty on the space position #_position.
  TLorentzVector  _position_error;

  /// The detector element which produced this SpacePoint.
  DetectorPoint  _detector;
  /// The fully qualified geometry path for this point in space.  May
  /// be removed if deemed unnecessary.
  std::string    _geometry_path;

  ClassDef(SpacePoint, 1)
}; // ~class SpacePoint
  
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
