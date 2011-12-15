#ifndef CovarianceMatrix_hh
#define CovarianceMatrix_hh

#include <vector>

#include "Interface/SymmetricMatrix.hh"
#include "Optics/PhaseSpaceVector.hh"

//*** Legacy Includes ***//
#include <algorithm>
#include <map>

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"


#include "PhaseSpaceVector.hh"
#include "AnalysisPlaneBank.hh"
//***********************//

namespace CLHEP
{
	class HepSymMatrix;
}

namespace MAUS
{

//*************************
// Forward Declarations
//*************************
class Matrix<double>;
class CovarianceMatrix;

//############################
// Free Functions
//############################

/* @brief Return a rotated covariance matrix
 */
CovarianceMatrix rotate(const CovarianceMatrix& covariances, double angle);

std::ostream& operator<<(std::ostream& out, CovarianceMatrix cov);

//############################
// CovarianceMatrix
//############################

/** @class CovarianceMatrix
 *  Extend SymmetricMatrix to represent a 6x6 matrix of covariances
 *  (second moments) of phase space coordinates {t, E, x, Px, y, Py}.
 *	For example, element [1][3] is <E x> and [5][4] is <Py Px>.
 */
class CovarianceMatrix : public SymmetricMatrix
{
public: 
  /* @brief Create an identity matrix.
	 */
  CovarianceMatrix();

  /* @brief Copy constructor
	 */
  CovarianceMatrix(const CovarianceMatrix& covariances);

  /* @brief Base class copy constructor.
	 */
  CovarianceMatrix(const SymmetricMatrix& matrix);
  
  /* @brief HepSymMatrix copy constructor
	 */
  CovarianceMatrix(const ::CLHEP::HepSymMatrix& covariances);

  /* @brief See SetCovariances for a description of parameters.
	 */
  CovarianceMatrix(double mass, double momentum, double charge,
                   double emittance_t, double beta_t,
                   double alpha_t, double Ltwiddle_t,
                   double emittance_l, double beta_l,
                   double alpha_l, double Bz,
                   double dispersion_x, double dispersion_prime_x,
                   double dispersion_y, double dispersion_prime_y);

  /* @brief See SetCovariances for a description of parameters.
	 */
  CovarianceMatrix(double mass, double momentum, double energy,
                   double emittance_x, double beta_x,
                   double alpha_x, double emittance_y,
                   double beta_y, double alpha_y,
                   double emittance_l, double beta_l,
                   double alpha_l,
                   double dispersion_x, double dispersion_prime_x,
                   double dispersion_y, double dispersion_prime_y);

  //********************************
  //*** Public member functions  ***
  //********************************
  
  /* @brief
   *  Set the matrix elements using the Penn parameterisation for a
   *  cylindrically symmetric matrix.
   *
   *Arguments:
   *  mass        - mass of ???
   *  momentum    - momentum of ???
   *  charge      - charge of the particle
   *  emittance_t - transverse emittance
   *  beta_t      - transverse Twiss beta
   *  alpha_t     - transverse Twiss alpha
   *  Ltwiddle_t  - parameterised, canonical, angular momentum
   *  Bz          - z component of the magnetic field at r=0
   *  dispersion_x        - x dispersion
   *  dispersion_prime_x  - ??time derivative of x dispersion??
   *  dispersion_y        - y dispersion
   *  dispersion_prime_y  - ??time derivative of y dispersion??
	 */
  void SetCovariances(double mass, double momentum, double charge,
                      double emittance_t, double beta_t,
                      double alpha_t, double Ltwiddle_t,
                      double emittance_l, double beta_l,
                      double alpha_l, double Bz,
                      double dispersion_x, double dispersion_prime_x,
                      double dispersion_y, double dispersion_prime_y);
  
  /* @brief Set the matrix elements using the Twiss parameterisation.
   *
   *Arguments:
   *  mass        - mass of ???
   *  momentum    - momentum of ???
   *  energy      - energy of ???
   *  emittance_x - x emittance
   *  beta_x      - x Twiss beta
   *  alpha_x     - x Twiss alpha
   *  emittance_y - y emittance
   *  beta_y      - y Twiss beta
   *  alpha_y     - y Twiss alpha
   *  emittance_l - longitudinal emittance
   *  beta_l      - longitudinal Twiss beta
   *  alpha_l     - longitudinal Twiss alpha
   *  dispersion_x        - x dispersion
   *  dispersion_prime_x  - ??time derivative of x dispersion??
   *  dispersion_y        - y dispersion
   *  dispersion_prime_y  - ??time derivative of y dispersion??
	 */
  void SetCovariances(double mass, double momentum, double energy,
                      double emittance_x, double beta_x,
                      double alpha_x, double emittance_y,
                      double beta_y, double alpha_y,
                      double emittance_l, double beta_l,
                      double alpha_l,
                      double dispersion_x, double dispersion_prime_x,
                      double dispersion_y, double dispersion_prime_y);

