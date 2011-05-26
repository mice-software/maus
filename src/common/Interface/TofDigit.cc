// TofDigit.cc
//
// A class representing a hit on a single Tof PMT
//
// A. Fish - 12th September 2005

#include "Interface/TofDigit.hh"

#include "Interface/VmeAdcHit.hh"
#include "Interface/VmefAdcHit.hh"
#include "Interface/VmeTdcHit.hh"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>

using namespace std;


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
  m_hit = NULL;

  _triggerTdc = triggerTdc;

  m_good = false;
  m_calibrated = false;

  if(m_fadc) _adc = m_fadc->GetCharge(ceaPedMax);
  if(m_tdc ) _tdc = m_tdc->tdc();
  if(_triggerTdc >= 0) _tdc -= triggerTdc;
  _pe = 0;

  if(pmt >= 0 && slab >= 0 && triggerTdc >= 0)
  {
    m_good = true;
    _rawTime = static_cast<double>( _tdc * 25 );
    _time = -99999;
  }
}

TofDigit::TofDigit( TofHit* hit, int adc, int tdc, double pe, double time, int pmt )
{
  miceMemory.addNew( Memory::TofDigit );

  m_fadc = NULL;
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
  m_calibrated = true;
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

