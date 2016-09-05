// MAUS WARNING: THIS IS LEGACY CODE.
#include "Config/MiceModule.hh"
#include "Utils/Exception.hh"
#include "Config/ModuleConverter.hh"
#include "Utils/Squeak.hh"

#include <sstream>
#include <cstdlib>

std::map<int, double> ModuleConverter::_pdgPidToMass; //mapping pid to mass
std::map<int, double> ModuleConverter::_pdgPidToCharge; //mapping pid to mass


MCHit ModuleConverter::ModuleToHit(const MiceModule* mod)
{
  MCHit hit;
  hit.setPdg(mod->propertyInt("Pid"));
  double           mass   = PdgPidToMass(hit.pdg());
  std::string      longS  = mod->propertyString("LongitudinalVariable");
  double           longD  = mod->propertyDouble(longS);
  HepLorentzVector mom(CLHEP::Hep3Vector(0,0,0), mass);
  if     (longS == "Energy")        {mom[3] = longD;      mom[2] = sqrt(longD*longD  -mass*mass);}
  else if(longS == "KineticEnergy") {mom[3] = longD+mass; mom[2] = sqrt(mom[3]*mom[3]-mass*mass);}
  else if(longS == "Momentum" || longS == "ZMomentum") {mom[2] = longD; mom[3] = sqrt(longD*longD+mass*mass);}
  else throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Beam ellipse longitudinal variable "+longS+" not recognised in Module "+mod->name(), "ModuleConverter::ModuleToHit(MiceModule*)"));
  HepLorentzVector pos(mod->globalPosition(), mod->propertyDouble("Time"));
  mom *= mod->globalRotation();
  hit.setPosition(pos.v());
  hit.setTime    (pos.t());
  hit.setMomentum(mom.v());
  hit.setEnergy  (mom.t());
  hit.setCharge  (PdgPidToCharge(hit.pdg()));
  hit.setMass    (mass);
  return hit;
}

CLHEP::HepSymMatrix ModuleConverter::ModuleToBeamEllipse(const MiceModule* mod, MCHit hit)
{
  CLHEP::HepSymMatrix cov(6,0);
  std::string ellipseDef  = mod->propertyString("EllipseDefinition");
  if(ellipseDef == "Matrix")
  {
    std::string ints[6] = {"t","E","x","Px","y","Py"};
    for(int i=0; i<6; i++)
    {
            cov[i][i] = mod->propertyDouble("Covariance("+ints[i]+","+ints[i]+")");
            for(unsigned int j=i; j<6; j++) 
                    try{cov[i][j] = mod->propertyDouble("Covariance("+ints[i]+","+ints[j]+")");} catch(...){}
    }
  }

  double dx=0., dy=0., dpx=0., dpy=0.; //dispersion, dispersion prime
  try{dx  = mod->propertyDouble("Dispersion_X"     );} catch(MAUS::Exceptions::Exception exc) {}
  try{dy  = mod->propertyDouble("Dispersion_Y"     );} catch(MAUS::Exceptions::Exception exc) {}
  try{dpx = mod->propertyDouble("DispersionPrime_X");} catch(MAUS::Exceptions::Exception exc) {}
  try{dpy = mod->propertyDouble("DispersionPrime_Y");} catch(MAUS::Exceptions::Exception exc) {}
  if(ellipseDef == "Penn")
  {
    double bz = 0;
    try{bz = mod->propertyDouble("Bz");} catch(...) {}
    cov = SetCovariances(hit, bz, mod->propertyDouble("Emittance_T"), mod->propertyDouble("Beta_T"), mod->propertyDouble("Alpha_T"), 
                         mod->propertyDouble("NormalisedAngularMomentum"),
                         mod->propertyDouble("Emittance_L"), mod->propertyDouble("Beta_L"), mod->propertyDouble("Alpha_L"),
                         dx, dpx, dy, dpy);
    
  }
  
  if(ellipseDef == "Twiss")
  {

    cov = SetCovariances(hit, mod->propertyDouble("Emittance_X"), mod->propertyDouble("Beta_X"), mod->propertyDouble("Alpha_X"), dx, dpx,
                         mod->propertyDouble("Emittance_Y"), mod->propertyDouble("Beta_Y"), mod->propertyDouble("Alpha_Y"), dy, dpy,
                         mod->propertyDouble("Emittance_L"), mod->propertyDouble("Beta_L"), mod->propertyDouble("Alpha_L"));
  }
  return cov;
}

void ModuleConverter::FillPdgPidToMass()
{
  if(_pdgPidToMass.empty())
  {
    int    pid [19] = {0, 11,          12, 13,          14, 22,      111,       211,     321,      2112,       
                             2212,1000010020,1000010030, 1000020030,1000020040,     130,     310, 311,     3122};
    double mass[19] = {0, 0.510998910, 0., 105.6583668, 0., 0., 134.9766, 139.57018, 493.667, 939.56536, 
                       938.271996, 1876.1239,  2809.432, 2809.41346, 3728.4001, 497.614, 497.614, 497.614, 1115.683};
    for(int i=0; i<19; i++) _pdgPidToMass[pid[i]] = mass[i];
  }
}

void ModuleConverter::FillPdgPidToCharge()
{
  if(_pdgPidToCharge.empty())
  {
    int    all_pids[19] = {0, 11, 12, 13,14, 22, 111, 211, 321, 2112, 2212, 130, 310, 311, 3122};
    double q       [19] = {0, -1, 0., -1, 0., 0., 0.,   1,  +1,    0,   +1,   0,   0,   0,    0};
    for(int i=0; i<19; i++) _pdgPidToCharge[all_pids[i]] = q[i];
  }
}

