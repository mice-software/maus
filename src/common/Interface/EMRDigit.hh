///////////////////////////////////////////////////////////////////////////////
/// EMRDigit
///
/// \brief Calorimeter ADC & TDC information for a channel
///
/// An EMRDigit is a store of information available after a hit has been 
/// digitized. This class contains methods for calculating the amplitudes and 
/// related quantities given hit parameters.
///
/// \author 2006-03-04 Rikard Sandstrom
////////////////////////////////////////////////////////////////////////////////

#ifndef EMRDigit_HH
#define EMRDigit_HH
#include <vector>

#include "Interface/Memory.hh"

class VmefAdcHit;
class EMRHit;
class dataCards;

class EMRDigit
{
public:

  EMRDigit();

  ~EMRDigit();

private:

};

#endif
