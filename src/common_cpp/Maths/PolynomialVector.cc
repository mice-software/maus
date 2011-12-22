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

#include "Maths/PolynomialVector.hh"

#include <iomanip>
#include <sstream>
#include <math.h>

#include "gsl/gsl_sf_gamma.h"

#include "Interface/Mesh.hh"
#include "Interface/Squeal.hh"
#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "Maths/Vector.hh"

namespace MAUS
{

bool PolynomialVector::print_headers_=true;

PolynomialVector::PolynomialVector(int numberOfInputVariables,
																	 Matrix<double> polynomialCoefficients)
	:	point_dimmension_(numberOfInputVariables), index_key_by_power_(),
		index_key_by_vector_(), coefficient_matrix_(polynomialCoefficients)
{
  SetCoefficients(numberOfInputVariables, polynomialCoefficients);
}

PolynomialVector::PolynomialVector(
	std::vector<PolynomialCoefficient>	coefficients)
	:	point_dimmension_(0), index_key_by_power_(), index_key_by_vector_(),
		coefficient_matrix_()
{
  SetCoefficients(coefficients);
}

void PolynomialVector::SetCoefficients(int pointDim, Matrix<double> coeff)
{
  int nPCoeff      = coeff.number_of_columns();
  point_dimmension_        = pointDim;
  index_key_by_power_  = std::vector< std::vector<int> >();
  index_key_by_vector_ = std::vector< std::vector<int> >();
  polynomial_vector_ = std::vector<PolynomialCoefficient>();

  for(int i=0; i<nPCoeff;   ++i)
    index_key_by_power_.push_back(IndexByPower (i, pointDim));
  for(int i=0; i<nPCoeff;   ++i) 
    index_key_by_vector_.push_back(IndexByVector(i, pointDim));

  for(size_t i=0; i<coefficient_matrix_.number_of_rows();   ++i) 
    for(int j=0; j<nPCoeff;   ++j)
      polynomial_vector_.push_back( PolynomialVector::PolynomialCoefficient(index_key_by_vector_[j], i, coefficient_matrix_(i+1, j+1) ) );
}

void PolynomialVector::SetCoefficients(std::vector<PolynomialCoefficient> coeff)
{
  point_dimmension_        = 0;
  index_key_by_power_  = std::vector< std::vector<int> >();
  index_key_by_vector_ = std::vector< std::vector<int> >();
  polynomial_vector_      = coeff;

  int maxPolyOrder = 0;
  int valueDim     = 0;
  for(unsigned int i=0; i<coeff.size();   ++i)
  {
    int polyOrder = coeff[i].InVariables().size();
    for(unsigned int j=0; j<coeff[i].InVariables().size();   ++j)
      if(coeff[i].InVariables()[j] > point_dimmension_) point_dimmension_ = coeff[i].InVariables()[j];
    if(coeff[i].OutVariable()    > valueDim)  valueDim  = coeff[i].OutVariable();
    if(polyOrder > maxPolyOrder) maxPolyOrder = polyOrder;
  }
  point_dimmension_++; //PointDim indexes from 0
  coefficient_matrix_ = Matrix<double>(valueDim+1, NumberOfPolynomialCoefficients(point_dimmension_, maxPolyOrder+1), 0.);

  for(size_t i=0; i<coefficient_matrix_.number_of_columns();   ++i)
    index_key_by_power_.push_back(IndexByPower (i, point_dimmension_));
  for(size_t i=0; i<coefficient_matrix_.number_of_columns();   ++i) 
  {
    index_key_by_vector_.push_back(IndexByVector(i, point_dimmension_));
    for(unsigned int j=0; j<coeff.size();   ++j) 
      if(coeff[j].InVariables() == index_key_by_vector_.back()) 
      {
        int dim = coeff[j].OutVariable();
        coefficient_matrix_(dim+1,i+1) = coeff[j].Coefficient();
      }
  }
}

void PolynomialVector::SetCoefficients(Matrix<double> coeff)
{
  for(size_t i=0; i<coeff.number_of_rows() && i<coefficient_matrix_.number_of_rows();   ++i)
    for(size_t j=0; j<coeff.number_of_columns() && j<coefficient_matrix_.number_of_columns();   ++j)
      coefficient_matrix_(i+1,j+1) = coeff(i+1,j+1);
}

Matrix<double> PolynomialVector::GetCoefficientsAsMatrix() const
{
  return coefficient_matrix_;
}

std::vector<PolynomialVector::PolynomialCoefficient>
PolynomialVector::GetCoefficientsAsVector() const
{
  return polynomial_vector_;
}

PolynomialVector* PolynomialVector::Recentred(double * point) const
{
	throw(Squeal(Squeal::nonRecoverable, "Recentred not implemented", "PolynomialVector::Recentred"));
}

void  PolynomialVector::F(const double* point, double* value) const
{
	Vector<double> pointV(PointDimension(), 1.);
	Vector<double> valueV(ValueDimension(), 1.);
	for(unsigned int i=0; i<PointDimension();   ++i)
	{
		pointV(i+1) = point[i];
	}

	F(pointV, valueV);

	for(unsigned int i=0; i<ValueDimension();   ++i)
	{
		value[i]  = valueV(i+1);
	}
}

void  PolynomialVector::F(const Vector<double>& point, Vector<double>& value)
	const
{
	//create a vector of point coordinate products that form, together with the
	//coefficents, the polynomial terms
	Vector<double> polynomial_vector(index_key_by_vector_.size(), 1.);
	MakePolyVector(point, polynomial_vector);

	value = coefficient_matrix_ * polynomial_vector;
}

unsigned int PolynomialVector::PointDimension() const
{
	return point_dimmension_;
}

unsigned int PolynomialVector::ValueDimension() const
{
	return coefficient_matrix_.number_of_rows();
}

unsigned int PolynomialVector::PolynomialOrder() const
{
	if(index_key_by_vector_.size()>0)
	{
		return index_key_by_vector_.back().size()+1;
	}
	else
	{
		return 0;
	}
}

PolynomialVector * PolynomialVector::Clone() const
{
	return new PolynomialVector(*this);
}


Vector<double>&  PolynomialVector::MakePolyVector(const Vector<double>& point,
                                                  Vector<double>& polyVector)
                                                 const
{
    for(unsigned int i=0; i<index_key_by_vector_.size();   ++i)
        for(unsigned int j=0; j<index_key_by_vector_[i].size();   ++j)
            polyVector(i+1) *= point( index_key_by_vector_[i][j]+1 );
    return polyVector;
}

double*  PolynomialVector::MakePolyVector(const double* point,
                                          double* polyVector) const
{
    for(unsigned int i=0; i<index_key_by_vector_.size();   ++i)
    {
        polyVector[i] = 1.;
        for(unsigned int j=0; j<index_key_by_vector_[i].size();   ++j)
            polyVector[i] *= point[ index_key_by_vector_[i][j] ];
    }
    return polyVector;
}

//Turn int index into a std::vector<int> 'a' of length 'd' with values x_1^a_1 x_2^a_2 ... x_d^a_d
// e.g. x_1^4 x_2^3 = {4,3,0,0}
std::vector<int> PolynomialVector::IndexByPower(int index, int nInputVariables)
{
    std::vector<int> powerIndex(nInputVariables, 0);
    std::vector<int> vectorIndex = IndexByVector(index, nInputVariables);
    for(unsigned int i=0; i<vectorIndex.size();   ++i)
        powerIndex[vectorIndex[i]]++;
    return powerIndex;
}

//Turn int index into an index for element of polynomial
// e.g. x_1^4 x_2^3 = {1,1,1,1,2,2,2}
std::vector<int> PolynomialVector::IndexByVector(int index, int nInputVariables)
{
    if(index == 0) return std::vector<int>();
    std::vector<int> indices(1,-1);
    nInputVariables--;
    for(int i=0; i<index;   ++i)
    {
        if     (indices.front() == nInputVariables) { indices = std::vector<int>(indices.size()+1, 0); indices.back()--;}
        if     (indices.back()  == nInputVariables)
        {
            int j=indices.size()-1;
            while(indices[j] == nInputVariables) j--;
            for(int k=indices.size()-1; k>=j; k--) indices[k] = indices[j]+1;
        }
        else    indices.back()++;
    }
    return indices;
}

unsigned int PolynomialVector::NumberOfPolynomialCoefficients(int pointDimension, int order)
{
    int n=0;
    if(order<=0) return 0;
    for(int i=1; i<order;   ++i)
        n += gsl_sf_choose(pointDimension+i-1, i);
    return n+1;
}

std::ostream& operator<<(std::ostream& out, const PolynomialVector& pv)
{
    if(PolynomialVector::print_headers_) pv.PrintHeader(out, '.', ' ', 14, true);
    out << '\n' << pv.GetCoefficientsAsMatrix();
    return out;
}

template <class Container >
void PolynomialVector::PrintContainer(std::ostream& out, const Container& container, char T_separator, char str_separator, int length, bool pad_at_start)
{
//  class Container::iterator it;
  std::stringstream strstr1("");
  std::stringstream strstr2("");
  class Container::const_iterator it1 = container.begin();
  class Container::const_iterator it2 = it1; 
  while(it1!=container.end())
  {
    it2++;
    if(it1 != container.end() && it2 != container.end())
         strstr1 << (*it1) << T_separator;
    else strstr1 << (*it1);
    it1++;
  }

  if(int(strstr1.str().size()) > length && length > 0) strstr2 << strstr1.str().substr(1, length);
  else                                                 strstr2 << strstr1.str();

  if(!pad_at_start) out << strstr2.str();
  for(int i=0; i<length - int(strstr2.str().size());   ++i) out << str_separator;
  if(pad_at_start) out << strstr2.str();
}

void PolynomialVector::PrintHeader(std::ostream& out, char int_separator, char str_separator, int length, bool pad_at_start) const
{
  if(index_key_by_power_.size() > 0) PrintContainer< std::vector<int> >(out, index_key_by_power_[0], int_separator, str_separator, length-1, pad_at_start);
  for(unsigned int i=1; i<index_key_by_power_.size();   ++i)
    PrintContainer<std::vector<int> >(out, index_key_by_power_[i], int_separator, str_separator, length, pad_at_start);
}

Vector<double> PolynomialVector::Means(std::vector<std::vector<double> > values, std::vector<double> weights)
{
  if(values.size() < 1)    throw(Squeal(Squeal::recoverable, "No input values", "PolynomialVector::Means"));
  if(values[0].size() < 1) throw(Squeal(Squeal::recoverable, "Dimension < 1",   "PolynomialVector::Means"));
  if(weights.size() != values.size()) weights = std::vector<double>(values.size(), 1.);
  size_t npoints     = values.size();
  size_t dim         = values[0].size();
  Vector<double>    means(dim,0);
  double             totalWeight = 0;
  for(int x=0; x<npoints; x++) totalWeight += weights[x];

  std::vector<double> normalized_weights;
  for (size_t x=0; x<npoints; x++) {
    normalized_weights.push_back(weights[x] / totalWeight);
  }

  double mean;
  for(int i=0; i<dim; ++i)
  {
    mean = 0.;
    for(int x=0; x<npoints; ++x) {
      mean += values[x][i] * normalized_weights[x];
    }

    means(i+1) = mean;
  }

  return means;
}

SymmetricMatrix PolynomialVector::Covariances(
    const std::vector<std::vector<double> >&          values,
    const std::vector<double>&                        weights,
    const Vector<double>&                             means)
{
  size_t npoints = values.size();
  if(npoints < 1) {
    throw(Squeal(Squeal::recoverable,
                 "No input values",
                 "PolynomialVector::Covariances()"));
  }
  size_t dim = values[0].size();
  if(dim < 1) {
    throw(Squeal(Squeal::recoverable,
                 "Dimension < 1",
                 "PolynomialVector::Covariances()"));
  }
  Vector<double> means_vector(dim);
  if(means.size() != dim) {
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
  for (size_t x=0; x<npoints; x++) {
    totalWeight += weights_vector[x];
  }

  std::vector<double> normalized_weights;
  for (size_t x=0; x<npoints; x++) {
    normalized_weights.push_back(weights_vector[x] / totalWeight);
  }

  double sum;
  for(size_t i=0; i<dim; ++i) {
    for(size_t j=0; j<=i; ++j) {
      sum = 0.;
      for(size_t x=0; x<npoints; x++)
      {
        sum += values[x][i] * values[x][j] * normalized_weights[x];
      }

      covariances.set(i+1, j+1, sum - means_vector[i] * means_vector[j]);
    }
  }

  return covariances;
}

PolynomialVector* PolynomialVector::PolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >&  points,
  const std::vector< std::vector<double> >& values, 
  int                                        polynomialOrder,
  const VectorMap*                          weightFunction)
{
  if(weightFunction == NULL) return PolynomialLeastSquaresFit(points, values, polynomialOrder);
  else
  {
    std::vector<double> weights(points.size());
    for(unsigned int i=0; i<points.size(); ++i)
    {
      weightFunction->F(&points[i][0], &weights[i]);
    }
    return PolynomialLeastSquaresFit(points, values, polynomialOrder, weights);
  }
}

PolynomialVector* PolynomialVector::PolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >& points,
  const std::vector< std::vector<double> >& values, 
  int                                        polynomialOrder,
  const std::vector<double>&                weights)
{
  //Algorithm: We have F2 = sum_i ( f_k f_l) where f are polynomial terms; FY = sum_i (f_)

  int pointDim = points[0].size();
  int valueDim = values[0].size();
  int nPoints  = points.size();
  int nCoeffs  = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);

  Matrix<double> Fy(nCoeffs, valueDim, 0.);
  Matrix<double> F2(nCoeffs, nCoeffs,  0.);

  Matrix<double> dummy(valueDim, nCoeffs, 0.);
  for(int i=0; i<valueDim; ++i)
    for(int j=0; j<nCoeffs; ++j)
      dummy(i+1, j+1) = 1;
  PolynomialVector* temp = new PolynomialVector(pointDim, dummy);

  std::vector<double>    tempFx(nCoeffs,0); //tempfx = x_i^j
  std::vector<double>    wt    (nPoints,1);
  if(weights.size() > 0) wt = weights; 

  //sum over points, values
  for(int i=0; i<nPoints;   ++i)
  {
    temp->MakePolyVector(&points[i][0], &tempFx[0]);
    for(int k=0; k<nCoeffs;  k++) 
      for(int j=0; j<nCoeffs;    ++j) 
        F2(j+1, k+1) += tempFx[k]*tempFx[j]*wt[i];
    for(int j=0; j<nCoeffs;   ++j)
      for(int k=0; k<valueDim; k++)
        Fy(j+1,k+1) += values[i][k]*tempFx[j]*wt[i];
  }

  Matrix<double> F2_inverse;
  try
  {
    F2_inverse = inverse(F2);
  }
  catch(Squeal squee)
  {
    delete temp;
    throw(Squeal(Squeal::recoverable,
                 "Could not find least squares fit for data",
                 "PolynomialVector::PolynomialLeastSquaresFit"));
  }
  Matrix<double> A = F2_inverse * Fy;
  delete temp;
  temp = new PolynomialVector(pointDim, transpose(A));
  return temp;
}

