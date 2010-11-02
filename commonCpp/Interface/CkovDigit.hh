///////////////////////////////////////////////////////////////////////////////
/// CkovDigit
///
/// \brief CKOV ADC information for a channel
///
/// A CkovDigit is a store of information available after a hit has been 
/// digitized. This class contains methods for calculating the amplitudes and 
/// related quantities given hit parameters. It was based on the EmCalDigit
///
/// \author 2006-10-07 Malcolm Ellis
////////////////////////////////////////////////////////////////////////////////

#ifndef CkovDigit_HH
#define CkovDigit_HH
#include <vector>

#include "Memory.hh"
#include "CkovHit.hh"
#include "MICERun.hh"
#include "dataCards.hh"

#include "VmeAdcHit.hh"
#include "VmefAdcHit.hh"
#include "VmeTdcHit.hh"

class CkovDigit
{
public:

  CkovDigit()		{ miceMemory.addNew( Memory::CkovDigit ); };

  CkovDigit( int pmt, int plane, dataCards* );
  CkovDigit( CkovHit* , dataCards* );
  
  /// Create digit from TDC and fADC data
  CkovDigit( VmefAdcHit * fadc, VmeTdcHit * tdc, int pmt, int plane );

  /// Create digit from fADC data only!
  CkovDigit( VmefAdcHit * fadc, int pmt, int plane );
  
  /// Create digit from TDC and ADC data
  CkovDigit( VmeAdcHit * adc, VmeTdcHit * tdc, int pmt, int plane );

  /// The destructor, calls CkovHit::UnassignDigit for all its hit mothers.
  ~CkovDigit();

  /// Adds the hit mothers of the digit to be removed to the
  /// hit mothers of the digit that will remain. At the end, deletes the digit 
  /// it took as argument.

  void MergeWithOther( CkovDigit* dig);

	void	setVmeAdcHit( VmeAdcHit* hit );
	void	setVmefAdcHit( VmefAdcHit* hit );
	void	setVmeTdcHit( VmeTdcHit* hit );


  /// Get/Set ADC and TDC 
  int           GetADC() const       { return m_adc; }
  void          SetADC(int adc)      { m_adc = adc; }

  VmeAdcHit*	vmeAdcHit() const	{ return m_adcHit; }
  VmefAdcHit*	vmefAdcHit() const	{ return m_fadcHit; }    
  VmeTdcHit*	vmeTdcHit() const	{ return m_tdcHit; }

  int           GetTDC() const       { return m_tdc; }
  void          SetTDC(int tdc)      { m_tdc = tdc; }

  double        GetPe() const        { return m_pe; };
  void          SetPe( double pe )   { m_pe = pe; };

  /// Get/Set the PMT
  int           GetPmt() const          { return pmtNo; };
  void          SetPmt( int pmt )       { pmtNo = pmt; };

  int           GetPlane() const        { return planeNo; };
  void          SetPlane( int plane )   { planeNo = plane; };

  const std::vector<CkovHit*>& 	GetMotherHits() const 		{return m_hitMothers;};
  void SetMotherHits( std::vector<CkovHit*>& v ) 		{ m_hitMothers = v; };

  void AddHitToThisDigit( CkovHit*, dataCards*);
  void Print() ;

  bool	isGood() const				{ return m_good; }
  void	setIsGood(bool isGood) 		{ m_good = isGood; }
  
private:
  void CreateDigit();
  std::vector<CkovHit*> m_hitMothers;
  int			pmtNo;
  int			planeNo;
  VmefAdcHit *	m_fadcHit;
  VmeTdcHit *	m_tdcHit;
  VmeAdcHit *	m_adcHit;
  int			m_adc;
  int			m_tdc;
  bool			m_good;
  double		m_pe;
};

#endif
