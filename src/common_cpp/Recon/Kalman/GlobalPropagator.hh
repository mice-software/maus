#include "src/common_cpp/Recon/Kalman/KalmanPropagatorBase.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/GlobalErrorTracking.hh"

namespace MAUS {
namespace Kalman {

/** GlobalPropagator implements KalmanPropagatorBase wrapping
 *  GlobalErrorTracking for use in Kalman filtering.
 *
 *  
 */
class GlobalPropagator: public Propagator_base {
  public:
    GlobalPropagator();
    ~GlobalPropagator();

    virtual void Propagate(const State& start_state, State& end_state);

    virtual TMatrixD CalculatePropagator(const State& start_state, const State& end_state);

    virtual TMatrixD CalculateProcessNoise(const State& start_state, const State& end_state);

    void SetField(BTField* field) {_tracking.SetField(field);}

    void SetMass(double mass) {_mass = mass;}
    double GetMass() const {return _mass;}

    /** GlobalPropagator owns memory */
    GlobalErrorTracking* GetTracking() {return &_tracking;}

  private:
    GlobalPropagator& operator=(const GlobalPropagator& rhs);
    GlobalPropagator(const GlobalPropagator& rhs);
    GlobalErrorTracking _tracking;
    double _mass;
};
} //  Kalman
} //  MAUS

