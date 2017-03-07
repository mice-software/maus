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
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
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

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/DataStructure/Global/BasePoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Only elements in the DataStructure should inherit from TObject.
class SpacePoint : public BasePoint {
 public:

  /// Default constructor - initialises to 0/NULL
  SpacePoint();

  /// Copy constructor - any pointers are deep copied
  SpacePoint(const SpacePoint &space_point);

  /// Destructor
  virtual ~SpacePoint();

  /// Assignment operator - any pointers are deep copied
  SpacePoint& operator=(const SpacePoint &space_point);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  SpacePoint* Clone() const;

  // Getters and Setters for the member variables
  /// Set the calibrated 'charge' or energy deposit of the matching
  /// detector measurement.
  void set_charge(double charge);

  /// Get the calibrated 'charge' or energy deposit of the matching
  /// detector measurement.
  double get_charge() const;

  /// Set the name for the mapper which produced the track, #_mapper_name.
  void set_mapper_name(std::string mapper_name);

  /// Get the name for the mapper which produced the track, #_mapper_name.
  std::string get_mapper_name() const;

 private:

  /// The name of the mapper which produced this spacepoint.
  std::string    _mapper_name;

  /// The calibrated 'charge' or energy deposit of the detector
  /// measurement.
  double         _charge;

  MAUS_VERSIONED_CLASS_DEF(SpacePoint);
}; // ~class SpacePoint

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
