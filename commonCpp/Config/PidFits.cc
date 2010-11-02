#include "PidFits.hh"

#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "MiceModule.hh"

#include "gsl/gsl_sf_gamma.h"

using CLHEP::mm;
using CLHEP::cm;
using CLHEP::eplus;

///////////////////////////////////////////////////////////////////////

PidFits::PidFits()
{
  m_tof1Thickness = 0; // cm
  m_tof2Thickness = 0; // cm
  m_tof1Pos = 0.;
  m_tof2Pos = 0.;

  c = 299.792458; //mm/ns
  m_mu = 105.6583692; //MeV/c^2
  mass2 = m_mu*m_mu;

  miceMemory.addNew( Memory::PidFits );
  // the MICEEvent
  MICEEvent theEvent;

  // the MICE Run
  MICERun theRun;

  // the MiceModel
  MiceModule* model = new MiceModule( MyDataCards.fetchValueString( "MiceModel" ) );
  theRun.miceModule = model;

  theRun.DataCards = & MyDataCards;

  std::vector<const MiceModule*> modulesCal =
    theRun.miceModule->findModulesByPropertyString( "SensitiveDetector", "EMCAL" );
  std::vector<const MiceModule*> modulesTof =
    theRun.miceModule->findModulesByPropertyString( "Detector", "TOF" );
  std::vector<const MiceModule*> modulesDiffuser =
    theRun.miceModule->findModulesByPropertyString( "Diffuser", "true" );

  // count layers in cal and fill a vector with their thicknesses
  m_layers = 0;
  fillCalLayerVector(modulesCal);
  setTOFProperties(modulesTof);
  setDiffuserProperties(modulesDiffuser);

}

///////////////////////////////////////////////////////////////////////

PidFits::~PidFits()
{
  miceMemory.addDelete( Memory::PidFits );
}


///////////////////////////////////////////////////////////////////////

PidFits* PidFits::fInstance = (PidFits*) NULL;

PidFits* PidFits::getInstance()
{

  if(!fInstance) fInstance = new PidFits;
  return fInstance;

}

void PidFits::countCalLayers(std::vector<const MiceModule*> modulesCal)
{
  // count the number of layers in the calorimeter
  for (unsigned int i = 0; i<modulesCal.size(); ++i){
    if (modulesCal[i]->propertyInt("Layer") >= m_layers){
      m_layers = modulesCal[i]->propertyInt("Layer")+1; // layers start counting from 0
      m_cellLength.push_back(0); // will fill details later
    }
  }
}

void PidFits::fillCalLayerVector(std::vector<const MiceModule*> modulesCal)
{
  countCalLayers(modulesCal);
  // fill a vector containing thickness of calorimeter layers
  for (unsigned int i = 0; i<modulesCal.size(); ++i){
    if (m_cellLength[modulesCal[i]->propertyInt("Layer")] == 0)
      m_cellLength[modulesCal[i]->propertyInt("Layer")] =
        modulesCal[i]->dimensions().z();
  }
}

void PidFits::setTOFProperties(std::vector<const MiceModule*> modulesTof)
{
  for (unsigned int i = 0; i<modulesTof.size(); ++i){
    if (modulesTof[i]->propertyInt("Station") == 1){
      m_tof1Thickness = modulesTof[i]->dimensions().z();
      m_tof1Pos = modulesTof[i]->globalPosition().z();
    } else if (modulesTof[i]->propertyInt("Station") == 2){
      m_tof2Thickness = modulesTof[i]->dimensions().z();
      m_tof2Pos = modulesTof[i]->globalPosition().z();
    }
  }
}