  /* @brief Return true if the Covariance matrix is positive definite
	 */
  bool IsPositiveDefinite();

  //*************************
  // Befriending
  //*************************

  //These need use of the protected base class copy constructor to cast the
	//return type as a SymmetricMatrix
  friend CovarianceMatrix rotate(const CovarianceMatrix& covariances,
																 const double angle);
																 
protected:
  /* @brief Matrix copy constructor. Used when we are certain the matrix
	 *				resulting from some operation on the convariances is symmetric.
	 */
	CovarianceMatrix(const Matrix<double>& matrix);
};

} //namespace MAUS














//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Legacy CovarianceMatrix
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

namespace G4MICE
{
	class PolynomialVector;
}
class MiceModule;

using CLHEP::HepSymMatrix;

using namespace CLHEP;

class CovarianceMatrix
{
public: 
	CovarianceMatrix();
	//Penn parameterisation for a cylindrically symmetric matrix; 
	//x_l is longitudinal; x_t is transverse
	//Ltwiddle is parameterised canonical angular momentum
	//B_z is Bz at r=0
	CovarianceMatrix(double emittance_t, double beta_t, double alpha_t, double Ltwiddle_t, 
                         double emittance_l, double beta_l, double alpha_l, 
	                 PhaseSpaceVector mean, double weight=1);
	CovarianceMatrix(double emittance_x, double beta_x, double alpha_x,  
	                 double emittance_y, double beta_y, double alpha_y, 
	                 double emittance_l, double beta_l, double alpha_l, 
	                 PhaseSpaceVector mean, double weight=1);
	CovarianceMatrix(const MiceModule* mod);
	//Basic matrix as input
	CovarianceMatrix(HepSymMatrix covariances, PhaseSpaceVector mean, bool covIsKinetic=true); 
	CovarianceMatrix(HepSymMatrix covariances, PhaseSpaceVector mean, double weight, bool covIsKinetic=true); 
	//covariances normalised so det(C')=1, norm the normalisation factor so C = norm*C' 
	CovarianceMatrix(HepSymMatrix covariances, PhaseSpaceVector mean, double norm, double weight, bool covIsKinetic=true); 

	//Get various data
	PhaseSpaceVector GetMean()                 const {return _mean;}
	HepSymMatrix     GetKineticCovariances()   const {return _covNorm*_norm;}
	HepSymMatrix     GetCanonicalCovariances() const;
	double           GetSixDEmit()             const;
	double           GetWeight()               const {return _weight;}
	double           GetNormalisation()        const {return _norm;}
	//Covariance matrix normalised so |M|=1
	HepSymMatrix     GetNormCanCovariances()   const;
	HepSymMatrix     GetNormKinCovariances()   const {return _covNorm;}
	//Raw means covariance centered about a reference PhaseSpaceVector
	HepSymMatrix     GetRawNormCanCovariances(PhaseSpaceVector reference) const;
	//axis = (0,1,2) for (t,x,y) 
	double    GetTwoDEmit (int axis) const;
	double    GetTwoDBeta (int axis) const;
	double    GetTwoDAlpha(int axis) const;
	double    GetTwoDGamma(int axis) const;
	//Transverse quantities are mean of 2D quantities
	double    GetBetaTrans()   const;
	double    GetAlphaTrans()  const;
	double    GetGammaTrans()  const;
	double    GetEmitTrans()   const;
	//Use B_z on axis to calculate LTwiddle
	double    GetLTwiddle() const; //parameterised canonical angular momentum
	double    GetLCan()     const;
	double    GetLKin()     const;
  //disp_x = 
  double    GetDispersion     (int axis) const {return -GetKineticCovariances()[2*axis][1]*GetMean().E()/GetKineticCovariances()[1][1];}
  double    GetDispersionPrime(int axis) const {return -GetKineticCovariances()[2*axis][1]*GetMean().E()/GetKineticCovariances()[1][1];}

