#include "src/common_cpp/Recon/Kalman/KalmanPropagatorBase.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTracking.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
/** Global::Propagator implements KalmanPropagatorBase wrapping
 *  Global::ErrorTracking for use in Kalman filtering.
 *
 *  
 */
class Propagator: public Propagator_base {
  public:
    Propagator();
    ~Propagator();

    virtual void Propagate(const TrackPoint& start_tp, TrackPoint& end_tp);

    virtual TMatrixD CalculatePropagator(const TrackPoint& start_tp,
                                         const TrackPoint& end_tp);

    virtual TMatrixD CalculateProcessNoise(const TrackPoint& start_tp,
                                           const TrackPoint& end_tp);

    void SetField(BTField* field) {_tracking.SetField(field);}

    void SetMass(double mass) {_mass = mass;}
    double GetMass() const {return _mass;}

    /** Propagator owns memory */
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