void PidFits::setDiffuserProperties(std::vector<const MiceModule*> modulesDiffuser)
{
  m_diffuserThickness = 0; m_diffuserPos = -6094.*mm;
  for (unsigned int i = 0; i<modulesDiffuser.size(); ++i){
    if (m_diffuserThickness == 0){
//       m_diffuserThickness = modulesDiffuser[i]->dimensions().z();
      m_diffuserThickness = modulesDiffuser[i]->dimensions().y(); // that is the thickness...
      m_diffuserPos = modulesDiffuser[i]->globalPosition().z();
    }
  }
//   std::cout << "diffs=" <<  modulesDiffuser.size() << " "<< modulesDiffuser[0]->dimensions().x() << " "<< modulesDiffuser[0]->dimensions().y() << " "<< modulesDiffuser[0]->dimensions().z() << " " << m_diffuserPos << std::endl;
}

double PidFits::getBaryExpSW(HepLorentzVector* p)
{
  double baryExp = -1.;
  if (p->isTimelike() == false) return 0.5*m_cellLength[0]; //null vectors are no good for .gamma()
  double x = p->beta()*p->gamma()*p->cosTheta(); // cos theta for part of track transversally
  //fits for plastic part, p must be after layer 0:
  if (x<=1e-6) { // beta is limited by machine precision and never 0
    baryExp = 0.5*m_cellLength[0];
  } else if (x<2.5) {
    baryExp = m_cellLength[0] -8.268307 -11.895816*x +70.249759*x*x +14.155903*x*x*x -6.172258*x*x*x*x;
  } else {
    baryExp = m_cellLength[0] +1946.499090 -1087.370892*x +185.505118*x*x;
  }
  return baryExp;
}

double PidFits::getADCExpSW(double e)
{
  double adcExp = -1.;
  // new fit May'06
  if (e<160.){ // inside the KL layer
    adcExp =  -312.137363+2.871344*e;
  } else if (e<315.){
    adcExp = -14283.553783 +213.449080*e -1.195022*e*e +0.003161*e*e*e -3.127e-6*e*e*e*e;
  } else { // punching through
    adcExp = 40830.248127 -222.008603*e + 0.317612*e*e;
  }
  return adcExp;
}

double PidFits::getTofExp(HepLorentzVector* pUp, HepLorentzVector* pDn, HepLorentzVector* xUp, HepLorentzVector* xDn)
{
  double tofExp = -1.;
  // It would be ok here to ask for Lorentz boost directly since the muon mass has already been assumed in the functions that returned the Lorentz boost. However it will return nan for a zero vector.
  double tUp =getTofExpUpstream(pUp, xUp);
  double tDn = getTofExpDownstream(pDn, xDn);
  double tMid = getTofExpCenter(pUp, pDn, xUp, xDn);
  tofExp = tUp+tDn+tMid;
  return tofExp;
}

double PidFits::getTofExpCenter(HepLorentzVector* pUp, HepLorentzVector* pDn, HepLorentzVector* xUp, HepLorentzVector* xDn)
{
  double dz = xDn->z()-xUp->z();
  double betaZUpC = c*pUp->z()/sqrt(pUp->vect().mag2()+mass2);
  double betaZDnC = c*pDn->z()/sqrt(pDn->vect().mag2()+mass2);
  double t1 = dz/(betaZUpC-betaZDnC)*(log(betaZUpC)-log(betaZDnC)); // assuming beta_z changes lineary
  double sintanDn = sin(pDn->theta())*tan(pDn->theta());
  double corrSinTanDn = -0.2386-1.651*sintanDn-14.64*pow(sintanDn,2)+14.2*pow(sintanDn,3)-4.281*pow(sintanDn,4);
  double corrSinTanUp = 0.2274 -2.966*sintanDn-2.927*pow(sintanDn,2);
  double corrDeltaSinTheta = -0.6512+0.491*(sin(pDn->theta())-sin(pUp->theta()));
  double t = t1 +corrSinTanDn +corrSinTanUp +corrDeltaSinTheta;
  return t;
}

