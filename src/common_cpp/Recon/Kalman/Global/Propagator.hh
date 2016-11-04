/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 * Copyright Chris Rogers, 2016
 */

#include "src/common_cpp/Recon/Kalman/KalmanPropagatorBase.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTracking.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
/** Global::Propagator implements KalmanPropagatorBase wrapping
 *  Global::ErrorTracking for use in Kalman filtering.
 *
 *  This propagator uses a state vector of length 6, going like
 *  time, x, y, energy, px, py. Pz is calculated from the particle mass and z
 *  is determined by the measurement (i.e. it is the z-position of each
 *  measurement site). Propagation is done by ErrorTracking, which does a Runge-
 *  Kutta integration to propagate tracks from one plane to the next.
 *
 *  The Propagator has a tracking object and a record of the propagated
 *  particle's assumed mass (if this is wrong, tracking is not consistent).
 */
class Propagator: public Propagator_base {
  public:
    /** Constructor
     * constructs _tracking with default constructor and mass to 0.
     */
    Propagator();

    /** Destructor does nothing */
    ~Propagator();

    /** Propagate tracks at start_tp to end_tp position
     *
     *  @param start_tp: the track point to be propagated; propagates the track
     *                   point predicted state vector.
     *  @param end_tp: the end track point; fills the track point predicted
     *                 state vector.
     *
     *  Converts the track point to a double array suitable for use by
     *  ErrorTracking; calls the ErrorTracking Propagate method; converts the
     *  resultant array to a track point (filling end_tp).
     */
    virtual void Propagate(const TrackPoint& start_tp, TrackPoint& end_tp);

    /** Use the ErrorTracking to calculate a propagator.
     *
     *  @param start_tp: We really calculate the propagator (Jacobian) in the
     *                   the neighbourhood of the state vector defined here.
     *  @param end_tp: only purpose is to define the z-position to which the
     *                 propagator is calculated.
     *  @returns a propagator matrix (which transforms state vector from one
     *  measurement site to another). This is like an accelerator physics
     *  transfer matrix.
     */
    virtual TMatrixD CalculatePropagator(const TrackPoint& start_tp,
                                         const TrackPoint& end_tp);

    /** Use the ErrorTracking to calculate the process noise.
     *
     *  @param start_tp: We really calculate the propagator (Jacobian) in the
     *                   the neighbourhood of the state vector defined here.
     *  @param end_tp: only purpose is to define the z-position to which the
     *                 propagator is calculated.
     *  @returns a propagator matrix (which transforms state vector from one
     *  measurement site to another). This is like an accelerator physics
     *  transfer matrix.
     */
    virtual TMatrixD CalculateProcessNoise(const TrackPoint& start_tp,
                                           const TrackPoint& end_tp);

    /** Set the ErrorTracking's field.
     *
     *  @param field: field used for tracking
     */

    void SetField(BTField* field) {_tracking.SetField(field);}

    /** Set the mass hypothesis for the particle
     *
     *  @param mass: the mass hypothesis for the particle to be propagated.
     */
    void SetMass(double mass) {_mass = mass;}

    /** Get the mass hypothesis for the particle
     *
     *  @returns the mass hypothesis for the particle to be propagated.
     */
    double GetMass() const {return _mass;}

    /** Get the tracking object that is used for all calculations
     *
     *  @returns the ErrorTracking object. Propagator owns memory (not caller).
     */
    ErrorTracking* GetTracking() {return &_tracking;}

  private:
    Propagator& operator=(const Propagator& rhs);
    Propagator(const Propagator& rhs);
    ErrorTracking _tracking;
    double _mass;
};
}
} //  Kalman
} //  MAUS

