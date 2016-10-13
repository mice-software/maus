#include "Geant4/G4Material.hh"

#include "gtest/gtest.h"
#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Global/MaterialModelDynamic.hh"
#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"

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

        MaterialModel::EnableMaterial("Galactic");
        MaterialModel::EnableMaterial("lH2");
        MaterialModel::EnableMaterial("POLYSTYRENE");
        MaterialModel::EnableMaterial("LITHIUM_HYDRIDE");
    }

    virtual ~MaterialModelTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}

    double _mass = 105.658;
    std::vector<double> _nist_energy;
};

TEST_F(MaterialModelTest, dEdxHydrogen) {
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

TEST_F(MaterialModelTest, dEdxPolystyrene) {
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

TEST_F(MaterialModelTest, dEdxDisabled) {
    MaterialModel::DisableMaterial("lH2");

    MaterialModelDynamic material(0., 0., 0.);
    EXPECT_NEAR(material.dEdx(226., _mass, 1.), 0., 1e-9);
    MaterialModel::EnableMaterial("lH2");
}

TEST_F(MaterialModelTest, AxialLookupGetBounds) {
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

TEST_F(MaterialModelTest, EnableDisableMaterial) {
    MaterialModelAxialLookup::BuildLookupTable(-1000., 4000.);
    MaterialModelAxialLookup test_axial;
    MaterialModelDynamic test_dyn;
    G4Material* null = NULL;
    // before lower bound; should default to first material (G4_Galactic)
    test_dyn.SetMaterial(0., 0., 0.);
    test_axial.SetMaterial(0., 0., 0.);
    EXPECT_NE(test_dyn.GetMaterial(), null);
    EXPECT_NE(test_axial.GetMaterial(), null);

    MaterialModel::DisableMaterial("lH2");

    test_dyn.SetMaterial(0., 0., 0.);
    test_axial.SetMaterial(0., 0., 0.);
    EXPECT_EQ(test_dyn.GetMaterial(), null);
    EXPECT_EQ(test_axial.GetMaterial(), null);

    MaterialModel::EnableMaterial("lH2");

    test_dyn.SetMaterial(0., 0., 0.);
    test_axial.SetMaterial(0., 0., 0.);
    EXPECT_NE(test_dyn.GetMaterial(), null);
    EXPECT_NE(test_axial.GetMaterial(), null);
}

TEST_F(MaterialModelTest, AxialLookupSetMaterial) {
    MaterialModelAxialLookup::BuildLookupTable(1000., 4000.);
    MaterialModelAxialLookup test;
    G4Material* null = NULL;
    // before lower bound; should default to first material (G4_Galactic)
    test.SetMaterial(0., 0., 0.);
    ASSERT_NE(test.GetMaterial(), null);
    EXPECT_EQ(test.GetMaterial()->GetName(), "G4_Galactic");

    // boundary to polystyrene
    test.SetMaterial(0., 0., 1749.998);
    ASSERT_NE(test.GetMaterial(), null);
    EXPECT_EQ(test.GetMaterial()->GetName(), "G4_Galactic");
    test.SetMaterial(0., 0., 1750.002);
    ASSERT_NE(test.GetMaterial(), null);
    EXPECT_EQ(test.GetMaterial()->GetName(), "G4_POLYSTYRENE");

    // boundary to LiH
    test.SetMaterial(0., 0., 3749.998);
    ASSERT_NE(test.GetMaterial(), null);
    EXPECT_EQ(test.GetMaterial()->GetName(), "G4_Galactic");
    test.SetMaterial(0., 0., 3750.002);
    ASSERT_NE(test.GetMaterial(), null);
    EXPECT_EQ(test.GetMaterial()->GetName(), "G4_LITHIUM_HYDRIDE");

    // after upper bound; should default to last material (LiH)
    test.SetMaterial(0., 0., 5000.);
    ASSERT_NE(test.GetMaterial(), null);
    EXPECT_EQ(test.GetMaterial()->GetName(), "G4_LITHIUM_HYDRIDE");
}

TEST_F(MaterialModelTest, needModeTests) {
    EXPECT_TRUE(false) << "Need more tests";
}

}
