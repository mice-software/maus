// TofSlabHit.hh
//
// A class representing a slab hit on Tof
//
// A. Fish - 2nd October 2005

#ifndef INTERFACE_TOFSLABHIT_H
#define INTERFACE_TOFSLABHIT_H

#include "Interface/Memory.hh"
#include "Recon/TOF/TofCalibrationMap.hh"
#include "Recon/TOF/PmtKey.hh"
#include "Recon/TOF/TriggerKey.hh"

class VmeAdcHit;
class VmeTdcHit;
class TofDigit;
class MICERun;
class MiceModule;
class TofSpacePoint;

//! TofSlabHit class is used to hold measurements done when we have signals in both channels of one slabe in some of the TOF stations.
//! TofSlabHit objects are created from two TofDigit objects.
class TofSlabHit
{
  public :
	//! Default constructor.
	TofSlabHit()	{miceMemory.addNew( Memory::TofSlabHit );};

	//! Constructor.
	TofSlabHit( TofDigit*, TofDigit* );

	//! Constructor.
	TofSlabHit( const MiceModule*, TofDigit*, TofDigit* );

	//! Destructor.
	~TofSlabHit() 		{ miceMemory.addDelete( Memory::TofSlabHit ); };

	//! Returns the first digit used to creat this TofSlabHit.
	TofDigit*	digit1() const	 		{ return _digit1; };

	//! Returns the second digit used to creat this TofSlabHit.
	TofDigit*	digit2() const   		{ return _digit2; };

   //! Returns one of the digits used to creat this TofSlabHit. The argument can be 0 pr 1.
	TofDigit*	digit( int const pmt ) const;

	TofSpacePoint* spacePoint()	const	{return _spoint;};

	bool		isGood() const			{ return m_good; };
	bool		isCalibrated() const	{ return m_calibrated; };
	int 		station() const		{ return m_station; };
	int   	plane() const			{ return m_plane; };
	int		slabNumber() const	{ return m_slabNumber; };

	//! Returns the number of photo electrons. WORNING : do not use for real data!!!
	double	pe() const;

	double	RawTime() const		{ return rawTime; };
	double	CorrectedTime() const{ return correctedTime; };
	double	time() const			{ return correctedTime; };

	//! Returns the difference between the times (after calibration corrections) of the two digits.
	double   dt();

	//! Returns the difference between the raw times (before calibration corrections) of the two digits.
	double   rawdt();

	const 	MiceModule* GetModule()	 {return theModule;};

	void		SetStation(int station) 	{ m_station = station; };
	void		SetPlane(int plane)			{ m_plane = plane; };
	void		SetSlab(int slabNumber)		{ m_slabNumber = slabNumber; };
	void 		SetGood( bool g )			{ m_good = g; };
	void		SetRawTime(double t)		{ rawTime = t; };
	void		SetCorrectedTime(double t)	{ correctedTime = t; };
	void		SetSpacePoint(TofSpacePoint* sp)	{ _spoint = sp; };
	void 		SetDigit1( TofDigit* Digit)	{ _digit1 = Digit; };
	void 		SetDigit2( TofDigit* Digit)	{ _digit2 = Digit; };


	//! Dumps some information about this digit to the standard output.
	void 		Print( bool verbose = true );

	//! Applies calibration corrections. Use this function only for real data.
	bool		CalibrateTime(TofCalibrationMap* calib, TriggerKey* Tkey);

	//! Applies calibration corrections. Use this function only for Monte Calro data.
	bool		CalibrateTime(double c);

  private :

   bool Initialize(TofDigit* dig1, TofDigit* dig2);

	//! TOF station number.
	int	m_station;

	//! X or Y plane.
	int 	m_plane;

	//! Slab number.
	int 	m_slabNumber;

	//! True if this slab hit is good.
	bool	m_good;

	//! True if this slab hit is calibrated.
	bool	m_calibrated;

	//! Raw time berore calibration corrections.
	double	rawTime;

	//! Time of after calibration corrections. This data member is set only if the slab hit is calibrated.
	double	correctedTime;

	//! Pointer to the first TofDigit used to create this slab hit.
	TofDigit*   _digit1;

	//! Pointer to the second TofDigit used to create this slab hit.
	TofDigit*   _digit2;

	//! Pointer to the TofSpacePoint created from this slab hit. Points to NULL if no TofSpacePoint created.
	TofSpacePoint* _spoint;

	//! Pointer to the MiceModule of the slab.
	const MiceModule* theModule;

};

#endif

