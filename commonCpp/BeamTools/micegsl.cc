// @(#) $Id: micegsl.cc,v 1.3 2005-10-28 11:38:33 rogers Exp $  $Name:  $
//
// Simple C++ interface to GNU Scientific Library
//
// Yagmur Torun

#include "micegsl.hh"
#include "gsl/gsl_mode.h"
#include "gsl/gsl_errno.h"
#include "gsl/gsl_sf.h"

#include <iostream>
using namespace std;

double GSL::SpecialFunctions::Bessel::J0(const double x)
{
  gsl_sf_result result;
  int status = gsl_sf_bessel_J0_e(x, &result);
  if (status)
    cout << "gsl error(" << status << "): " << gsl_strerror(status) << endl;
  return result.val;
}

double GSL::SpecialFunctions::Bessel::J1(const double x)
{
  gsl_sf_result result;
  int status = gsl_sf_bessel_J1_e(x, &result);
  if (status)
    cout << "gsl error(" << status << ") in Bessel::J1: "
	 << gsl_strerror(status) << endl;
  return result.val;
}

double GSL::SpecialFunctions::EllipticIntegrals::IncompleteE(const double phi,
							const double k)
{
  gsl_sf_result result;
  int status = gsl_sf_ellint_E_e(phi, k, GSL_MODE_DEFAULT, &result);
  if (status)
    cout << "gsl error(" << status << ") in EllipticIntegrals::IncompleteE: "
	 << gsl_strerror(status) << endl;
  return result.val;
}

double GSL::SpecialFunctions::EllipticIntegrals::IncompleteF(const double phi,
							const double k)
{
  gsl_sf_result result;
  int status = gsl_sf_ellint_F_e(phi, k, GSL_MODE_DEFAULT, &result);
  if (status)
    cout << "gsl error(" << status << ") in EllipticIntegrals::IncompleteF: "
	 << gsl_strerror(status) << endl;
  return result.val;
}

double GSL::SpecialFunctions::EllipticIntegrals::IncompleteP(const double phi,
							const double k,
							double n)
{
  gsl_sf_result result;
  int status = gsl_sf_ellint_P_e(phi, k, n, GSL_MODE_DEFAULT, &result);
  if (status)
    cout << "gsl error(" << status << ") in EllipticIntegrals::IncompleteP: "
	 << gsl_strerror(status) << endl;
  return result.val;
}
