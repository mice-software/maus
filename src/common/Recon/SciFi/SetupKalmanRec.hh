// SetupKalmanRec.hh
//
// M.Ellis 20/10/20003
//
// Code to setup the Kalman package for the reconstruction of the MICE SciFi tracker

/**
* setupSciFiKalman is a routine to initialise the Kalman track fitting package
* with the geometry, material description and navigation logic for the 
* Scintillating Fibre tracker option.
*/

#include "Interface/MICERun.hh"

#include "recpack/EAlgebra.h"

using namespace Recpack;

void	setupSciFiKalman( MICERun* );

void	create_setup( MICERun* );

void	set_verbosity( MICERun* );

void	setupSciFiKalman( MICERun*, bool verbose );

void	addVolumeToKalman( MICERun*, MiceModule* );

void    addSurfacesDaughters( MiceModule* module );

