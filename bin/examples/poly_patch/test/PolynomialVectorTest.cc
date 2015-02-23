#include "gtest/gtest.h"

#include "math/SolveFactory.hh"
#include "math/PolynomialVector.hh"

TEST(PolynomialVectorTest, TestMakeDerivVector) {
    double coeffs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    double pos[] = {2, 3};
    PolynomialVector pvec(2, MMatrix<double>(2, 10, &coeffs[0]));
    std::cerr << pvec << std::endl;
    int pow[] = {0, 0};
    double deriv_vec[] = {-99, -99, -99, -99, -99, -99, -99, -99, -99, -99, 0};
    // last element is one past the end, check vector is of correct length
    double ref_deriv[] = {1, 2, 3, 2*2, 2*3, 3*3, 2*2*2, 2*2*3, 3*3*2, 3*3*3, 0};
    pvec.MakeDerivVector(pos, pow, deriv_vec);
    for (int i = 0; i < 11; ++i)
        EXPECT_NEAR(deriv_vec[i], ref_deriv[i], 1e-6)
                                                     << "Failed on index " << i;        
}

TEST(PolynomialVectorTest, TestMakeDerivVector2) {
    double coeffs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    double pos[] = {2, 3};
    PolynomialVector pvec(2, MMatrix<double>(2, 10, &coeffs[0]));
    std::cerr << pvec << std::endl;

    int pow[] = {1, 0};
    double deriv_vec[] = {-99, -99, -99, -99, -99, -99, -99, -99, -99, -99, 0};
    // last element is one past the end, check vector is of correct length
    double ref_deriv[] = {0, 1, 0, 2*2, 3, 0, 3*2*2, 2*2*3, 3*3, 0, 0};
    pvec.MakeDerivVector(pos, pow, deriv_vec);
    for (int i = 0; i < 11; ++i)
        EXPECT_NEAR(deriv_vec[i], ref_deriv[i], 1e-6)
                                                     << "Failed on index " << i;        

}

TEST(PolynomialVectorTest, TestMakeDerivVector3) {
    double coeffs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    double pos[] = {2, 3};
    PolynomialVector pvec(2, MMatrix<double>(2, 10, &coeffs[0]));
    std::cerr << pvec << std::endl;

    int pow[] = {0, 2};
    double deriv_vec[] = {-99, -99, -99, -99, -99, -99, -99, -99, -99, -99, 0};
    // last element is one past the end, check vector is of correct length
    double ref_deriv[] = {0, 0, 0, 0, 0, 2*1, 0, 0, 2*1*2, 3*2*3, 0};
    pvec.MakeDerivVector(pos, pow, deriv_vec);
    for (int i = 0; i < 11; ++i)
        EXPECT_NEAR(deriv_vec[i], ref_deriv[i], 1e-6)
                                                     << "Failed on index " << i;        

}

TEST(PolynomialVectorTest, TestFDeriv) {
    double coeffs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    PolynomialVector pvec(2, MMatrix<double>(2, 10, &coeffs[0]));
    double pos[] = {0., 0.};
    int pow[] = {0, 0};
    double deriv[] = {-99, -99};
    pvec.FDeriv(pos, pow, deriv);
    EXPECT_NEAR(deriv[0], 1, 1e-6);
    EXPECT_NEAR(deriv[1], 2, 1e-6);

    // double ref_deriv[] = {0, 0, 0, 0, 0, 0, 0, 0, 2*1, 0, 0};
    double pos2[] = {2, 3};
    int pow2[] = {1, 2};
    double deriv2[] = {-99, -99};
    pvec.FDeriv(pos2, pow2, deriv2);
    EXPECT_NEAR(deriv2[0], 34, 1e-6);
    EXPECT_NEAR(deriv2[1], 36, 1e-6);
}

TEST(PolynomialVectorTest, TestPolynomialSolveError) {
    EXPECT_TRUE(false) << "Test that we catch bad input to PolynomialSolve";
}


