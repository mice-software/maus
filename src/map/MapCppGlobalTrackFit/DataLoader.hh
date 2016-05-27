#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {
class SciFiEvent;
class TOFEvent;

class DataLoader {
  public:
    /** fit is a borrowed reference (caller owns the memory) */
    DataLoader(std::vector<MAUS::DataStructure::Global::DetectorPoint> detectors,
               Kalman::TrackFit* fit);
    ~DataLoader() {}

    /** Load events from event
     */
    void load_event(ReconEvent& event);

    /** Load events from event; detectors is a vector of detectors active for
     *  reconstruction
     */
    void load_global_event(ReconEvent& event);

    void SetWillRequireTrackerTriplets(bool will_require) {_will_require_tracker_triplets = will_require;}
    bool GetWillRequireTrackerTriplets() {return _will_require_tracker_triplets;}
  private:
    void load_virtual_event();
    void load_tof_event(TOFEvent* event);
    void load_tof_detector(const MAUS::TOF0SpacePointArray& sp_vector,
                           MAUS::DataStructure::Global::DetectorPoint det);
    Kalman::State load_tof_space_point(TOFSpacePoint* sp);
    void load_scifi_event(SciFiEvent* event);
    Kalman::State load_scifi_space_point(SciFiSpacePoint* sp);
    void validate_detectors();
    MAUS::DataStructure::Global::DetectorPoint SciFiToDetectorPoint(int tracker, int station) const;

    Kalman::TrackFit* _fitter;
    std::vector<MAUS::DataStructure::Global::DetectorPoint> _detectors;
    bool _will_require_tracker_triplets;
    // Maps State.GetId() to the detector that made the measurement

    static size_t _dimension;
    static double _err;
};
}


