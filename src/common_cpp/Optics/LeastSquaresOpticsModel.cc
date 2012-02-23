/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Chris Rogers (G4MICE)
 * Author: Peter Lane (MAUS update)
 */

#include "Optics/LeastSquaresTransferMapCalculator.hh"

#include <math.h>

#include <iomanip>
#include <sstream>
#include <vector>

#include "gsl/gsl_sf_gamma.h"

#include "Interface/Interpolator.hh"
#include "Interface/Mesh.hh"
#include "Interface/Squeal.hh"
#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "Maths/Vector.hh"

namespace MAUS {

Vector<double> LeastSquaresTransferMapCalculator::Means
       (std::vector<std::vector<double> > values, std::vector<double> weights) {
  if (values.size() < 1) {
    throw(Squeal
           (Squeal::recoverable, "No input values", "LeastSquaresTransferMapCalculator::Means"));
  }
  if (values[0].size() < 1) {
    throw(Squeal
            (Squeal::recoverable, "Dimension < 1", "LeastSquaresTransferMapCalculator::Means"));
  }
  if (weights.size() != values.size()) {
    weights = std::vector<double>(values.size(), 1.);
  }
  size_t npoints     = values.size();
  size_t dim         = values[0].size();
  Vector<double>    means(dim, 0);
  double             totalWeight = 0;
  for (size_t x = 0; x < npoints; x++) {
    totalWeight += weights[x];
  }
  std::vector<double> normalized_weights;
  for (size_t x = 0; x < npoints; x++) {
    normalized_weights.push_back(weights[x] / totalWeight);
  }

  double mean;
  for (size_t i = 0; i < dim; ++i) {
    mean = 0.;
    for (size_t x = 0; x < npoints; ++x) {
      mean += values[x][i] * normalized_weights[x];
    }
    means(i+1) = mean;
  }

  return means;
}

SymmetricMatrix LeastSquaresTransferMapCalculator::Covariances(
    const std::vector<std::vector<double> >&          values,
    const std::vector<double>&                        weights,
    const Vector<double>&                             means) {
  size_t npoints = values.size();
  if (npoints < 1) {
    throw(Squeal(Squeal::recoverable,
                 "No input values",
                 "LeastSquaresTransferMapCalculator::Covariances()"));
  }
  size_t dim = values[0].size();
  if (dim < 1) {
    throw(Squeal(Squeal::recoverable,
                 "Dimension < 1",
                 "LeastSquaresTransferMapCalculator::Covariances()"));
  }
  Vector<double> means_vector(dim);
  if (means.size() != dim) {
    means_vector = Means(values, weights);
  } else {
    means_vector = means;
  }
  std::vector<double> weights_vector;
  if (weights.size() != npoints) {
    weights_vector = std::vector<double>(npoints, 1.);
  } else {
    weights_vector = weights;
  }

  SymmetricMatrix covariances(dim);

  double totalWeight = 0;
  for (size_t x = 0; x < npoints; ++x) {
    totalWeight += weights_vector[x];
  }

  std::vector<double> normalized_weights;
  for (size_t x = 0; x < npoints; ++x) {
    normalized_weights.push_back(weights_vector[x] / totalWeight);
  }

  double sum;
  for (size_t i = 0; i < dim; ++i) {
    for (size_t j = 0; j <= i; ++j) {
      sum = 0.;
      for (size_t x = 0; x < npoints; x++) {
        sum += values[x][i] * values[x][j] * normalized_weights[x];
      }
      covariances.set(i+1, j+1, sum - means_vector[i] * means_vector[j]);
    }
  }
  return covariances;
}

LeastSquaresTransferMapCalculator* LeastSquaresTransferMapCalculator::PolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >& points,
  const std::vector< std::vector<double> >& values,
  unsigned int                              polynomialOrder,
  const VectorMap*                          weightFunction) {
  if (weightFunction == NULL) {
    return PolynomialLeastSquaresFit(points, values, polynomialOrder);
  } else {
    std::vector<double> weights(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
      weightFunction->F(&points[i][0], &weights[i]);
    }
    return PolynomialLeastSquaresFit(points, values, polynomialOrder, weights);
  }
}

