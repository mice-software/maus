#include <sstream>

#include "gtest/gtest.h"

#include "math/PolynomialVector.hh"
#include "math/PolynomialPatch.hh"
#include "math/PPSolveFactory.hh"

#include "TCanvas.h"
#include "TH2.h"
#include "TGraph.h"

typedef PolynomialVector::PolynomialCoefficient PolyCoeff;

// NEEDS MORE HERE
TEST(PolynomialPatchTest, TestConstructorDestructor) {
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

void test_points(int dim, int lower, int upper, std::vector< std::vector<int> > pts) {
      int upper_size = 1;
      int lower_size = 1;
      for (int i = 0; i < dim; ++i)
          upper_size *= upper+1;
      for (int i = 0; i < dim; ++i)
          lower_size *= lower+1;
      if (lower < 0)
          lower_size = 0;
      if (upper < 0)
          upper_size = 0;
      // size should be difference in area of the squares
      EXPECT_EQ(pts.size(), upper_size - lower_size);
      for (size_t i = 0; i < pts.size(); ++i) {
          // each pts element should have length dim
          EXPECT_EQ(pts[i].size(), dim);
          // each pts element should have indices with lower < size <= upper
          bool in_bounds = true;
          for (int j = 0; j < dim; ++j) {
              in_bounds = in_bounds || (pts[i][j] > lower && pts[i][j] <= upper);
          }
          EXPECT_TRUE(in_bounds);
          // each pts element should be unique
          for (size_t j = 0; j < pts.size(); ++j) {
              if (j == i)
                  continue;
              bool equal = true;
              for (int k = 0; k < dim; ++k)
                  equal &= pts[i][k] == pts[j][k];
              EXPECT_FALSE(equal);
          }
      }
}

TEST(PolynomialPatchTest, TestNearbyPointsSquares) {
    for (int upper = 0; upper < 5; ++upper) {
        for (int lower = 0; lower < upper; ++lower) {
            for (int dim = 1; dim < 5; ++dim) {
                std::cerr << "Testing GetNearbyPointsSquares dim: "  << dim << " lower: " << lower << " upper: " << upper << std::endl;
                std::vector< std::vector<int> > pts =
                    PPSolveFactory::GetNearbyPointsSquares(dim, lower, upper);
                test_points(dim, lower, upper, pts);
            }
        }
    }
}

TEST(PolynomialPatchTest, DISABLED_TestLeastSquaresFitTwoD) {
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
    // BUG
    PolynomialPatch* pp1 = NULL ; // PPSolveFactory().LeastSquaresFit(grid, values, 1);
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
                std::vector<double> position = it.Position();
                double delta = (it.Position()[j]-it_delta.Position()[j]);
                // just above it.Position()
                position[j] += 1e-6*delta;
                double valPos[] = {0., 0.};
                pp1->F(&position[0], valPos);
                PolynomialVector* poly1 = pp1->GetPolynomialVector(&position[0]);
                // just below it.Position()
                position[j] -= 2e-6*delta;
                double valNeg[] = {0., 0.};
                pp1->F(&position[0], valNeg);
                PolynomialVector* poly2 = pp1->GetPolynomialVector(&position[0]);
                // should be equal (even though we are on adjacent mesh points)
                for (size_t k = 0; k < it_delta.State().size(); ++k)
                    EXPECT_NEAR(valPos[k], valNeg[k], 6);
                // half way between it.Position() and it_delta.Position()
                position[j] += 0.5*delta;
                double valHalf[] = {0., 0.};
                pp1->F(&position[0], valHalf);
                // at it_delta.Position()
                position[j] += 1.0*delta;
                double valDelta[] = {0., 0.};
                pp1->F(&position[0], valDelta);
                continue; // disable printing
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
    // PPSolveFactory().LeastSquaresFit(grid, values, 3);
}

TEST(PolynomialPatchTest, TestLeastSquaresFitThreeD) {
    int np = 5;
    ThreeDGrid* grid = new ThreeDGrid(1., 2., 3., 4., 5., 6., np, np, np);
    std::vector<std::vector<double> > values;
    for (Mesh::Iterator it = grid->Begin(); it < grid->End(); ++it) {
        std::vector<double> x = it.Position();
        std::vector<double> a_value(3);
        double twopi = asin(1.)*4;
        a_value[0] = sin(twopi*x[0]/np)*sin(twopi*x[1]/np)*sin(twopi*x[2]/np);
        a_value[1] = cos(twopi*x[0]/np)*cos(twopi*x[1]/np)*cos(twopi*x[2]/np);
        a_value[2] = 1.;
        values.push_back(a_value);
    }
    for (size_t pp_order = 2; pp_order < 3; ++pp_order) {
        std::cerr << "Building pp of order " << pp_order << std::endl;
        //PolynomialPatch* pp1 =
        //            PolynomialPatch::LeastSquaresFit(grid, values, pp_order);
        //if (pp1); // cleanup compiler warning
    }
}


// check continuity or nth derivative around point
void check_cont(PolynomialPatch* pp, std::vector<double> point, std::vector<int> deriv) {
    
}

void test_poly(PolynomialPatch* pp, ThreeDGrid* grid, size_t smooth_order) {
}

std::vector<double> get_value(std::vector<double> x, int np) {
    static const double twopi = asin(1.)*4;
    std::vector<double> a_value(3);
    a_value[0] = sin(twopi*x[0]/np)*sin(twopi*x[1]/np)*sin(twopi*x[2]/np);
    a_value[1] = cos(twopi*x[0]/np)*cos(twopi*x[1]/np)*cos(twopi*x[2]/np);
    a_value[2] = 1.;
    return a_value;
}

void plot(int n_points, std::vector<double> start, std::vector<double> end, PolynomialPatch* patch, int n_grid_points, std::string title) {
    TCanvas* c1 = new TCanvas("canvas", "canvas");
    c1->Draw();
    double delta_sq = 0;
    for (size_t i = 0; i < start.size(); ++i)
        delta_sq += (start[i]-end[i])*(start[i]-end[i]);
    TH2D* h2 = new TH2D("h1", (title+";pos [AU]; Fit [AU]").c_str(), 10000, start[0]-(end[0]-start[0])/10., end[0]+(end[0]-start[0])/10., 1000, -5., 5.);
    h2->SetStats(false);
    h2->Draw();
    std::vector<TGraph*> graphs(2*patch->ValueDimension(), NULL);
    for (size_t j = 0; j < graphs.size(); ++j) {
        graphs[j] = new TGraph(n_points);
    }
    for (int i = 0; i < n_points; ++i) {
        std::cerr << i << " " << delta_sq*i << " ** ";
        std::vector<double> point = start;
        for (size_t j = 0; j < point.size(); ++j) {
            point[j] += (end[j]-start[j])*i/n_points;
            std::cerr << point[j] << " ";
        }
        std::cerr << " ** ";
        std::vector<double> value(patch->ValueDimension(), 0.);
        patch->F(&point[0], &value[0]);
        for (size_t j = 0; j < patch->ValueDimension(); ++j) {
            graphs[j]->SetPoint(i, point[0], value[j]);
            std::cerr << value[j] << " ";
        }
        value = get_value(point, n_grid_points);
        std::cerr << " ** ";
        for (size_t j = 0; j < patch->ValueDimension(); ++j) {
            graphs[j+patch->ValueDimension()]->SetPoint(i, point[0], value[j]);
            std::cerr << value[j] << " ";
        }
        std::cerr << std::endl;
    }
    graphs[0]->SetTitle("Fit Bx");
    graphs[1]->SetTitle("Fit By");
    graphs[2]->SetTitle("Fit Bz");
    graphs[3]->SetTitle("True Bx");
    graphs[4]->SetTitle("True By");
    graphs[5]->SetTitle("True Bz");
    for (size_t j = 0; j < graphs.size(); ++j) {
        graphs[j]->SetFillColor(0);
        graphs[j]->SetLineColor(j+1);
        graphs[j]->SetMarkerColor(j+1);
        graphs[j]->Draw("l");
    }
    c1->BuildLegend();
    static int test_index = 0;
    std::stringstream test_name;
    test_name << "test_" << test_index << ".pdf";
    c1->Print(test_name.str().c_str());
    test_index++;
}

void print_test(int np) {
    std::vector<double> pos, test_value;
    pos = std::vector<double>(2, 2.);
    test_value = get_value(pos, np);
    std::cerr << "2. 2. ** " << test_value[0] << std::endl;
    pos = std::vector<double>(2, 2.);
    pos[1] = 3.;
    test_value = get_value(pos, np);
    std::cerr << "2. 3. ** " << test_value[0] << std::endl;
    pos = std::vector<double>(2, 2.);
    pos[0] = 3.;
    test_value = get_value(pos, np);
    std::cerr << "3. 2. ** " << test_value[0] << std::endl;
    pos = std::vector<double>(2, 3.);
    test_value = get_value(pos, np);
    std::cerr << "3. 3. ** " << test_value[0] << std::endl;
}

TEST(PolynomialPatchTest, TestThreeDSolveSinCos) {
    int np = 10;
    TwoDGrid* grid2 = new TwoDGrid(1., 1., 0., 0., np, np);
    ThreeDGrid* grid3 = new ThreeDGrid(1., 1., 1., 0., 0., 0., np, np, np);
    Mesh* grid_array[] = {grid2, grid3};

    for (int grid_index = 1; grid_index < 2; ++grid_index) {
        Mesh* grid = grid_array[grid_index];
        std::vector<std::vector<double> > values;
        for (Mesh::Iterator it = grid->Begin(); it < grid->End(); ++it) {
            values.push_back(get_value(it.Position(), np));
        }
        for (int smooth_order = 3; smooth_order < 4; ++smooth_order) {
            for (int pp_order = 2; pp_order <= 2; ++pp_order) {
                std::cerr << "Building pp of order " << pp_order << " smooth " << smooth_order << std::endl;
                PPSolveFactory fac(grid,
                                   values,
                                   pp_order,
                                   smooth_order);
                PolynomialPatch* pp1 = fac.Solve();
                std::cerr << "Testing" << std::endl;
                //if (pp1); // cleanup compiler warning
                std::vector<double> start = grid->Begin().Position();
                std::vector<double> end = (grid->End()-1).Position();
                std::stringstream ss;
                ss << "Dimension: " << grid_index+2 << " Poly Order: " << pp_order << " Smooth order: " << smooth_order << std::endl;
                plot(1000, start, end, pp1, np, ss.str());
                for (Mesh::Iterator it = grid->Begin(); it < grid->Begin()+1; ++it) {
                    std::vector<double> test_value(3);
                    pp1->F(&it.Position()[0], &test_value[0]);
                    for (size_t i = 0; i < 3; ++i)
                        EXPECT_NEAR(values[it.ToInteger()][i], test_value[i], 1e-6);
                }
            }
        }
    }
    print_test(np);
}