PolynomialVector* PolynomialVector::ConstrainedPolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >&                points, 
  const std::vector< std::vector<double> >&                values, 
  int                                                      polynomialOrder,
  std::vector< PolynomialVector::PolynomialCoefficient >  coeffs,
  const std::vector<double>&                              weights)
{
  //Algorithm: we want g(x) = old_f(x) + new_f(x), 
  //where old_f has polynomial terms in poly, new_f has all the rest
  //Use value - f(point) as input and do LLS forcing old_f(x) polynomial terms to 0
  //Nb: in this constrained case we have to go through the output vector and treat each like a 1D vector
  int pointDim   = points[0].size();
  int valueDim   = values[0].size();
  int nPoints    = points.size();
  if(nPoints<1)                   throw(Squeal(Squeal::recoverable, "No data for LLS Fit", "PolynomialVector::ConstrainedPolynomialLeastSquaresFit(...)"));
  if(int(values.size())!=nPoints) throw(Squeal(Squeal::recoverable, "Misaligned array in LLS Fit", "PolynomialVector::ConstrainedPolynomialLeastSquaresFit(...)")); 
  int nCoeffsNew = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);
  Matrix<double> A(valueDim, nCoeffsNew, 0.);

  std::vector<double>    wt    (nPoints,   1);
  if(weights.size() > 0) wt = weights; 

  PolynomialVector  newPolyVector1D(pointDim, Matrix<double>(1, nCoeffsNew)); //guaranteed to be of right order
  std::vector< std::vector<double> > tempFx(nPoints, std::vector<double>(nCoeffsNew) );
  for(int i=0; i<nPoints;   ++i)
    newPolyVector1D.MakePolyVector(&points[i][0], &tempFx[i][0]);

  for(int dim=0; dim<valueDim ; dim++)
  {
    std::vector<PolynomialVector::PolynomialCoefficient> oldCoeffs1D;
    for(unsigned int i=0; i<coeffs.size();   ++i)
      if(coeffs[i].OutVariable() == dim && int(coeffs[i].InVariables().size()) < polynomialOrder) 
        oldCoeffs1D.push_back(coeffs[i]);
    PolynomialVector oldPolyVector1D(oldCoeffs1D);
    int              nCoeffsOld = oldCoeffs1D.size();
    int              deltaCoeff = nCoeffsNew - nCoeffsOld;
    if(deltaCoeff <= 0) break;


    std::vector<int> needsCalculation; //index of variables that need calculation
    for(int i=0; i<nCoeffsNew;   ++i) 
    {
      bool exists = true;
      for(unsigned int j=0; j<oldCoeffs1D.size();   ++j)
        if(IndexByVector(i, pointDim) == oldCoeffs1D[j].InVariables()) exists = false;
      if(exists) needsCalculation.push_back(i);
    }

    Vector<double> Fy(deltaCoeff, 0);
    Matrix<double> F2(deltaCoeff, deltaCoeff,  0.);

    for(int i=0; i<nPoints && needsCalculation.size()>0;   ++i) //optimisation note - this algorithm spends all its time in this loop
    {
      std::vector<double> oldValue(valueDim);
      oldPolyVector1D.F(&points[i][0], &oldValue[0]);
      for(int k=0; k<deltaCoeff;  k++)
      {
        Fy(k+1) += (values[i][dim] - oldValue[dim])*tempFx[i][needsCalculation[k]]*wt[i]; //ynew - yold
        for(int j=0; j<deltaCoeff;    ++j) 
          F2(j+1,k+1) += tempFx[i][needsCalculation[k]]*tempFx[i][needsCalculation[j]]*wt[i];
      }
    }
    
    Matrix<double> F2_inverse;
    try
    {
      F2_inverse = inverse(F2);
    }
    catch(Squeal squee) { 
      throw(Squeal(Squeal::recoverable, "Could not find least squares fit for data", "PolynomialVector::ConstrainedPolynomialLeastSquaresFit")); }
    Vector<double> AVec = F2_inverse * Fy;
    for(int i=0; i<deltaCoeff;   ++i) A(dim+1, needsCalculation[i]+1) = AVec(i+1);
  }

  PolynomialVector tempPoly(coeffs);
  for(unsigned int i=0; i<coeffs.size();   ++i)
    for(unsigned int j=0; j<tempPoly.index_key_by_vector_.size();   ++j)
      if(tempPoly.index_key_by_vector_[j] == coeffs[i].InVariables())
      {
        A(coeffs[i].OutVariable()+1,j+1) = coeffs[i].Coefficient();
      }

  return new PolynomialVector(pointDim, A);
}


