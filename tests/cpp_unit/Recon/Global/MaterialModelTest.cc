#include "Geant4/G4NistManager.hh"

#include "gtest/gtest.h"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"

namespace MAUS {

class MaterialModelTest : public ::testing::Test {
  protected:
    MaterialModelTest()  {
        G4NistManager* manager = G4NistManager::Instance();
//        _galactic.SetMaterial(manager->FindOrBuildMaterial("G4_Galactic"));
        _hydrogen.SetMaterial(manager->FindOrBuildMaterial("G4_lH2"));
//        _polystyrene.SetMaterial(manager->FindOrBuildMaterial("G4_POLYSTYRENE"));
//        _lead.SetMaterial(manager->FindOrBuildMaterial("G4_Pb"));      
    }

    virtual ~MaterialModelTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}

    MaterialModel _galactic;
    MaterialModel _hydrogen;
    MaterialModel _polystyrene;
    MaterialModel _lead;
    double _mass = 105.658;
};

TEST_F(MaterialModelTest, dEdxHydrogen) {
    EXPECT_NEAR(_hydrogen.dEdx( 130.0, _mass, 1.), -20./350., 1.0e-7);
    EXPECT_NEAR(_hydrogen.dEdx( 230.0, _mass, 1.), -15./350., 1.0e-7);
}

TEST_F(MaterialModelTest, dtheta2dxHydrogen) {
    EXPECT_NEAR(_hydrogen.dtheta2dx( 130.0, _mass, 1.), 4.7386302422092031e-05, 1.0e-7);
    EXPECT_NEAR(_hydrogen.dtheta2dx( 230.0, _mass, 1.), 2.8463355575589116e-05, 1.0e-7);
}

TEST_F(MaterialModelTest, estrag2) {
    EXPECT_NEAR(_hydrogen.estrag2(230.0, _mass, 1.), 0.04404808285535615, 1.0e-7);
}



}
