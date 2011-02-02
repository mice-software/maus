#include "EmCalDigit.hh"

#include "MICEEvent.hh"
#include "dataCards.hh"
#include "EmCalHit.hh"
#include "VmefAdcHit.hh"
#include "CLHEP/Random/Randomize.h"
using CLHEP::RandPoisson;
using CLHEP::RandGauss;
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::eplus;
#include "gsl/gsl_sf_gamma.h"

EmCalDigit::EmCalDigit()
{
 miceMemory.addNew( Memory::EmCalDigit ); 

  // initialize
  m_tdcVector.clear();
  m_hitMothers.clear();
  m_adc = 0;
  m_side = -1;
}
EmCalDigit::EmCalDigit(VmefAdcHit* aHit, int side, int cell, dataCards* cards, double triggerTime )
{
  miceMemory.addNew( Memory::EmCalDigit ); 

  // initialize
  m_adc = aHit->GetCharge();
  m_side = side;
  m_cell = cell;
  m_fadc = aHit;
}

EmCalDigit::EmCalDigit(EmCalHit* aHit, int side, dataCards* cards, double widthX, double triggerTime )
{
  miceMemory.addNew( Memory::EmCalDigit ); 

  // initialize
  m_tdcVector.clear();
  m_peAmpVector.clear();
  m_tVector.clear();
  m_hitMothers.clear();
  m_hitMothers.push_back(aHit);
  m_adc = 0;
  m_side = side;
  m_cell = aHit->GetCellNumber();

  static double ampCof       = cards->fetchValueDouble("EMCalAmplificationCof");
  static double ampFluct     = cards->fetchValueDouble("EMCalAmplificationFluct");
  static double tWin         = cards->fetchValueDouble("EMCalSamplingTimeWindow") ;
  static double attLengthL   = cards->fetchValueDouble("EMCalAttLengthLong");
  static double attLengthS   = cards->fetchValueDouble("EMCalAttLengthShort");
  static double nLTemp       = cards->fetchValueDouble("EMCalAttLengthLongNorm");
  static double nSTemp       = cards->fetchValueDouble("EMCalAttLengthShortNorm");
  static double energyW      = cards->fetchValueDouble("EMCalEnergyPerPhoton");
  static double effColl      = cards->fetchValueDouble("EMCalLightCollectionEff");
  static double qEff         = cards->fetchValueDouble("EMCalQuantumEff");
  static double lgEff        = cards->fetchValueDouble("EMCalLightGuideEff");
  static double lgSpeed      = cards->fetchValueDouble("EMCalLightSpeed");

  // normalize attenuation components
  static double nL = nLTemp/(nLTemp+nSTemp);
  static double nS = nSTemp/(nLTemp+nSTemp);

  int layerNumber = aHit->GetLayerNumber();
  double pos;
  if (layerNumber%2!=0){
    pos = aHit->GetY();
  } else {
    pos = aHit->GetX();
  }
  double distance = GetDistanceToReadout(pos, widthX, side); // 0 negative side, 1 positive side
  double tReal = CalcTimeAfterTrigger(aHit->GetTime(), distance, triggerTime, lgSpeed);
  int pe = CalculatePEAtPMT(distance, aHit->GetEdep(), attLengthL, attLengthS,
    nL, nS, energyW, effColl, qEff, lgEff);       // after attenuation
  int peAmplified = (int)floor(pe*RandGauss::shoot(ampCof,ampFluct));  // after amplification

  // Add this digit to list of digits created by the hit, if
  // a) there are a electrons in the pmt
  // b) the signal starts during or before sampling
  // Digits not fullfilling these requirements will be removed by the same code that created it
  if ((peAmplified>0)&&(tReal<tWin)){
    m_peAmpVector.push_back(peAmplified);
    m_tVector.push_back(tReal);
    aHit->AssignDigit(this);
  }
}

EmCalDigit::~EmCalDigit()
{
 miceMemory.addDelete( Memory::EmCalDigit );

  m_tdcVector.clear();
  for (unsigned int i = 0; i<m_hitMothers.size(); i++){
    if (m_hitMothers[i]) m_hitMothers[i]->UnassignDigit(this);
  }
  m_hitMothers.clear();
}

double EmCalDigit::CalcTimeAfterTrigger(double timeHit, double distance, double triggerTime, double lgSpeed )
{
  double attTime = distance/lgSpeed; // time for photons to arrive at pmt
  return timeHit+attTime-triggerTime;
}

int EmCalDigit::CalculatePEAtPMT(double distance, double eDep, double attLengthL,
  double attLengthS, double nL, double nS, double energyW, double effColl,
  double qEff, double lgEff)
{
  double meanPhN;
  int numberOfPh, collectedPh, pe;

  // Photons in hit
  meanPhN    = eDep/energyW;
  numberOfPh = (int)floor(RandPoisson::shoot(meanPhN));

  // At the photomultiplier, after attenuation
  double attenuation = nL*exp(-distance/attLengthL)+nS*exp(-distance/attLengthS);
  collectedPh = (int)floor(numberOfPh*effColl*attenuation*lgEff+0.5);
  pe = (int)floor(RandGauss::shoot(collectedPh*qEff,1)+0.5); // gaussian distribute with 1 electron as sigma
  if (pe < 0) pe = 0;

  return pe;
}

