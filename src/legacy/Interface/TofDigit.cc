// TofDigit.cc
//
// A class representing a hit on a single Tof PMT
//
// A. Fish - 12th September 2005

#include "TofDigit.hh"

#include "VmeAdcHit.hh"
#include "VmefAdcHit.hh"
#include "VmeTdcHit.hh"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>

using namespace std;

/// Initialise
TofDigit::TofDigit( VmeAdcHit* adc, VmeTdcHit* tdc, int stat, int plane, int pmt, int slab,
                    double ped, double gain, double t2time, std::vector<double>& tw )
{
  miceMemory.addNew( Memory::TofDigit );

  m_adc = adc;
  m_tdc = tdc;
  m_stationNumber = stat;
  m_planeNumber = plane;
  m_pmt = pmt;
  m_slab = slab;
  m_slabHit = NULL;

  m_good = false;

  if( m_adc && m_tdc && pmt >= 0 && slab >= 0 )
  {
    _adc = m_adc->adc();
    _tdc = m_tdc->tdc();
    m_good = true;
    _pe = (int)(( m_adc->adc() - ped ) / gain );

    _time = ( (double)m_tdc->tdc() * t2time );
    double adcroot = 1.0 / sqrt( (double)m_adc->adc() - ped );
    double pow = 1.;
    for( unsigned int i = 0; i < tw.size(); ++i )
    {
       _time -= pow * tw[i];
       pow *= adcroot;
    }
  }
}

TofDigit::TofDigit( VmefAdcHit* fadc, VmeTdcHit* tdc, int stat, int plane, int slab, int pmt, int triggerTdc, string name, int psChanWidth, double cableLengthCorrTdc )
{
  miceMemory.addNew( Memory::TofDigit );

  m_fadc = fadc;
  m_tdc = tdc;
  m_stationNumber = stat;
  m_planeNumber = plane;
  m_pmt = pmt;
  m_pmtName = name;
  m_slab = slab;
  m_slabHit = NULL;

  _triggerTdc = triggerTdc; // The TDC of the trigger on whichever board the PMT TDC is recorded
  _psChanWidth = psChanWidth; // One TDC unit corresponds to this may picoseconds
  _cableLengthCorrTdc = cableLengthCorrTdc; // The time correction in TDC channels due to differing cable lengths. calculated by MICE/Applications/TofCalibration.cc

  _adc = m_fadc->GetCharge(ceaPedMax);
  _tdc = m_tdc->tdc() - _triggerTdc;
  _pe = 0;
  
  m_good = false;
  if(pmt >= 0 && slab >= 0 )
  {
    m_good = true;
    _rawTime = static_cast<double>( _tdc * _psChanWidth );
    _time = _rawTime - _cableLengthCorrTdc;
  }
/*
  if( m_tdc && pmt >= 0 && slab >= 0 )
  {
    _adc = 0;
    _tdc = m_tdc->tdc() - _triggerTdc;
    m_good = true;
    if ( m_fadc ) _pe = m_fadc->GetCharge();

    _rawTime = static_cast<double>( _tdc * _psChanWidth ) / 1000.; //basic time units are ns
    _time = _rawTime - _cableLengthCorrTdc;
  }
*/
}


TofDigit::TofDigit( VmefAdcHit* fadc, VmeTdcHit* tdc, int stat, int plane, int slab, int pmt, int triggerTdc, string name )
{
  miceMemory.addNew( Memory::TofDigit );

  m_fadc = fadc;
  m_tdc = tdc;
  m_stationNumber = stat;
  m_planeNumber = plane;
  m_pmt = pmt;
  m_pmtName = name;
  m_slab = slab;
  m_slabHit = NULL;

  _triggerTdc = triggerTdc; // The TDC of the trigger on whichever board the PMT TDC is recorded

  _adc = m_fadc->GetCharge(ceaPedMax);
  _tdc = m_tdc->tdc() - _triggerTdc;
  _pe = 0;
  
  m_good = false;
  if(pmt >= 0 && slab >= 0 )
  {
    m_good = true;
    _rawTime = static_cast<double>( _tdc * 25 );
    _time = -99999;
  }
}

