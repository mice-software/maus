// MAUS WARNING: THIS IS LEGACY CODE.
#include "Interface/RFData.hh"


RFData::RFData(Hep3Vector cavityPos, double phase, double peakField, double frequency, double phaseError,
               double energyError)
       : _cavityPos(cavityPos), _phase(phase), _peakField(peakField), 
         _frequency(frequency), _phaseError(phaseError), _energyError(energyError)
{}

RFData::RFData()
       : _cavityPos(0,0,0), _phase(0), _peakField(0), _frequency(-1), _phaseError(0), _energyError(0)
{}