LeastSquaresTransferMapCalculator* LeastSquaresTransferMapCalculator::PolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >& points,
  const std::vector< std::vector<double> >& values,
  unsigned int                                        polynomialOrder,
  const std::vector<double>&                weights) {
  // Algorithm: We have F2 = sum_i ( f_k f_l) where f are polynomial terms;
  // FY = sum_i (f_)

  int pointDim = points[0].size();
  int valueDim = values[0].size();
  int nPoints  = points.size();
  int nCoeffs  = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);

  Matrix<double> Fy(nCoeffs, valueDim, 0.);
  Matrix<double> F2(nCoeffs, nCoeffs,  0.);

  Matrix<double> dummy(valueDim, nCoeffs, 0.);
  for (int i = 0; i < valueDim; ++i)
    for (int j = 0; j < nCoeffs; ++j)
      dummy(i+1, j+1) = 1;
  LeastSquaresTransferMapCalculator* temp = new LeastSquaresTransferMapCalculator(pointDim, dummy);

  std::vector<double> tempFx(nCoeffs, 0);  // tempfx = x_i^j
  std::vector<double> wt(nPoints, 1);
  if (weights.size() > 0) wt = weights;

  // sum over points, values
  for (int i = 0; i < nPoints;   ++i) {
    temp->MakePolyVector(&points[i][0], &tempFx[0]);
    for (int k = 0; k < nCoeffs;  k++)
      for (int j = 0; j < nCoeffs;    ++j)
        F2(j+1, k+1) += tempFx[k]*tempFx[j]*wt[i];
    for (int j = 0; j < nCoeffs;   ++j)
      for (int k = 0; k < valueDim; k++)
        Fy(j+1, k+1) += values[i][k]*tempFx[j]*wt[i];
  }

  Matrix<double> F2_inverse;
  try {
    F2_inverse = inverse(F2);
  } catch(Squeal squee) {
    delete temp;
    throw(Squeal(Squeal::recoverable,
                 "Could not find least squares fit for data",
                 "LeastSquaresTransferMapCalculator::PolynomialLeastSquaresFit"));
  }
  Matrix<double> A = F2_inverse * Fy;
  delete temp;
  temp = new LeastSquaresTransferMapCalculator(pointDim, transpose(A));
  return temp;
}