PolynomialVector* PolynomialVector::Chi2ConstrainedLeastSquaresFit(
  const std::vector< std::vector<double> >&                xin,
  const std::vector< std::vector<double> >&                xout, 
  int                                                      polynomialOrder,
  std::vector< PolynomialVector::PolynomialCoefficient >  coeffs, 
  double                                                  chi2Start,
  double                                                  discardStep,
  double *                                                chi2End,
  double                                                  chi2Limit, 
  std::vector<double>                                      weights,
  bool                                                    firstIsMean)
{
  int    nParticles   = xin.size();
  int    dimensionOut = xout[0].size();
  if (int(weights.size()) != nParticles)
  {
    weights = std::vector<double>(xin.size(), 1.);
  }
  std::vector<double> weights_in = std::vector<double>(weights);
  std::vector<double> amps(nParticles);
  Vector<double> means(dimensionOut,0);
  if(!firstIsMean)
  {
     means = Means(xout, weights);
  }
  else
  {
    for(int i=0; i<dimensionOut; ++i)
    {
      means(i+1) = xout[0][i];
    }
  }

  SymmetricMatrix covariances = Covariances(xout, weights, means);
  SymmetricMatrix inverse_covaraiance_matrix;
  try
  {
    inverse_covaraiance_matrix = inverse(covariances);
  }
  catch(Squeal squee)
  {
    throw(Squeal(Squeal::recoverable,
                 "Failed to find least squares fit for data",
                 "PolynomialVector::Chi2ConstrainedLeastSquaresFit"));
  }

  double totalWeight = 0.;
  double discard       = chi2Start;
  double chi2          = chi2Limit*2.;
  int    nCoefficients = PolynomialVector::NumberOfPolynomialCoefficients(
                            xin[0].size(), polynomialOrder)
                       * xout[0].size();
  int    nGood         = nParticles;

  if(discard <= 0.)
  {
    for(int i=0; i<nParticles; ++i) //if chi2Start ill-defined, just use maximum chi2 in sample
    {
      Vector<double> xoutVec(dimensionOut, 0);
      for(int j=0; j<dimensionOut; ++j)
      {
        xoutVec(j+1) = xout[i][j] - means(j+1);
      }
      amps[i] = (transpose(xoutVec) * inverse_covaraiance_matrix * xoutVec)(1);
      if(amps[i] > discard) discard = amps[i];
      totalWeight += weights[i];
    }
  }
  discard /= discardStep; //Set discard to the largest amplitude in the data

  PolynomialVector* pvec = new PolynomialVector(
    std::vector<PolynomialVector::PolynomialCoefficient>());
  while( nGood >= nCoefficients && chi2 > chi2Limit)
  {
    chi2               = 0.;
    nGood              = nParticles;
    if(pvec != NULL) delete pvec;

    //optimisation note - algorithm spends all its time doing the CPLS Fit
    try
    {
      pvec = PolynomialVector::ConstrainedPolynomialLeastSquaresFit(
                xin, xout, polynomialOrder, coeffs, weights);
    }
    catch(Squeal squee)
    {
      pvec = NULL;
      chi2 = chi2Limit * 2.;
    }

    for (int i=0; i<nParticles && pvec!=NULL; ++i)
    {
      if (fabs(weights[i]) > 1.e-9) 
      {
        std::vector<double> pout(dimensionOut, 0.);
        pvec->F(&xin[i][0], &pout[0]);
        Vector<double> residualVec(dimensionOut,0);
        for(int j=0; j<dimensionOut;   ++j)
        {
          residualVec(j+1) = pout[j] - xout[i][j];
        }
        //watch weights handling here
        chi2 +=   weights[i] * weights[i] / totalWeight/totalWeight
                * (transpose(residualVec) * inverse_covaraiance_matrix * residualVec)(1);
      }
    }
    for(int i=0; i<nParticles;   ++i)
      if(amps[i] > discard) {totalWeight -= weights[i]; weights[i] = 0.; nGood--;}
    discard /= discardStep;
    std::cout << "ConstrainedPolynomialLeastSquaresFit - chi2: " << chi2 << " cut: " << discard << " cut_step: " << discardStep << " weight: " << totalWeight
              << " pvec: " << pvec << std::endl;
  }
  if(chi2 > chi2Limit || pvec == NULL)
  {
    std::stringstream err;
    err << "PolynomialVector::Chi2ConstrainedLeastSquaresFit failed to converge. Best fit had <chi2>="
                                << chi2 << " compared to limit " << chi2Limit << std::endl;
    throw(Squeal(Squeal::recoverable, err.str(), "PolynomialVector::Chi2ConstrainedLeastSquaresFit(...)"));
  }
  if(chi2End != NULL) *chi2End = discard; //save discard at end for future use
  return pvec;
}

