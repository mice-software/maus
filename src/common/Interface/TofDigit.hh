// TofDigit.hh
//
// A class representing a hit on a single Tof PMT
//
// A. Fish - 12th September 2005

#ifndef INTERFACE_TOFDIGIT_H
#define INTERFACE_TOFDIGIT_H

#include "Interface/dataCards.hh"
#include "Interface/TofHit.hh"
#include <vector>
#include <string.h>

using namespace std;

class VmeAdcHit;
class VmefAdcHit;
class VmeTdcHit;

class TofSlabHit;


//!  TofDigit class is used to hold all information recorded
//!  when we have a signal in some of the channels of the TOF detectros.
//!  TofDigit objects are created both in simulations or in analysis
//!  of real data.

class TofDigit
{
  public :
	 //! Default constructor.
    TofDigit() { miceMemory.addNew( Memory::TofDigit ); };

	 //! This constructor is for Monte Carlo data.
    TofDigit( TofHit*, int, int, double, double, int );

	 //! This constructor is for real data.
    TofDigit( VmefAdcHit*, VmeTdcHit*, int, int, int, int, int, string );

	 //! Destructor.
    ~TofDigit()	{ miceMemory.addDelete( Memory::TofDigit ); };

	 //! Dumps some information about this digit to the standard output.
    void Print( bool verbose = true );

    int 	stationNumber() 			const { return m_stationNumber; }
    int 	planeNumber() 				const { return m_planeNumber; }
    int 	slab() 						const	{ return m_slab; }
    int 	pmt() 						const { return m_pmt; }
    int 	adc() 						const	{ return _adc; };
    int 	tdc() 						const	{ return _tdc; };
    int 	triggerTdc() 				const	{ return _triggerTdc; };

	 //! Returns the number of photo electrons. WORNING : do not use for real data!!!
    double	pe() 						const	{ return _pe; };

    double	time() 					const	{ return _time; };
    double	rawTime() 				const	{ return _rawTime; };
    bool		isGood() 				const	{ return m_good; }
    bool		isCalibrated() 		const	{ return m_calibrated; };

    VmefAdcHit*	vmefAdcHit() 	const	{ return m_fadc; }
    VmeTdcHit*		vmeTdcHit() 	const	{ return m_tdc; }
    TofSlabHit*	slabHit() 		const	{ return m_slabHit; }
    TofHit*			tofHit() 		const { return m_hit; };

    void	setTofHit( TofHit* hit )	{ m_hit = hit; };
    void	setAdc( int Adc )				{ _adc = Adc; };
    void	setTdc( int Tdc )				{ _tdc = Tdc; };
    void	setPE( double PE )			{ _pe = PE; };
    void	setTime( double time )		{ _time = time; };

    void	setStationNumber( int stat )	{ m_stationNumber = stat; };
    void	setPlaneNumber( int plane )	{ m_planeNumber = plane; };
    void	setSlab( int slab )				{ m_slab = slab; };
    void	setPmt( int pmt )					{ m_pmt = pmt; };

    void	setVmefAdcHit(VmefAdcHit* hit){ m_fadc=hit;}
    void	setVmeTdcHit(VmeTdcHit* hit)	{ m_tdc=hit;}

    void	assignVmeHits(VmefAdcHit* fadcHit, VmeTdcHit* tdcHit);

    void	setIsGood(bool isGood) 				{ m_good = isGood; }
    void	setSlabHit( TofSlabHit* hit )		{ m_slabHit = hit; };
    void	assignSlabHit( TofSlabHit* slab ){ m_slabHit = slab;}
    void	assignTofHit( TofHit* hit ) 		{ m_hit = hit; };


  private :
	 //!  TOF station number.
    int 	m_stationNumber;

	 //!  X or Y plane.
    int 	m_planeNumber;

	 //! PMT number. Can be 0 or 1.
    int 	m_pmt;

	 //! Slab number.
    int 	m_slab;

	 //! Pointer to the VmefAdcHit used to create this digit. Points to NULL if this digit is created in simulation.
    VmefAdcHit*	m_fadc;

	 //! Pointer to the VmeTdcHit used to create this digit. Points to NULL if this digit is created in simulation.
    VmeTdcHit*	m_tdc;

	 //! Pointer to the TofHit used to create this digit. Points to NULL if this digit is created by using real data.
    TofHit*	m_hit;

	 //! Pointer to the TofSlabHit created from this digit. Points to NULL if no TofSlabHit created.
    TofSlabHit* m_slabHit;

	 //! Name of the PMT as in TofChannelMap. This name is not set if the digit is created in simulation!!!
    string	m_pmtName;

	 //! Number of photo electrons. WORNING : the calculation of this data member is implemented only in the simulation. For the real data _pe is not set!!!
    double 	_pe;

	 //! The fADC record of the signal.
    int 	_adc;

	 //! The TDC record of the signal.
    int 	_tdc;

	 //! The TDC record of the trigger signal on whichever board the PMT TDC is recorded.
    int 	_triggerTdc;

	 //! Time of the signal in picoseconds. (_tdc - _triggerTdc)*25;
    double	_rawTime;

	 //! Time of the signal after calibration corrections. This data member is set only if the digit is calibrated.
    double	_time;

	 //! True if this digit is good.
    bool	m_good;

	 //! True if this digit is calibrated.
	 bool m_calibrated;

};

#endif