LeastSquaresTransferMapCalculator* LeastSquaresTransferMapCalculator::ConstrainedPolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >&                points,
  const std::vector< std::vector<double> >&                values,
  unsigned int                                                   polynomialOrder,
  std::vector< LeastSquaresTransferMapCalculator::PolynomialCoefficient >  coeffs,
  const std::vector<double>&                              weights) {
  // Algorithm: we want g(x) = old_f(x) + new_f(x), where old_f has polynomial
  // terms in poly, new_f has all the rest. Use value - f(point) as input and
  // do LLS forcing old_f(x) polynomial terms to 0
  // Nb: in this constrained case we have to go through the output vector and
  // treat each like a 1D vector
  size_t nPoints    = points.size();
  if (nPoints < 1) {
    throw(Squeal(
      Squeal::recoverable,
      "No data for LLS Fit",
      "LeastSquaresTransferMapCalculator::ConstrainedPolynomialLeastSquaresFit(...)"));
  }
  if (values.size() != nPoints) {
    throw(Squeal(
      Squeal::recoverable,
      "Misaligned array in LLS Fit",
      "LeastSquaresTransferMapCalculator::ConstrainedPolynomialLeastSquaresFit(...)"));
  }
  int pointDim   = points[0].size();
  int valueDim   = values[0].size();
  int nCoeffsNew = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);
  Matrix<double> A(valueDim, nCoeffsNew, 0.);

  std::vector<double> wt(nPoints, 1);
  if (weights.size() > 0) wt = weights;

  // guaranteed to be of right order
  LeastSquaresTransferMapCalculator  newPolyVector1D(pointDim, Matrix<double>(1, nCoeffsNew));
  std::vector< std::vector<double> > tempFx(nPoints, std::vector<double>(nCoeffsNew) );
  for (size_t i = 0; i < nPoints; ++i)
    newPolyVector1D.MakePolyVector(&points[i][0], &tempFx[i][0]);

  for (int dim = 0; dim < valueDim ; dim++) {
    std::vector<LeastSquaresTransferMapCalculator::PolynomialCoefficient> oldCoeffs1D;
    for (size_t i = 0; i < coeffs.size(); ++i) {
      if (coeffs[i].OutVariable() == dim &&
          coeffs[i].InVariables().size() < polynomialOrder) {
        oldCoeffs1D.push_back(coeffs[i]);
      }
    }
    LeastSquaresTransferMapCalculator oldPolyVector1D(oldCoeffs1D);
    int              nCoeffsOld = oldCoeffs1D.size();
    int              deltaCoeff = nCoeffsNew - nCoeffsOld;
    if (deltaCoeff <= 0) break;

    // index of variables that need calculation
    std::vector<int> needsCalculation;
    for (int i = 0; i < nCoeffsNew; ++i) {
      bool exists = true;
      for (size_t j = 0; j < oldCoeffs1D.size(); ++j)
        if (IndexByVector(i, pointDim) == oldCoeffs1D[j].InVariables())
          exists = false;
      if (exists) needsCalculation.push_back(i);
    }

    Vector<double> Fy(deltaCoeff, 0);
    Matrix<double> F2(deltaCoeff, deltaCoeff,  0.);

    // optimisation note - this algorithm spends all its time in this loop
    for (size_t i = 0; i < nPoints && needsCalculation.size() > 0; ++i) {
      std::vector<double> oldValue(valueDim);
      oldPolyVector1D.F(&points[i][0], &oldValue[0]);
      for (int k = 0; k < deltaCoeff; k++) {
        Fy(k+1) += (values[i][dim] -  // ynew - yold
              oldValue[dim])*tempFx[i][needsCalculation[k]]*wt[i];
        for (int j = 0; j < deltaCoeff;    ++j)
          F2(j+1, k+1) += tempFx[i][needsCalculation[k]]*
                          tempFx[i][needsCalculation[j]]*wt[i];
      }
    }

    Matrix<double> F2_inverse;
    try {
      F2_inverse = inverse(F2);
    } catch(Squeal squee) {
      throw(Squeal(Squeal::recoverable,
            "Could not find least squares fit for data",
            "LeastSquaresTransferMapCalculator::ConstrainedPolynomialLeastSquaresFit"));
    }
    Vector<double> AVec = F2_inverse * Fy;
    for (int i = 0; i < deltaCoeff; ++i) A(dim+1, needsCalculation[i]+1) = AVec(i+1);
  }

  LeastSquaresTransferMapCalculator tempPoly(coeffs);
  for (size_t i = 0; i < coeffs.size(); ++i)
    for (size_t j = 0; j < tempPoly.index_key_by_vector_.size(); ++j)
      if (tempPoly.index_key_by_vector_[j] == coeffs[i].InVariables()) {
        A(coeffs[i].OutVariable()+1, j+1) = coeffs[i].Coefficient();
      }

  return new LeastSquaresTransferMapCalculator(pointDim, A);
}