PolynomialVector* PolynomialVector::Chi2SweepingLeastSquaresFit(
    const                                                   VectorMap& vec,
    int                                                     polynomialOrder,
    std::vector< PolynomialVector::PolynomialCoefficient >  coeffs, 
    double                                                  chi2Max,
    std::vector<double>&                                    delta,
    double                                                  deltaFactor,
    int                                                     maxNumberOfSteps) {
  //build particles in shell
  //try to do chi2 fit
  //if chi2 fit works, increase shell size
  //else increase polynomial order, revise fit
  PolynomialVector*   pvec1 = NULL;
  PolynomialVector*   pvec2 = NULL;
  int    step = 0; 
  for(int i_order=1; i_order<=polynomialOrder; i_order++)
  {
    std::vector<std::vector<double> > in;
    std::vector<std::vector<double> > out;
    double chi2 = -1;
    while(chi2 < chi2Max && step < maxNumberOfSteps) {
      in.push_back(std::vector<double>(delta.size(), 0.));
      step++;
      std::vector<std::vector<double> > in_mod = PointBox(delta, i_order+1);
      for(size_t i=0; i<in_mod.size();   ++i) in.push_back(in_mod[i]);
      vec.FAppend(in, out);
      if(pvec2 != NULL && pvec1 != NULL) delete  pvec2;
      if(pvec1 != NULL) pvec2 = pvec1;
      pvec1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
      chi2  = pvec1->GetAvgChi2OfDifference(in, out);
      if(chi2 < chi2Max)
        for(size_t i=0; i<delta.size();   ++i) delta[i] *= deltaFactor;
    }
    delete pvec1;
    pvec1 = NULL;
  }
  for(size_t i=0; i<delta.size();   ++i) delta[i] /= deltaFactor;
  return pvec2;
}

