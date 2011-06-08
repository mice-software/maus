// TofSlabHit.hh
//
// A class representing a slab hit on Tof
//
// A. Fish - 2nd October 2005

#ifndef INTERFACE_TOFSLABHIT_H
#define INTERFACE_TOFSLABHIT_H

#include "Interface/Memory.hh"
#include "TofCalibrationMap.hh"
#include "PmtKey.hh"
#include "TriggerKey.hh"

class VmeAdcHit;
class VmeTdcHit;
class TofDigit;
class MICERun;
class MiceModule;

class TofSlabHit
{
  public :

	TofSlabHit()			{};
	TofSlabHit( TofDigit*, TofDigit* );
	TofSlabHit( MICERun*, const MiceModule*, TofDigit* );
	TofSlabHit( MICERun*, const MiceModule*, TofDigit*, TofDigit* );
	~TofSlabHit() 		{ miceMemory.addDelete( Memory::TofSlabHit ); };
    
	TofDigit*	digit1() const	 		{ return _digit1; }
	TofDigit*	digit2() const   		{ return _digit2; }
	TofDigit*	digit( int const pmt ) const;

	bool		isGood() const			{ return m_good; };
	int 		station() const			{ return m_station; };
	int   		plane() const			{ return m_plane; };
	int			slabNumber() const		{ return m_slabNumber; };
	double		pe() const;
	double		RawTime() const			{ return rawTime; };
	double		CorrectedTime() const	{ return correctedTime; };
	double		time() const			{ return correctedTime; };

	void		SetStation(int station) 	{ m_station = station; };
	void		SetPlane(int plane)			{ m_plane = plane; };
	void		SetSlab(int slabNumber)		{ m_slabNumber = slabNumber; };
	void 		SetGood( bool g )			{ m_good = g; };
	void		SetRawTime(double t)		{ rawTime = t; };
	void		SetCorrectedTime(double t)	{ correctedTime = t; };

	bool		CalibrateTime(TofCalibrationMap* calib, TriggerKey* Tkey);
	bool		CalibrateTime();

	void 		SetDigit1( TofDigit* Digit)	{ _digit1 = Digit; };
	void 		SetDigit2( TofDigit* Digit)	{ _digit2 = Digit; };

	void 		Print( bool verbose = true );

  private :

	int		m_station;
	int 		m_plane;
	int 		m_slabNumber;
	bool		m_good;

	double		rawTime;
	double		correctedTime;

	TofDigit*   _digit1;
	TofDigit*   _digit2;

	MICERun* theRun;
	const MiceModule* theModule;

};

#endif