TEST(PolynomialVectorTest, TestPolynomialSolveNoDerivs) {
    std::vector<std::vector<double> > deriv_pos;
    std::vector<std::vector<double> > deriv_values;
    std::vector<std::vector<int> > deriv_indices;
    double coeffs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    PolynomialVector ref_vec(2, MMatrix<double>(2, 10, &coeffs[0]));
    for (int poly_order = 0; poly_order < 4; ++poly_order) {
        int nPolyCoeffs = PolynomialVector::NumberOfPolynomialCoefficients(2, poly_order+1);
        std::vector<std::vector<double> > pos;
        std::vector<std::vector<double> > values;
        for (int i = 0; i < nPolyCoeffs; ++i) {
            std::vector<int> index = PolynomialVector::IndexByPower(i, 2);
            pos.push_back( std::vector<double>(2) );
            pos.back()[0] = index[0];
            pos.back()[1] = index[1];
            values.push_back(std::vector<double>(2));
            ref_vec.F(&pos.back()[0], &values.back()[0]);
        }

        PolynomialVector* test_vec = SolveFactory(poly_order+1, -1, pos[0].size(), 1).PolynomialSolve(pos, values, deriv_pos, deriv_values, deriv_indices);
        std::cerr << ref_vec << std::endl;
        std::cerr << *test_vec << std::endl;
        for (size_t i = 0; i < pos.size(); ++i) {
            std::vector<double> test_val(2);
            test_vec->F(&pos[i][0], &test_val[0]);
            EXPECT_NEAR(test_val[0], values[i][0], 1e-6);
            EXPECT_NEAR(test_val[1], values[i][1], 1e-6);
        }
    }
}

TEST(PolynomialVectorTest, TestPolynomialSolvePseudoDerivs) {
    std::vector<std::vector<double> > deriv_pos;
    std::vector<std::vector<double> > deriv_values;
    std::vector<std::vector<int> > deriv_indices;
    double coeffs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    PolynomialVector ref_vec(2, MMatrix<double>(2, 10, &coeffs[0]));
    for (int poly_order = 3; poly_order < 4; ++poly_order) {
        int nPolyCoeffs = PolynomialVector::NumberOfPolynomialCoefficients(2, poly_order+1);
        std::vector<std::vector<double> > pos;
        std::vector<std::vector<double> > values;
        int i = 0;
        for (; i < nPolyCoeffs && i < 5; ++i) {
            std::vector<int> index = PolynomialVector::IndexByPower(i, 2);
            pos.push_back( std::vector<double>(2) );
            pos.back()[0] = index[0];
            pos.back()[1] = index[1];
            values.push_back(std::vector<double>(2));
            ref_vec.F(&pos.back()[0], &values.back()[0]);
        }
        for (; i < nPolyCoeffs; ++i) {
            std::vector<int> index = PolynomialVector::IndexByPower(i, 2);
            deriv_pos.push_back( std::vector<double>(2) );
            deriv_pos.back()[0] = index[0];
            deriv_pos.back()[1] = index[1];
            deriv_indices.push_back(std::vector<int>(2, 0));
            deriv_values.push_back(std::vector<double>(2));
            for (size_t j = 0; j < index.size(); ++j)
                std::cerr << j << " ";
            std::cerr << "DERIV" << std::endl;
            ref_vec.FDeriv(&deriv_pos.back()[0],
                           &deriv_indices.back()[0],
                           &deriv_values.back()[0]);
        }
        PolynomialVector* test_vec = SolveFactory(poly_order+1, -1, pos[0].size(), 1).PolynomialSolve(pos, values, deriv_pos, deriv_values, deriv_indices);
        std::cerr << ref_vec << std::endl;
        std::cerr << *test_vec << std::endl;
        for (size_t i = 0; i < pos.size(); ++i) {
            std::vector<double> test_val(2);
            test_vec->F(&pos[i][0], &test_val[0]);
            EXPECT_NEAR(test_val[0], values[i][0], 1e-6);
            EXPECT_NEAR(test_val[1], values[i][1], 1e-6);
        }
    }
}