int ModuleConverter::MassToPdgPid(double mass, double tolerance)
{
  FillPdgPidToMass();
  for(std::map<int, double>::iterator it=_pdgPidToMass.begin(); it!=_pdgPidToMass.end(); it++)
  {
    if(it->second+tolerance > mass && it->second-tolerance < mass) return it->first;
  }
  std::stringstream iss;
  iss << mass; 
  throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise pid "+iss.str()+" for mass calculation", "ModuleConverter::MassToPdgPid"));
}

double ModuleConverter::PdgPidToMass(int pid)
{
  pid = abs(pid);
  FillPdgPidToMass();
  if(_pdgPidToMass.find(pid) != _pdgPidToMass.end()) return _pdgPidToMass[pid];
  else {std::stringstream iss; iss << pid; throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise pid "+iss.str()+" for mass calculation", "ModuleConverter::PdgPidToMass"));}
}

double ModuleConverter::PdgPidToCharge(int pid)
{
  int abs_pid = abs(pid);
  FillPdgPidToCharge();
  if(_pdgPidToCharge.find(abs_pid) != _pdgPidToCharge.end()) return _pdgPidToCharge[abs_pid]*abs_pid/pid*CLHEP::eplus;
  else {std::stringstream iss; iss << pid; throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise pid "+iss.str()+" for charge calculation", "ModuleConverter::PdgPidToCharge"));}
}

CLHEP::HepSymMatrix ModuleConverter::SetCovariances(MCHit hit, double bz,
                                      double emittance_t, double beta_t, double alpha_t, double Ltwiddle_t,
                                      double emittance_l, double beta_l, double alpha_l,
                                      double disp_x, double disp_p_x, double disp_y, double disp_p_y)
{
	double m = hit.mass();
	double p = hit.momentum().mag();
	double E = sqrt(p*p+m*m);
	double k = CLHEP::c_light*bz/2./p;
	double gamma_t = (1+alpha_t*alpha_t+(beta_t*k - Ltwiddle_t)*(beta_t*k-Ltwiddle_t))/beta_t;
	double gamma_l = (1+alpha_l*alpha_l)/beta_l;
	double sigmaxx   =  emittance_t*m*beta_t/p;
	double sigmaxpx  = -emittance_t*m*alpha_t;
	double sigmapxpx =  emittance_t*m*p*gamma_t;
	double sigmaxpy  = -emittance_t*m*(beta_t*k-Ltwiddle_t)*hit.charge();

  CLHEP::HepSymMatrix _covNorm(6,0);
	_covNorm[0][0] =  emittance_l*m*beta_l/p;
	_covNorm[1][1] =  emittance_l*m*gamma_l*p;
	_covNorm[0][1] = -emittance_l*m*alpha_l;
	_covNorm[2][2] =  _covNorm[4][4] = sigmaxx;
	_covNorm[3][3] =  _covNorm[5][5] = sigmapxpx;
	_covNorm[2][3] =  _covNorm[4][5] = sigmaxpx;
	_covNorm[3][4] = -sigmaxpy;
	_covNorm[2][5] =  sigmaxpy;
  _covNorm[1][2] =  -disp_x  *_covNorm[1][1]/E;
  _covNorm[1][4] =  -disp_y  *_covNorm[1][1]/E;
  _covNorm[1][3] =  disp_p_x*_covNorm[1][1]/E;
  _covNorm[1][5] =  disp_p_y*_covNorm[1][1]/E;
  return _covNorm;
}

CLHEP::HepSymMatrix ModuleConverter::SetCovariances(MCHit hit,
                                      double emittance_x, double beta_x, double alpha_x, double disp_x, double disp_p_x,
                                      double emittance_y, double beta_y, double alpha_y, double disp_y, double disp_p_y,
                                      double emittance_l, double beta_l, double alpha_l)
{
	double m = hit.mass();
	double p = hit.momentum().mag();
  double E = hit.energy();
	double gamma_x = (1+alpha_x*alpha_x)/beta_x;
	double gamma_y = (1+alpha_y*alpha_y)/beta_y;
	double gamma_l = (1+alpha_l*alpha_l)/beta_l;

  CLHEP::HepSymMatrix _covNorm(6,0);
	_covNorm[0][0] =  emittance_l*m*beta_l/p;
	_covNorm[1][1] =  emittance_l*m*gamma_l*p;
	_covNorm[0][1] = -emittance_l*m*alpha_l;
	_covNorm[2][2] =  emittance_x*m*beta_x/p;
	_covNorm[3][3] =  emittance_x*m*p*gamma_x;
	_covNorm[2][3] = -emittance_x*m*alpha_x;
	_covNorm[4][4] =  emittance_y*m*beta_y/p;
	_covNorm[5][5] =  emittance_y*m*p*gamma_y;
	_covNorm[4][5] = -emittance_y*m*alpha_y;
  _covNorm[1][2] =  disp_x*_covNorm[1][1]/E;
  _covNorm[1][4] =  disp_y*_covNorm[1][1]/E;
  _covNorm[1][3] =  disp_p_x*_covNorm[1][1]/E;
  _covNorm[1][5] =  disp_p_y*_covNorm[1][1]/E;
  return _covNorm;
}


