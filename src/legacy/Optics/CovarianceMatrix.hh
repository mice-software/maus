#ifndef CovarianceMatrix_hh
#define CovarianceMatrix_hh

#include <vector>

#include "Interface/SymmetricMatrix.hh"

#include "PhaseSpaceVector.hh"

namespace MAUS
{


/* @brief Return a rotated covariance matrix
 */
CovarianceMatrix rotate(const CovarianceMatrix& covariances, double angle);


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
  
  /* @brief Copy constructor
	 */
  CovarianceMatrix(CovarianceMatrix& covariances);
  
  /* @brief "Copy constructor" for HepSymMatrix
	 */
  CovarianceMatrix(const HepSymMatrix& covariances);

  //********************************
  // *** Public member functions ***
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
}

} //namespace MAUS

std::ostream& operator<<(std::ostream& out, CovarianceMatrix cov);

#endif