double PidFits::getTofExpDownstream(HepLorentzVector* pUp, HepLorentzVector* x)
{
  HepLorentzVector* pDn  = getPVectAfterTrackDnExp(pUp);
  double zTOF2 = m_tof2Pos-0.5*m_tof2Thickness; // upstream side
  double dz = -(x->z()-zTOF2);
  double betaZUpC = c*pUp->z()/sqrt(pUp->vect().mag2()+mass2);
  double betaZDnC = c*pDn->z()/pDn->e(); // already assuming muon
  double t1 = dz/(betaZUpC-betaZDnC)*(log(betaZUpC)-log(betaZDnC)); // assuming beta_z changes lineary
  double sintan = sin(pUp->theta())*tan(pUp->theta());
  double corrSinTan = -0.006438+0.48614*sintan+0.94976*sintan*sintan;
  double t = t1+corrSinTan;
  return t;
}

double PidFits::getTofExpUpstream(HepLorentzVector* p, HepLorentzVector* x)
{ 
  HepLorentzVector* pBeforeTracker = getPVectBeforeTrackUpExp(p);
  HepLorentzVector* pBeforeDiffuser = getPVectBeforeDiffuserExp(pBeforeTracker);
  HepLorentzVector* pAfterTof1 = getPVectAfterTof1Exp(pBeforeDiffuser);
  double t1 = getTofExpUpstreamTracker(pBeforeTracker, p, x);
  double t2 = getTofExpDiffuser(pBeforeDiffuser, pBeforeTracker);
  double t3 = getTofExpTof1Diffuser(pAfterTof1, pBeforeDiffuser);
  double t = t1+t2+t3;
  return t;
}

double PidFits::getTofExpUpstreamTracker(HepLorentzVector* pUp, HepLorentzVector* pDn, HepLorentzVector* x)
{
  double zDif = m_diffuserPos+0.5*m_diffuserThickness; // downstream side
  double dz = x->z()-zDif;
  double betaZUpC = c*pUp->z()/pUp->e();// already assuming muon
  double betaZDnC = c*pDn->z()/sqrt(pDn->vect().mag2()+mass2); 
  double t1 = dz/(betaZUpC-betaZDnC)*(log(betaZUpC)-log(betaZDnC)); // assuming beta_z changes lineary
  double sintan = sin(pDn->theta())*tan(pDn->theta());
  double corrSinTan = -0.002466+0.4185*sintan+0.7192*sintan*sintan;
  double t = t1+corrSinTan;
  return t;
}

double PidFits::getTofExpDiffuser(HepLorentzVector* pUp, HepLorentzVector* pDn)
{
  double t = 0.;
  double dz = m_diffuserThickness;
  if (dz>0.) {
    double betaZDnC = c*pUp->z()/pUp->e(); // already assuming muon
    double betaZUpC = c*pDn->z()/pDn->e(); // already assuming muon
    t = dz/(betaZUpC-betaZDnC)*(log(betaZUpC)-log(betaZDnC)); // assuming beta_z changes lineary
  }
  return t;
}

double PidFits::getTofExpTof1Diffuser(HepLorentzVector* pUp, HepLorentzVector* pDn)
{
  // assumes no energy loss between tof1 and diffuser
  double zTof1 = m_tof1Pos+0.5*m_tof1Thickness; // downstream side
  double zDif = m_diffuserPos-0.5*m_diffuserThickness; // upstream side
  double dz = zDif-zTof1;
  double betaZUpC = c*pUp->z()/pUp->e();// already assuming muon
  double betaZDnC = c*pDn->z()/sqrt(pDn->vect().mag2()+mass2); 
  double t1 = dz/(betaZUpC-betaZDnC)*(log(betaZUpC)-log(betaZDnC)); // assuming beta_z changes lineary
  double sintan = sin(pDn->theta())*tan(pDn->theta());
  double corrSinTan = 0.001429-0.1468*sintan-0.6993*sintan*sintan;
  double t = t1+corrSinTan;
  return t;
}

