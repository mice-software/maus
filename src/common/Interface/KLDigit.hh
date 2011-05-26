///////////////////////////////////////////////////////////////////////////////
/// KLDigit
///
/// Calorimeter ADC information for a channel
///
/// An KLDigit is a store of information available after a hit has been 
/// digitized. This class contains methods for calculating the amplitudes and 
/// related quantities given hit parameters.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef KLDigit_HH
#define KLDigit_HH
#include <vector>

#include "Interface/Memory.hh"

class VmefAdcHit;
class KLHit;
class KLCellHit;
class dataCards;

class KLDigit
{
public:

  KLDigit();

  /// The destructor, calls KLHit::UnassignDigit for all its hit mothers.
  ~KLDigit();

  KLDigit( VmefAdcHit* aHit, int side, int cell);

  /// Get/Set ADC amplitude
  inline int           GetADC()              { return m_adc; }
  inline void          SetADC(int adc)       { m_adc = adc; }

  /// Get/Set the side (which PMT) of the cell.
  inline int           GetSide()             {return m_side;};
  inline void          SetSide(int i)        {m_side    = i;};

  /// Get/Set the cell number.
  inline int           GetCell()             {return m_cell;};
  inline void          SetCell(int i)        {m_cell    = i;};


  VmefAdcHit*		vmefAdcHit() const	{ return m_fadc; };

  KLCellHit*  cellHit() const         { return m_cellHit; }
  void   setCellHit( KLCellHit* hit ) { m_cellHit = hit; };

  /// Print fAdc digit info
  void Print() ;

private:

  VmefAdcHit* m_fadc;
  int         m_adc;
  int         m_side;
  int         m_cell;

  KLCellHit* m_cellHit;

};

#endif
