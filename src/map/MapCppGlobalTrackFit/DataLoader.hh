#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {
class SciFiEvent;
class TOFEvent;

class DataLoader {
    typedef MAUS::DataStructure::Global::DetectorPoint DetectorPoint;
  public:
    /** fit is a borrowed reference (caller owns the memory) */
    DataLoader(std::vector<DetectorPoint> detectors,
               Kalman::TrackFit* fit);
    ~DataLoader() {}

    /** Load events from event
     */
    void load_event(ReconEvent& event);

    void SetWillRequireTrackerTriplets(bool will_require) {_will_require_tracker_triplets = will_require;}
    bool GetWillRequireTrackerTriplets() {return _will_require_tracker_triplets;}
  private:
    void load_virtual_event();
    void load_tof_event(TOFEvent* event);
    void load_tof_detector(const MAUS::TOF0SpacePointArray& sp_vector,
                           MAUS::DataStructure::Global::DetectorPoint det);
    Kalman::State load_tof_space_point(TOFSpacePoint* sp);
    void load_scifi_event(SciFiEvent* event);
    void set_kalman_track();
    Kalman::State load_scifi_space_point(SciFiSpacePoint* sp);
    void validate_detectors();
    MAUS::DataStructure::Global::DetectorPoint SciFiToDetectorPoint(int tracker, int station) const;

    Kalman::TrackFit* _fitter;
    std::vector<MAUS::DataStructure::Global::DetectorPoint> _detectors;
    bool _will_require_tracker_triplets;
    inline bool WillUse(MAUS::DataStructure::Global::DetectorPoint det) const;
    // Maps State.GetId() to the detector that made the measurement

    static size_t _dimension;
    static double _err;

    class Detection {
        public:
            Detection(Kalman::Measurement_base* _meas, Kalman::TrackPoint point);
            Kalman::Measurement_base* _meas;
            Kalman::TrackPoint _point;
    };
    std::map<DetectorPoint, std::vector<Detection> > _points;
    void AddDetection(Kalman::Measurement_base* meas, Kalman::TrackPoint point, DetectorPoint det);

};

bool DataLoader::WillUse(MAUS::DataStructure::Global::DetectorPoint det) const {
    return std::find(_detectors.begin(), _detectors.end(), det)
                                                      != _detectors.end();
}
}