LeastSquaresTransferMapCalculator* LeastSquaresTransferMapCalculator::Chi2ConstrainedLeastSquaresFit(
  const std::vector< std::vector<double> >&                xin,
  const std::vector< std::vector<double> >&                xout,
  unsigned int                                             polynomialOrder,
  std::vector< LeastSquaresTransferMapCalculator::PolynomialCoefficient >  coeffs,
  double                                                  chi2Start,
  double                                                  discardStep,
  double *                                                chi2End,
  double                                                  chi2Limit,
  std::vector<double>                                      weights,
  bool                                                    firstIsMean) {
  size_t nParticles   = xin.size();
  size_t dimensionOut = xout[0].size();
  if (weights.size() != nParticles) {
    weights = std::vector<double>(xin.size(), 1.);
  }
  std::vector<double> weights_in = std::vector<double>(weights);
  std::vector<double> amps(nParticles);
  Vector<double> means(dimensionOut, 0);
  if (!firstIsMean) {
     means = Means(xout, weights);
  } else {
    for (size_t i = 0; i < dimensionOut; ++i) {
      means(i+1) = xout[0][i];
    }
  }

  SymmetricMatrix covariances = Covariances(xout, weights, means);
  SymmetricMatrix inverse_covaraiance_matrix;
  try {
    inverse_covaraiance_matrix = inverse(covariances);
  }
  catch(Squeal squee) {
    throw(Squeal(Squeal::recoverable,
                 "Failed to find least squares fit for data",
                 "LeastSquaresTransferMapCalculator::Chi2ConstrainedLeastSquaresFit"));
  }

  double totalWeight = 0.;
  double discard       = chi2Start;
  double chi2          = chi2Limit*2.;
  int    nCoefficients = LeastSquaresTransferMapCalculator::NumberOfPolynomialCoefficients(
                            xin[0].size(), polynomialOrder)
                       * xout[0].size();
  int    nGood         = nParticles;

  if (discard <= 0.) {
    // if chi2Start ill-defined, just use maximum chi2 in sample
    for (size_t i = 0; i < nParticles; ++i) {
      Vector<double> xoutVec(dimensionOut, 0);
      for (size_t j = 0; j < dimensionOut; ++j) {
        xoutVec(j+1) = xout[i][j] - means(j+1);
      }
      amps[i] = (transpose(xoutVec) * inverse_covaraiance_matrix * xoutVec)(1);
      if (amps[i] > discard) discard = amps[i];
      totalWeight += weights[i];
    }
  }
  discard /= discardStep; // Set discard to the largest amplitude in the data

  LeastSquaresTransferMapCalculator* pvec = new LeastSquaresTransferMapCalculator(
    std::vector<LeastSquaresTransferMapCalculator::PolynomialCoefficient>());
  while (nGood >= nCoefficients && chi2 > chi2Limit) {
    chi2               = 0.;
    nGood              = nParticles;
    if (pvec != NULL) delete pvec;

    // optimisation note - algorithm spends all its time doing the CPLS Fit
    try {
      pvec = LeastSquaresTransferMapCalculator::ConstrainedPolynomialLeastSquaresFit(
                xin, xout, polynomialOrder, coeffs, weights);
    }
    catch(Squeal squee) {
      pvec = NULL;
      chi2 = chi2Limit * 2.;
    }

    for (size_t i = 0; i < nParticles && pvec != NULL; ++i) {
      if (fabs(weights[i]) > 1.e-9) {
        std::vector<double> pout(dimensionOut, 0.);
        pvec->F(&xin[i][0], &pout[0]);
        Vector<double> residualVec(dimensionOut, 0);
        for (size_t j = 0; j < dimensionOut; ++j) {
          residualVec(j+1) = pout[j] - xout[i][j];
        }
        // watch weights handling here
        chi2 +=   weights[i] * weights[i] / totalWeight/totalWeight
                * (transpose(residualVec) * inverse_covaraiance_matrix *
                residualVec)(1);
      }
    }
    for (size_t i = 0; i < nParticles;   ++i)
      if (amps[i] > discard) {
        totalWeight -= weights[i];
        weights[i] = 0.;
        nGood--;
      }
    discard /= discardStep;
    std::cout << "ConstrainedPolynomialLeastSquaresFit - chi2: " << chi2
              << " cut: " << discard << " cut_step: " << discardStep
              << " weight: " << totalWeight << " pvec: " << pvec << std::endl;
  }
  if (chi2 > chi2Limit || pvec == NULL) {
    std::stringstream err;
    err << "LeastSquaresTransferMapCalculator::Chi2ConstrainedLeastSquaresFit failed to "
        << "converge. Best fit had <chi2> = " << chi2
        << " compared to limit " << chi2Limit << std::endl;
    throw(Squeal(Squeal::recoverable, err.str(),
          "LeastSquaresTransferMapCalculator::Chi2ConstrainedLeastSquaresFit(...)"));
  }
  if (chi2End != NULL) *chi2End = discard; // save discard at end for future use
  return pvec;
}

LeastSquaresTransferMapCalculator* LeastSquaresTransferMapCalculator::Chi2SweepingLeastSquaresFit(
    const                                                   VectorMap& vec,
    unsigned int                                            polynomialOrder,
    std::vector< LeastSquaresTransferMapCalculator::PolynomialCoefficient >  coeffs,
    double                                                  chi2Max,
    std::vector<double>&                                    delta,
    double                                                  deltaFactor,
    int                                                     maxNumberOfSteps) {
  // build particles in shell
  // try to do chi2 fit
  // if chi2 fit works, increase shell size
  // else increase polynomial order, revise fit
  LeastSquaresTransferMapCalculator*   pvec1 = NULL;
  LeastSquaresTransferMapCalculator*   pvec2 = NULL;
  int    step = 0;
  for (size_t i_order = 1; i_order <= polynomialOrder; i_order++) {
    std::vector<std::vector<double> > in;
    std::vector<std::vector<double> > out;
    double chi2 = -1;
    while (chi2 < chi2Max && step < maxNumberOfSteps) {
      in.push_back(std::vector<double>(delta.size(), 0.));
      step++;
      std::vector<std::vector<double> > in_mod = PointBox(delta, i_order+1);
      for (size_t i = 0; i < in_mod.size(); ++i) in.push_back(in_mod[i]);
      vec.FAppend(in, out);
      if (pvec2 != NULL && pvec1 != NULL) delete  pvec2;
      if (pvec1 != NULL) pvec2 = pvec1;
      pvec1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
      chi2  = pvec1->GetAvgChi2OfDifference(in, out);
      if (chi2 < chi2Max)
        for (size_t i = 0; i < delta.size(); ++i) delta[i] *= deltaFactor;
    }
    delete pvec1;
    pvec1 = NULL;
  }
  for (size_t i = 0; i < delta.size(); ++i) delta[i] /= deltaFactor;
  return pvec2;
}

