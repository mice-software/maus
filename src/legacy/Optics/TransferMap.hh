#ifndef TransferMap_hh
#define TransferMap_hh

#include <ostream>
#include <vector>
#include "Interface/PolynomialVector.hh"
#include "Interface/Squeal.hh"
#include "CovarianceMatrix.hh"
#include "Tensor.hh"
#include "Tensor3.hh"

//Forward declarations for OpticsModel.hh
class OpticsModel;

namespace MAUS
{

//Forward declarations for src/legacy/Interface/PolynomialVector.hh
class PolynomialVector;

//Forward declarations for PhaseSpaceVector.hh
class PhaseSpaceVector;

/** @class TransferMap is a polynomial mapping, M, of a phase space vector from
 *  a plane at Z1 to another plane at Z2 such that a PhaseSpaceVector with
 *  coordinates U transforms like (using Einstein's summation convention)
 *    U(Z2)_p = M_ip U(Z1)_i + M_ijp U(Z1)_i U(Z1)_j
 *            + M_ijkp U(Z1)_i U(Z1)_j U(Z1)_k + ...
 *  extending to arbitrary order. A reference trajectory is assumed, that is the
 *  transformation is applied about some phase space vector that is taken to be
 *  a zero point. In other words, U(Z) is actually the delta vector from some
 *  reference trajectory U_0:
 *     V(Z) = U_0(Z) + U(Z),
 *  where V(Z) is the phase space vector that the TransferMap is applied to.
 */
class TransferMap
{
public:
  //******************************
  // Constructors
  //******************************
   
  /** @brief constructor for different input and output reference trajectories.
   *  @params polynomial                the mapping as a polynomial vector
   *  @params reference_trajectory_in   input reference trajectory
   *  @params reference_trajectory_out  output reference trajectory
   */
	TransferMap(const PolynomialVector& polynomial,
              const PhaseSpaceVector& reference_trajectory_in,
              const PhaseSpaceVector& reference_trajectory_out);

  /** @brief constructor for identical input and output reference trajectories.
   *  @params polynomial            the mapping as a polynomial vector
   *  @params reference_trajectory  input/output reference trajectory
   */
  TransferMap(const PolynomialVector& polynomial,
              const PhaseSpaceVector& reference_trajectory);

  /** @brief copy constructor
   */
  TransferMap(const TransferMap& original_instance);

  /** @brief destructor
   */
	~TransferMap() {;}

  //******************************
  // First-order Map Functions
  //******************************
  
	Matrix<double> CreateTransferMatrix() const;
  
  //******************************
  //          operators
  //******************************

  /** @brief transports a beam envelope (covariance matrix) using a first-order
   *  transfer mapping (transfer matrix). The function performs a similarity
   *  transform on the covariance matrix: M^T C M.
   *
   *  @params aCovMatrix the matrix of second moments of the phase space
   *                     variables {t, E, x, Px, y, Py} to be transported
   */
	CovarianceMatrix operator*(CovarianceMatrix const & covariances) const;

  /** @brief transports a phase space vector ({t, E, x, Px, y, Py}) using a
   *  polynomial vector mapping. The matrix of polynomial vector coefficients
   *  operates on the delta of the initial vector and the initial
   *  reference trajectory vector. The final reference trajectory is added onto
   *  the transformed delta and returns the resulting vector.
   *
   *  @params aPhaseSpaceVector the phase space vector to be transported
   */
	PhaseSpaceVector operator*(PhaseSpaceVector const & vector) const;

	friend std::ostream& operator<<(std::ostream& out, const TransferMap& tm);

protected:

   TransferMap();

  /** @brief set the polynomial mapping as well as the input and output
   *  reference trajectories.
   *  @params polynomial                the mapping as a polynomial vector
   *  @params reference_trajectory_in   input reference trajectory
   *  @params reference_trajectory_out  output reference trajectory
   */
  void Initialize(const PolynomialVector& polynomial,
									const PhaseSpaceVector& reference_trajectory_in,
									const PhaseSpaceVector& reference_trajectory_out);
  
	PolynomialVector					polynomial_;
	PhaseSpaceVector					reference_trajectory_in_;
	PhaseSpaceVector					reference_trajectory_out_;
};

std::ostream& operator<<(std::ostream& out, const TransferMap& map);

} //namespace MAUS












//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Legacy TransferMap
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "src/legacy/Interface/Differentiator.hh"
#include "CovarianceMatrix.hh"
#include "PhaseSpaceVector.hh"
#include "OpticsModel.hh"
#include "Tensor3.hh"

//
//TransferMap is a polynomial mapping of a phase space vector from a plane at Z1 to another plane at Z2,
//such that PhaseSpaceVector with coordinates U transform like
//   U(Z2)_p = M_ip U(Z1)_i + M_ijp U(Z1)_i U(Z1)_j + M_ijkp U(Z1)_i U(Z1)_j U(Z1)_k + ...
//extending to arbitrary order. A reference trajectory is assumed, that is the transformation is applied about
//some phase space vector that is taken to be a zero point.
//
//Currently first order transformations use the CLHEP::HepMatrix object, second order transformations use the
//Tensor3 object and higher order transformations use the Tensor object
//
//At some point I would like to move to the more general G4MICE::PolynomialVector object. For now I have implemented an 
//interface to this object
//
//