void run_test(int start_coefficient) {
    std::vector<std::vector<double> > deriv_pos;
    std::vector<std::vector<double> > deriv_values;
    std::vector<std::vector<int> > deriv_indices;
    std::vector<double> coeffs(20, 0.);
    for (int i = 0; i < 20; ++i)
        coeffs[i] = start_coefficient+i;
    PolynomialVector ref_vec(2, MMatrix<double>(2, 10, &coeffs[0]));
    for (int poly_order = 3; poly_order < 4; ++poly_order) {
        int nPolyCoeffs = PolynomialVector::NumberOfPolynomialCoefficients(2, poly_order+1);
        std::vector<std::vector<double> > pos;
        std::vector<std::vector<double> > values;
        int i = 0;
        for (; i < nPolyCoeffs && i < 5; ++i) {
            std::vector<int> index = PolynomialVector::IndexByPower(i, 2);
            pos.push_back( std::vector<double>(2) );
            pos.back()[0] = index[0];
            pos.back()[1] = index[1];
            values.push_back(std::vector<double>(2));
            ref_vec.F(&pos.back()[0], &values.back()[0]);
        }
        for (; i < nPolyCoeffs; ++i) {
            std::vector<int> index = PolynomialVector::IndexByPower(i, 2);
            deriv_pos.push_back( std::vector<double>(2) );
            deriv_pos.back()[0] = index[0];
            deriv_pos.back()[1] = index[1];
            deriv_indices.push_back(index);
            deriv_values.push_back(std::vector<double>(2));
            ref_vec.FDeriv(&deriv_pos.back()[0],
                           &deriv_indices.back()[0],
                           &deriv_values.back()[0]);
        }
        PolynomialVector* test_vec = SolveFactory(poly_order+1, -1, pos[0].size(), 1).PolynomialSolve(pos, values, deriv_pos, deriv_values, deriv_indices);
        for (size_t i = 0; i < pos.size(); ++i) {
            std::vector<double> test_val(2);
            test_vec->F(&pos[i][0], &test_val[0]);
            EXPECT_NEAR(test_val[0], values[i][0], 1e-6);
            EXPECT_NEAR(test_val[1], values[i][1], 1e-6);
        }
    }
}

TEST(PolynomialVectorTest, TestMakeSquareDerivVector) {
    // std::vector<double> SolveFactory::MakeSquareDerivVector(std::vector<double> positions, std::vector<int> deriv_indices, int lower, int upper) const {
    /*
    SolveFactory fac(1, -1, pos.size())
    std::vector<double> pos(4, 1.);
    pos[1] = 2.;
    pos[2] = 3.;
    pos[3] = 4.;
    std::vector<int> deriv_indices(4, 0);
    std::vector<double> derivs;
    derivs = .MakeSquareDerivVector(pos, deriv_indices, 1);
    EXPECT_EQ(derivs.size(), 2*2*2*2);
    EXPECT_EQ(derivs.back(), 1.*2.*3.*4);
    EXPECT_EQ(derivs.front(), 1.);
    derivs = SolveFactory::MakeSquareDerivVector(pos, deriv_indices, 2);
    EXPECT_EQ(derivs.size(), 3*3*3*3);
    EXPECT_EQ(derivs.back(), 1.*1.*2.*2.*3.*3.*4.*4.);
    deriv_indices = std::vector<int>(4, 1);
    derivs = SolveFactory::MakeSquareDerivVector(pos, deriv_indices, 2);
    EXPECT_EQ(derivs.size(), 3*3*3*3);
    EXPECT_EQ(derivs.back(), (2.*1.)*(2.*2.)*(2.*3.)*(2.*4));
    derivs = SolveFactory::MakeSquareDerivVector(pos, deriv_indices, 3);
    EXPECT_EQ(derivs.size(), 4*4*4*4);
    EXPECT_EQ(derivs.back(), (3.*1.*1.)*(3.*2.*2.)*(3.*3.*3.)*(3.*4.*4.));
    deriv_indices[0] = 3;
    deriv_indices[1] = 2;
    deriv_indices[2] = 1;
    deriv_indices[3] = 0;
    derivs = SolveFactory::MakeSquareDerivVector(pos, deriv_indices, 3);
    EXPECT_EQ(derivs.size(), 4*4*4*4);
    EXPECT_EQ(derivs.back(), (3.*2.*1.)*(3.*2.*2.)*(3.*3.*3.)*(4.*4.*4.));
    EXPECT_EQ(derivs.front(), 0.);
    */
}

TEST(PolynomialVectorTest, TestPolynomialSolveDerivs) {
    run_test(0);
}

TEST(PolynomialVectorTest, TestPolynomialSolveLoad) {
    for (int i = 0; i < 1000; ++i) {
        run_test(i);
    }
}