LeastSquaresTransferMapCalculator* LeastSquaresTransferMapCalculator::Chi2SweepingLeastSquaresFitVariableWalls
      (const VectorMap& vec,
      unsigned int polynomialOrder,
      std::vector< LeastSquaresTransferMapCalculator::PolynomialCoefficient > coeffs,
      double chi2Max, std::vector<double>& delta,
      double deltaFactor, int maxNumberOfSteps,
      std::vector<double> max_delta) {
  LeastSquaresTransferMapCalculator*   pvec1 = Chi2SweepingLeastSquaresFit
                                         (vec, polynomialOrder, coeffs, chi2Max,
                                          delta, deltaFactor, maxNumberOfSteps);
  if (pvec1 == NULL) return NULL;
  LeastSquaresTransferMapCalculator* pvec2 = NULL;
  int    step = 0;
  for (size_t i_order = pvec1->PolynomialOrder(); i_order <= polynomialOrder;
       i_order++) {
    for (size_t i = 0; i < delta.size(); ++i) {
      double chi2 = -1;
      while (chi2 < chi2Max && step < maxNumberOfSteps &&
             delta[i]*deltaFactor < max_delta[i]) {
        if (chi2 < chi2Max)
          delta[i] *= deltaFactor;
        step++;
        std::vector<std::vector<double> > in = PointBox(delta, i_order+1);
        in.push_back(std::vector<double>(delta.size(), 0.));
        std::vector<std::vector<double> > out;
        vec.FAppend(in, out);
        if (pvec2 != NULL && pvec1 != NULL) delete  pvec2;
        if (pvec1 != NULL) pvec2 = pvec1;
        pvec1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
        chi2  = pvec1->GetAvgChi2OfDifference(in, out);
        if (chi2 > chi2Max)
          delta[i] /= deltaFactor;
      }
    }
    delete pvec1;
    pvec1 = NULL;
  }
  return pvec2;
}


void LeastSquaresTransferMapCalculator::PolynomialCoefficient::SpaceTransform
                       (std::vector<int> space_in, std::vector<int> space_out) {
  std::map<int, int> mapping; // probably optimise this
  for (size_t i = 0; i < space_out.size(); ++i)
    for (size_t j = 0; j < space_in.size(); ++j) {
      // mapping[space_in_index] returns space_out_index
      if (space_out[i] == space_in[j]) mapping[j] = i;
    }
  std::vector<int> in_variables(_inVarByVec);
  for (size_t con = 0; con < in_variables.size(); con++) {
    if (mapping.find(in_variables[con]) != mapping.end()) {
      in_variables[con] = mapping[in_variables[con]];
    } else { throw(Squeal(Squeal::recoverable,
                    "Input variable not found in space transform",
                    "LeastSquaresTransferMapCalculator::PolynomialCoefficient::SpaceTransform"));
    }
  }
  if (mapping.find(_outVar) != mapping.end()) {
    _outVar = mapping[_outVar];
  } else {
    throw(Squeal(Squeal::recoverable,
                 "Output variable not found in space transform",
                 "LeastSquaresTransferMapCalculator::PolynomialCoefficient::SpaceTransform"));
  }
  _inVarByVec = in_variables;
}