void EmCalDigit::MergeWithOther(EmCalDigit* anotherDigit)
{
  // Add the hit mothers of the digit to be removed to the
  // hit mothers of the digit that will remain.

  const std::vector<EmCalHit*>& otherMothers = anotherDigit->GetMotherHits();

  for (unsigned int i = 0; i<otherMothers.size(); i++){
    bool duplicateHit = false;
    for (unsigned int j = 0; j<m_hitMothers.size(); j++){
      if (m_hitMothers[j]==otherMothers[i]){
        duplicateHit = true;
        break;
      }
    }
    if (duplicateHit==false) {
      otherMothers[i]->AssignDigit(this);
      m_hitMothers.push_back(otherMothers[i]);
    }
  }

  std::vector<int> otherPE = anotherDigit->GetPEAmpVector();
  m_peAmpVector.insert(m_peAmpVector.end(),
    otherPE.begin(), otherPE.end());

  std::vector<double> otherTimes = anotherDigit->GetTimeVector();
  m_tVector.insert(m_tVector.end(),
    otherTimes.begin(), otherTimes.end());
}

void EmCalDigit::CalculateAmplitudes( dataCards* cards )
{  
  static double timeWidth     = cards->fetchValueDouble("EMCalTdcConversionFactor");
  static int    tdcBits       = cards->fetchValueInt("EMCalTdcBits");
  static double decayT        = cards->fetchValueDouble("EMCalElectronicsDecayTime");
  static double tWin          = cards->fetchValueDouble("EMCalSamplingTimeWindow");
  static double adcConv       = cards->fetchValueDouble("EMCalAdcConversionFactor");
  static double tdcThreshold  = cards->fetchValueDouble("EMCalTdcThreshold");

  static const int maxEl = (int)floor(pow(2.0, tdcBits));
  std::vector< double > amplitudeVector;
  amplitudeVector.insert(amplitudeVector.begin(),maxEl,0.);

  double intCharge = 0;
  //   bin with the width of tdc bits (timeWidth)
  for (unsigned int j = 0; j < m_peAmpVector.size(); j++){
    for (int i = 0; i < maxEl; i++){
      double amp = CalcFlashCharge(m_peAmpVector[j], i*timeWidth-m_tVector[j], decayT);
      amplitudeVector[i]+=amp;
    }
    intCharge += CalcIntegratedCharge(m_peAmpVector[j], m_tVector[j], decayT, tWin);
  }
  m_adc = (int)floor(intCharge/adcConv);

  // Fill the tdc vector
  bool foundStart =  false;
  int tmin=0, tmax=0;
  for (unsigned int i = 0; i < amplitudeVector.size(); i++) {
    // check beginning
    if ((foundStart == false) && (amplitudeVector[i]>=tdcThreshold)) {
      tmin = i;
      foundStart = true;
    }
    // check end
    if ((foundStart) && ((amplitudeVector[i]<tdcThreshold) |((int)i==maxEl-1))) {
      tmax=i-1;
      foundStart = false;
      m_tdcVector.push_back(tmin);
      m_tdcVector.push_back(tmax - tmin + 1); //time over threshold
    }
  }
}

double EmCalDigit::CalcFlashCharge(int peAmplified, double t, double tau)
{
  // t is time after trigger
  double charge = 0.; // default to zero

  if (t < 0){
    charge = 0;
  } else {
    charge = 0.5*(t/tau)*(t/tau)*exp(-t/tau)*peAmplified*eplus;
    // 0.5 is the norm of the integral 0 to inf.
  }
  return charge;
}

double EmCalDigit::CalcIntegratedCharge(int peAmplified, double tReal, double decayT, double tWin)
{
  // tReal is time_hit-time_trigger, so integration limits must be set acc.
  double charge = 0.; // default to zero
  double t1, t2;
  t1 = -tReal;
  t2 = tWin - tReal;
  if (t1 < 0) t1 = 0.; // dont count before signal arrives!
  if (t2 < 0) return 0.; // whole signal out of gate

  // Upper incomplete gamma functions, normalized
  double integral = gsl_sf_gamma_inc_Q(3,t1/decayT)-gsl_sf_gamma_inc_Q(3,t2/decayT);
//   double integral = (t2-t1)/tWin; //debug
  charge = integral*peAmplified/eplus; // in number of electron charges
  return charge;
}

void EmCalDigit::Print()
{
  	std::cout << "____________________________" << std::endl;
  	std::cout << "  EmCalDigit" << std::endl;
  	std::cout << "  Cell Number : "<< m_cell << std::endl;
  	std::cout << "  PMT Numbe : " << m_side << std::endl;
  	std::cout << "  ADC : " << m_adc << std::endl;
  	std::cout << "____________________________" << std::endl;
}

