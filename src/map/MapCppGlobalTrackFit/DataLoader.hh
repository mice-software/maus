#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {
class SciFiEvent;
class TOFEvent;
namespace Kalman {
  class GlobalMeasurement;
}

class DataLoader {
  public:
    DataLoader(std::vector<MAUS::DataStructure::Global::DetectorPoint> detectors);
    ~DataLoader() {}

    /** Load events from event
     */
    void load_event(ReconEvent& event, Kalman::GlobalMeasurement* measurement);

    /** Load events from event; detectors is a vector of detectors active for
     *  reconstruction
     */
    void load_global_event(ReconEvent& event);

    Kalman::Track get_fit_data() const {return _fit_data;}
    Kalman::Track get_all_data() const {return _all_data;}

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

    Kalman::Track _all_data;
    Kalman::Track _fit_data;
    std::vector<MAUS::DataStructure::Global::DetectorPoint> _detectors;
    bool _will_require_tracker_triplets;
    // Maps State.GetId() to the detector that made the measurement
    std::map<int, MAUS::DataStructure::Global::DetectorPoint> _state_to_detector_map;

    static size_t _dimension;
    static double _err;
};
}