// Return chi2 of some set of output variables compared with the polynomial
// vector of some set of input variables
double LeastSquaresTransferMapCalculator::GetAvgChi2OfDifference(
    std::vector< std::vector<double> > in,
    std::vector< std::vector<double> > out) {
  std::vector< std::vector<double> > out_p;
  if (in.size() < 1) {
    throw(Squeal(Squeal::recoverable,
                 "No data in input",
                 "LeastSquaresTransferMapCalculator::GetAvgChi2OfDifference(...)"));
  }
  if (in.size() != out.size()) {
    throw(Squeal(Squeal::recoverable,
     "Input data and output data misaligned for calculation of chi2 difference",
     "LeastSquaresTransferMapCalculator::GetAvgChi2OfDifference(...)"));
  }
  for (size_t i = 0; i < in.size(); ++i) {
    if (in[i].size() != PointDimension() || out[i].size() != ValueDimension())
      throw(Squeal(Squeal::recoverable,
                   "Bad input data for calculation of chi2 difference",
                   "LeastSquaresTransferMapCalculator::GetAvgChi2OfDifference(...)"));
  }
  this->FAppend(in, out_p);

  std::vector< std::vector<double> > diff(in.size());
  for (size_t i = 0; i < out_p.size(); ++i) {
    diff[i] = std::vector<double>( ValueDimension() );
    for (size_t j = 0; j < ValueDimension(); ++j)
      diff[i][j] = out[i][j]-out_p[i][j];
  }

  double chi2 = 0;
  SymmetricMatrix cov_inv = Covariances(diff,
                                        std::vector<double>(diff.size(), 1.),
                                        Vector<double>(diff.size(), 0.));
  for (size_t i = 0; i < diff.size(); ++i) {
    Vector<double> diff_mv(diff[i]);
    chi2 += (transpose(diff_mv) * cov_inv * diff_mv)(1);
  }
  chi2 /= static_cast<double>(diff.size());
  return chi2;
}

// Make a shell of points on the outside of a hypercube with dimension same as
// delta length, corners of hypercube are at delta[0], delta[1], ... delta[n]
// and -delta[0], ..., -delta[n] number of points is at least
// 2*NumberOfPolynomialCoefficients(i_order, delta.size())-1 so that it will
// always define a polynomial of order i_order (one point +ve, one -ve hence
// factor 2) Require evenly spaced points, sometimes this means I make more
// points on the shell
static const double PI = atan(1)*4.;
std::vector< std::vector<double> > LeastSquaresTransferMapCalculator::PointBox(
  std::vector<double>  delta,
  int                  i_order) {
  int min_size         = 3*NumberOfPolynomialCoefficients(i_order, delta.size());
  int n_points_per_dim = 2;
  int dim              = delta.size();
  while (::pow(n_points_per_dim, dim)-::pow(n_points_per_dim-2, dim)
         <= min_size) {
    n_points_per_dim++;
  }
  int max_point = n_points_per_dim/2;
  if ( 2*(n_points_per_dim/2) != n_points_per_dim ) // odd n_points
    max_point = (n_points_per_dim-1)/2;

  std::vector< std::vector<double> > pos;
  std::vector<int>    grid_size(dim, n_points_per_dim);
  std::vector<double> grid_min(delta);
  std::vector<double> grid_spacing(delta);
  for (size_t i = 0; i < grid_spacing.size(); ++i)
    grid_spacing[i] *= 2./static_cast<double>(n_points_per_dim-1);
  for (size_t i = 0; i < grid_min.size(); ++i)
    grid_min[i] *= -1.;
  NDGrid grid(dim, &grid_size[0], &grid_spacing[0], &grid_min[0]);
  for (Mesh::Iterator it = grid.Begin(); it < grid.End(); it++) {
    std::vector<int> place = it.State();
    for (size_t i = 0; i < place.size(); ++i)
      // If I am on the outside of the grid
      if (place[i] == 1 || place[i] == n_points_per_dim) {
        pos.push_back(it.Position());
        break; // end inner i loop to detect if it is on outside of grid
      }
  }
  return pos;
}

// Algorithm - take the PointBox output and scale so that length is 1 in
// scale_matrix coordinate system
std::vector< std::vector<double> > LeastSquaresTransferMapCalculator::PointShell(
  Matrix<double>  scale_matrix,
  int             i_order) {
  size_t point_dim = scale_matrix.number_of_rows();
  Matrix<double> scale_inv = inverse(scale_matrix);
  std::vector<std::vector<double> > point_box
    = PointBox(std::vector<double>(point_dim, 1.), i_order);
  for (size_t i = 0; i < point_box.size(); ++i) {
    Vector<double> point(&point_box[i][0], point_dim);
    double scale  = (transpose(point) * scale_inv * point)(1);
    point /= ::pow(scale, static_cast<double>(point_dim));
    for (size_t j = 0; j < point_dim; ++j) {
      point_box[i][j]=point(j+1);
    }
  }
  return point_box;
}

}  // namespace MAUS