/* added by D.Adey for adc and tdc only Tof */

TofDigit::TofDigit( VmeTdcHit* tdc, int stat, int plane, int pmt, int slab,
                    double ped, double gain, double t2time, std::vector<double>& tw )
{
  miceMemory.addNew( Memory::TofDigit );

  m_adc = NULL;
  m_tdc = tdc;
  m_stationNumber = stat;
  m_planeNumber = plane;
  m_pmt = pmt;
  m_slab = slab;
  m_slabHit = NULL;

  m_good = false;

  if(m_tdc && pmt >= 0 && slab >= 0 )
  {
    _adc = 0;
    _tdc = m_tdc->tdc();
    m_good = true;
    _pe = (int)(( _adc - ped ) / gain );

    _time = ( (double)m_tdc->tdc() * t2time );
    //double adcroot = 0;
    double pow = 1.;
    for( unsigned int i = 0; i < tw.size(); ++i )
    {
       _time -= pow * tw[i];
       //pow *= adcroot;
    }
  }
}

TofDigit::TofDigit( VmeAdcHit* adc, int stat, int plane, int pmt, int slab,
                    double ped, double gain, std::vector<double>& tw )
{
  miceMemory.addNew( Memory::TofDigit );

  m_adc = adc;
  m_tdc = NULL;
  m_stationNumber = stat;
  m_planeNumber = plane;
  m_pmt = pmt;
  m_slab = slab;
  m_slabHit = NULL;

  m_good = false;

  if( m_adc && m_tdc && pmt >= 0 && slab >= 0 )
  {
    _adc = m_adc->adc();
    _tdc = 0;
    m_good = true;
    _pe = (int)(( m_adc->adc() - ped ) / gain );

    _time = 0;
    double adcroot = 1.0 / sqrt( (double)m_adc->adc() - ped );
    double pow = 1.;
    for( unsigned int i = 0; i < tw.size(); ++i )
    {
       //_time -= pow * tw[i];
       pow *= adcroot;
    }
  }
}

TofDigit::TofDigit( TofHit* hit, int adc, int tdc, double pe, double time, int pmt )
{
  miceMemory.addNew( Memory::TofDigit );

  m_adc = NULL;
  m_tdc = NULL;
  m_slabHit = NULL;

  _adc = adc;
  _tdc = tdc;
  m_stationNumber = hit->GetStationNo();
  m_planeNumber = hit->GetPlaneNo();
  m_pmt = pmt;
  m_slab = hit->GetStripNo();

  m_hit = hit;

  _pe = pe;
  _time = time;
  _rawTime = time;
  m_good = true;
}

/// Assign VME Hits

void TofDigit::assignVmeHits( VmeAdcHit* adc, VmeTdcHit* tdc)
{
  m_adc = adc;
  m_tdc = tdc;
  if( m_adc && m_tdc && m_pmt > 0 )
    m_good = true;
}

void TofDigit::assignVmeHits( VmefAdcHit* fadc, VmeTdcHit* tdc)
{
  m_fadc = fadc;
  m_tdc = tdc;
  if( m_fadc && m_tdc && m_pmt > 0 )
    m_good = true;
}

void TofDigit::Print( bool verbose )
{

	if ( verbose ) {
  	   cout << "____________________________" << endl;
  	   cout << "  TofDigit" << endl;
	   cout << "  Station Number : " << m_stationNumber << endl;
	   cout << "  Plane Number : "<<  m_planeNumber<< endl;
  	   cout << "  Strip Number : "<< m_slab << endl;
  	   cout << "  PMT Numbe : " << m_pmt << endl;
  	   cout << "  PMT Name : " << m_pmtName << endl;
  	   cout << "  TDC : " << _tdc << endl;
 	   cout << "  time (ps): " <<_time<<
    	cout << "  Charge : " << _pe << endl;
    	cout << "____________________________" << endl;

	}
	if ( !verbose ) {
		cout << "TofDigit: (" << m_stationNumber << ", " << m_planeNumber << ", " << m_slab << ", " << m_pmt << "), time (ps): " << _time/CLHEP::ns << ", Charge: " << _pe << endl; 
	}
}

