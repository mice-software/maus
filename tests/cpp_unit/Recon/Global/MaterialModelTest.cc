#include "gtest/gtest.h"
#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Global/MaterialModelDynamic.hh"
#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"

namespace MAUS {

class MaterialModelDynamicTest : public ::testing::Test {
  protected:
    MaterialModelDynamicTest()  {
        std::string mod_path = getenv("MAUS_ROOT_DIR");
        mod_path += "/tests/cpp_unit/Recon/Global/TestGeometries/";
        MiceModule* materials = new MiceModule(mod_path+"MaterialModelTest.dat");
        GlobalsManager::SetMonteCarloMiceModules(materials);
        auto nist_t = {60., 70., 80., 90., 100., 120., 140., 170., 200.};
        _nist_energy = std::vector<double>(nist_t);
    }

    virtual ~MaterialModelDynamicTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}

    double _mass = 105.658;
    std::vector<double> _nist_energy;
};

TEST_F(MaterialModelDynamicTest, dEdxHydrogen) {
    MaterialModelDynamic material(0., 0., 0.);
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

TEST_F(MaterialModelDynamicTest, dEdxPolystyrene) {
    MaterialModelDynamic material(0., 0., 2000.);
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

TEST_F(MaterialModelDynamicTest, AxialLookup) {
    MaterialModelAxialLookup::BuildLookupTable(-1000., 4000.);
    MaterialModelAxialLookup::PrintLookupTable(std::cerr);
    double lower, upper;
    MaterialModelAxialLookup::GetBounds(-1500., lower, upper);
    EXPECT_NEAR(lower, -1500., 0.1);
    EXPECT_NEAR(upper, -1000., 0.1);
    MaterialModelAxialLookup::GetBounds(-500., lower, upper);
    EXPECT_NEAR(lower, -1000., 0.1);
    EXPECT_NEAR(upper, -250., 0.1);
    MaterialModelAxialLookup::GetBounds(3500., lower, upper);
    EXPECT_NEAR(lower, 2250., 0.1);
    EXPECT_NEAR(upper, 3750., 0.1);
    MaterialModelAxialLookup::GetBounds(4000., lower, upper);
    EXPECT_NEAR(lower, 3750., 0.1);
    EXPECT_NEAR(upper, 4000., 0.1);
}

TEST_F(MaterialModelDynamicTest, needModeTests) {
    EXPECT_TRUE(false) << "Need more tests";
}

}
