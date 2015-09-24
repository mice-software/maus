#include "TransferMap.hh"

#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include "CLHEP/Matrix/Matrix.h"
#include "Maths/Matrix.hh"

using MAUS::Matrix;

int          TransferMap::_order  = 1;

TransferMap::TransferMap(CLHEP::HepMatrix firstOrderMap, PhaseSpaceVector referenceIn, PhaseSpaceVector referenceOut,
                         OpticsModel* Optics)
	   : _secondOrderMap(6,6,6,0), _referenceTrajectoryIn(referenceIn), _referenceTrajectoryOut(referenceOut), 
             _optics(Optics), _polynomial(NULL), _canonicalMap(true)
{
	SetFirstOrderMap(firstOrderMap);
}

TransferMap::TransferMap(CLHEP::HepMatrix transverseMap, CLHEP::HepMatrix longitudinalMap, PhaseSpaceVector referenceIn, 
                         PhaseSpaceVector referenceOut, OpticsModel* Optics)
	   : _secondOrderMap(6,6,6,0), _referenceTrajectoryIn(referenceIn), _referenceTrajectoryOut(referenceOut), 
             _optics(Optics), _polynomial(NULL), _canonicalMap(true)
{
	SetFirstOrderMap(transverseMap, longitudinalMap);
}

TransferMap::TransferMap(PhaseSpaceVector referenceIn, OpticsModel* Optics)
	   : _secondOrderMap(6,6,6,0), _referenceTrajectoryIn(referenceIn), _referenceTrajectoryOut(referenceIn), 
             _optics(Optics), _polynomial(NULL), _canonicalMap(true)
{
	SetFirstOrderMap(CLHEP::HepMatrix(6,6,1));
}

TransferMap::TransferMap() : _optics(NULL)
{
	SetFirstOrderMap(CLHEP::HepMatrix(6,6,0));
}

/*
  CLHEP::HepMatrix coeffs = polynomial.GetCoefficients();
  _order = 0;
  for(int i=1; i<coeffs.num_col(); i++)
  {
    std::vector<int> index = G4MICE::PolynomialMap::IndexByVector(i, coeffs.num_row());
    if(index.size() > uint(_order))
    {
      _order++;
      if(_order == 1) _firstOrderMap  = CLHEP::HepMatrix(6,6,0);
      if(_order == 2) _secondOrderMap = Tensor3(6,6,6);
      if(_order >  2) _higherOrderMaps.push_back( new Tensor(6,_order+1,0) );
    }
    index.insert(index.begin(), 0);
    for(int j=0; j<6; j++)
    {
      index[0] = j;
      if(_order == 1) _firstOrderMap  [index[0]][index[1]]           = coeffs[j][i];
      if(_order == 2) _secondOrderMap [index[2]][index[0]][index[1]] = coeffs[j][i];
      if(_order >  2) _higherOrderMaps.back()->Set(index, coeffs[j][i]);
    }
  }
*/

void TransferMap::SetFirstOrderMap(CLHEP::HepMatrix transverseMap, CLHEP::HepMatrix longitudinalMap)
{
	_firstOrderMap = dsum(longitudinalMap, transverseMap);
}

void TransferMap::SetFirstOrderMap(CLHEP::HepMatrix firstOrderMap)
{
	_firstOrderMap = firstOrderMap;
}

CLHEP::HepMatrix TransferMap::GetFirstOrderMap() const
{
	if(_polynomial!=NULL) {
    Matrix<double> coefficients = _polynomial->GetCoefficientsAsMatrix();
    return MAUS::CLHEP::HepMatrix(coefficients.submatrix(1,6,2,6));
  }
	return _firstOrderMap;
}