double PidFits::getRangeExpSW(HepLorentzVector* p)
{
  double rangeExp = -1;
  if (p->isTimelike() == false) return 0.5*m_cellLength[0]; //null vectors are no good for .gamma()
  double x = p->beta()*p->gamma()*p->cosTheta(); // cos theta for part of track transversally
  //fits for plastic part, p must be after layer 0:
  if (x<=1e-6) rangeExp = 0.5*m_cellLength[0];// beta is limited by machine precision and never 0
  else rangeExp = m_cellLength[0]+ 1.50433*x +14.2086*x*x +121.749*x*x*x -46.1066*x*x*x*x +4.68409*x*x*x*x*x; //Dec 06

  if (rangeExp > layerToLength(m_layers)) rangeExp = layerToLength(m_layers); //end of cal

  return rangeExp;
}

HepLorentzVector* PidFits::getPVectAfterTof1Exp(HepLorentzVector* p)
{
  HepLorentzVector* pout = new HepLorentzVector();
  double psquare = p->vect().mag2();
  if ( ( p->cosTheta()<=0. ) || ( ((psquare>0.))==false) ) return new HepLorentzVector(0.,0.,0., m_mu); //will never leave
  double beta = 1./sqrt(1.+mass2/psquare); // assume muon
  double y = 0.5946/(beta*beta) -1.434/beta +0.6556 +0.9006*beta -0.6432*beta*beta; // taylor expanded bethe-block [MeV]
  double eLoss = y/p->cosTheta();
  double e_fin = sqrt(psquare+mass2)+eLoss;
  if (e_fin < m_mu) e_fin = m_mu;
  pout->setE(e_fin);
  Hep3Vector p_fin = p->vect();
  p_fin.setTheta(acos(p->cosTheta()-(-0.390+0.3929*p->cosTheta()))); //change of angle
  if (p_fin.mag2()>0.) p_fin.setMag(sqrt(e_fin*e_fin - mass2)); // keep direction identical to before
  pout->setVect(p_fin);
  return pout;
}

HepLorentzVector* PidFits::getPVectBeforeTrackUpExp(HepLorentzVector* p)
{
  HepLorentzVector* pout = new HepLorentzVector();
  double psquare = p->vect().mag2();
  if ( ( p->cosTheta()<=0. ) || ( ((psquare>0.))==false )) return new HepLorentzVector(0.,0.,0., m_mu); //will never leave
  double beta = 1./sqrt(1.+mass2/psquare); // assume muon
  double y = 1.344/(beta*beta) +0.2889/beta -0.2327 -0.1245*beta +0.6842*beta*beta; // taylor expanded bethe-block [MeV]
  double eLoss = y/p->cosTheta();
  double e_fin = sqrt(psquare+mass2)+eLoss;
  if (e_fin < m_mu) e_fin = m_mu;
  pout->setE(e_fin);
  Hep3Vector p_fin = p->vect();
  p_fin.setTheta(acos(p->cosTheta()-(-0.511+0.5131*p->cosTheta()))); //change of angle
  if (p_fin.mag2()>0.) p_fin.setMag(sqrt(e_fin*e_fin - mass2)); // keep direction identical to before
  pout->setVect(p_fin);
  return pout;
}

HepLorentzVector* PidFits::getPVectBeforeDiffuserExp(HepLorentzVector* p)
{
  HepLorentzVector* pout = new HepLorentzVector();
  double psquare = p->vect().mag2();
  if ( (p->cosTheta()<=0.) || ( ((psquare>0.))==false) ) return new HepLorentzVector(0.,0.,0., m_mu); //will never leave
  double beta = 1./sqrt(1.+mass2/psquare); // assume muon
  double y = -57.7/(beta*beta) +162.2/beta -24.85 -188.1*beta +121.4*beta*beta; // taylor expanded bethe-block [MeV]
  double eLoss = y/p->cosTheta()*m_diffuserThickness/cm; //-dE/dl*l >0
  double e_fin = sqrt(psquare+mass2)+eLoss;
  if (e_fin < m_mu) e_fin = m_mu;
  pout->setE(e_fin);
  Hep3Vector p_fin = p->vect();
  if (p_fin.mag2()>0.) p_fin.setMag(sqrt(e_fin*e_fin - mass2)); // keep direction identical to before
  p_fin.setTheta(acos(p->cosTheta()-(-0.3219+0.3272*p->cosTheta()))); //change of angle
  pout->setVect(p_fin);
  return pout;
}

