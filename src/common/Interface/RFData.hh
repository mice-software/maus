// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef RFDATA_HH
#define RFDATA_HH

#include "CLHEP/Vector/ThreeVector.h"

using namespace CLHEP;

class RFData
{
public:
  RFData(Hep3Vector cavityPos, double phase, double peakField, double frequency, double phaseError, 
         double energyError);
  RFData();
  RFData(const RFData& rhs) : _cavityPos(rhs._cavityPos), _phase(rhs._phase), _peakField(rhs._peakField), 
                              _frequency(rhs._frequency), _phaseError(rhs._phaseError), 
                              _energyError(rhs._energyError)
  {;}
  ~RFData() {;}
 
  void   SetPhase(double rfPhase)                {_phase = rfPhase;}    
  void   SetPeakField(double rfPeakField)        {_peakField = rfPeakField;}    
  void   SetFrequency(double rfFrequency)        {_frequency = rfFrequency;}  
  void   SetPhaseError(double phaseError)        {_phaseError = phaseError;}  
  void   SetEnergyError(double energyError)      {_energyError = energyError;}  
  void   SetCavityPosition(Hep3Vector cavityPos) {_cavityPos = cavityPos;}

  double     GetPhase()          {return _phase;}
  double     GetPeakField()      {return _peakField;}
  double     GetFrequency()      {return _frequency;}
  double     GetPhaseError()     {return _phaseError;}
  double     GetEnergyError()    {return _energyError;}
  Hep3Vector GetCavityPosition() {return _cavityPos;}

private:
  Hep3Vector _cavityPos;
  double     _phase;
  double     _peakField;
  double     _frequency;
  double     _phaseError;
  double     _energyError;
};

#endif

