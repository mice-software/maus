#include "gtest/gtest.h"
#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"

namespace MAUS {

class MaterialModelTest : public ::testing::Test {
  protected:
    MaterialModelTest()  {
        std::string mod_path = getenv("MAUS_ROOT_DIR");
        mod_path += "/tests/cpp_unit/Recon/Global/TestGeometries/";
        MiceModule* materials = new MiceModule(mod_path+"MaterialModelTest.dat");
        GlobalsManager::SetMonteCarloMiceModules(materials);
        auto nist_t = {60., 70., 80., 90., 100., 120., 140., 170., 200.};
        _nist_energy = std::vector<double>(nist_t);
    }

    virtual ~MaterialModelTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}

    double _mass = 105.658;
    std::vector<double> _nist_energy;
};

TEST_F(MaterialModelTest, dEdxHydrogen) {
    MaterialModel material(0., 0., 0.);
    auto dedx_auto = {5.409, 5.097, 4.870, 4.699, 4.568,
                      4.385, 4.267, 4.161, 4.104};  // MeV cm^2/g
    std::vector<double> pdg_dedx(dedx_auto);
    double pdg_density = 0.0708;  // g/cm^3
    for (size_t i = 0; i < _nist_energy.size(); ++i) {
        double energy = _nist_energy[i] + _mass;
        std::cerr << _nist_energy[i] << "  "
                  << material.dEdx(energy, _mass, 1.) << "  "
                  << pdg_dedx[i]*pdg_density/cm << std::endl;
    }
}

TEST_F(MaterialModelTest, dEdxPolystyrene) {
    MaterialModel material(0., 0., 2000.);
    auto dedx_auto = {2.612, 2.466, 2.359, 2.276, 2.211,
                     2.118, 2.058, 2.003, 1.971};  // MeV cm^2/g
    std::vector<double> pdg_dedx(dedx_auto);
    double pdg_density = 1.060;  // g/cm^3
    for (size_t i = 0; i < _nist_energy.size(); ++i) {
        double energy = _nist_energy[i] + _mass;
        std::cerr << _nist_energy[i] << "  "
                  << material.dEdx(energy, _mass, 1.) << "  "
                  << pdg_dedx[i]*pdg_density/cm << std::endl;
    }
}


}
