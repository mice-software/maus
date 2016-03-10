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

class GlobalErrorTracking {
public:
    enum ELossModel {bethebloch_forwards, bethebloch_backwards, no_eloss};
    enum MCSModel {moliere_forwards, moliere_backwards, no_mcs};
    enum EStragModel {estrag_forwards, estrag_backwards, no_estrag};

    GlobalErrorTracking();

    void Propagate(double x[29], double target_z);
    void PropagateTransferMatrix(double x[44], double target_z);

    void UpdateTransferMatrix(const double point[4]);

    void FieldDerivative(const double* point, double* derivative) const;

    void SetDeviations(double dx, double dy, double dz, double dt);
    std::vector<double> GetDeviations() const;

    /** Set the field for use by GlobalErrorTracking
     *  @param BTField* field:- Set the field used for tracking - GlobalErrorTracking
     *         cannot handle electric fields properly. If NULL, uses MC field
     *         defined in Globals. This is a borrowed pointer. Caller
     *         maintains ownership of the memory allocated to field. 
     */
    void SetField(BTField* field);
    BTField* GetField() const {return _field;}

    std::vector<std::vector<double> > GetMatrix() const;

    void SetStepSize(double step_size) {_step_size = step_size;}
    double GetStepSize() const {return _step_size;}

    void SetEnergyLossModel(ELossModel eloss_model) {_eloss_model = eloss_model;}
    bool GetEnergyLossModel() const {return _eloss_model;}

    void SetMCSModel(MCSModel mcs_model) {_mcs_model = mcs_model;}
    bool GetMCSModel() const {return _mcs_model;}

    void SetEStragModel(EStragModel estrag_model) {_estrag_model = estrag_model;}
    bool GetEStragModel() const {return _estrag_model;}


private:
    GlobalErrorTracking(const GlobalErrorTracking& tracking); // disable copy constructor

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
    double _step_size = 1.;
    double _absolute_error = 1e-4;
    double _relative_error = 1e-4;
    ELossModel _eloss_model = bethebloch_forwards;
    MCSModel _mcs_model = moliere_forwards;
    EStragModel _estrag_model = no_estrag;
    int  _max_n_steps = 100000;
    static constexpr double c_l = 299.792458; // mm*ns^{-1}
    // transient...
    double _charge;
    std::vector< std::vector<double> > _matrix;
    MaterialModel _mat_mod;
    static GlobalErrorTracking* _tz_for_propagate;
};
}
#endif

