// ZustandVektor.hh
//
// This class contains a LorentzVector for position and time, and another for 
// momentum and energy. It also contains a 8x8 symmetric covariance matrix 
// where the upper left block diagonal matrix is the 4x4 position time
// covariance matrix and the lower right is the corresponding momentum energy
// matrix.
//
// Rikard Sandstrom and Aron Fish

#ifndef INTERFACE_ZUSTANDVEKTOR_HH
#define INTERFACE_ZUSTANDVEKTOR_HH 1

#include "CLHEP/Vector/LorentzVector.h"
using CLHEP::HepLorentzVector;
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
#include "CLHEP/Matrix/SymMatrix.h"
using CLHEP::HepSymMatrix;

class MCHit;

class ZustandVektor
{
  public :

    ZustandVektor();

    ZustandVektor( MCHit* );
 
  ~ZustandVektor(){};

  private :

    HepLorentzVector _xLorentz;
    HepLorentzVector _pLorentz;
    HepSymMatrix     _matrix;
    bool _truth;
    int _pid;

  public :
  // inline functions
    inline HepLorentzVector GetXLorentz() const {return _xLorentz;};
    inline HepLorentzVector GetPLorentz() const {return _pLorentz;};
    inline Hep3Vector GetPosition() const {return _xLorentz.vect();};  
    inline Hep3Vector GetMomentum() const {return _pLorentz.vect();};  
    inline double GetTime()   const {return _xLorentz.t();};
    inline double GetEnergy() const {return _pLorentz.e();};
    inline void SetXLorentz(HepLorentzVector x) {_xLorentz = x;};
    inline void SetPLorentz(HepLorentzVector p) {_pLorentz = p;};
    inline void SetXLorentz(double x, double y, double z, double t)
      {_xLorentz.set(x,y,z,t);};
    inline void SetPLorentz(double px, double py, double pz, double E)
      {_pLorentz.set(px,py,pz,E);};
    inline void SetPosition(Hep3Vector x) {_xLorentz.setVect(x);};  
    inline void SetMomentum(Hep3Vector p) {_pLorentz.setVect(p);};  
    inline void SetTime(double t) {_xLorentz.setT(t);};
    inline void SetEnergy(double E) {_pLorentz.setE(E);};

    inline void SetMatrixElement(int row, int col, double val) 
      { _matrix[row][col] = val;};
    inline double GetMatrixElement(int row, int col) const 
      {return _matrix[row][col];};
    inline HepSymMatrix GetXSigmas() const {return _matrix.sub(1,4);};
    inline HepSymMatrix GetPSigmas() const {return _matrix.sub(5,8);};
    inline HepSymMatrix GetFullMatrix() const {return _matrix;};

    inline bool GetTruth() const {return _truth;};
    inline int  GetPID()   const {return _pid;};
    inline void SetTruth(bool t) {_truth = t;};
    inline void SetPID(int id) {_pid = id;};
};

#endif

