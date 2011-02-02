#ifndef TpgDigitsParameters_HH
#define TpgDigitsParameters_HH

#include <stdio.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

/////////////////////////////////////////////////////////////////////////////
/// C3DVector
/////////////////////////////////////////////////////////////////////////////

template <class T>
class C3DVector
{
public:
  C3DVector():m_dim1(0), m_dim2(0), m_dim3(0){;}
  C3DVector(int n1, int n2, int n3) {
    m_dim1 = n1;
    m_dim2 = n2;
    m_dim3 = n3;            
    for (int i1=0; i1 < n1; i1++){
      std::vector< std::vector<T> > x1;
      for (int i2=0; i2 < n2; i2++){
        std::vector<T> x2(n3);
        x1.push_back(x2);
      }
      m_3DVector.push_back(x1);
    }
  }
   
  T GetAt(int x, int y, int z){
    if ((x >= m_dim1) | (y >= m_dim2) | (z >= m_dim3)){
      cout << "Index out of bounds in C3DVector:" << endl;
      cout << "x: " << x << ", y: "<< y << ", z: " << z << endl;
      cout << "m_dim1: " << m_dim1 << ", m_dim2: "<< m_dim2 << ", m_dim3: " <<
      m_dim3 << endl;
      exit(EXIT_FAILURE);
    }
    return m_3DVector[x][y][z];
  }
  
  void SetAt(int x, int y, int z, T aT){
    if ((x >= m_dim1) | (y >= m_dim2) | (z >= m_dim3)){
      cout << "Index out of bounds in C4DVector:" << endl;
      cout << "x: " << x << ", y: "<< y << ", z: " << z << endl;
      cout << "m_dim1: " << m_dim1 << ", m_dim2: "<< m_dim2 << ", m_dim3: " <<
      m_dim3 << endl;
      exit(EXIT_FAILURE);
    }
    m_3DVector[x][y][z] = aT;
  }

private:
   std::vector< std::vector< std::vector <T> > > m_3DVector;
   int m_dim1;
   int m_dim2;
   int m_dim3;
};



///
/// \brief Digitization parameters for the TPG
///
/// Class defines the digitization parameters of the TPG.
/// Parameters are set in dataCards and should reflect experimental or Garfield
/// simulated values.
///
/// Created:
/// \author 21.08.02 Ed McKigney
///
/// Modified:
/// \author 21.11.03 R. Sandstrom 


class TpgDigitsParameters
{
public:

  TpgDigitsParameters();
 ~TpgDigitsParameters();
 
  static TpgDigitsParameters*  getInstance();

  bool isDeadStrip(int detNr, int layerNr, int stripNr); 

  inline bool   GetMixture(){return fMixture;};
  inline void     SetMixture(bool mixture){fMixture=mixture;};

  inline double GetClustersPerCM(){return fClustersPerCM;};
  inline void     SetClustersPerCM(double n){fClustersPerCM=n;};

  inline std::string GetElectronsPerClusterFile() {return fElectronsPerClusterFile;};
  inline void     SetElectronsPerClusterFile(std::string s){fElectronsPerClusterFile=s;};
  inline double GetElectronsPerCluster(){return fElectronsPerCluster;};
  inline void     SetElectronsPerCluster(double n){fElectronsPerCluster=n;};

  inline double GetDriftVelocity(){return fDriftVelocity;};
  inline void     SetDriftVelocity(double v){fDriftVelocity=v;};

  inline double GetSigmaTrans(){return fSigmaTrans;};
  inline void     SetSigmaTrans(double s){fSigmaTrans=s;};
  inline double GetSigmaLong(){return fSigmaLong;};
  inline void     SetSigmaLong(double s){fSigmaLong=s;};

  inline double GetEnergyPerPair(){return fEnergyPerPair;};
  inline void     SetEnergyPerPair(double w){fEnergyPerPair=w;};

  inline double GetAmplificationMean(){return fAmplificationMean;};
  inline void     SetAmplificationMean(double w){fAmplificationMean=w;};
  inline double GetAmplificationSigma(){return fAmplificationSigma;};
  inline void     SetAmplificationSigma(double w){fAmplificationSigma=w;};
  
  inline double   GetNoiseAdcSigma(){return fNoiseAdcSigma;};
  inline void     SetNoiseAdcSigma(double w){fNoiseAdcSigma=w;};

  inline int      GetDigitThreshold(){return fDigitThreshold;};
  inline void     SetDigitThreshold(int w){fDigitThreshold=w;};  

  
  inline double GetGEMSpreadSigma(){return fGEMSpreadSigma;};
  inline void     SetGEMSpreadSigma(double w){fGEMSpreadSigma=w;};    

  inline double GetElectronicsDecayTime(){return fElectronicsDecayTime;};
  inline void     SetElectronicsDecayTime(double t){fElectronicsDecayTime=t;};
  
  inline double GetAdcCountsPerfC(){return fAdcCountsPerfC;};
  inline void     SetAdcCountsPerfC(double a){fAdcCountsPerfC = a;};
  
  inline double GetSamplingPeriod(){return fSamplingPeriod;};
  inline void     SetSamplingPeriod(double t){fSamplingPeriod=t;};
  
  inline double GetStripSpace(){ return fStripSpace;};
  inline void     SetStripSpace(double s){fStripSpace=s;};
  inline int    GetStripsPerDir(){ return fStripsPerDir;};
  inline void     SetStripsPerDir(int s){fStripsPerDir=s;};
  inline double GetDeadStripFraction(){return fDeadStripFraction;};
  inline void     SetDeadStripFraction(double f){fDeadStripFraction=f;};
  
  inline std::vector<double> GetElectronVector(){return fElectronVector;};
   
  // Strips can have negative value.
  // "floor" is necessary for not going out of bounds. This is ok for both an odd
  // and even number of strips since they go from -1,0,1 (3) and -2,-1,0,1 (4)
  // respectively.
  inline int GetIndexedStripNr(int s) {return s += (int)floor(fStripsPerDir/2);};
  
private:
  void    DefineTpgParameters(); ///< Assigns values from dataCard to members.
  void    FillParameterVectors(); 

  static  TpgDigitsParameters* fInstance;

  bool  fSensitive;
  bool  fMixture;
  
  double fClustersPerCM;
  
  std::string fElectronsPerClusterFile;
  double fElectronsPerCluster;

  double fDriftVelocity;

  double fSigmaTrans;
  double fSigmaLong;

  double fEnergyPerPair;

  double fAmplificationMean;
  double fAmplificationSigma;
  
  double   fNoiseAdcSigma;
  
  int fDigitThreshold;
  
  double fGEMSpreadSigma;
  
  double fElectronicsDecayTime;
  double fAdcCountsPerfC;
  
  double fSamplingPeriod;
  
  double fStripSpace;
  int    fStripsPerDir;
  
  std::string fDeadStripFile;

  double fDeadStripFraction;
  
  std::vector<double> fElectronVector;
  C3DVector<bool> fDeadStrip3D;
};

#endif