CovarianceMatrix TransferMap::operator*(const CovarianceMatrix& aCovMatrix) const
{
	HepSymMatrix in;
	if(_canonicalMap)  in = aCovMatrix.GetRawNormCanCovariances(_referenceTrajectoryIn);
	else               in = aCovMatrix.GetKineticCovariances();
	double     norm = aCovMatrix.GetNormalisation();
	CLHEP::HepSymMatrix aMatrix = in.similarity(GetFirstOrderMap());
	PhaseSpaceVector Mean       = *this*aCovMatrix.GetMean();
	CovarianceMatrix covOut     = CovarianceMatrix(HepSymMatrix(6,1), Mean, norm, 1, false);
	if(_canonicalMap) covOut.SetRawNormCanCovariances(aMatrix, norm, _referenceTrajectoryOut);
	else              covOut.SetCovariances(aMatrix);
	return covOut;
}

PhaseSpaceVector TransferMap::operator*(const PhaseSpaceVector& aPhaseSpaceVector) const
{
	if(_polynomial)
	{
		MAUS::Vector<double> vec(aPhaseSpaceVector.getSixVector());
		_polynomial->F(vec, vec);
		vec += MAUS::Vector<double>(_referenceTrajectoryOut.getSixVector());
		// PhaseSpaceVector out(_referenceTrajectoryOut.z(), _referenceTrajectoryIn.m());
	}
	
	CLHEP::HepVector psVec = aPhaseSpaceVector.getCanonicalSixVector();
	//Transform to deviation coordinates
	psVec   -= _referenceTrajectoryIn.getCanonicalSixVector();
	//Do the multiplication
	psVec    = GetFirstOrderMap()  * psVec;
	if(GetOrder() > 1)
	{
		CLHEP::HepVector correction = GetSecondOrderMap().Poisson(psVec);//.ExponentialPoisson(psVec, GetOrder());
		correction[0]              /= CLHEP::c_light;
		psVec                      += correction;
	}
	for(int i=3; i<GetOrder(); i++)
		psVec  = psVec;

	//Transform back to lab coordinates
	psVec += _referenceTrajectoryOut.getCanonicalSixVector();
	PhaseSpaceVector transformedVec;
	CLHEP::HepLorentzVector Q(psVec[2], psVec[4], _referenceTrajectoryOut.z(), psVec[0]);
	CLHEP::HepLorentzVector A(0,0,0,0);
	if(_optics!=NULL)
		A  = _optics->GetTheField()->GetVectorPotential(Q);
	transformedVec.setCanonicalSixVector(psVec, A, _referenceTrajectoryOut.z(), _referenceTrajectoryIn.m());
	if(_optics!=NULL)
		_optics->SetFields(transformedVec);

	return transformedVec;
}

Tensor  TransferMap::GetMap(int order)    const
{
	if(order > GetOrder() || order - 2 > int(_higherOrderMaps.size()) ) return Tensor(6, order+1, 0);
	if(order < 3) throw(MAUS::Exception(MAUS::Exception::recoverable, "Use GetMap for maps of order > 2", "TransferMap::GetOrder(int)"));
	return *_higherOrderMaps[order-3];
}

TransferMap TransferMap::operator*(const TransferMap& aTransferMap) const
{
	CLHEP::HepMatrix firstOrderMap = _firstOrderMap*aTransferMap.GetFirstOrderMap();
	return TransferMap(firstOrderMap, aTransferMap.GetReferenceIn(), _referenceTrajectoryOut, _optics);
}

TransferMap TransferMap::operator*=(const TransferMap& aTransferMap)
{
	*this = (*this)*aTransferMap;
	return *this; 
}

TransferMap TransferMap::Inv(int& failflag) const
{
	TransferMap out(*this);
	out.SetFirstOrderMap(GetFirstOrderMap().inverse(failflag));
	out.SetReferenceIn(_referenceTrajectoryOut);
	out.SetReferenceOut(_referenceTrajectoryIn);
	return out;
}

