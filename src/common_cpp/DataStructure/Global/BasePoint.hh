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

/** @class MAUS::DataStructure::Global::BasePoint
 *  @ingroup globalrecon
 *  @brief A base class for a point in 4D space.
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/13
 *
 *  A base class for elements which provide a 4D point in space.
 *  Currently, this is used by the default SpacePoint and TrackPoint
 *  classes.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALBASEPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALBASEPOINT_HH_


// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "Rtypes.h"
#include "TObject.h"
#include "TLorentzVector.h"

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Only elements in the DataStructure should inherit from TObject.
class BasePoint : public TObject {
 public:

  /// Default constructor - initialises to 0/NULL
  BasePoint();

  /// Copy constructor - any pointers are deep copied
  BasePoint(const BasePoint &base_point);

  /// Destructor
  virtual ~BasePoint();

  /// Assignment operator - any pointers are deep copied
  BasePoint& operator=(const BasePoint &base_point);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  void Clone(BasePoint *bpNew) const;

  // Getters and Setters for the member variables
  /// Set the 4D position of the point.
  void set_position(TLorentzVector position);

  /// Get the 4D position of the point.
  TLorentzVector get_position() const;

  /// Set the error on the 4D position of the point.
  void set_position_error(TLorentzVector position_error);

  /// Get the error on the 4D position of the point.
  TLorentzVector get_position_error() const;

  /// Set the MAUS::global::recon::DetectorPoint flag for the
  /// BasePoint, defining which detector is associated with the point.
  void set_detector(DetectorPoint detector);

  /// Get the MAUS::global::recon::DetectorPoint flag, #_detector, for the
  /// BasePoint, which defines the detector associated with the point.
  DetectorPoint get_detector() const;

  /// Set the fully qualified geometry path for this point.
  void set_geometry_path(std::string geometry_path);

  /// Get the fully qualified geometry path for this point.
  std::string get_geometry_path() const;

  /// Set the number of photoelectrons measured at this point.
  void set_num_photoelectrons(double num_photoelectrons);

  /// Get the number of photoelectrons measured at this point.
  double get_num_photoelectrons() const;

  /// Set the ADC charge measured at this point.
  void set_ADC_charge(int ADC_charge);

  /// Get the ADC charge measured at this point.
  int get_ADC_charge() const;

  /// Set the ADC charge product at this point.
  void set_ADC_charge_product(int ADC_charge_product);

  /// Get the ADC charge product at this point.
  int get_ADC_charge_product() const;

  /// A special setter, for the RegisterBaseClass method in JsonCppProcessors.
  void set_base(BasePoint value);

 private:

  /// The central 4D base position for the point.
  TLorentzVector  _position;
  /// The 4D uncertainty on the position #_position.
  TLorentzVector  _position_error;

  /// The detector element associated with this point.
  DetectorPoint  _detector;
  /// The fully qualified geometry path for this point.
  std::string    _geometry_path;

  /// Number of photoelectrons measured at this point.
  double _num_photoelectrons;

  /// ADC charge measured at this point.
  int _ADC_charge;

  /// ADC charge product at this point.
  int _ADC_charge_product;

  MAUS_VERSIONED_CLASS_DEF(BasePoint);
}; // ~class BasePoint

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
