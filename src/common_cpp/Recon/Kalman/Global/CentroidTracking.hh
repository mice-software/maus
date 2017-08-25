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
 * along with MAUS.  If not, see <http://   www.gnu.org/licenses/>.
 *
 * Copyright Chris Rogers, 2016
 */

#include <sstream>
#include <vector>
#include <string>

#include "src/legacy/BeamTools/BTField.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

#ifndef recon_kalman_GlobalCentroidTracking_hh_
#define recon_kalman_GlobalCentroidTracking_hh_

class G4Material;
class BTField;

namespace MAUS {
class MaterialModel;

namespace Kalman {
namespace Global {

/** @class CentroidTracking Propagate a track and accompanying error through
 *  materials and fields
 *
 *  Track propagation is done by standard Lorentz force law F = q (v ^ B).
 *
 *  Through materials, energy loss yields loss in mean energy according to e.g.
 *  Bethe Bloch model.
 *
 *  We have to be careful to consider the case where forward-going events are
 *  extrapolated forwards; forward-going tracks are extrapolated backwards; and
 *  backward-going tracks are extrapolated backwards or forwards. These different
 *  cases are handled by the ELossModel and TrackingModel.
 *
 *  Numerical integration is done using GSL RK4 routines. Note that these 
 *  routines require a global function pointer. So during tracking we
 *  assign a static version of CentroidTracking and a static member function
 *  pointer to point at the tracking.
 *
 *  ELossModel controls model of energy loss through material
 *    - bethe_bloch_forwards: dE/dz is negative: energy will decrease if step
 *           size is positive, irrespective of pz direction
 *    - bethe_bloch_backwards: dE/dz is negative: energy will increase if step
 *          size is positive, irrespective of pz direction
 *    - no_eloss: dE/dz is 0.
 *  TrackingModel: controls how tracking through the electromagnetic fields is
 *  done
 *    - em_forwards_dynamic: track forwards, using dynamic step size routine to
 *      determine step size (where we choose smaller step size near materials)
 *    - em_backwards_dynamic: track backwards, using dynamic step size routine to
 *      determine step size (where we choose smaller step size near materials)
 *    - em_forwards_static: track forwards, using fixed step size
 *    - em_backwards_static: track forwards, using fixed step size
 *  GeometryModel: controls how geometry is managed in the fields
 *    - geant4: use the full geant4 material model. At each step, lookup up the
 *      distance to the nearest boundary and use this to determine step size
 *    - axial_lookup: use a lookup table of materials that is determined at run
 *      time by looking up the materials on axis. Assumes all materials are
 *      cylindrical.
 *
 *  Other things:-
 *    - min_step_size, max_step_size: stepping starts at min_step_size.
 *    - max_n_steps: maximum number of steps. If tracking goes for more steps,
 *      tracking will abort with GSL_FAILURE.
 *
 *  TODO: hard coded max_n_steps, no "error tolerance" in dynamic step size
 *        routines
 */
class CentroidTracking {
 public:
    enum ELossModel {bethe_bloch_forwards, bethe_bloch_backwards, no_eloss};
    enum TrackingModel {em_forwards_dynamic, em_backwards_dynamic,
                        em_forwards_static, em_backwards_static};
    enum GeometryModel {geant4, axial_lookup};
    enum TrackingSuccess {success = GSL_SUCCESS, failure = GSL_FAILURE};

    /** Constructor; just sets some default things
     *
     *  Defaults field to Globals::GetMCFieldConstructor()
     */
    CentroidTracking();

    /** Propagate a track to z position target_z
     *
     *  @param x: array of length 29; first 8 parameters are 
     *            (time, x, y, z), (total energy, px, py, pz)
     *      x is a borrowed pointer; caller owns memory allocated to x.
     *      Propagation is done "in-place" i.e. x will be updated.
     *  @param target_z target position to which the track will be propagated
     */
    TrackingSuccess Propagate(double x[8], double target_z);