HepLorentzVector* PidFits::getPVectAfterTof2Exp(HepLorentzVector* p)
{
  HepLorentzVector* pout = new HepLorentzVector();
  double psquare = p->vect().mag2();
  if ((p->cosTheta()<=0.) || ( ((psquare>0.))==false) ) return new HepLorentzVector(0.,0.,0., m_mu); //will never leave tof
  double beta = 1./sqrt(1.+mass2/psquare); // assume muon
  double l = m_tof2Thickness/cm/p->cosTheta(); //cm
  double y = 112.494/(beta*beta) -520.188/beta +917.926 -722.081*beta +213.905*beta*beta; // taylor expanded bethe-block [MeV/cm]
  double eLoss = y*l;
  double e_fin = sqrt(psquare+mass2)-eLoss;
  if (e_fin < m_mu) e_fin = m_mu;
  pout->setE(e_fin);
  Hep3Vector p_fin = p->vect();
  if (p_fin.mag2()>0.) p_fin.setMag(sqrt(e_fin*e_fin - mass2)); // keep direction identical to before
  pout->setVect(p_fin);
  return pout;
}

HepLorentzVector* PidFits::getPVectAfterTrackDnExp(HepLorentzVector* p)
{
  HepLorentzVector* pout = new HepLorentzVector();
  double psquare = p->vect().mag2();
  if ((p->cosTheta()<=0.) || ( ((psquare>0.))==false) ) return new HepLorentzVector(0.,0.,0., m_mu); //will never leave
  double beta = 1./sqrt(1.+mass2/psquare); // assume muon
  double y = -8.49671/(beta*beta) +4.81395e+01/beta  -8.52172e+01 +6.56883e+01*beta -1.81637e+01*beta*beta; // taylor expanded bethe-block [MeV]
  double eLoss = y/p->cosTheta();
  double e_fin = sqrt(psquare+mass2)-eLoss;
  if (e_fin < m_mu) e_fin = m_mu;
  pout->setE(e_fin);
  Hep3Vector p_fin = p->vect();
  p_fin.setTheta(acos(p->cosTheta()+(0.7174-0.7241*p->cosTheta()))); //change of angle
  if (p_fin.mag2()>0.) p_fin.setMag(sqrt(e_fin*e_fin - mass2)); // keep direction identical to before
  pout->setVect(p_fin);
  return pout;
}

HepLorentzVector* PidFits::getPVectAfterLayer0Exp(HepLorentzVector* p)
{
  HepLorentzVector* pout = new HepLorentzVector();
  double psquare = p->vect().mag2();
  if ((p->cosTheta()<=0.) || ( ((psquare>0.))==false) ) return new HepLorentzVector(0.,0.,0., m_mu); //will never leave
  double beta = 1./sqrt(1.+mass2/psquare); // assume muon
//   double y = 2.08927e+02/(beta*beta) -3.39144e+02/beta -5.83394e+01 +3.42975e+02*beta -1.28795e+02*beta*beta; // taylor expanded bethe-block [MeV] (gammal fit)
  double y = 119.106/(beta*beta) -175.564/beta -37.1309 +164.933*beta -50.0836*beta*beta; // taylor expanded bethe-block [MeV]
  double eLoss = y/p->cosTheta();
  double e_fin = sqrt(psquare+mass2)-eLoss;
  if (e_fin < m_mu) e_fin = m_mu;
  pout->setE(e_fin);
  Hep3Vector p_fin = p->vect();
  p_fin.setTheta(acos(p->cosTheta()+(-0.2572+0.234*p->cosTheta()))); //change of angle
  if (p_fin.mag2()>0.) p_fin.setMag(sqrt(e_fin*e_fin - mass2)); // keep direction identical to before
  pout->setVect(p_fin);
  return pout;
}

