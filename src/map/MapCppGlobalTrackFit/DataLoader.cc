#include <algorithm>

#include "TMatrixD.h" // ROOT

#include "src/common_cpp/Recon/Kalman/GlobalMeasurement.hh"

#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFEvent.hh"

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"

// These includes are for virtual planes:
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"

#include "src/map/MapCppGlobalTrackFit/DataLoader.hh"

using namespace MAUS::DataStructure::Global;

namespace MAUS {
size_t DataLoader::_dimension = 6;
double DataLoader::_err = 1e6;

DataLoader::DataLoader(std::vector<MAUS::DataStructure::Global::DetectorPoint> detectors)
  :_all_data(_dimension), _fit_data(_dimension), _detectors(detectors) {
    std::sort(_detectors.begin(), _detectors.end());
    validate_detectors();
}


void DataLoader::load_event(ReconEvent& event, Kalman::GlobalMeasurement* measurement) {
    std::cerr << "DataLoader::load_event " << &event << std::endl;
    _state_to_detector_map =
                  std::map<int, MAUS::DataStructure::Global::DetectorPoint>();
    _all_data = Kalman::Track(_dimension);
    _fit_data = Kalman::Track(_dimension);
    TOFEvent* tof_event = event.GetTOFEvent();
    SciFiEvent* scifi_event = event.GetSciFiEvent();
    load_tof_event(tof_event);
    load_scifi_event(scifi_event);
    load_virtual_event();
    measurement->SetStateToDetectorMap(_state_to_detector_map);
}

Kalman::State DataLoader::load_tof_space_point(TOFSpacePoint* sp) {
    double pos_arr[] = {sp->GetTime()};
    double cov_arr[] = {
      0.05
    };
    TMatrixD vector(1, 1, pos_arr);
    TMatrixD covariance(1, 1, cov_arr);
    Kalman::State state(vector, covariance, sp->GetGlobalPosZ());
    return state;
}

void DataLoader::load_tof_detector(const MAUS::TOF0SpacePointArray& sp_vector,
                               MAUS::DataStructure::Global::DetectorPoint det) {
    for (size_t i = 0; i < sp_vector.size(); ++i) {
        TOFSpacePoint one_sp = sp_vector.at(i);
        Kalman::State state = load_tof_space_point(&one_sp);
        _all_data.Append(state);
        state.SetId(_all_data.GetLength());
        _state_to_detector_map[state.GetId()] = det;
        if (std::find(_detectors.begin(), _detectors.end(), det) != _detectors.end() && i == 0) {
            _fit_data.Append(state);
        }
    }

}

void DataLoader::load_tof_event(TOFEvent* event) {
    std::cerr << "DataLoader::load_tof_event " << event << std::endl;
    TOFEventSpacePoint space_points = event->GetTOFEventSpacePoint();

    // Load TOF0
    load_tof_detector(space_points.GetTOF0SpacePointArray(), kTOF0);
    // Load TOF1
    load_tof_detector(space_points.GetTOF1SpacePointArray(), kTOF1);
    // Load TOF2
    load_tof_detector(space_points.GetTOF2SpacePointArray(), kTOF2);

    std::cerr << "DataLoader::load_tof_event end - size " << _fit_data.GetLength() << std::endl;
}

void DataLoader::load_virtual_event() {
    VirtualPlaneManager* virtual_manager = Globals::GetGeant4Manager()->GetVirtualPlanes();
    std::vector<VirtualPlane*> planes = virtual_manager->GetPlanes();
    bool will_add_to_fit_data = std::find(_detectors.begin(), _detectors.end(), kVirtual) != _detectors.end();

    for (size_t i = 0; i < planes.size(); ++i) {
        if (planes[i]->GetPlaneIndependentVariableType() == BTTracker::z) {
            double z_pos = planes[i]->GetPlaneIndependentVariable();
            Kalman::State state(0, z_pos);
            state.SetId(_all_data.GetLength());
            _all_data.Append(state);
            _state_to_detector_map[state.GetId()] = kVirtual;
            if (will_add_to_fit_data)
                _fit_data.Append(state);
        }
    }
}

Kalman::State DataLoader::load_scifi_space_point(SciFiSpacePoint* sp) {
    double pos_arr[] = {
                    sp->get_global_position().x(),
                    sp->get_global_position().y()};
    double cov_arr[] = {
      0.4, 0.0,
      0.0, 0.4,
    };
    TMatrixD vector(2, 1, pos_arr);
    TMatrixD covariance(2, 2, cov_arr);
    Kalman::State state(vector, covariance, sp->get_global_position().z());
    return state;
}

DetectorPoint DataLoader::SciFiToDetectorPoint(int tracker, int station) const {
    switch (tracker) {
        case 0:
            switch (station) {
                case 1:
                    return kTracker0_1;
                case 2:
                    return kTracker0_2;
                case 3:
                    return kTracker0_3;
                case 4:
                    return kTracker0_4;
                case 5:
                    return kTracker0_5;
                default:
                    throw MAUS::Exception(MAUS::Exception::recoverable, "Bad station id", "DataLoader::SciFiToDetectorPoint");
            }
            break;
        case 1:
            switch (station) {
                case 1:
                    return kTracker1_1;
                case 2:
                    return kTracker1_2;
                case 3:
                    return kTracker1_3;
                case 4:
                    return kTracker1_4;
                case 5:
                    return kTracker1_5;
                default:
                    throw MAUS::Exception(MAUS::Exception::recoverable, "Bad station id", "DataLoader::SciFiToDetectorPoint");
            }
            break;
        default:
            throw MAUS::Exception(MAUS::Exception::recoverable, "Bad tracker id", "DataLoader::SciFiToDetectorPoint");
    }
}

void DataLoader::load_scifi_event(SciFiEvent* event) {
    std::cerr << "DataLoader::load_scifi_event" << std::endl;
    SciFiSpacePointPArray space_points = event->spacepoints();
    for (size_t i = 0; i < space_points.size(); ++i) {
        std::cerr << "    LoadSciFiEvent " << i << std::flush;
        if (_will_require_tracker_triplets && space_points[i]->get_channels()->GetEntries() != 3) {
            std::cerr << " failed triplet check " << space_points[i]->get_channels()->GetEntries() << std::endl;
            continue;
        }
        std::cerr << " passed triplet check" << std::endl;
        Kalman::State state = load_scifi_space_point(space_points[i]);
        _all_data.Append(state);
        DetectorPoint point = SciFiToDetectorPoint(space_points[i]->get_tracker(), space_points[i]->get_station());        
        state.SetId(_all_data.GetLength());
        _state_to_detector_map[state.GetId()] = point;
        if (std::find(_detectors.begin(), _detectors.end(), point) != _detectors.end()) {
            _fit_data.Append(state);
        }
    }
}

void DataLoader::validate_detectors() {
    auto det_list = {
      kTOF0, kTOF1, kTOF2,
      kTracker0_1, kTracker0_2, kTracker0_3, kTracker0_4, kTracker0_5,
      kTracker1_1, kTracker1_2, kTracker1_3, kTracker1_4, kTracker1_5,
      kVirtual,
    };
    std::vector<DetectorPoint> det_vector(det_list);
    for (size_t i = 0; i < _detectors.size(); ++i) {
        if (std::find(det_vector.begin(), det_vector.end(), _detectors[i]) == det_vector.end()) {
            throw MAUS::Exception(MAUS::Exception::recoverable,
                                  "Global track fit detector type not implemented",
                                  "DataLoader::validate_detectors");            
        }
    }
}
}
