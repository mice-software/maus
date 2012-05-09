/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#ifndef COMMON_CPP_RECONSTRUCTION_TRACK_POINT_HH
#define COMMON_CPP_RECONSTRUCTION_TRACK_POINT_HH

#include <iostream>

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
     
namespace MAUS {

class CovarianceMatrix;
class Detector;
class Vector<double>;

/* @class TrackPoint a phase space vector with redundant t/E and z/Pz
 * coordinates as well as an ID that links the track to the detector that
 * measured it.
 *
 * The redundant sets of coordinates can be filled in by explicitly using the
 * FillInCoordinates() function or by setting all of the coordinates at once
 * using the array or parameterised constructors (which implicitly call
 * FillInCoordinates()). If t < 0 it fills in t and E from z, Pz, and the given
 * mass parameter. If t >= 0 and z < 0, it fills in z and Pz from t, E, and
 * the mass.
 */
class TrackPoint : public MAUS::PhaseSpaceVector {
 public:
  /* @brief	Construct with all elements initialized to zero.
   */
  TrackPoint();

  /* @brief  Copy constructor.
   */
  TrackPoint(const TrackPoint & original_instance);

  /* @brief  Base class copy constructor.
   */
  explicit TrackPoint(const PhaseSpaceVector & original_instance);

  /* @brief  Vector<double> copy constructor.
   */
  explicit TrackPoint(const Vector<double> & original_instance);

  /* @brief Create with coordinates from an array.
     Order is t, E, x, Px, y, Py, z, Pz.
   */
  explicit TrackPoint(double const * const array);

  /* @brief	Reconstructed track point constructor. The track point was not
   * generated by a detector, so an uncertanty matrix must be supplied. The
   * detector_id field is set to Detector::None. 
   */
  TrackPoint(const double t, const double E,
                   const double x, const double Px,
                   const double y, const double Py,
                   const double z, const double Pz,
                   const CovarianceMatrix & uncertainties);

  /* @brief	Detector-generated track point constructor. The Detector reference
   * is used to copy it's ID and uncertainty matrix.
   * 
   */
  TrackPoint(const double t, const double E,
                   const double x, const double Px,
                   const double y, const double Py,
                   const double z, const double Pz,
                   const Detector & detector);

  ~TrackPoint();

  const bool operator==(const TrackPoint& rhs) const;

  // *************************
  //       Accessors
  // *************************
 
  double z() const;
  void set_z(const double z);

  double z_momentum() const;
  void set_z_momentum(const double z_momentum);
  double Pz() const;
  void set_Pz(const double Pz);

  void set_detector_id(unsigned int id);
  unsigned int detector_id() const;

  void set_uncertainties(const CovarianceMatrix & uncertainties);
  const CovarianceMatrix & uncertainties() const;
 
  void FillInAxialCoordinates(const double mass);  
  void FillInTemporalCoordinates(const double mass);  
 protected:
  double z_;
  double z_momentum_;
  unsigned int detector_id_;  // = 0 if this was not measured in a detector
  CovarianceMatrix * uncertainties_;
  
};

MAUS::MAUSPrimaryGeneratorAction::PGParticle PrimaryGeneratorParticle(
    const TrackPoint & point, const int pid);

std::ostream& operator<<(std::ostream & out, const TrackPoint & vector);
}  // namespace MAUS

#endif