double PidFits::layerToLength(int layerNum)
{
  // layerNum = 0 is lead fiber layer
  if (layerNum<0) return 0.;
  double z = 0.;
  for (int lay = 0; lay < layerNum; lay++){
    z += m_cellLength[lay];
  }
  z += 0.5*m_cellLength[layerNum]; // center of layer
  return z;
}

int PidFits::lengthToLayer(double z)
{
  int layer = -1;
  double zmax = 0.;
  double zmin = 0.;
  for (int lay = 0; lay < m_layers; lay++){
    zmin = layerToLength(lay)-0.5*m_cellLength[lay];
    zmax = layerToLength(lay)+0.5*m_cellLength[lay];
    if ((zmin<z) && (z<=zmax)) {
      layer = lay;
      break;
    }
  }
  return layer;
}

double PidFits::getEVisExpLayer0(double eIn, double eOut)
{
  return 0.2305*(eIn-eOut); // from looking at rangeT=1 particles
}

double PidFits::getEVisExpLayer1Plus(double eIn)
{
  double eVis = -1.;
  if (eIn < 182.) eVis = 0.997710*eIn; //ok
  else eVis = 3.16532e+02-7.44172e-01*eIn; // punching through
  if (eVis < 0.) eVis = 0.;
  return eVis;
}

double PidFits::getAverageADC(double eDep)
{
  double adc;
  //////////////////////////
  // should be from datacards:
  static double ampCof       = MyDataCards.fetchValueDouble("EMCalAmplificationCof");
  static double tWin         = MyDataCards.fetchValueDouble("EMCalSamplingTimeWindow") ;
  static double attLengthL   = MyDataCards.fetchValueDouble("EMCalAttLengthLong");
  static double attLengthS   = MyDataCards.fetchValueDouble("EMCalAttLengthShort");
  static double nLTemp       = MyDataCards.fetchValueDouble("EMCalAttLengthLongNorm");
  static double nSTemp       = MyDataCards.fetchValueDouble("EMCalAttLengthShortNorm");
  static double energyW      = MyDataCards.fetchValueDouble("EMCalEnergyPerPhoton");
  static double effColl      = MyDataCards.fetchValueDouble("EMCalLightCollectionEff");
  static double qEff         = MyDataCards.fetchValueDouble("EMCalQuantumEff");
  static double lgEff        = MyDataCards.fetchValueDouble("EMCalLightGuideEff");
  static double decayT       = MyDataCards.fetchValueDouble("EMCalElectronicsDecayTime");
  static double adcConv      = MyDataCards.fetchValueDouble("EMCalAdcConversionFactor");

  // normalize attenuation components
  static double nL = nLTemp/(nLTemp+nSTemp);
  static double nS = nSTemp/(nLTemp+nSTemp);

  double distance = 500.; // mm, assumption
  double tReal = 5.; // ns, assumption

  double meanPhN    = eDep/energyW;
  double attenuation = nL*exp(-distance/attLengthL)+nS*exp(-distance/attLengthS);
  double collectedPh = (int)floor(meanPhN*effColl*attenuation*lgEff+0.5);
  int pe = (int)floor(collectedPh*qEff+0.5);
  unsigned long int peAmplified = (unsigned long int)floor(pe*ampCof);
  double t1 = -tReal;
  double t2 = tWin - tReal;
  if (t1 < 0) t1 = 0.; // dont count before signal arrives!
  if (t2 < 0) return 0.; // whole signal out of gate
  double integral = gsl_sf_gamma_inc_Q(3,t1/decayT)-gsl_sf_gamma_inc_Q(3,t2/decayT);
  long double charge = integral*peAmplified/eplus; // in number of electron charges
  adc = floor(charge/adcConv);  
  return adc;
}

