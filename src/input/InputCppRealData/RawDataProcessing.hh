#ifndef _MAUS_INPUTCPPREALDATA_RAWDATPROCESSING_HH_
#define _MAUS_INPUTCPPREALDATA_RAWDATPROCESSING_HH_

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits.h>
#include <algorithm>
#include <fstream>
#include <sstream>

// do not increment inside following macro! (e.g.: MAX( ++a, ++b );
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////
class fADCDataProcessor
{
 public:

  fADCDataProcessor(){}
  ~fADCDataProcessor(){}

  int GetMaxPosition();
  int GetSignalArea(int&);
  int GetPedestalArea(int&);
  int	GetPoint(int i) const		{ return data[i]; }
  std::vector<int> GetData() const	{ return data; }
  int GetArea();
  int GetPedestal() const		{ return pedestal; }

  int GetCharge( int Algorithm = ceaPedMax );

  enum ChargeEstimationAlgorithm {
    ceaMinMax = 0, //Simplest algorithm
    ceaFractionDescriminatorThreshold = 1, // not implemented
    ceaPedMax = 3
  };

 protected:

  void SetPedestal();
  int ChargeMinMax();
  int ChargePedMax();
  int charge;
  std::vector<int> data;
  int pedestal;
};




#endif