	//I explicitly don't use Covariance Matrix as I am worried about the mean...
	CovarianceMatrix Sum(CovarianceMatrix cov,  HepSymMatrix mat) const 
	{return CovarianceMatrix(cov.GetKineticCovariances()+mat, cov.GetMean());}
	CovarianceMatrix Diff(CovarianceMatrix cov, HepSymMatrix mat) const
	{return CovarianceMatrix(cov.GetKineticCovariances()-mat, cov.GetMean());}
	//Set various data
	void SetWeight(double weight) {_weight = weight;}
	void SetMean(PhaseSpaceVector mean) {_mean = mean;}
	//Set full covariance matrix; HepMatrix should be 6x6; 
	//determinant is covariances' determinant - set it negative and I will calculate it myself
	void SetCovariances(HepSymMatrix covariances, double determinant=-1);
	//using canonical momenta
	void SetCanonicalCovariances(HepSymMatrix covariances, double determinant=-1);
	//Set longitudinal and transverse covariances from twiss parameters
	void SetCovariances(double emittance_t, double beta_t, double alpha_t, double Ltwiddle_t, 
	                    double emittance_l, double beta_l, double alpha_l);
	void SetCovariances(double emittance_x, double beta_x, double alpha_x,
	                    double emittance_y, double beta_y, double alpha_y,
	                    double emittance_l, double beta_l, double alpha_l);
	void SetDispersions     (double dispersion_x, double dispersion_y);
  void SetDispersionsPrime(double disp_prime_x, double disp_prime_y);
	//Set covariance matrix using a covariance matrix normalised to determinant = 1
	void SetNormCovariances(HepSymMatrix covariances) {_covNorm = covariances;}
	//Raw means covariance centered about some other particle (not the mean)
	//Assumes covariance matrix is normalised to determinant 1 with norm the normalisation factor
	//Assumes mean has already been set elsewhere
	void SetRawNormCanCovariances(HepSymMatrix rawCov, double norm, PhaseSpaceVector reference);

	//"Convolution" of a covariance matrix and some other distribution
	CovarianceMatrix   operator + (const HepSymMatrix& cov) const  { return  Sum(*this,cov);}
	CovarianceMatrix & operator +=(const HepSymMatrix& cov)   { *this = Sum(*this,cov); return *this;}
	CovarianceMatrix   operator - (const HepSymMatrix& cov) const  {return  Diff(*this,cov);}
	CovarianceMatrix & operator -=(const HepSymMatrix& cov)   {*this = Diff(*this,cov); return *this;}

	//Analysis Plane output
	AnalysisPlaneBank GetAnalysisPlaneBank() const;

	std::ostream& Print(std::ostream& out) const 
	{ out << GetMean() << GetKineticCovariances(); return out;}

	//Do an interpolation of covMatrices using some phase space variable
	//covMatrix must be ordered; axis controls the variable as chosen by PhaseSpaceVector(axis)
	static CovarianceMatrix Interpolate(std::vector<CovarianceMatrix> cov, std::string variableName, double variable);
	//Return true if the Covariance matrix is positive definite
	static bool IsPositiveDefinite(HepSymMatrix mat);
	bool        IsPositiveDefinite() {return IsPositiveDefinite(_covNorm);}
	//Return a rotated covariance matrix
	CovarianceMatrix Rotate(double angle) const;
	//Return rotation matrix in transverse
	static HepMatrix Rotation(double angle);

	static std::vector<std::string> bunchVariables()  
		{return std::vector<std::string>(_bunchVars, _bunchVars+23);}
	static std::vector<std::string> psvVariables();
	static std::vector<std::string> variableTypes()
		{return std::vector<std::string>(_varTypes, _varTypes+5);}
	static std::vector<std::string> listOfMyPhaseSpaceVariables()
		{return std::vector<std::string>(_phaseSpace, _phaseSpace+6);}



	double SingleParticleVariable(PhaseSpaceVector psv, std::string variable); //amplitude + normal psv variables etc
	double BunchVariable(std::string variable) const; //beta, emittance, etc
	void   SetBunchVariable(std::string variable, double value);

