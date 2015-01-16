#include "gtest/gtest.h"

#include "rogers_math/PolynomialVector.hh"
#include "rogers_math/PolynomialPatch.hh"

typedef PolynomialVector::PolynomialCoefficient PolyCoeff;

// NEEDS MORE HERE
TEST(PolynomialPatchTest, TestConstructorDestructor) {
    Mesh* mesh = NULL;
    std::vector<PolynomialVector*> poly;
    PolynomialPatch();

    try {
        PolynomialPatch(NULL, std::vector<PolynomialVector*>());
        EXPECT_TRUE(false);
    } catch (MAUS::Exception exc) {
    }
    EXPECT_TRUE(false) << "Need more tests here";
}

double check(PolynomialPatch* patch,
             Mesh::Iterator it,
             double x_offset,
             double y_offset) {
    std::vector<double> pos = it.Position();
    pos[0] += x_offset;
    pos[1] += y_offset;
    double value[] = {0.};
    patch->F(&pos[0], value);
    return value[0];
}

// Make a two d patch of constant polynomials; check that we can call F() ok
TEST(PolynomialPatchTest, TestF) {
    ASSERT_TRUE(false);
    TwoDGrid* grid = new TwoDGrid(1., 2., -5., -10., 3, 3);
    std::vector<PolynomialVector*> polynomials;
    for (Mesh::Iterator it = grid->Begin(); it < grid->End(); ++it) {
        double it_index = grid->ToInteger(it);
        polynomials.push_back(new PolynomialVector(2, MMatrix<double>(1, 1, it_index)));
    }
    PolynomialPatch* patch = new PolynomialPatch(grid, polynomials);

    for (Mesh::Iterator it = grid->Begin(); it < grid->End(); ++it) {
        EXPECT_DOUBLE_EQ(it.ToInteger(), check(patch, it, 0., 0.));
        EXPECT_DOUBLE_EQ(it.ToInteger(), check(patch, it, -0.49, -0.99));
        EXPECT_DOUBLE_EQ(it.ToInteger(), check(patch, it, 0.49, 0.99));
        if (it.ToInteger() != 8)
            EXPECT_GT(fabs(it.ToInteger()-check(patch, it, 0.51, 1.01)), 0.1);
        if (it.ToInteger() != 0)
            EXPECT_GT(fabs(it.ToInteger()-check(patch, it, -0.51, -1.01)), 0.1);
    }

    delete patch;
}

int mag_sq(std::vector<int> mag) {
    int sum = 0;
    for (size_t i = 0; i < mag.size(); ++i) 
      sum += mag[i]*mag[i];
    return sum;
}

TEST(PolynomialPatchTest, TestNearbyPoints) {
    std::vector< std::vector<int> > pts =
                                    PolynomialPatch::GetNearbyPoints(4, 3, 100);
    EXPECT_EQ(pts.size(), 20);
    for (size_t i = 0; i < pts.size(); ++i) {
        EXPECT_EQ(pts[i].size(), 4);
    }
    for (size_t i = 1; i < pts.size(); ++i) 
        EXPECT_GE(mag_sq(pts[i]), mag_sq(pts[i-1]));
    for (size_t i = 0; i < pts.size(); ++i) {
        for (size_t j = 0; j < pts[i].size(); ++j)
            std::cerr << pts[i][j] << " ";
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
}

TEST(PolynomialPatchTest, TestLeastSquaresFitTwoD) {
    TwoDGrid* grid = new TwoDGrid(1., 2., -5., -10., 10, 10);
    std::vector<std::vector<double> > values;
    for (Mesh::Iterator it = grid->Begin(); it < grid->End(); ++it) {
        std::vector<double> x = it.Position();
        std::vector<double> a_value(2);
        // y_0 = 1+2 x_0+3 x_0^2
        a_value[0] = 1+2*x[0]+3*x[0]*x[0];
        // y_1 = 4 + 5 x_0 + 6 x_1 + 7 x_0^2 + 8 x_0 x_1 + 9 x_1^2
        a_value[1] = 4+5*x[0]+6*x[1]+7*x[0]*x[0]+8*x[1]*x[0]+9*x[1]*x[1];
        values.push_back(a_value);
    }
    PolynomialPatch* pp1 = PolynomialPatch::LeastSquaresFit(grid, values, 2, 0);
    // check that we return the exact value for the midpoint
    for (Mesh::Iterator it = grid->Begin(); it < grid->End(); ++it) {
        double val[] = {0., 0.};
        std::vector<double> pos = it.Position();
        pp1->F(&pos[0], val);
        for (size_t j = 0; j < values[it.ToInteger()].size(); ++j)
            EXPECT_NEAR(val[j], values[it.ToInteger()][j], 3);
        continue; // disable printing
        std::cerr << "i " << it.ToInteger() << " x0: " << pos[0] << " x1: "  << pos[1]
                  << " y0_m: " << val[0] << " y1_m: "  << val[1]
                  << " y0_t: " << values[it.ToInteger()][0] << " y1_t: "  << values[it.ToInteger()][1] << std::endl;
    }
    Mesh::Iterator begin = grid->Begin();    
    Mesh::Iterator last = grid->End()-1;
    for (Mesh::Iterator it = begin; it < grid->End(); ++it) {
        for (size_t j = 0; j < it.State().size(); ++j) {
            for (int delta_step = -1; delta_step < 2; delta_step += 2) {
                Mesh::Iterator it_delta = it;
                it_delta[j] += delta_step;
                if (it_delta < begin || it_delta > last)
                    continue; // it_delta is off the mesh buondary, try again
                std::vector<double> position = it.Position();
                // midpoint between it and it_delta
                double delta = (it.Position()[j]-it_delta.Position()[j]);
                position[j] += (0.5+1e-6)*delta;
                double valPos[] = {0., 0.};
                pp1->F(&position[0], valPos);
                PolynomialVector* poly1 = pp1->GetPolynomialVector(&position[0]);
                position[j] -= 2e-6*delta;
                double valNeg[] = {0., 0.};
                pp1->F(&position[0], valNeg);
                PolynomialVector* poly2 = pp1->GetPolynomialVector(&position[0]);
                EXPECT_NE(poly1, poly2);
                for (size_t k = 0; k < it_delta.State().size(); ++k)
                    EXPECT_NEAR(valPos[k], valNeg[k], 6);
                //continue;
                std::cerr << "Step on " << j << " axis by " << delta_step;
                for (size_t k = 0; k < it_delta.State().size(); ++k)
                    std::cerr << " x" << k << " " << position[k];
                for (size_t k = 0; k < 2; ++k)
                    std::cerr << " y" << k << "p " << valPos[k];
                for (size_t k = 0; k < 2; ++k)
                    std::cerr << " y" << k << "n " << valNeg[k];
                std::cerr << " " << poly1 << " " << poly2 << std::endl;
            }
        }
    }
    //PolynomialPatch::LeastSquaresFit(grid, values, 3, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Running tests" << std::endl;
    int test_out = RUN_ALL_TESTS();
    return test_out;
}

