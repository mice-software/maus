#include <time.h>

#include "gtest/gtest.h"

#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTracking.hh"

void just_run(MAUS::Kalman::Global::ErrorTracking& propagator, double* x_in) {
        propagator.Propagate(&x_in[0], 21000.);
}

namespace MAUS {

std::vector<double> ellipse(double x, double y, double z,
                            double px, double py, double pz, double mass) {
    double energy = ::sqrt(px*px+py*py+pz*pz+mass*mass);
    double x_in[] = {
          0., x, y, z, energy, px, py, pz,
          1., 0., 0., 0., 0., 0.,
              1., 0., 0., 0., 0.,
                  1., 0., 0., 0.,
                      1., 0., 0.,
                          1., 0.,
                              1.
    };
    return std::vector<double>(x_in, x_in+29);

}


// This test is included for profiling and checking using e.g. GDML geometry
// Is not intended to run normally (MAUS attempts to load the geometry, finds it
// does not exist and bails)
TEST(ErrorTrackingRealGeometryTest, PropagateTrackTest) {
    clock_t t0, t1, t2;
    t0 = clock();
    typedef Kalman::Global::ErrorTracking ErrorTracking;

    std::string mod_name = getenv("MAUS_ROOT_DIR");
    mod_name += "/tests/cpp_unit/Recon/Kalman/geometry_07469/";
    mod_name += "ParentGeometryFile.dat";
    MiceModule* mod = NULL;
    try {
        mod = new MiceModule(mod_name);
    } catch (MAUS::Exception exc) {
        std::cerr << "Didn't find a real geometry - this is normal so we skip "
                  << "ErrorTrackingRealGeometryTest" << std::endl;
        return;
    }

    MaterialModel::EnableMaterial("G4_Pb");
    MaterialModel::EnableMaterial("AIR");
    MaterialModel::EnableMaterial("POLYSTYRENE");
    MaterialModel::EnableMaterial("POLYCARBONATE");
    MaterialModel::EnableMaterial("POLYVINYL_TOLUENE");
    MaterialModel::EnableMaterial("POLYVINYL_ACETATE");
    MaterialModel::EnableMaterial("AEROGEL_112a");
    MaterialModel::EnableMaterial("AEROGEL_107a");
    MaterialModel::EnableMaterial("Al");
    MaterialModel::EnableMaterial("CELLULOSE_CELLOPHANE");
    MaterialModel::EnableMaterial("POLYVINYL_CHLORIDE_LOWD");
    MaterialModel::EnableMaterial("POLYVINYL_CHLORIDE");
    MaterialModel::EnableMaterial("He");

    // globals owns mod
    GlobalsManager::SetMonteCarloMiceModules(mod);
    double mass = 105.658;

    ErrorTracking propagator;
    propagator.SetMinStepSize(10.);
    propagator.SetMaxStepSize(100.);
    propagator.SetDeviations(0.001, 0.001, 0.001, 0.001);
    /*
    propagator.SetEnergyLossModel(ErrorTracking::bethe_bloch_forwards);
    propagator.SetMCSModel(ErrorTracking::moliere_forwards);
    propagator.SetTrackingModel(ErrorTracking::em_forwards_dynamic);
    propagator.SetEStragModel(ErrorTracking::no_estrag);
    */
    propagator.SetEnergyLossModel(ErrorTracking::no_eloss);
    propagator.SetMCSModel(ErrorTracking::no_mcs);
    propagator.SetTrackingModel(ErrorTracking::em_forwards_dynamic);
    propagator.SetEStragModel(ErrorTracking::no_estrag);
    t1 = clock();
    std::cerr << "Propagating dynamic no materials" << std::flush;
    for (size_t i = 0; i < 20; ++i) {
        std::cerr << i << " " << std::flush;
        std::vector<double> x_in = ellipse(0., 0., 13000., 1., 0., 200., mass);
        propagator.Propagate(&x_in[0], 21000.);
    }
    std::cerr << std::endl;
    t2 = clock();
    propagator.SetTrackingModel(ErrorTracking::em_forwards_static);
    std::cerr << "Propagating static no materials" << std::flush;
    for (size_t i = 0; i < 20; ++i) {
        std::cerr << i << " " << std::flush;
        std::vector<double> x_in = ellipse(0., 0., 13000., 1., 0., 200., mass);
        propagator.Propagate(&x_in[0], 21000.);
    }
    std::cerr << std::endl;
    t3 = clock();
    propagator.SetTrackingModel(ErrorTracking::em_forwards_static);
    std::cerr << "Set up time                   " << (float)(t1-t0)/CLOCKS_PER_SEC << " s" << std::endl;
    std::cerr << "Run time dynamic no materials " << (float)(t2-t1)/CLOCKS_PER_SEC << " s" << std::endl;
    std::cerr << "Run time static no materials  " << (float)(t3-t2)/CLOCKS_PER_SEC << " s" << std::endl;
}

}
