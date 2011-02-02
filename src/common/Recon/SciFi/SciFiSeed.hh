// SciFiSeed.hh
//
// Routine that takes three space point positions and a magnetic field value and determines the 3 momentum of the particle
//
// A. Fish 31/8/2006

#ifndef SCIFISEED_HH
#define SCIFISEED_HH

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

bool	momentumFromPoints( CLHEP::Hep3Vector p1, CLHEP::Hep3Vector p2, CLHEP::Hep3Vector p3, double sigx, double sigy, double B, double Q, double& px, double& py, double& pz, int& pos, double& sigpx, double& sigpy, double& sigpz, bool mcs );

#endif