PolynomialVector* PolynomialVector::Chi2SweepingLeastSquaresFitVariableWalls
                      (const VectorMap& vec, int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                       double chi2Max, std::vector<double>& delta, double deltaFactor, int maxNumberOfSteps, std::vector<double> max_delta)
{
  PolynomialVector*   pvec1 = Chi2SweepingLeastSquaresFit(vec, polynomialOrder, coeffs, chi2Max, delta, deltaFactor, maxNumberOfSteps);
  if(pvec1==NULL) return NULL;
  PolynomialVector*   pvec2 = NULL;
  int    step = 0; 
  for(int i_order=pvec1->PolynomialOrder(); i_order<=polynomialOrder; i_order++)
  {
    for(size_t i=0; i<delta.size();   ++i) {
      double chi2 = -1;
      while(chi2 < chi2Max && step < maxNumberOfSteps && delta[i]*deltaFactor<max_delta[i]) {
        if(chi2 < chi2Max)
          delta[i] *= deltaFactor;
        step++;
        std::vector<std::vector<double> > in = PointBox(delta, i_order+1);
        in.push_back(std::vector<double>(delta.size(), 0.));
        std::vector<std::vector<double> > out;
        vec.FAppend(in, out);
        if(pvec2 != NULL && pvec1 != NULL) delete  pvec2;
        if(pvec1 != NULL) pvec2 = pvec1;
        pvec1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
        chi2  = pvec1->GetAvgChi2OfDifference(in, out);
        if(chi2 > chi2Max)
          delta[i] /= deltaFactor;
      }
    }
    delete pvec1;
    pvec1 = NULL;
  }
  return pvec2;
}


