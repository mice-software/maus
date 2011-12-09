#include <sstream>
#include <iomanip>
#include <cstdlib>

#include "Interface/Squeal.hh"

#include "CovarianceMatrix.hh"

using CLHEP::HepSymMatrix;
using CLHEP::HepVector;
using CLHEP::c_light;

namespace MAUS
{

//############################
// Free Functions
//############################

//****************************
// Conversion Functions
//****************************

CovarianceMatrix rotate(const CovarianceMatrix& covariances, double angle)
{
	//FIXME
  HepMatrix rotation(6,6,1);
  double fcos = cos(angle);
  double fsin = sin(angle);
  
  rotation      *=  fcos;
  rotation[0][0] =  1.;
  rotation[1][1] =  1.;
  rotation[4][2] = -fsin;
  rotation[5][3] = -fsin;
  rotation[2][4] =  fsin;
  rotation[3][5] =  fsin;
  
  ((HepSymMatrix) this) = similarity(rotation);
}

//############################
// CovarianceMatrix
//############################

//****************************
// Constructors
//****************************

CovarianceMatrix::CovarianceMatrix() : SymmetricMatrix(6)
{
	for (size_t index=1; index<=6; ++index)
	{
		(*this)(index, index) = 1.0;
	}
}

CovarianceMatrix::CovarianceMatrix(double emittance_t,
                                   double beta_t,
                                   double alpha_t,
                                   double Ltwiddle_t,
                                   double emittance_l,
                                   double beta_l,
                                   double alpha_l,
                                   PhaseSpaceVector const & mean)
	: SymmetricMatrix(6)
{
  SetCovariances(emittance_t, beta_t, alpha_t, Ltwiddle_t, emittance_l, beta_l,
                 alpha_l, mean);
}

CovarianceMatrix::CovarianceMatrix(double emittance_x,
                                   double beta_x,
                                   double alpha_x,
                                   double emittance_y,
                                   double beta_y,
                                   double alpha_y,
                                   double emittance_l,
                                   double beta_l,
                                   double alpha_l,
                                   PhaseSpaceVector const & means)
	: SymmetricMatrix(6)
{
  SetCovariances(emittance_x, beta_x, alpha_x, emittance_y, beta_y, alpha_y,
                 emittance_l, beta_l, alpha_l, means);
}

CovarianceMatrix(CovarianceMatrix& covariances)
	: SymmetricMatrix((SymmetricMatrix) covariances)
{ }

CovarianceMatrix(const HepSymMatrix& covariances)
	: SymmetricMatrix(covariances)
{ }

void CovarianceMatrix::SetCovariances(double mass, double momentum,
                                      double charge,double emittance_t,
                                      double beta_t, double alpha_t,
                                      double Ltwiddle_t, double emittance_l,
                                      double beta_l, double alpha_l,
                                      double Bz, double dispersion_x,
                                      double dispersion_prime_x,
                                      double dispersion_y,
                                      double dispersion_prime_y)
{
  double energy     = sqrt(momentum * momentum + mass * mass);
  double kappa      = CLHEP::c_light * Bz / (2.* momentum);
  double gamma_t    = (1 + alpha_t*alpha_t
                       + (beta_t*kappa - Ltwiddle_t)*(beta_t*kappa-Ltwiddle_t))
                    / beta_t;
  double gamma_l    = (1+alpha_l*alpha_l)/beta_l;

  //calculate the covariances <A B> = sigma(A, B)
  double sigma_t_t  = emittance_l * mass * beta_l / momentum;
  double sigma_E_E  = emittance_l * mass * gamma_l * momentum;
  double sigma_t_E  = -emittance_l * mass * alpha_l;
  double sigma_x_x  =  emittance_t * mass * beta_t / momentum;
  double sigma_Px_Px=  emittance_t * mass * momentum * gamma_t;
  double sigma_x_Px = -emittance_t * mass * alpha_t;
  double sigma_x_Py = -emittance_t * mass * (beta_t*kappa-Ltwiddle_t) * charge;
  double sigma_E_x  = -dispersion_x * sigma_E_E / energy;
  double sigma_E_y  = -dispersion_y * sigma_E_E / energy;
  double sigma_E_Px = dispersion_prime_x * sigma_E_E / energy;
  double sigma_E_Py = dispersion_prime_y * sigma_E_E / energy;
  
  //put the covariances in the correct position within the matrix
  this[0][0] = sigma_t_t;
  this[1][1] = sigma_E_E;
  this[0][1] = sigma_t_E;
  //<x x> = <y y>
  this[2][2] = this[4][4] = sigma_x_x;
  //<Px Px> = <Py Py>
  this[3][3] = this[5][5] = sigma_Px_Px;
  //<x Px> = <y Py>
  this[2][3] = this[4][5] = sigma_x_Px;
  //<Px y> = -<x Py>
  this[3][4] = -sigma_x_Py;
  this[2][5] = sigma_x_Py;
  this[1][2] = sigma_E_x;
  this[1][4] = sigma_E_y;
  this[1][3] = sigma_E_Px;
  this[1][5] = sigma_E_Py;
}

void CovarianceMatrix::SetCovariances(double mass, double momentum,
                                      double energy, double emittance_x,
                                      double beta_x, double alpha_x,
                                      double emittance_y, double beta_y,
                                      double alpha_y, double emittance_l,
                                      double beta_l, double alpha_l,
                                      double dispersion_x,
                                      double dispersion_prime_x,
                                      double dispersion_y,
                                      double dispersion_prime_y)
{
  double gamma_x = (1+alpha_x*alpha_x)/beta_x;
  double gamma_y = (1+alpha_y*alpha_y)/beta_y;
  double gamma_l = (1+alpha_l*alpha_l)/beta_l;
  
  //calculate the covariances <A B> = sigma(A, B)
  double sigma_t_t  = emittance_l * mass * beta_l / momentum;
  double sigma_E_E  = emittance_l * mass * gamma_l * momentum;
  double sigma_t_E  = -emittance_l * mass * alpha_l;
  double sigma_x_x  = emittance_x * mass * beta_x / momentum;
  double sigma_Px_Px= emittance_x * mass * momentum * gamma_x;
  double sigma_x_Px = -emittance_x * mass * alpha_x;
  double sigma_y_y  = emittance_y * mass * beta_y /momentum;
  double sigma_Py_Py= emittance_y * mass * momentum * gamma_y;
  double sigma_y_Py = -emittance_y * mass * alpha_y;
  double sigma_E_x  = dispersion_x * sigma_E_E / energy;
  double sigma_E_y  = dispersion_y * sigma_E_E / energy;
  double sigma_E_Px = dispersion_prime_x * sigma_E_E / energy;
  double sigma_E_Py = dispersion_prime_y * sigma_E_E / energy;
  
  //put the covariances in the correct position within the matrix
  this[0][0] = sigma_t_t;
  this[1][1] = sigma_E_E;
  this[0][1] = sigma_t_E;
  this[2][2] = sigma_x_x;
  this[3][3] = sigma_Px_Px;
  this[2][3] = sigma_x_Px;
  this[4][4] = sigma_y_y;
  this[5][5] = sigma_Py_Py;
  this[4][5] = sigma_y_Py;
  this[1][2] = sigma_E_x;
  this[1][4] = sigma_E_y;
  this[1][3] = sigma_E_Px;
  this[1][5] = sigma_E_Py;
  
}

bool CovarianceMatrix::IsPositiveDefinite()
{
	//FIXME
  int min_row = 1;
  for(int max_row=1; max_row<=num_row(); max_row++)
  {
    //Sylvester Criterion - all the leading principle minors must be positive
    if (sub(min_row, max_row).determinant() <= 0)
    {
      return false;
    }
  }
  
  return true;
}

} //namespace MAUS
