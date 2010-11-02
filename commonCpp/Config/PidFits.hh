#ifndef PidFits_HH
#define PidFits_HH

#include "Interface/Memory.hh"
#include "CLHEP/Vector/LorentzVector.h"

using CLHEP::HepLorentzVector;

class MiceModule;

class PidFits 
{
public:

  static PidFits* getInstance();
  PidFits();

 ~PidFits();
  
  double getBaryExpSW(HepLorentzVector* p);
  double getADCExpSW(double e);
  double getTofExp(HepLorentzVector* pUp, HepLorentzVector* pDn, HepLorentzVector* xUp, HepLorentzVector* xDn);
  double getTofExpCenter(HepLorentzVector* pUp, HepLorentzVector* pDn, HepLorentzVector* xUp, HepLorentzVector* xDn);
  double getTofExpDownstream(HepLorentzVector* p, HepLorentzVector* x);
  double getTofExpUpstream(HepLorentzVector* p, HepLorentzVector* x);
  double getTofExpUpstreamTracker(HepLorentzVector* pUp, HepLorentzVector* pDn, HepLorentzVector* x);
  double getTofExpDiffuser(HepLorentzVector* pUp, HepLorentzVector* pDn);
  double getTofExpTof1Diffuser(HepLorentzVector* pUp, HepLorentzVector* pDn);
  double getRangeExpSW(HepLorentzVector* p); /// Given track at exit TOF2, calc range in mm
  HepLorentzVector* getPVectBeforeDiffuserExp(HepLorentzVector* p); /// Given track at exit of diffuser, calc expected at entrance of diffuser
  HepLorentzVector* getPVectBeforeTrackUpExp(HepLorentzVector* p); /// Given track at exit upstream tracker, calc expected track at exit of diffuser
  HepLorentzVector* getPVectAfterTof2Exp(HepLorentzVector* p); /// Given track at entrance TOF2, calc expected track at exit TOF2
  HepLorentzVector* getPVectAfterTof1Exp(HepLorentzVector* p);
  HepLorentzVector* getPVectAfterTrackDnExp(HepLorentzVector* p); /// Given track at entrance downstream tracker, calc expected track at entrance TOF2
  HepLorentzVector* getPVectAfterLayer0Exp(HepLorentzVector* p); /// Given track at exit of TOF2, calc expected track at exit of layer 0 in calorimeter
  double layerToLength(int layerNum);
  int    lengthToLayer(double z);
  double getEVisExpLayer0(double eIn, double eOut);
  double getEVisExpLayer1Plus(double eIn);
  double getAverageADC(double eDep);

  inline int getNumCalLayers() {return m_layers;};
  inline std::vector< double > getEmCalCellLengths() {return m_cellLength;};
  inline double getEmCalNumLayers() {return m_layers;};

private:

  void   countCalLayers(std::vector<const MiceModule*>);
  void   fillCalLayerVector(std::vector<const MiceModule*>);
  void   setTOFProperties(std::vector<const MiceModule*>);
  void   setDiffuserProperties(std::vector<const MiceModule*>);
  
// varibles
//   static const double c;
//   static const double m_mu;
//   static const double mass2;
  double c;
  double m_mu;
  double mass2;

  std::vector< double > m_cellLength;
  int m_layers;
  double m_tof1Thickness;
  double m_tof2Thickness;
  double m_tof1Pos;
  double m_tof2Pos;
  double m_diffuserThickness;
  double m_diffuserPos;

  static PidFits* fInstance;

};

//   const double PidFits::c = 299.792458; //mm/ns
//   const double PidFits::m_mu = 105.6583692; //MeV/c^2
//   const double PidFits::mass2 = m_mu*m_mu;

#endif
