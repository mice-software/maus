#include <sstream>

#include "src/legacy/BeamTools/BTField.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

#ifndef recon_kalman_GlobalErrorTracking_hh_
#define recon_kalman_GlobalErrorTracking_hh_

class G4Material;
class BTField;

namespace MAUS {
class MaterialModel;

namespace Kalman {
namespace Global {

/** Propagate a track and accompanying error through materials and fields
 *
 *  MCS Models:
 *    - moliere_forwards: d(theta^2)/dz is positive; RMS theta^2 will grow
 *                        if step size is positive, irrespective of pz
 *                        direction.
 *    - moliere_backwards: d(theta^2)/dz is negative; RMS theta^2 will grow
 *                        if step size is negative, irrespective of pz
 *                        direction.
 *    - no_mcs: d(theta^2)/dz is 0.
 */

class ErrorTracking {
public:
    enum ELossModel {bethe_bloch_forwards, bethe_bloch_backwards, no_eloss};
    enum MCSModel {moliere_forwards, moliere_backwards, no_mcs};
    enum EStragModel {estrag_forwards, estrag_backwards, no_estrag};
    enum TrackingModel {em_forwards, em_backwards};

    ErrorTracking();

    void Propagate(double x[29], double target_z);

    // BUG: Not tested for inversion ( if P V1 = V2; P^-1 V2 = V1 ) 
    void PropagateTransferMatrix(double x[44], double target_z);

    void UpdateTransferMatrix(const double point[4], double direction);

    void FieldDerivative(const double* point, double* derivative) const;

    void SetDeviations(double dx, double dy, double dz, double dt);
    std::vector<double> GetDeviations() const;

    /** Set the field for use by ErrorTracking
     *  @param BTField* field:- Set the field used for tracking - ErrorTracking
     *         cannot handle electric fields properly. If NULL, uses MC field
     *         defined in Globals. This is a borrowed pointer. Caller
     *         maintains ownership of the memory allocated to field. 
     */
    void SetField(BTField* field);
    BTField* GetField() const {return _field;}

    std::vector<std::vector<double> > GetMatrix() const;

    void SetMaxStepSize(double max_step_size) {_max_step_size = max_step_size;}
    double GetMaxStepSize() const {return _max_step_size;}

    void SetMinStepSize(double min_step_size) {_min_step_size = min_step_size;}
    double GetMinStepSize() const {return _min_step_size;}

    void SetEnergyLossModel(ELossModel eloss_model) {_eloss_model = eloss_model;}
    ELossModel GetEnergyLossModel() const {return _eloss_model;}

    void SetMCSModel(MCSModel mcs_model) {_mcs_model = mcs_model;}
    MCSModel GetMCSModel() const {return _mcs_model;}

    void SetEStragModel(EStragModel estrag_model) {_estrag_model = estrag_model;}
    EStragModel GetEStragModel() const {return _estrag_model;}

    void SetTrackingModel(TrackingModel track_model) {_track_model = track_model;}
    TrackingModel GetTrackingModel() const {return _track_model;}

    static ostream& print(std::ostream& out, const double x[29]);

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
    double _gsl_h = 1.;  // the step that gsl uses (can be negative)
    double _absolute_error = 1e-4;
    double _relative_error = 1e-4;
    ELossModel _eloss_model = no_eloss;
    MCSModel _mcs_model = no_mcs;
    EStragModel _estrag_model = no_estrag;
    TrackingModel _track_model = em_forwards;
    int  _max_n_steps = 100000;
    static constexpr double c_l = 299.792458; // mm*ns^{-1}
    // transient...
    double _charge;
    double _mass_squared;
    std::vector< std::vector<double> > _matrix;
    MaterialModel _mat_mod;
    static ErrorTracking* _tz_for_propagate;
    static const double _float_tolerance;
    static std::stringstream tracking_fail;
}; // class ErrorTracking
} // namespace Global
} // namespace Kalman
} // namespace MAUS
#endif

