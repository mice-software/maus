#include <algorithm>

#include "TMatrixD.h" // ROOT

#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFEvent.hh"

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"

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


void DataLoader::load_event(ReconEvent& event) {
    std::cerr << "DataLoader::load_event " << &event << std::endl;
    _all_data = Kalman::Track(_dimension);
    _fit_data = Kalman::Track(_dimension);
    TOFEvent* tof_event = event.GetTOFEvent();
    SciFiEvent* scifi_event = event.GetSciFiEvent();
    load_tof_event(tof_event);
    load_scifi_event(scifi_event);
}

Kalman::State DataLoader::load_tof_space_point(TOFSpacePoint* sp) {
    double pos_arr[] = {sp->GetTime(),
                    sp->GetGlobalPosX(),
                    sp->GetGlobalPosY(), 0., 0., 0.};
    double cov_arr[] = {
        0.06, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0,  0.4, 0.0, 0.0, 0.0, 0.0,
        0.0,  0.0, 0.4, 0.0, 0.0, 0.0,
        0.0,  0.0, 0.0, _err,0.0, 0.0,
        0.0,  0.0, 0.0, 0.0, _err,0.0,
        0.0,  0.0, 0.0, 0.0, 0.0, _err,
    };
    TMatrixD vector(6, 1, pos_arr);
    TMatrixD covariance(6, 6, cov_arr);
    Kalman::State state(vector, covariance, sp->GetGlobalPosZ());
    return state;
}

void DataLoader::load_tof_event(TOFEvent* event) {
    std::cerr << "DataLoader::load_tof_event " << event << std::endl;
    TOFEventSpacePoint space_points = event->GetTOFEventSpacePoint();

    // Load TOF0
    for (size_t i = 0; i < space_points.GetTOF0SpacePointArray().size(); ++i) {
        TOFSpacePoint tof0_sp = space_points.GetTOF0SpacePointArray()[i];
        Kalman::State state = load_tof_space_point(&tof0_sp);
        _all_data.Append(state);
        if (std::find(_detectors.begin(), _detectors.end(), kTOF0) != _detectors.end() && i == 0) {
            _fit_data.Append(state);
        }
    }

    // Load TOF1
    for (size_t i = 0; i < space_points.GetTOF1SpacePointArray().size(); ++i) {
        TOFSpacePoint tof1_sp = space_points.GetTOF1SpacePointArray()[i];
        Kalman::State state = load_tof_space_point(&tof1_sp);
        _all_data.Append(state);
        if (std::find(_detectors.begin(), _detectors.end(), kTOF1) != _detectors.end() && i == 0) {
            _fit_data.Append(state);
        }
    }

    // Load TOF2
    for (size_t i = 0; i < space_points.GetTOF2SpacePointArray().size(); ++i) {
        TOFSpacePoint tof2_sp = space_points.GetTOF2SpacePointArray()[i];
        Kalman::State state = load_tof_space_point(&tof2_sp);
        _all_data.Append(state);
        if (std::find(_detectors.begin(), _detectors.end(), kTOF2) != _detectors.end() && i == 0) {
            _fit_data.Append(state);
        }
    }
    std::cerr << "DataLoader::load_tof_event end - size " << _fit_data.GetLength() << std::endl;
}

Kalman::State DataLoader::load_scifi_space_point(SciFiSpacePoint* sp) {
    double pos_arr[] = {0.,
                    sp->get_global_position().x(),
                    sp->get_global_position().y(), 0., 0., 0.};
    double cov_arr[] = {
        _err,0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.4, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.4, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, _err,0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, _err,0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, _err,
    };
    TMatrixD vector(6, 1, pos_arr);
    TMatrixD covariance(6, 6, cov_arr);
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