std::ostream& operator<<(std::ostream& out, TransferMap tm)
{
	out << tm.GetReferenceIn() << "\n" << tm.GetReferenceOut() << "\n";
	if(tm.GetPolynomialMap() != NULL) {out << *tm._polynomial << std::endl; return out;}
	out << tm.GetFirstOrderMap();
	if(tm.GetOrder() > 1)
		out << tm.GetSecondOrderMap();
	if(tm.GetOrder() > 2)
		for(int i=3; i<=tm.GetOrder(); i++)
			out << tm.GetMap(i) << std::endl;
	return out;
}

TransferMap TransferMap::Rotate(double angle) const
{
	CLHEP::HepMatrix rotation = CovarianceMatrix::Rotation(angle);
	TransferMap rotated(*this);
	rotated.SetFirstOrderMap(GetFirstOrderMap()*rotation);
	return rotated;
}

CovarianceMatrix TransferMap::Decompose(double em_t, double em_x, double em_y) const
{
	double beta[3]  = {0,0,0};
	double alpha[3] = {0,0,0};
	double refP     = GetReferenceIn().P();

	for(int i=0; i<3; i++)
	{
		CLHEP::HepMatrix sub   = GetFirstOrderMap().sub(i*2+1, i*2+2, i*2+1, i*2+2);
		double    sinmu = sub[0][1]/fabs(sub[0][1])*fabs(sin( acos( (sub[0][0]+sub[1][1])/2. ) ));
		beta[i]         = refP * sub[0][1]/sinmu;
		alpha[i]        = (sub[0][0] - sub[1][1])/(2.*sinmu);
		if(beta[i]!=beta[i] || alpha[i]!=alpha[i]) 
		{
			beta[i]  = 1.;
			alpha[i] = 0.;
		}
	}

	CovarianceMatrix covCan(em_x, beta[1], alpha[1], em_y, beta[2], alpha[2], em_t, beta[0], alpha[0], 
	                        GetReferenceIn());
	CovarianceMatrix covKin(covCan.GetKineticCovariances(), GetReferenceIn(), false);
	return covCan;
}


CovarianceMatrix TransferMap::Decompose(double angle, double em_t, double em_x, double em_y) const
{
	TransferMap      rotatedTM  =  Rotate(angle);
	CovarianceMatrix rotatedCov = rotatedTM.Decompose(em_t, em_x, em_y);
	return rotatedCov.Rotate(-angle);
}

