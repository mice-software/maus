///////////////////////////////////////////////////////////////////////////////
/// EmCalTrack
///
/// \brief Reconstructed calorimeter variables
///
///
/// \author 2006-12-21 Rikard Sandstrom
////////////////////////////////////////////////////////////////////////////////

#ifndef EmCalTrack_HH
#define EmCalTrack_HH
#include <vector>

#include "Memory.hh"

class EmCalDigit;

class EmCalTrack
{
public:

  EmCalTrack();
  EmCalTrack(std::vector< EmCalDigit* >& digits, std::vector< double > cellLenghts, double, double);

  ~EmCalTrack();

  inline void setTDCPeaks(int p) {m_tdcPeaks = p;};
  inline void setBarycenter(double b) {m_barycenter = b;};
  inline void setTotalProdADC(double a) {m_totalProdADC = a;};
  inline void setMaxADClay(int l) {m_maxADClay = l;};
  inline void setRange(int r) {m_range = r;};
  inline void setRangeH(int r) {m_rangeH = r;};
  inline void setHolesQ(double h) {m_holesQ = h;};
  inline void setHighQ(double h) {m_highQ = h;};
  inline void setMaxLSubR(int l) {m_maxLSubR = l;};
  inline void setADCLeft(std::vector< int > v) {m_adcLeft = v;};
  inline void setADCRight(std::vector< int > v) {m_adcRight = v;};
  inline void setADCProd(std::vector< double > v) {m_adcProd = v;};
  inline void setADCAdj(std::vector< double > v) {m_adcAdj = v;};
  inline void setADCQ(std::vector< double > v) {m_adcQ = v;};
  inline void setN(std::vector< int > v) {m_n = v;};
  inline void setNH(std::vector< int > v) {m_nH = v;};

  inline int getTDCPeaks() {return m_tdcPeaks;};
  inline double getBarycenter() {return m_barycenter;};
  inline double getTotalProdADC() {return m_totalProdADC;};
  inline int getMaxADClay() {return m_maxADClay;};
  inline int getRange() {return m_range;};
  inline int getRangeH() {return m_rangeH;};
  inline double getHolesQ() {return m_holesQ;};
  inline double getHighQ() {return m_highQ;};
  inline int getMaxLSubR() {return m_maxLSubR;};
  inline std::vector< int > getADCLeft() {return m_adcLeft;};
  inline std::vector< int > getADCRight() {return m_adcRight;};
  inline std::vector< double > getADCProd() {return m_adcProd;};
  inline std::vector< double > getADCAdj() {return m_adcAdj;};
  inline std::vector< double > getADCQ() {return m_adcQ;};
  inline std::vector< int > getN() {return m_n;};
  inline std::vector< int > getNH() {return m_nH;};
  
  inline const std::vector< EmCalDigit*>& GetMotherDigits() {return m_digitMothers;};
  inline void SetMotherDigits(std::vector< EmCalDigit* >& v) {m_digitMothers = v;};

private:
  double calcBarycenter(double adcTot, std::vector< double > adcProd);

  std::vector< double > m_cellLength;
  int m_layers;
  double m_highLvlTriggerSW;
  double m_highLvlTriggerKL;

  int m_tdcPeaks;
  double m_totalProdADC;
  double m_barycenter;
  int m_maxADClay;
  int m_range;
  int m_rangeH;
  double m_holesQ; 
  double m_highQ;
  int m_maxLSubR; 
  std::vector< int > m_adcLeft;
  std::vector< int > m_adcRight;
  std::vector< double > m_adcProd;
  std::vector< double > m_adcAdj;
  std::vector< double > m_adcQ;
  std::vector< int > m_n;
  std::vector< int > m_nH;
  
  std::vector< EmCalDigit* > m_digitMothers;
  
};

#endif
