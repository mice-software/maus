#include "EmCalTrack.hh"

#include "MICEEvent.hh"
#include "dataCards.hh"
#include "EmCalHit.hh"
#include "EmCalDigit.hh"
#include "CLHEP/Random/Randomize.h"
#include "CLHEP/Units/SystemOfUnits.h"


EmCalTrack::EmCalTrack()
{
  miceMemory.addNew( Memory::EmCalTrack ); 
  m_digitMothers.clear();
  m_tdcPeaks = 0;
}

EmCalTrack::EmCalTrack(std::vector< EmCalDigit* >& digits, std::vector< double > cellLengths, double highLvlTriggerSW, double highLvlTriggerKL)
{
  miceMemory.addNew( Memory::EmCalTrack ); 
  m_digitMothers.clear();
  m_tdcPeaks = 0;

//   m_digitMothers.resize( digits.size() );
  for( unsigned int i = 0; i < digits.size(); ++i )
  {
//     m_digitMothers[i] = digits[i];
    m_digitMothers.push_back( digits[i] );
  }
  
  m_layers  = cellLengths.size();
  m_cellLength = cellLengths;
  m_highLvlTriggerSW = highLvlTriggerSW;
  m_highLvlTriggerKL = highLvlTriggerKL;

  for (int layer = 0; layer < m_layers; ++layer){ //reserve space, and initialize to 0
    m_adcLeft.push_back(0);
    m_adcRight.push_back(0);
    m_adcProd.push_back(0.);
    m_adcAdj.push_back(0.);
    m_adcQ.push_back(0.);
    m_n.push_back(0);
    m_nH.push_back(0);
  }
  m_tdcPeaks = 0;
  m_totalProdADC = 0.;

  int numDigits = digits.size();
  for( int i = 0; i < numDigits; ++i )
  {
    int layerNum = digits[i]->GetMotherHits()[0]->GetLayerNumber();
    int side  = digits[i]->GetSide();
    int adc   = digits[i]->GetADC();
    std::vector< int > tdcVector = digits[i]->GetTDCVector();

    int size = tdcVector.size();
    if (size>m_tdcPeaks){
      m_tdcPeaks = size/2; // vector consists of pairs of elements
    }

    if (side == 0) {// dont double count
      m_adcLeft[layerNum] += adc;
    } else {
      m_adcRight[layerNum] += adc;
    }
  }

  int nTemp = 0;
  int nHTemp = 0;
  double maxADC = 0;
  m_maxADClay = -1;
  m_range = -1;
  m_rangeH = -1;
  m_holesQ = -1; 
  m_highQ = -1; 
  m_maxLSubR = -1000;

  for (int lay = 0; lay < m_layers; lay++){
    if((m_adcLeft[lay]+m_adcRight[lay])>0) m_adcProd[lay] += (double)2.*(m_adcLeft[lay]*m_adcRight[lay])/(m_adcLeft[lay]+m_adcRight[lay]);
    if(m_adcProd[lay]) m_totalProdADC += m_adcProd[lay];
    if (m_adcProd[lay]>0.) m_n[lay]++; // number of digits in the layer
    if (lay>0) { // plastic
      if (m_adcProd[lay]>(m_highLvlTriggerSW*m_cellLength[lay])) m_nH[lay]++;
    } else { // fiber+lead
      if (m_adcProd[lay]>(m_highLvlTriggerKL*m_cellLength[lay])) m_nH[lay]++;
    }
    nTemp += m_n[lay];
    nHTemp += m_nH[lay];
    if (m_nH[lay]>0) { // "range" of high level hits
      m_rangeH = lay;
    }
    if (m_n[lay]>0) { // "range" of any level hits
      m_range = lay;
    } else {
    }
    if (m_adcProd[lay]/m_cellLength[lay]>maxADC) {
      m_maxADClay = lay; // look for the bragg peak
      maxADC = m_adcProd[lay]/m_cellLength[lay];
      if (lay==0) maxADC*=1./0.6789; // correct for lower sampling fraction in 0th layer
    }
  }
  
  if (nTemp) m_highQ=(double)nHTemp/nTemp;

  for (int lay = 0; lay < m_layers-1; lay++){
    if (m_adcProd[lay+1]>0) {
      m_adcAdj[lay] = (double)m_adcProd[lay]/m_adcProd[lay+1];
    } else {
      m_adcAdj[lay] = -1.;
    }
    if (m_totalProdADC) m_adcQ[lay] = (double)m_adcProd[lay]/(double)m_totalProdADC;
  }
  int nHoles = 0;
  for (int lay = 0; lay<m_maxADClay; lay++){
    if (m_nH[lay]==0) nHoles++;
  }
  if (m_maxADClay > 0) {
    m_holesQ = (double)nHoles/m_maxADClay;
  } else if ((m_maxADClay == 0) && (nHoles == 0)) {
    m_holesQ = 0.;
  } else { //no max ADC layer assigned
    m_holesQ = -1.;
  }

  m_maxLSubR = m_maxADClay-m_range; // should normally be ? for a well behavied track
  m_barycenter =  calcBarycenter(m_totalProdADC, m_adcProd);
}

EmCalTrack::~EmCalTrack()
{
  miceMemory.addDelete( Memory::EmCalTrack );
  m_digitMothers.clear();
}

double EmCalTrack::calcBarycenter(double adcTot, std::vector< double > m_adcProd)
{
  double barycenter = -1.;
  if (adcTot > 0) {
    double bari = 0;
    double previousL = 0;
    for (int lay = 0; lay < m_layers; lay++){
      bari += (double)m_adcProd[lay]*(0.5*m_cellLength[lay]+previousL);
      previousL += m_cellLength[lay];
    }
    barycenter = bari/adcTot;
  } else {
    barycenter = 0.;
  }
  return barycenter;
}
