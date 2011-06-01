// MAUS WARNING: THIS IS LEGACY CODE.
#include "CLHEP/Matrix/SymMatrix.h"
#include "Interface/MCHit.hh"
#include "Config/MiceModule.hh"

#ifndef ModuleConverter_hh
#define ModuleConverter_hh 1

class ModuleConverter
{
public:
  //convert from Module to mchit
  static MCHit                ModuleToHit(const MiceModule* mod);
  //convert from Module to beam ellispe
  static CLHEP::HepSymMatrix  ModuleToBeamEllipse(const MiceModule* mod, MCHit hit);

	static CLHEP::HepSymMatrix SetCovariances( MCHit hit, double bz, 
                                             double emittance_t, double beta_t, double alpha_t, double Ltwiddle_t, 
                       	                     double emittance_l, double beta_l, double alpha_l,
                                             double disp_x, double disp_p_x, double disp_y, double disp_p_y);

	static CLHEP::HepSymMatrix SetCovariances( MCHit hit,
                                             double emittance_x, double beta_x, double alpha_x, double disp_x, double disp_p_x, 
	                                           double emittance_y, double beta_y, double alpha_y, double disp_y, double disp_p_y,
                        	                   double emittance_l, double beta_l, double alpha_l);
  static int    MassToPdgPid  (double mass, double tolerance); //SHOULD BE IN UTILS
  static double PdgPidToMass  (int pid); //SHOULD BE IN UTILS
  static double PdgPidToCharge(int pid); //SHOULD BE IN UTILS

private:
  static void FillPdgPidToMass();
  static void FillPdgPidToCharge();
 
  static std::map<int, double> _pdgPidToMass; //mapping pid to mass
  static std::map<int, double> _pdgPidToCharge; //mapping pid to mass
};

#endif

