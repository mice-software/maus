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

#ifndef recon_kalman_GlobalErrorTracking_hh_
#define recon_kalman_GlobalErrorTracking_hh_

class G4Material;
class BTField;

namespace MAUS {
class MaterialModel;

namespace Kalman {
namespace Global {

/** @class ErrorTracking Propagate a track and accompanying error through
 *  materials and fields
 *
 *  Track propagation is done by standard Lorentz force law F = q (v ^ B).
 *  Electric field is not implemented. Error propagation is done by integrating
 *  the derivatives of the force law to find the Jacobian.
 *
 *  Through materials, energy loss yields loss in mean energy according to e.g.
 *  Bethe Bloch model. Curvature of bethe bloch yields growth in energy spread,
 *  while scattering yields growth in px and py spread (error). Energy straggling
 *  is not implemented.
 *
 *  We have to be careful to consider the case where forward-going events are
 *  extrapolated forwards; forward-going tracks are extrapolated backwards; and
 *  backward-going tracks are extrapolated backwards or forwards. These different
 *  cases are handled by the MCSModel and ELossModel.
 *
 *  Numerical integration is done using GSL RK4 routines. Note that these 
 *  routines require a global function pointer. So during tracking we
 *  assign a static version of ErrorTracking and a static member function
 *  pointer to point at the tracking.
 *
 *  MCSModel controls model of scattering through material
 *    - moliere_forwards: d(theta^2)/dz is positive; RMS theta^2 will grow
 *                        if step size is positive, irrespective of pz
 *                        direction.
 *    - moliere_backwards: d(theta^2)/dz is negative; RMS theta^2 will grow
 *                        if step size is negative, irrespective of pz
 *                        direction.
 *    - no_mcs: d(theta^2)/dz is 0.
 *  ELossModel controls model of energy loss through material
 *    - bethe_bloch_forwards: dE/dz is negative: energy will decrease if step
 *           size is positive, irrespective of pz direction
 *    - bethe_bloch_backwards: dE/dz is negative: energy will increase if step
 *          size is positive, irrespective of pz direction
 *    - no_eloss: dE/dz is 0.
 *  EStragModel controls model of energy straggling through material
 *    - not implemented. use no_estrag.                            
 *  TrackingModel: controls how tracking through the electromagnetic fields is
 *  done
 *    - em_forwards_dynamic: track forwards, using dynamic step size routine to
 *      determine step size (where we choose smaller step size near materials)
 *    - em_forwards_dynamic: track backwards, using dynamic step size routine to
 *      determine step size (where we choose smaller step size near materials)
 *    - em_forwards_static: track forwards, using fixed step size
 *    - em_backwards_static: track forwards, using fixed step size
 *  GeometryModel: controls how geometry is managed in the fields
 *    - geant4: use the full geant4 material model. At each step, lookup up the
 *      disstance to the nearest boundary and use this to determine step size
 *    - axial_lookup: use a lookup table of materials that is determined at run
 *      time by looking up the materials on axis. Assumes all materials are
 *      cylindrical.
 *
 *  Other things:-
 *    - dx, dy, dz, dt: in order to do the tracking, we need derivatives of the
 *      field. These are found numerically, with scale set by these parameters.
 *    - min_step_size, max_step_size: in order to do the tracking, we need 
 *      derivatives of the field. These are found numerically, with scale set by
 *      these parameters. Stepping starts at min_step_size.
 *    - max_n_steps: maximum number of steps. If tracking goes for more steps,
 *      tracking will abort with GSL_FAILURE.
 */
class ErrorTracking {
 public:
    enum ELossModel {bethe_bloch_forwards, bethe_bloch_backwards, no_eloss};
    enum MCSModel {moliere_forwards, moliere_backwards, no_mcs};
    enum EStragModel {estrag_forwards, estrag_backwards, no_estrag};
    enum TrackingModel {em_forwards_dynamic, em_backwards_dynamic,
                        em_forwards_static, em_backwards_static};
    enum GeometryModel {geant4, axial_lookup};

    /** Constructor; just sets some default things
     *
     *  Defaults field to Globals::GetMCFieldConstructor()
     */
    ErrorTracking();

    /** Propagate a track and error ellipse to z position target_z
     *
     *  @param x: array of length 29; first 8 parameters are 
     *            (time, x, y, z), (total energy, px, py, pz)
     *     next 21 parameters are
     *             V_tt,  V_tx,  V_ty,  V_tE,  V_tpx,  V_tpy
     *                    V_xx,  V_xy,  V_xE,  V_xpx,  V_xpy
     *                           V_yy,  V_yE,  V_ypx,  V_ypy
     *                                  V_EE,  V_Epx,  V_Epy
     *                                         V_pxpx, V_pxpy
     *                                                 V_pypy
     *      x is a borrowed pointer; caller owns memory allocated to x.
     *      Propagation is done "in-place" i.e. x will be updated.
     *  @param target_z target position to which the track will be propagated
     *
     *  Nb: if you are reading this in doxygen, probably the formatting gets
     *  screwed up
     */
    void Propagate(double x[29], double target_z);