    /** Set the field for use by CentroidTracking
     *  @param BTField* field:- Set the field used for tracking - CentroidTracking
     *         cannot handle electric fields properly. If NULL, uses MC field
     *         defined in Globals. This is a borrowed pointer. Caller
     *         maintains ownership of the memory allocated to field. 
     */
    void SetField(const BTField* field);

    /** Get the field used by CentroidTracking. Note this is a borrowed pointer,
     *  borrowed from caller to SetField (Globals::MCFieldConstructor by default) 
     */
    const BTField* GetField() const {return _field;}

    /** Set the maximum step size used by the tracking. Will always take the
     *  absolute value (tracking determines direction).
     *
     *  @throws Exception if _min_step_size > max_step_size
     */
    void SetMaxStepSize(double max_step_size);
    /** Get the maximum step size used by the tracking */
    double GetMaxStepSize() const {return _max_step_size;}

    /** Set the minimum step size used by the tracking. Will always take the
     *  absolute value (tracking determines direction).
     *
     *  @throws Exception if min_step_size > _max_step_size
     */
    void SetMinStepSize(double min_step_size);
    /** Get the minimum step size used by the tracking */
    double GetMinStepSize() const {return _min_step_size;}

    /** Set the energy loss model
     *  @param eloss_model: string from "bethe_bloch_forwards",
     *         "bethe_bloch_backwards", "no_eloss"
     */
    void SetEnergyLossModel(std::string eloss_model);
    /** Set the energy loss model */
    void SetEnergyLossModel(ELossModel eloss_model) {_eloss_model = eloss_model;}
    /** Get the energy loss model */
    ELossModel GetEnergyLossModel() const {return _eloss_model;}

    /** Set the tracking model
     *  @param tracking_model: string from "em_forwards_dynamic",
     *         "em_backwards_dynamic", "em_forwards_static",
     *         "em_backwards_static"
     */
    void SetTrackingModel(std::string tracking_model);
    /** Set the tracking model */
    void SetTrackingModel(TrackingModel track_model) {_track_model = track_model;}
    /** Get the tracking model */
    TrackingModel GetTrackingModel() const {return _track_model;}

    /** Set the geometry model
     *  @param geometry_model: string from "geant4", "axial_lookup"
     */
    void SetGeometryModel(std::string geometry_model);
    /** Set the geometry model */
    void SetGeometryModel(GeometryModel geometry) {_geometry = geometry;}
    /** Get the geometry model */
    GeometryModel GetGeometryModel() const {return _geometry;}

    /** Set the charge of the particle to be tracked, in units of electron charge
     */
    void SetCharge(double charge) {_charge = charge;}
    /** Get the charge of the particle to be tracked, in units of electron charge
     */
    double GetCharge() const {return _charge;}

    /** Print an output/input of the Propagate function to out
     *
     *  @param out print to this output stream
     *  @param x: array formatted as per Propagate function input
     */
    static ostream& print(std::ostream& out, const double x[8]);

 private:
    CentroidTracking(const CentroidTracking& tracking); // disable copy constructor

    // x_in is a vector of size 8, going like t,x,y,z; E,px,py,pz
    static int EquationsOfMotion(double z, const double x[8], double dxdt[8],
                          void* params);
    static int EMEquationOfMotion(double z, const double x[8], double dxdt[8],
                          void* params);
    static int MaterialEquationOfMotion(double z, const double x[8], double dxdt[8],
                          void* params);

    const BTField* _field;
    // scale for derivative calculation
    double _min_step_size = 0.1;  // magnitude of the step size
    double _max_step_size = 100.;  // magnitude of the step size
    ELossModel _eloss_model = no_eloss;
    TrackingModel _track_model = em_forwards_dynamic;
    GeometryModel _geometry = geant4;
    int  _max_n_steps = 100000;
    static constexpr double c_l = 299.792458; // mm*ns^{-1}
    // transient...
    double _gsl_h = 1.;  // the step that gsl uses (can be negative)
    double _charge;
    double _mass_squared;
    static CentroidTracking* _tz_for_propagate;
    static const double _float_tolerance;
    static std::stringstream tracking_fail;
}; // class CentroidTracking
} // namespace Global
} // namespace Kalman
} // namespace MAUS
#endif

