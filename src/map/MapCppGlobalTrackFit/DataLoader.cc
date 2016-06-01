#include <algorithm>

#include "TMatrixD.h" // ROOT

#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"
#include "src/common_cpp/Recon/Kalman/Global/Measurement.hh"

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
using namespace MAUS::Kalman;
using namespace MAUS::Kalman::Global;

namespace MAUS {
size_t DataLoader::_dimension = 6;
double DataLoader::_err = 1e6;

DataLoader::DataLoader(std::vector<MAUS::DataStructure::Global::DetectorPoint> detectors,
                       Kalman::TrackFit* fit)
  : _fitter(fit), _detectors(detectors) {
    if (_fitter == NULL) {
        throw MAUS::Exception(Exception::recoverable,
                              "Could not cope with NULL fit",
                              "MapCppGlobalTrackFit::DataLoader");
    }
    // std::sort(_detectors.begin(), _detectors.end());
    validate_detectors();
}


void DataLoader::load_event(ReconEvent& event) {
    TOFEvent* tof_event = event.GetTOFEvent();
    SciFiEvent* scifi_event = event.GetSciFiEvent();
    // Warning - the order is important... the seed state must correspond to the
    // first element in track
    load_scifi_event(scifi_event);
    load_tof_event(tof_event);
    load_virtual_event();
    set_kalman_track();
}

void DataLoader::set_kalman_track() {
    std::cerr << "set_kalman_track" << std::endl;
    for (size_t i = 0; i < _detectors.size(); ++i) {
        if (_points.find(_detectors[i]) == _points.end())
            continue;
        std::vector<Detection> det = _points[_detectors[i]];
        for (size_t j = 0; j < det.size(); ++j) {
            int tp_id = _fitter->GetTrack().GetLength();
            det[j]._point.SetId(tp_id);
            _fitter->GetTrack().Append(det[j]._point);
            _fitter->AddMeasurement(tp_id, det[j]._meas->Clone());
            std::cerr << "    Append detection " << _detectors[i] << " " << tp_id << std::endl;
        }
    }
    
}

Kalman::State DataLoader::load_tof_space_point(TOFSpacePoint* sp) {
    double pos_arr[] = {sp->GetTime()};
    double cov_arr[] = {
      0.05
    };
    TMatrixD vector(1, 1, pos_arr);
    TMatrixD covariance(1, 1, cov_arr);
    Kalman::State data(vector, covariance);
    return data;
}

void DataLoader::load_tof_detector(const MAUS::TOF0SpacePointArray& sp_vector,
                               MAUS::DataStructure::Global::DetectorPoint det) {
    for (size_t i = 0; i < sp_vector.size(); ++i) {
        TOFSpacePoint one_sp = sp_vector.at(i);
        Kalman::State state(0);
        Measurement_base* meas = NULL;
        if (i == 0 && WillUse(det)) {
            meas = Measurement::GetDetectorToMeasurementMap()[det];
            state = load_tof_space_point(&one_sp);
        } else if (WillUse(kVirtual)) { // treat it as a virtual detector instead
            meas = Measurement::GetDetectorToMeasurementMap()
                                              [DataStructure::Global::kVirtual];
        } else {
            continue;
        }
        double z_position = one_sp.GetGlobalPosZ();
        int meas_dim = state.GetDimension();
        Kalman::TrackPoint track_point(6, meas_dim, z_position, 0);
        track_point.SetData(state);
        AddDetection(meas, track_point, det);
    }
}

void DataLoader::load_tof_event(TOFEvent* event) {
    TOFEventSpacePoint space_points = event->GetTOFEventSpacePoint();

    // Load TOF0
    load_tof_detector(space_points.GetTOF0SpacePointArray(), kTOF0);
    // Load TOF1
    load_tof_detector(space_points.GetTOF1SpacePointArray(), kTOF1);
    // Load TOF2
    load_tof_detector(space_points.GetTOF2SpacePointArray(), kTOF2);
}

void DataLoader::load_virtual_event() {
    if (!WillUse(kVirtual))
        return;
    VirtualPlaneManager* virtual_manager =
                               Globals::GetGeant4Manager()->GetVirtualPlanes();
    std::vector<VirtualPlane*> planes = virtual_manager->GetPlanes();
    Measurement_base* meas = Measurement::GetDetectorToMeasurementMap()
                                          [DataStructure::Global::kVirtual];

    for (size_t i = 0; i < planes.size(); ++i) {
        if (planes[i]->GetPlaneIndependentVariableType() == BTTracker::z) {
            double z_pos = planes[i]->GetPlaneIndependentVariable();
            Kalman::TrackPoint track_point(6, 0, z_pos, 0);
            track_point.SetData(Kalman::State(0));
            AddDetection(meas, track_point, kVirtual);
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
    Kalman::State state(vector, covariance);
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
    SciFiSpacePointPArray space_points = event->spacepoints();
    for (size_t i = 0; i < space_points.size(); ++i) {
        if (_will_require_tracker_triplets && space_points[i]->get_channels()->GetEntries() != 3) {
            continue;
        }
        Kalman::State state(0);
        DetectorPoint det = SciFiToDetectorPoint(space_points[i]->get_tracker(),
                                                 space_points[i]->get_station());       
        Measurement_base* meas = NULL;
        if (WillUse(det)) {
            meas = Measurement::GetDetectorToMeasurementMap()[det];
            state = load_scifi_space_point(space_points[i]);
        } else if (WillUse(kVirtual)) {
            meas = Measurement::GetDetectorToMeasurementMap()
                                              [DataStructure::Global::kVirtual];
        } else {
            continue;
        }
        double z_position = space_points[i]->get_global_position().z();
        int meas_dim = state.GetDimension();
        Kalman::TrackPoint track_point(6, meas_dim, z_position, 0);
        track_point.SetData(state);
        AddDetection(meas, track_point, det);
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

DataLoader::Detection::Detection(Kalman::Measurement_base* meas, Kalman::TrackPoint point)
  : _meas(meas), _point(point) {
}

void DataLoader::AddDetection(Kalman::Measurement_base* meas, Kalman::TrackPoint point, DetectorPoint det) {
    std::cerr << "Detection " << det << std::endl;
    Detection detection(meas, point);
    if (_points.find(det) == _points.end()) {
        _points[det] = std::vector<Detection>(1, detection);
    } else {
        _points[det].push_back(detection);
    }
}

}