//Deserves some comment
//Here we solve the equation V^out = M V_in M^T for 3 decoupled 2D phase spaces.
//V^out = sxx*v_11	sxp*v_12   V^in = v_11	v_12    M = m_11	m_12
//        sxp*v_12      spp*v_22          v_12	v_22        m_21	m_22
//Pursuing the algebra reduces this to solving a 3x3 set of linear equations like
// sxx v_11 = m_11^2 v_11    + m_21^2 v_22    + 2 m_11 m_21 v_12
// spp v_22 = m_12^2 v_11    + m_22^2 v_22    + 2 m_22 m_12 v_12
// sxp v_12 = m_12 m_11 v_11 + m_21 m_22 v_22 + (m_22 m_11 + m_12 m_21) v_12
//which we do for each of the 2D phase spaces. Note spp is determined by condition |V^out| = |V^in|.
std::vector<CovarianceMatrix> TransferMap::Decompose(double sxx, double sxp, double spp, double em_t, double em_x, 
                                                     double em_y) const
{

	std::vector<CovarianceMatrix> covMatrix;
	std::vector<HepSymMatrix>     canonicalCovariance2D[3];
	double em[3] = {em_t, em_x, em_y};
	for(int a=0; a<3; a++)
	{
		CLHEP::HepMatrix sub    = GetFirstOrderMap().sub(a*2+1, a*2+2, a*2+1, a*2+2);
		double p         = sub[0][0];
		double q         = sub[1][1];
		double r         = sub[0][1];
		double s         = sub[1][0];

		double data[9]   = { p*p-sxx+1, r*r,       2*p*r,
		                     s*s,       q*q-spp+1, 2*q*s,
		                     s*p,       q*r,       (q*p+s*r)-sxp+1 };

		gsl_matrix_view     m           = gsl_matrix_view_array (data, 3, 3);
		gsl_vector_complex* eval        = gsl_vector_complex_alloc (3);
		gsl_matrix_complex* evec        = gsl_matrix_complex_alloc (3, 3);
		gsl_eigen_nonsymmv_workspace* w = gsl_eigen_nonsymmv_alloc (3);
		gsl_eigen_nonsymmv(&m.matrix, eval, evec, w);
		gsl_eigen_nonsymmv_free(w);
		gsl_eigen_nonsymmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_DESC);
		for (int i = 0; i < 3; i++)
		{
			gsl_complex eval_i = gsl_vector_complex_get (eval, i);
			gsl_vector_complex_view evec_i = gsl_matrix_complex_column (evec, i);
			if(GSL_IMAG(eval_i) == 0.) //eigenvalue real => eigenvector real
			{
				HepSymMatrix cov = HepSymMatrix(2,0);
				cov[0][0] = GSL_REAL(gsl_vector_complex_get(&evec_i.vector, 0));
				cov[1][1] = GSL_REAL(gsl_vector_complex_get(&evec_i.vector, 1));
				cov[0][1] = GSL_REAL(gsl_vector_complex_get(&evec_i.vector, 2));
				double det = cov.determinant();
				if(det > 0)
					canonicalCovariance2D[a].push_back(cov/det*em[a]);
			}
		}
		gsl_vector_complex_free(eval);
		gsl_matrix_complex_free(evec);
	}

	PhaseSpaceVector mean = GetReferenceIn();
	for(unsigned int i=0; i<canonicalCovariance2D[0].size(); i++)
		for(unsigned int j=0; j<canonicalCovariance2D[1].size(); j++)
			for(unsigned int k=0; k<canonicalCovariance2D[2].size(); k++)
			{
				HepSymMatrix cov(6,0);
				for(int a=0; a<2; a++)
					for(int b=0; b<2; b++)
					{
						cov[a][b]     = canonicalCovariance2D[0][i][a][b];
						cov[a+2][b+2] = canonicalCovariance2D[1][j][a][b];
						cov[a+4][b+4] = canonicalCovariance2D[2][k][a][b];
					}
				covMatrix.push_back(CovarianceMatrix(cov, mean, false));
			}
	return covMatrix;
}

double TransferMap::PhaseAdvance(int axis, double angle) const
{
	TransferMap      rotatedTM  =  Rotate(angle);
	return rotatedTM.PhaseAdvance(axis);
}

double TransferMap::PhaseAdvance(int axis) const
{
	CLHEP::HepMatrix subMatrix = GetFirstOrderMap().sub(axis*2+1, axis*2+2, axis*2+1, axis*2+2);
  // TM given by
  // cos(\phi) + \alpha sin(\phi), \beta sin(\phi) / p
  // - \gamma sin(\phi),            cos(\phi) - \alpha sin(\phi)
  // we can get the phase advance quadrant by considering both sin(phi) and
  // cos(phi) and using atan2(phi)
  double n00 = (subMatrix[0][0]-subMatrix[1][1])/2.; // alpha sin(phi)
  double sin_phi = sqrt(-n00*n00 - subMatrix[0][1]*subMatrix[1][0]); // ((-alpha^2 + beta gamma) sin^2(phi) )^0.5
  double cos_phi = (subMatrix[0][0] + subMatrix[1][1])/2.; // cos(phi)
	double phaseAdvance = atan2(sin_phi, cos_phi);
	if(phaseAdvance != phaseAdvance) throw(MAUS::Exception(MAUS::Exception::recoverable, "Complex phase advance", "TransferMap::PhaseAdvance")); 
	return phaseAdvance;
}

MAUS::PolynomialMap* TransferMap::GetPolynomialMap() {
  return _polynomial;
}

void TransferMap::SetPolynomialMap(MAUS::PolynomialMap* poly) {
  _polynomial = poly;
}



