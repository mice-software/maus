// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: micegsl.hh,v 1.1 2002-08-27 05:14:26 torun Exp $  $Name:  $
//
// Simple C++ interface to GNU Scientific Library
//
// Yagmur Torun

#ifndef MICEGSL_HH
#define MICEGSL_HH 1

struct GSL {

  struct SpecialFunctions {

    struct Bessel {
      static double J0(const double x);
      static double J1(const double x);
    };

    struct EllipticIntegrals {
      static double IncompleteE(const double phi, const double k);
      static double IncompleteF(const double phi, const double k);
      static double IncompleteP(const double phi, const double k,
                                const double n);
    };

  };

};

#endif