void PolynomialVector::PolynomialCoefficient::SpaceTransform(std::vector<int> space_in, std::vector<int> space_out)
{
  std::map<int, int> mapping; //probably optimise this
  for(unsigned int i=0; i<space_out.size();   ++i)
    for(unsigned int j=0; j<space_in.size();   ++j)
      if(space_out[i] == space_in[j]) mapping[j] = i; //mapping[space_in_index] returns space_out_index

  std::vector<int> in_variables(_inVarByVec.size());
  for(unsigned int con=0; con<in_variables.size(); con++)
  {
    if(mapping.find(in_variables[con]) != mapping.end()) in_variables[con] = mapping[in_variables[con]];
    else                                                throw(Squeal(Squeal::recoverable, "Input variable not found in space transform", 
                                                                             "PolynomialVector::PolynomialCoefficient::SpaceTransform"));
  }

  if(mapping.find(_outVar) != mapping.end()) _outVar = mapping[_outVar];
  else                                      throw(Squeal(Squeal::recoverable, "Output variable not found in space transform", 
                                                                  "PolynomialVector::PolynomialCoefficient::SpaceTransform"));

  _inVarByVec = in_variables;
}

//Return chi2 of some set of output variables compared with the polynomial vector of some set of input variables 
double PolynomialVector::GetAvgChi2OfDifference(
    std::vector< std::vector<double> > in,
    std::vector< std::vector<double> > out) {
  std::vector< std::vector<double> > out_p;
  if(in.size() < 1)
  {
    throw(Squeal(Squeal::recoverable,
                 "No data in input",
                 "PolynomialVector::GetAvgChi2OfDifference(...)"));
  }
  if(in.size() != out.size())
  {
    throw(Squeal(Squeal::recoverable,
                 "Input data and output data misaligned for calculation of chi2 difference",
                 "PolynomialVector::GetAvgChi2OfDifference(...)"));
  }
  for(size_t i=0; i<in.size(); ++i) 
  {
    if(in[i].size() != PointDimension() || out[i].size() != ValueDimension())
      throw(Squeal(Squeal::recoverable,
                   "Bad input data for calculation of chi2 difference",
                   "PolynomialVector::GetAvgChi2OfDifference(...)"));
  }
  this->FAppend(in, out_p);

  std::vector< std::vector<double> > diff(in.size());
  for(size_t i=0; i<out_p.size(); ++i)
  {
    diff[i] = std::vector<double>( ValueDimension() );
    for(size_t j=0; j<ValueDimension(); ++j)
      diff[i][j] = out[i][j]-out_p[i][j];
  }
  
  double chi2 = 0;
  SymmetricMatrix cov_inv = Covariances(diff,
                                        std::vector<double>(diff.size(), 1.),
                                        Vector<double>(diff.size(),
                                        0.));

  for(size_t i=0; i<diff.size(); ++i)
  {
    Vector<double> diff_mv(&(diff[i][0]), diff[i].size());
    chi2 += (transpose(diff_mv) * cov_inv * diff_mv)(1);
  }
  chi2 /= double(diff.size());
  return chi2;
}