class TransferMap
{
public:
	//Create the transfer map that transports from referenceIn to referenceOut using firstOrderMap
	TransferMap(HepMatrix firstOrderMap, PhaseSpaceVector referenceIn, PhaseSpaceVector referenceOut, 
	            OpticsModel* Optics);
	TransferMap(HepMatrix transverseMap, HepMatrix longitudinalMap, PhaseSpaceVector referenceIn, 
                    PhaseSpaceVector referenceOut, OpticsModel* Optics);
	//Create a transfer map that does nothing i.e. referenceIn=referenceOut, M=1
	TransferMap(PhaseSpaceVector referenceIn, OpticsModel* Optics);
	TransferMap();
	~TransferMap() {;}
	//Set up
	void SetFirstOrderMap(const HepMatrix transverseMap, const HepMatrix longitudinalMap);
	void SetFirstOrderMap(const HepMatrix firstOrderMap);
	void SetSecondOrderMap(Tensor3 secondOrderMap)            {_secondOrderMap = secondOrderMap;}
	void SetReferenceIn(const PhaseSpaceVector referenceIn)   {_referenceTrajectoryIn = referenceIn;}	
	void SetReferenceOut(const PhaseSpaceVector referenceOut) {_referenceTrajectoryOut = referenceOut;}
	void SetReferenceOutZ(double z)                           {_referenceTrajectoryOut.setZ(z);}
	void IncrementReferenceOutZ(double dz)                    {SetReferenceOutZ(_referenceTrajectoryOut.z()+dz);}
	void SetOpticsModel(OpticsModel* optics)                  {_optics = optics;}
	//Accessors or whatever
	PhaseSpaceVector GetReferenceIn()  const    {return _referenceTrajectoryIn;}
	PhaseSpaceVector GetReferenceOut() const    {return _referenceTrajectoryOut;}
	//Accessor for first order matrix
	HepMatrix GetFirstOrderMap()   const;
	HepMatrix GetTransverseMap()   const {return _firstOrderMap.sub(3,6,3,6);}
	HepMatrix GetLongitudinalMap() const {return _firstOrderMap.sub(1,2,1,2);}
	//Accessor for second and higher order maps
	Tensor3   GetSecondOrderMap()  const {return _secondOrderMap;}
	Tensor    GetMap(int order)    const;
	//Methods to transport objects
	//Return value is M^T V M
	CovarianceMatrix operator*(const CovarianceMatrix& aCovMatrix) const;
	//Return value is M u
	PhaseSpaceVector operator*(const PhaseSpaceVector& aPhaseSpaceVector) const;
	//Return value is M1*M2 where *this is M1 and aTransferMAp is M2
	TransferMap      operator*(const TransferMap&      aTransferMap) const;	
	TransferMap      operator*=(const TransferMap&     aTransferMap);
	//Return transfer map rotated by an angle theta
	TransferMap      Rotate(double angle) const;
	//Return the inverse of the transfer map
	TransferMap      Inv(int & failflag) const; 
	TransferMap      Inv() const {int i; return Inv(i);}
	//determinant of the 6d transfer map
	double           Det() const {return GetFirstOrderMap().determinant();}
	//decompose into a "matched" covariance matrix
	CovarianceMatrix Decompose(double em_t, double em_x, double em_y) const;
	//rotate by angle then decompose into a "matched" covariance matrix... then rotate cov matrix back
	CovarianceMatrix Decompose(double angle, double em_t, double em_x, double em_y) const;
	//return a cov matrix with coefficients for sxx_out/sxx_in, spp_out/spp_in and sxp_out/sxp_in
	std::vector<CovarianceMatrix> Decompose(double sxx, double sxp, double spp, double em_t, double em_x, 
	                                        double em_y) const;
	CovarianceMatrix Decompose(double sxx, double sxp, double spp, 
	                           double em_t, double em_x, double em_y, double angle) const;
	//Get the phase advance of the transfer matrix in the domain (0, pi), given by acos((m[0][0]+m[1][1])/2.); 
  //units are radians; if LarmorAngle is specified, unrotates matrix first
	double           PhaseAdvance(int axis) const;
	double           PhaseAdvance(int axis, double LarmorAngle) const;
  //
	G4MICE::PolynomialVector* GetPolynomialVector() {return _polynomial;}
	void              SetPolynomialVector(G4MICE::PolynomialVector* poly) {_polynomial = poly;}
	bool              IsCanonical()         {return _canonicalMap;}
	bool              IsCanonical(bool can) {_canonicalMap = can; return _canonicalMap;}

	//order
	static void      SetOrder(int order) {_order = order;}
	static int       GetOrder()          {return _order;}
	friend std::ostream& operator<<(std::ostream& out, TransferMap tm);

private:
	//Transfer maps of various orders
	HepMatrix            _firstOrderMap;
	Tensor3              _secondOrderMap;
	std::vector<Tensor*> _higherOrderMaps;
	//The reference trajectory is a property of the transfer map
	PhaseSpaceVector     _referenceTrajectoryIn;
	PhaseSpaceVector     _referenceTrajectoryOut;
	//Need this for transporting phase space vectors
	OpticsModel*         _optics;
	G4MICE::PolynomialVector*    _polynomial; //set to NULL to disable
	static int           _order;
	bool                 _canonicalMap;

};

std::ostream& operator<<(std::ostream& out, TransferMap tm);

#endif

