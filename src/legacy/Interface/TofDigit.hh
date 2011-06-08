// TofDigit.hh
//
// A class representing a hit on a single Tof PMT
//
// A. Fish - 12th September 2005

#ifndef INTERFACE_TOFDIGIT_H
#define INTERFACE_TOFDIGIT_H

#include "dataCards.hh"
#include "TofHit.hh"
#include <vector>
#include <string.h>

using namespace std;

class VmeAdcHit;
class VmefAdcHit;
class VmeTdcHit;

class TofSlabHit;

class TofDigit
{
  public :

    TofDigit() { miceMemory.addNew( Memory::TofDigit ); };
    TofDigit( int pmt )	{ miceMemory.addNew( Memory::TofDigit ); m_pmt = pmt; };
    TofDigit( TofHit*, int, int, double, double, int );
    //MICE TOF data:
    TofDigit( VmefAdcHit*, VmeTdcHit*, int, int, int, int, int, string, int, double );
    TofDigit( VmefAdcHit*, VmeTdcHit*, int, int, int, int, int, string );
    //KEK TOF data:
    TofDigit( VmeAdcHit*, VmeTdcHit*, int, int, int, int, double, double, double, std::vector<double>& );
    TofDigit( VmeTdcHit*, int, int, int, int, double, double, double, std::vector<double>& );
    TofDigit( VmeAdcHit*, int, int, int, int, double, double, std::vector<double>& );
    ~TofDigit()	{ miceMemory.addDelete( Memory::TofDigit ); };

			//Accessors

    void Print( bool verbose = true ) ;

    int 	stationNumber() const 		{ return m_stationNumber; }
    int 	planeNumber() const   		{ return m_planeNumber; }
    int 	slab() const	      		{ return m_slab; }
    int 	pmt() const 	      		{ return m_pmt; }
    int 	adc() const			{ return _adc; };
    int 	tdc() const			{ return _tdc; };
    int 	triggerTdc() const		{ return _triggerTdc; };
    int 	psChanWidth() const		{ return _psChanWidth; };
    double 	cableLengthCorrTdc() const	{ return _cableLengthCorrTdc; };
    double	pe() const			{ return _pe; };
    double	time() const			{ return _time; };
    double	rawTime() const			{ return _rawTime; };
    bool	isGood() const			{ return m_good; }

    VmeAdcHit*		vmeAdcHit() const	{ return m_adc; }
    VmefAdcHit*		vmefAdcHit() const	{ return m_fadc; }    
    VmeTdcHit*		vmeTdcHit() const	{ return m_tdc; }
    TofSlabHit*		slabHit() const		{ return m_slabHit; }
    TofHit*		tofHit() const 		{ return m_hit; };

    void	setTofHit( TofHit* hit )	{ m_hit = hit; };
    void	setAdc( int Adc )		{ _adc = Adc; };   
    void	setTdc( int Tdc )		{ _tdc = Tdc; };
    void	setPE( double PE )		{ _pe = PE; };
    void	setTime( double time )		{ _time = time; };

    void	setStationNumber( int stat )	{ m_stationNumber = stat; };
    void	setPlaneNumber( int plane )	{ m_planeNumber = plane; };
    void	setSlab( int slab )		{ m_slab = slab; };
    void	setPmt( int pmt )		{ m_pmt = pmt; };

    void	setVmeAdcHit(VmeAdcHit* hit)	{ m_adc=hit;}
    void	setVmefAdcHit(VmefAdcHit* hit)	{ m_fadc=hit;}
        
    void	setVmeTdcHit(VmeTdcHit* hit)	{ m_tdc=hit;}

    void	assignVmeHits(VmeAdcHit* adcHit, VmeTdcHit* tdcHit);
    void	assignVmeHits(VmefAdcHit* fadcHit, VmeTdcHit* tdcHit);
        
    void	setIsGood(bool isGood) 				{ m_good = isGood; }
    void	setSlabHit( TofSlabHit* hit )		{ m_slabHit = hit; };
    void	assignSlabHit( TofSlabHit* slab )	{ m_slabHit = slab;}
    void	assignTofHit( TofHit* hit ) 		{ m_hit = hit; };


  private :

    int 	m_stationNumber; //  TOF station
    int 	m_planeNumber;   //  x or y plane
    int 	m_pmt;
    int 	m_slab;

    VmeAdcHit*	m_adc;
    VmefAdcHit*	m_fadc;   
    VmeTdcHit*	m_tdc;
    TofSlabHit* m_slabHit;
    TofHit*	m_hit;

    string	m_pmtName;
    bool	m_good;
    double 	_pe;
    double	_time;
    double	_rawTime;
    int 	_adc;
    int 	_tdc;
    int 	_triggerTdc;
    int 	_psChanWidth;
    double 	_cableLengthCorrTdc;
};

#endif