//Make a shell of points on the outside of a hypercube with dimension same as delta length, corners
//of hypercube are at delta[0], delta[1], ... delta[n] and -delta[0], ..., -delta[n] 
//number of points is at least 2*NumberOfPolynomialCoefficients(i_order, delta.size())-1
//so that it will always define a polynomial of order i_order (one point +ve, one -ve hence factor 2)
//Require evenly spaced points, sometimes this means I make more points on the shell
static const double PI = atan(1)*4.;
std::vector< std::vector<double> > PolynomialVector::PointBox(
	std::vector<double>	delta,
	int									i_order)
{
  int min_size         = 3*NumberOfPolynomialCoefficients(i_order, delta.size());
  int n_points_per_dim = 2;
  int dim              = delta.size();
  while(::pow(n_points_per_dim, dim)-::pow(n_points_per_dim-2, dim) <= min_size)
	{
		n_points_per_dim++;
	}
  int max_point        = n_points_per_dim/2;
  if( 2*(n_points_per_dim/2) != n_points_per_dim ) //odd n_points
    max_point = (n_points_per_dim-1)/2;

  std::vector< std::vector<double> > pos;
  std::vector<int>    grid_size(dim, n_points_per_dim);
  std::vector<double> grid_min (delta);
  std::vector<double> grid_spacing(delta);
  for(size_t i=0; i<grid_spacing.size();   ++i) grid_spacing[i] *= 2./double(n_points_per_dim-1);
  for(size_t i=0; i<grid_min    .size();   ++i) grid_min    [i] *= -1.;
  NDGrid grid(dim, &grid_size[0], &grid_spacing[0], &grid_min[0]);
  for(Mesh::Iterator it=grid.Begin(); it<grid.End(); it++) {
    std::vector<int> place = it.State();
    for(size_t i=0; i<place.size();   ++i) if(place[i] == 1 || place[i] == n_points_per_dim) { //I am on the outside of the grid
      pos.push_back(it.Position());
      break; //end inner i loop to detect if it is on outside of grid
    }
  }
  return pos;
}

//Algorithm - take the PointBox output and scale so that length is 1 in scale_matrix coordinate system
std::vector< std::vector<double> > PolynomialVector::PointShell(
  Matrix<double>  scale_matrix,
  int             i_order)
{
  size_t point_dim = scale_matrix.number_of_rows();
  Matrix<double> scale_inv = inverse(scale_matrix);
  std::vector<std::vector<double> > point_box
    = PointBox(std::vector<double>(point_dim, 1.), i_order);
  for(size_t i=0; i<point_box.size(); ++i)
  {
    Vector<double> point(&point_box[i][0], point_dim);
    double scale  = (transpose(point) * scale_inv * point)(1);
    point /= ::pow(scale, double(point_dim));
    for(size_t j=0; j<point_dim; ++j)
    {
      point_box[i][j]=point(j+1);
    }
  }
  return point_box;
}

} //namespace MAUS
