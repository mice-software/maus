#include "TMatrixD.h"

#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/map/MapCppGlobalTrackFit/DataLoader.hh"
#include "src/map/MapCppGlobalTrackFit/SeedBase.hh"
#include "src/common_cpp/Recon/Kalman/Global/Propagator.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

namespace MAUS {


const double SeedBase::mu_mass = 105.658; // BUG
const double SeedBase::c_light = 300.; // BUG

void SeedManager::RegisterAlgorithm(std::string name, SeedBase* algorithm) {
    if (algorithm == NULL) {
        iterator it;
        it = _seed_algorithms.find(name);
        if (it != _seed_algorithms.end()) {
            delete it->second;
            _seed_algorithms.erase(it);
        } else {
            throw MAUS::Exception(Exception::recoverable,
                  "Registering NULL to name "+name+
                  " - normally this removes an entry, but no entry was found",
                  "SeedBase::RegisterAlgorithm");
        }
    } else {
      _seed_algorithms[name] = algorithm;
    }
}

void SeedManager::RegisterAll() {
    RegisterAlgorithm("USTrackerTofSeed", new USTrackerTofSeed());
    RegisterAlgorithm("Tof01Seed", new Tof01Seed());
}

SeedManager::~SeedManager() {
    for (iterator it = _seed_algorithms.begin(); it != _seed_algorithms.end(); ++it) {
        delete it->second;
    }
}


Kalman::State USTrackerTofSeed::GetSeed(ReconEvent* reco_event) {
    using namespace MAUS::DataStructure::Global;
    auto tof_seed_list = {kTOF1, kTOF2};
    std::vector<DetectorPoint> tof_seed_points(tof_seed_list);
    MAUS::Kalman::TrackFit tof_container(new Kalman::Global::Propagator());
    DataLoader tof_data(tof_seed_points, &tof_container);
    tof_data.load_event(*reco_event);
    Kalman::Track tof_track = tof_container.GetTrack();
    if (tof_track.GetLength() != 2) {
        throw MAUS::Exception(Exception::recoverable,
                              "tof_track was not of length 2",
                              "USTrackerTofSeed::build_seed");
    }
    double tof_dt = tof_track[1].GetData().GetVector()[0][0]-
                    tof_track[0].GetData().GetVector()[0][0];
    double tof_dz = (tof_track[1].GetPosition() -
                     tof_track[0].GetPosition());

    double beta = tof_dz/tof_dt/c_light;
    if (fabs(beta) >= 1.) {
        throw MAUS::Exception(Exception::recoverable,
                              "beta was >= 1",
                              "USTrackerTofSeed::build_seed");
    }
    double gamma = 1/::sqrt(1-beta*beta);
    double seed_pz = mu_mass*beta*gamma;
    if (seed_pz != seed_pz)
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to calculate seed pz (maybe beta > 1?)",
                              "USTrackerTofSeed::build_seed");

    auto pos_seed_list = {kTracker0_5, kTracker0_4};
    std::vector<DetectorPoint> pos_seed_points(pos_seed_list);
    MAUS::Kalman::TrackFit sf_container(new Kalman::Global::Propagator());
    DataLoader sf_data(pos_seed_points, &sf_container);
    sf_data.SetWillRequireTrackerTriplets(false);
    sf_data.load_event(*reco_event);
    Kalman::Track sf_track = sf_container.GetTrack();
    if (sf_track.GetLength() != 2) {
        throw MAUS::Exception(Exception::recoverable, "sf_track was not of length 2", "USTrackerTofSeed::build_seed");
    }
    Kalman::State seed_state(6);
    TMatrixD sf_0 = sf_track[0].GetData().GetVector();
    TMatrixD sf_1 = sf_track[1].GetData().GetVector();
    double dx = sf_1[0][0] - sf_0[0][0];
    double dy = sf_1[1][0] - sf_0[1][0];
    double dz = sf_track[1].GetPosition()-sf_track[0].GetPosition();

    TMatrixD vector(6, 1);
    vector[0][0] = tof_dt/tof_dz*(sf_track[0].GetPosition()-tof_track[0].GetPosition());
    vector[1][0] = sf_0[0][0];
    vector[2][0] = sf_0[1][0];
    vector[4][0] = dx/dz*seed_pz;
    vector[5][0] = dy/dz*seed_pz;
    vector[3][0] = ::sqrt(seed_pz*seed_pz + vector[4][0]*vector[4][0] +
                          vector[5][0]*vector[5][0] + mu_mass*mu_mass);
    seed_state.SetVector(vector);
    double err = 1e6;
    double errors[] = {
        err,  0.,  0.,  0.,  0., 0.,
         0., err,  0.,  0.,  0., 0.,
         0.,  0., err,  0.,  0., 0.,
         0.,  0.,  0., err,  0., 0.,
         0.,  0.,  0.,  0., err, 0.,
         0.,  0.,  0.,  0.,  0., err,
    };
    TMatrixD covariance(6, 6, errors);
    seed_state.SetCovariance(covariance);
    return seed_state;
}

Kalman::State Tof01Seed::GetSeed(ReconEvent* reco_event) {
    using namespace MAUS::DataStructure::Global;
    auto tof_seed_list = {kTOF1, kTOF2};
    std::vector<DetectorPoint> tof_seed_points(tof_seed_list);
    MAUS::Kalman::TrackFit tof_container(new Kalman::Global::Propagator());
    DataLoader tof_data(tof_seed_points, &tof_container);
    tof_data.load_event(*reco_event);
    Kalman::Track tof_track = tof_container.GetTrack();
    if (tof_track.GetLength() != 2) {
        throw MAUS::Exception(Exception::recoverable,
                              "tof_track was not of length 2",
                              "Tof01Seed::build_seed");
    }
    TMatrixD tof_0 = tof_track[0].GetData().GetVector();
    TMatrixD tof_1 = tof_track[1].GetData().GetVector();

    double tof_dt = tof_0[0][0]-
                    tof_1[0][0];
    double tof_dz = (tof_track[1].GetPosition() -
                     tof_track[0].GetPosition());
    double beta = tof_dz/tof_dt/c_light;
    double dx = 0.;
    double dy = 0.;

    if (fabs(beta) >= 1.) {
        throw MAUS::Exception(Exception::recoverable,
                              "beta was >= 1",
                              "Tof01Seed::build_seed");
    }
    double gamma = 1/::sqrt(1-beta*beta);
    double seed_pz = mu_mass*beta*gamma;
    if (seed_pz != seed_pz)
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to calculate seed pz (maybe beta > 1?)",
                              "Tof01Seed::build_seed");

    Kalman::State seed_state(6);

    TMatrixD vector(6, 1);
    vector[0][0] = tof_0[0][0];
    vector[1][0] = 0.;
    vector[2][0] = 0.;
    vector[4][0] = dx/tof_dz*seed_pz;
    vector[5][0] = dy/tof_dz*seed_pz;
    vector[3][0] = ::sqrt(seed_pz*seed_pz + vector[4][0]*vector[4][0] +
                          vector[5][0]*vector[5][0] + mu_mass*mu_mass);
    seed_state.SetVector(vector);
    double err = 1e6;
    double errors[] = {
        err,  0.,  0.,  0.,  0., 0.,
         0., err,  0.,  0.,  0., 0.,
         0.,  0., err,  0.,  0., 0.,
         0.,  0.,  0., err,  0., 0.,
         0.,  0.,  0.,  0., err, 0.,
         0.,  0.,  0.,  0.,  0., err,
    };
    TMatrixD covariance(6, 6, errors);
    seed_state.SetCovariance(covariance);
    return seed_state;
}

}
