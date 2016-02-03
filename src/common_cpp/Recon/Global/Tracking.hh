#include "src/legacy/BeamTools/BTField.hh"
#include "src/common_cpp/DataStructure/GlobalEvent.hh"

class G4Material;
class BTField;

namespace MAUS {
class TrackingZ {
public:
    enum ELossModel {bethebloch_forwards, bethebloch_backwards, no_eloss};
    enum MCSModel {moliere_forwards, moliere_backwards, no_mcs};
    enum EStragModel {no_estrag};

    TrackingZ();

    void Propagate(double* x, double target_z);

    void UpdateTransferMatrix(const double point[4]);

    void FieldDerivative(const double* point, double* derivative) const;

    void SetDeviations(double dx, double dy, double dz, double dt);
    std::vector<double> GetDeviations() const;

    /** Set the field for use by TrackingZ
     *  @param BTField* field:- the field - this is a borrowed pointer. Caller
     *         maintains ownership of the memory allocated to field. TrackingZ
     *         cannot handle electric fields properly.
     */
    void SetField(BTField* field) {_field = field;}
    BTField* GetField() const {return _field;}

    std::vector<std::vector<double> > GetMatrix() const;

    void SetStepSize(double step_size) {_step_size = step_size;}
    double GetStepSize() const {return _step_size;}

    void SetEnergyLossModel(bool eloss_model) {_eloss_model = eloss_model;}
    bool GetEnergyLossModel() const {return _eloss_model;}

    void SetMCSModel(bool mcs_model) {_mcs_model = mcs_model;}
    bool GetMCSModel() const {return _mcs_model;}


private:
    TrackingZ(const TrackingZ& tz); // disable copy constructor

    // x_in is a vector of size 8, going like t,x,y,z; E,px,py,pz
    static int EquationsOfMotion(double z, const double x[29], double dxdt[29],
                          void* params);
    static int EMEquationOfMotion(double z, const double x[29], double dxdt[29],
                          void* params);

    BTField* _field;
    // scale for derivative calculation
    double _dx, _dy, _dz, _dt;
    double _step_size = 1.;
    double _absolute_error = 1e-4;
    double _relative_error = 1e-4;
    bool _eloss_model = bethebloch_forwards;
    bool _mcs_model = moliere_forwards;
    bool _estrag_model = no_estrag;
    int  _max_n_steps = 100000;
    static constexpr double c_l = 299.792458; // mm*ns^{-1}
    // transient...
    double _charge;
    std::vector< std::vector<double> > _matrix;
    static TrackingZ* _tz_for_propagate;
};
}
