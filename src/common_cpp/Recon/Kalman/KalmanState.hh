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

#ifndef KALMANSITE_HH
#define KALMANSITE_HH

// C headers
#include <assert.h>

// C++ headers
#include <vector>

#include "TMath.h"
#include "TMatrixD.h"

#include "Utils/Exception.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

struct SciFiParams {
  /// Polystyrene's atomic number.
  double Z;
  /// Width of the fibre plane in mm.
  double Plane_Width;
  /// Fibre radiation lenght in mm
  double Radiation_Length;
  /// Fractional Radiation Length
  double R0(double lenght) { return lenght/Radiation_Length; }
  /// Density in g.cm-3
  double Density;
  /// Mean excitation energy in eV.
  double Mean_Excitation_Energy;
  /// Atomic number in g.mol-1 per styrene monomer
  double A;
  /// Channel width in mm
  double Pitch;
  /// Station Radius in mm
  double Station_Radius;
  /// RMS per channel measurement (um).
  double RMS;
};

struct AirParams {
  /// Air mean atomic number.
  double Z;
  /// Air radiation lenght in mm
  double Radiation_Length;
  /// Fractional Radiation Length
  double R0(double lenght) { return lenght/Radiation_Length; }
  /// Density in g.cm-3
  double Density;
  /// Mean excitation energy in eV.
  double Mean_Excitation_Energy;
  /// Atomic number in g.mol-1 per styrene monomer
  double A;
};

/** @class KalmanState
 *
 *  @brief A KalmanState is a tracker plane. Each tracker contains 15 (if all planes are hit).
 *
 *  Each KalmanState is a container for matrices which describe the state of the fit
 *  and the errors associated.
 *
 */
class KalmanState {
 public:
  KalmanState();

  ~KalmanState();

  KalmanState(const KalmanState &site);

  KalmanState& operator=(const KalmanState& site);

  enum State { Initialized = -1,
               Projected,
               Filtered,
               Smoothed,
               Excluded };

  /** @brief Initialises member matrices.
   *
   *  @param dim The number of parameters to be calculated.
   */
  void Initialise(int dim);

  void Build(SciFiCluster *cluster);

  void MoveToGlobalFrame(ThreeVector ref_pos);

  void set_spill(int spill) { _spill = spill; }

  void set_event(int event) { _event = event; }

  /** @brief Sets the state vector at the site.
   */
  void set_a(TMatrixD a, State current_state);

  /** @brief Set the mother cluster */
  void set_cluster(SciFiCluster* cluster) { _cluster = cluster; }

  void set_covariance_matrix(TMatrixD C, State current_state);

  /** @brief Sets the residual
   *
   *  This is the difference between the projected, filtered or smoothed estimation and
   *  the measurement.
   *
   */
  void set_residual(TMatrixD residual, State current_state);

  void set_covariance_residual(TMatrixD C, State kalman_state);

  void set_true_momentum(ThreeVector mc_mom) { _mc_mom = mc_mom; }

  void set_true_position(ThreeVector mc_pos) { _mc_pos = mc_pos; }

  void set_input_shift(TMatrixD input_shift)       { _input_shift = input_shift; }

  void set_input_shift_covariance(TMatrixD input_covariance) {
                _input_shift_covariance = input_covariance; }

  void set_shift(TMatrixD shift)                   { _shift = shift; }

  void set_shift_covariance(TMatrixD shift_covariance) {
                _shift_covariance = shift_covariance; }

  void set_measurement(double alpha)               { _v(0, 0) = alpha;
                                                     _v(1, 0) = 0.0; }

  void set_direction(ThreeVector dir)        { _direction = dir; }

  void set_z(double z)                             { _z = z; }

  void set_id(int id)                              { _id = id; }

  void set_chi2(double chi2, State kalman_state);

  void set_current_state(State kalman_state)       { _current_state = kalman_state; }

  int spill()                            const { return _spill; }

  int event()                            const { return _event; }

  State current_state()                  const { return _current_state; }

  TMatrixD a(State desired_state)        const;

  TMatrixD covariance_matrix(State ds)   const;

  TMatrixD residual(State desired_state) const;

  TMatrixD covariance_residual(State st) const;

  double chi2(State desired_state)       const;

  SciFiCluster* cluster()                const { return _cluster; }

  TMatrixD input_shift()                 const { return _input_shift; }

  TMatrixD input_shift_covariance()      const { return _input_shift_covariance; }

  TMatrixD shift()                       const { return _shift; }

  TMatrixD shift_covariance()            const { return _shift_covariance; }

  TMatrixD measurement()                 const { return _v; }

  double z()                             const { return _z; }

  int id()                               const { return _id; }

  ThreeVector direction()          const { return _direction; }

  ThreeVector true_momentum()      const { return _mc_mom; }

  ThreeVector true_position()      const { return _mc_pos; }

 private:
  /// The spill.
  int _spill;
  /// The event number.
  int _event;
  /// State of the site.
  State _current_state;
  /// Z placement of the site (mm).
  double _z;
  /// Site id.
  int _id;
  /// The Chi2 at this site.
  double _f_chi2;
  double _s_chi2;

  /// A pointer to the cluster used to form the state - does not assume control of memory
  SciFiCluster* _cluster;

  /// Orientation of the measuring plane.
  ThreeVector _direction;

  /// The state vector.
  TMatrixD _projected_a;
  TMatrixD _a;
  TMatrixD _smoothed_a;
  TMatrixD _a_excluded;

  /// The covariance matrix.
  TMatrixD _projected_C;
  TMatrixD _C;
  TMatrixD _smoothed_C;
  TMatrixD _C_excluded;

  /// The measurement.
  TMatrixD _v;

  /// The residuals.
  TMatrixD _pull;
  TMatrixD _residual;
  TMatrixD _smoothed_residual;
  TMatrixD _excluded_residual;

  TMatrixD _covariance_residual;
  TMatrixD _covariance_smoothed_residual;
  TMatrixD _covariance_excluded_residual;

  /// Alignment.
  TMatrixD _input_shift;
  TMatrixD _shift;

  TMatrixD _input_shift_covariance;
  TMatrixD _shift_covariance;

  ThreeVector _mc_pos;
  ThreeVector _mc_mom;
};

typedef std::vector<KalmanState*> KalmanStatesPArray;

} // ~namespace MAUS

#endif