    /** Propagate a track and transfer matrix to z position target_z
     *
     *  @param x: array of length 44; first 8 parameters are 
     *            (time, x, y, z), (total energy, px, py, pz)
     *     next 36 parameters are
     *             M_tt,  M_tE,  M_tx,  M_tpx,  M_ty,  M_tpy
     *             M_Et,   ...
     *             M_xt,   ...
     *             M_pxt,  ...
     *             M_yt,   ...
     *             M_pyt,  ...
     *      It is an error if total energy < total momentum. x is a borrowed
     *      pointer; caller owns memory allocated to x. propagation is done
     *      "in-place" i.e. x will be updated.
     *  @param target_z target position to which the track will be propagated
     *
     *  Nb: if you are reading this in doxygen, probably the formatting gets
     *  screwed up
     */

    // BUG: Not tested for inversion ( if P V1 = V2; P^-1 V2 = V1 )
    void PropagateTransferMatrix(double x[44], double target_z);

    /** Calculate the infinitesimal transfer matrix
     *
     *  Calculate the derivative transfer matrix N, defined by
     *        M(z->z+dz) = 1+N dz
     *  This is filled into _matrix, which can be accessed by a call to
     *  GetMatrix()
     *  @param point: array of length 8, given by
     *               (time, x, y, z), (total energy, px, py, pz)
     *  @param direction: should be either +1 or -1; defines the direction in
     *               which time is advancing. If -1, time is advancing backwards
     */
    void UpdateTransferMatrix(const double point[8], double direction);

    /** Get the infinitesimal transfer matrix. See UpdateTransferMatrix(...) */
    std::vector<std::vector<double> > GetMatrix() const;

    /** Set the deviations used for B field derivative calculation
     *  @param dx: deviation in x
     *  @param dy: deviation in y
     *  @param dz: deviation in z
     *  @param dt: deviation in time
     */
    void SetDeviations(double dx, double dy, double dz, double dt);

    /** Get the deviations used for B field derivative calculation
     *  @returns a vector of length 4, like {dx, dy, dz, dt}
     */
    std::vector<double> GetDeviations() const;

    /** Set the field for use by ErrorTracking
     *  @param BTField* field:- Set the field used for tracking - ErrorTracking
     *         cannot handle electric fields properly. If NULL, uses MC field
     *         defined in Globals. This is a borrowed pointer. Caller
     *         maintains ownership of the memory allocated to field. 
     */
    void SetField(BTField* field);

    /** Get the field used by ErrorTracking. Note this is a borrowed pointer,
     *  borrowed from caller to SetField (Globals::MCFieldConstructor by default) 
     */
    BTField* GetField() const {return _field;}

    /** Set the maximum step size used by the tracking
     *
     *  @throws Exception if _min_step_size > max_step_size
     */
    void SetMaxStepSize(double max_step_size);
    /** Get the maximum step size used by the tracking */
    double GetMaxStepSize() const {return _max_step_size;}

    /** Set the minimum step size used by the tracking.
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

    /** Set the multiple coulomb scattering model
     *  @param mcs_model: string from "moliere_forwards", "moliere_backwards",
     *                    "no_mcs"
     */
    void SetMCSModel(std::string mcs_model);
    /** Set the multiple coulomb scattering model */
    void SetMCSModel(MCSModel mcs_model) {_mcs_model = mcs_model;}
    /** Get the multiple coulomb scattering model */
    MCSModel GetMCSModel() const {return _mcs_model;}

    /** Set the energy straggling model (not used) */
    void SetEStragModel(std::string estrag_model);
    /** Set the energy straggling model (not used) */
    void SetEStragModel(EStragModel estrag_model) {_estrag_model = estrag_model;}
    /** Get the energy straggling model (not used) */
    EStragModel GetEStragModel() const {return _estrag_model;}

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
    static ostream& print(std::ostream& out, const double x[29]);

    /** Get the field derivative at a point in transverse variables x, y
     *
     *  @param point position/time at which the derivative should be calculated,
     *         like {x, y, z, time}. Caller owns the memory.
     *  @param derivatives: array of length 6, gets overwritten with 
     *         {dBx/dx, dBy/dx, dBz/dx, dBx/dy, dBy/dy, dBz/dy}.
     */
    void FieldDerivative(const double* point, double* derivative) const;

 private:
    ErrorTracking(const ErrorTracking& tracking); // disable copy constructor

    // x_in is a vector of size 8, going like t,x,y,z; E,px,py,pz
    static int EquationsOfMotion(double z, const double x[29], double dxdt[29],
                          void* params);
    static int EMEquationOfMotion(double z, const double x[29], double dxdt[29],
                          void* params);
    static int MaterialEquationOfMotion(double z, const double x[29], double dxdt[29],
                          void* params);
    static int MatrixEquationOfMotion(double z, const double x[29], double dxdt[29],
                          void* params);

    BTField* _field;
    // scale for derivative calculation
    double _dx, _dy, _dz, _dt;
    double _min_step_size = 0.1;  // magnitude of the step size
    double _max_step_size = 100.;  // magnitude of the step size
    ELossModel _eloss_model = no_eloss;
    MCSModel _mcs_model = no_mcs;
    EStragModel _estrag_model = no_estrag;
    TrackingModel _track_model = em_forwards_dynamic;
    GeometryModel _geometry = geant4;
    int  _max_n_steps = 100000;
    static constexpr double c_l = 299.792458; // mm*ns^{-1}
    // transient...
    double _gsl_h = 1.;  // the step that gsl uses (can be negative)
    double _charge;
    double _mass_squared;
    std::vector< std::vector<double> > _matrix;
    static ErrorTracking* _tz_for_propagate;
    static const double _float_tolerance;
    static std::stringstream tracking_fail;
}; // class ErrorTracking
} // namespace Global
} // namespace Kalman
} // namespace MAUS
#endif

