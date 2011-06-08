// VlpcHit.hh
//
// A description of a recorded hit on a VLPC
//
// M.Ellis 22/10/2003
// Imported to G4MICE April 2005

#ifndef VLPCHIT_H
#define VLPCHIT_H

#include <vector>

#include "Memory.hh"

class VlpcHit 
{
public :

    VlpcHit()		{ miceMemory.addNew( Memory::VlpcHit ); }; 

  VlpcHit( int cryo, int afe, int mcm, int chan, double adc ) :
        m_cryo( cryo ), m_afe( afe ), m_mcm( mcm ), m_chan( chan ), m_adc( adc ) { miceMemory.addNew( Memory::VlpcHit ); m_cass = -1; };
  
  VlpcHit( int cryo, int cass, int afe, int mcm, int chan, double adc, bool discrim ) :
        m_cryo( cryo ), m_cass( cass ), m_afe( afe ), m_mcm( mcm ), m_chan( chan ), m_adc( adc ) { miceMemory.addNew( Memory::VlpcHit ); m_discrim=discrim; };
  
  VlpcHit( int cryo, int cass, int afe, int mcm, int chan, double adc, bool discrim, double tdc ) :
        m_cryo( cryo ), m_cass( cass ), m_afe( afe ), m_mcm( mcm ), m_chan( chan ), m_adc( adc ) { miceMemory.addNew( Memory::VlpcHit );  m_discrim=discrim; m_tdc=tdc; };

  VlpcHit( int cryo, int cass, int afe, int mcm, int chan, double adc, bool discrim, double tdc, int trig_time, int bunch_num ) :
        m_cryo( cryo ), m_cass( cass ), m_afe( afe ), m_mcm( mcm ), m_chan( chan ), m_adc( adc ) { miceMemory.addNew( Memory::VlpcHit );  m_discrim=discrim; m_tdc=tdc; m_trig_time=trig_time; m_bunch_num=bunch_num; };

  ~VlpcHit() 		{ miceMemory.addDelete( Memory::VlpcHit ); };

  int		cryo() const { return m_cryo; };

  int		cassette() const { return m_cass; };

  int		afe() const { return m_afe; };

  int		mcm() const { return m_mcm; };

  int		chan() const { return m_chan; };

  double	adc() const { return m_adc; };

  bool          discrim() const { return m_discrim; };

  double        tdc() const { return m_tdc; };

  void		removeNoise( double delta ) { if( m_adc < 511 ) m_adc -= delta; else m_adc = 520; };
  
  int		triggerTime() const		{ return m_trig_time; };

  int		bunchNumber() const		{ return m_bunch_num; };

  void		setTrigInfo( int t, int b )	{ m_trig_time = t; m_bunch_num = b; };

private :

  int		m_cryo;
  int		m_cass;
  int		m_afe;
  int		m_mcm;
  int		m_chan;
  double	m_adc;
  bool          m_discrim;
  double        m_tdc;
  int		m_trig_time;
  int		m_bunch_num;
};

typedef std::vector<VlpcHit*> VlpcHitVector;

void	clearVlpcHitVector( VlpcHitVector& );

#endif