	double FullVariable(std::string variable) const; //format <variable type> <aux1> <aux2> ... covariance, standard deviation, etc
	void   SetFullVariable(std::string variable, double value);
	bool   isBad();
	static std::vector<std::string> listOfAuxiliaryVariables1(std::string variable);
	static std::vector<std::string> listOfAuxiliaryVariables2(std::string variable);

private: 
	//Returns a covariance matrix in kinetic form to canonical or vice versa
	HepSymMatrix TransformToKinetic(HepSymMatrix can, double Bz, double E, double dBdx) const;
	HepSymMatrix TransformToCanonical(HepSymMatrix kin, double Bz, double E, double dBdx) const;
	//Get solenoid kappa
	double GetKappa(double Bz, double p) const {return 150*Bz/p;}
	//Get solenoid b0
	double GetB0(double Bz) const {return 150*Bz;}
	HepSymMatrix _covNorm; //normalised so determinant is 1
	double       _norm;
	PhaseSpaceVector   _mean;
	double             _weight;
	static std::string LowerCase(std::string in) {for(unsigned int i=0; i<in.size(); i++) in[i] = tolower(in[i]); return in;}
	static const std::string _bunchVars[];
	static const std::string _psvVars[]; //mixed covariance matrix/phase space vector variables
	static const std::string _varTypes[];
	static const std::string _phaseSpace[]; //phase space 6-vector
	static const std::string _traceSpace[]; //trace space 6-vector
};

std::ostream& operator<<(std::ostream& out, CovarianceMatrix cov);
//Return particle amplitudes
double  GetTwoDAmp(int axis, PhaseSpaceVector event, CovarianceMatrix Covariance);
double  GetAmpTrans(PhaseSpaceVector event, CovarianceMatrix Covariance);
double  GetSixDAmp(PhaseSpaceVector event, CovarianceMatrix Covariance);

//
double  GetChromaticAmpTrans(PhaseSpaceVector event, CovarianceMatrix Covariance);

//Return particle amplitudes
//Speed optimisation using precalculated matrix inverse and emittance
//Maybe should sit in PhaseSpaceVector?
double  GetTwoDAmp(int axis, PhaseSpaceVector event, PhaseSpaceVector mean, HepSymMatrix inverseCovariance, double emittance);
double  GetAmpTrans(PhaseSpaceVector event, PhaseSpaceVector mean, HepSymMatrix inverseCovariance, double emittance);
double  GetSixDAmp(PhaseSpaceVector event, PhaseSpaceVector mean, HepSymMatrix inverseCovariance, double emittance);

//container for all moments up to arbitrary order
//if this becomes useful it should get its own file
class Tensor;

class MomentHeap
{
public:
	//MomentHeap now controls memory allocated to higherMoments
	MomentHeap(int maxOrder, HepVector mean, double mass); //central moments => needs mean
	MomentHeap(CovarianceMatrix covMatrix, std::vector<Tensor*> higherMoments);
	~MomentHeap();

	CovarianceMatrix GetCovarianceMatrix() const {return CovarianceMatrix( m_vars, PhaseSpaceVector(m_mean, m_mass) );}
	double           GetMoment(std::vector<int> position) const;
	//Watch out, fields below the diagonal will not be filled - need to do a symmetric tensor class sometime
	const Tensor*    GetTensor(int order) const;

	void             Add(double value, std::vector<int> place);
	std::ostream&    Print(std::ostream& out) const;
	int              MaxOrder() const {return m_higherMoments.size() + 2;}
	//Return the polynomial w(\vec{u}) that returns a statistical weight w for a phase space vector u
	//that maps the moments in to the moments target
	static G4MICE::PolynomialVector Weighting(MomentHeap in, MomentHeap target, int order);

private:
  double        m_mass;
	HepVector     m_mean;
	HepSymMatrix  m_vars;
	std::vector<Tensor*> m_higherMoments;

};

/*
class MomentHeap //A container for polynomial vector - loses some stuff
{
public:
	//MomentHeap now controls memory allocated to higherMoments
	MomentHeap(int maxOrder, HepVector mean); //central moments => needs mean
	MomentHeap();
	~MomentHeap();

	CovarianceMatrix GetCovarianceMatrix()                const;
	double           GetMoment(std::vector<int> position) const;
	double           SetMoment(std::vector<int> position) const;

	void             Add  (double value, std::vector<int> place);
	std::ostream&    Print(std::ostream& out) const;
	int              MaxOrder() const;
	//Return the polynomial w(\vec{u}) that returns a statistical weight w for a phase space vector u
	//that maps the moments in to the moments target
	static G4MICE::PolynomialVector Weighting(MomentHeap in, MomentHeap target, int order);

private:
	G4MICE::PolynomialVector * moments;
};
*/

std::ostream& operator<<(std::ostream& out, const MomentHeap& heap);

#endif

