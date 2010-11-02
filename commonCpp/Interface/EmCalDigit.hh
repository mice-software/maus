///////////////////////////////////////////////////////////////////////////////
/// EmCalDigit
///
/// \brief Calorimeter ADC & TDC information for a channel
///
/// An EmCalDigit is a store of information available after a hit has been 
/// digitized. This class contains methods for calculating the amplitudes and 
/// related quantities given hit parameters.
///
/// \author 2006-03-04 Rikard Sandstrom
////////////////////////////////////////////////////////////////////////////////

#ifndef EmCalDigit_HH
#define EmCalDigit_HH
#include <vector>

#include "Memory.hh"

class VmefAdcHit;
class EmCalHit;
class dataCards;

class EmCalDigit
{
public:

  EmCalDigit();

  /// The constructor
  /// 1. Adds the hit to the list of hit mothers (should be the first element).
  /// 2. Calls GetDistanceToReadout.
  /// 3. Uses the distance to call CalcTimeAfterTrigger.
  /// 4. Calls CalculatePEAtPMT to get number of photo electrons at the readout.
  /// 5. The photo electrons are amplified.
  /// 6. The arrival time and number of electrons at readout are added to member
  ///    vector if some conditions are met.
  /// 7. This digit is assigned to the hit by EmCalHit::AssignDigit. 

  EmCalDigit( EmCalHit* aHit, int side, dataCards*, double widthX, double triggerTime );
  EmCalDigit( VmefAdcHit* aHit, int side, int cell, dataCards* cards, double triggerTime );

  /// The destructor, calls EmCalHit::UnassignDigit for all its hit mothers.
  ~EmCalDigit();

  /// Adds the hit mothers of the digit to be removed to the
  /// hit mothers of the digit that will remain. At the end, deletes the digit 
  /// it took as argument.

  void MergeWithOther(EmCalDigit* dig);

  /// Creates an ADC amplitude and a vector of TDC information.
  /// Uses a temporary vector to store amplitudes, flash ADC style,
  /// each bin is the size of the TDC binning. The contribution from multiple
  /// hits is summed up so the vector is equivalent to amplitude as a function /// of time.  

  void CalculateAmplitudes( dataCards* );

  /// Get/Set ADC amplitude
  inline int           GetADC()              { return m_adc; }
  inline void          SetADC(int adc)       { m_adc = adc; }

  /// Get/Set the side (which PMT) of the cell.
  inline int           GetSide()             {return m_side;};
  inline void          SetSide(int i)        {m_side    = i;};

  /// Get/Set the cell number.
  inline int           GetCell()             {return m_cell;};
  inline void          SetCell(int i)        {m_cell    = i;};

  /// Get/Set TDC information
  inline std::vector< int > GetTDCVector() {return m_tdcVector;};
  inline void SetTDCVector(std::vector< int > v) {m_tdcVector = v;};
  inline const std::vector< EmCalHit*>& GetMotherHits() {return m_hitMothers;};
  inline void SetMotherHits(std::vector< EmCalHit* >& v) {m_hitMothers = v;};

  /// Get a vector of electrons after amplification, each element corresponds 
  /// to a unique hit. 
  inline std::vector< int > GetPEAmpVector() {return m_peAmpVector;};

  /// Get a vector of arrival time after attenuation, each element corresponds 
  /// to a unique hit.
  inline std::vector< double > GetTimeVector() {return m_tVector;};

  void Print() ;
  VmefAdcHit*		vmefAdcHit() const	{ return m_fadc; };
  void assignVmeHits( VmefAdcHit* fadc)  { m_fadc = fadc;}

private:

  /// Returns the arrival time of the photons after the trigger.
  double CalcTimeAfterTrigger(double hitTime, double distance,
    double triggerTime, double lgSpeed);

  /// Returns the number of photo electrons (unamplified) at the PMT.
  /// This takes energy deposition, light collection efficiency, 
  /// attenuation, light guide efficiency and quantum efficiency into account. 
  int    CalculatePEAtPMT(double distance, double eDep,
    double attLengthL, double attLengthS, double nL, double nS,
    double energyW, double effColl,
    double qEff, double lgEff );

  /// Returns the unintegrated amplitude at a given time for a given number of 
  /// amplified electrons. Tau is a function parameter, gives the signal width.
  double CalcFlashCharge(int peAmp, double t, double tau);

  /// Returns the integrated amplitude at a given time for a given number of 
  /// amplified electrons and time window tWin. DecayT is a function parameter,
  /// gives the signal width. Uses incomplete gamma functions.
  double CalcIntegratedCharge(int peAmp, double tReal, double decayT, double tWin);

  std::vector< int > m_tdcVector;
  std::vector< EmCalHit* > m_hitMothers;
  VmefAdcHit* m_fadc;
  std::vector< int > m_peAmpVector; // list of amplified pe's
  std::vector< double > m_tVector; // list of arrival times for amplified pe's
  int         m_adc;
  int         m_side;
  int         m_cell;

  /// Returns the distance between the hit and the readout.
  inline double GetDistanceToReadout(double x, double widthX, int side)
    {return 0.5*widthX + x*(1-2*side);};
};

#endif
