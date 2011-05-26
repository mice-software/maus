/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#ifndef SCIFIDIGIT_HH
#define SCIFIDIGIT_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>

#include "Interface/Memory.hh"

/**
* SciFiDigit is a class to handle the digitisation of hits in the Scintillating
* Fibre tracker
*/

class SciFiHit;
class VlpcHit;

class SciFiDigit
{
  public:

	//! default constructor
	SciFiDigit();

	//! constructor initialised from an input stream
	SciFiDigit( SciFiHit*, int, int );
	
        //! constructor intialised from a VlpcHit, calib and cabling information
 	SciFiDigit( VlpcHit*, int, int, int, int, double );

        //! copy constructor
        SciFiDigit( const SciFiDigit& );
     
	//! default destructor
	~SciFiDigit()			{ miceMemory.addDelete( Memory::SciFiDigit ); };

	//! return the tracker number (0 or 1)
	int getTrackerNo() const { return trackerNo; };
        void setTrackerNo( int t ) { trackerNo = t; };

	//! return the station number ( 0, 1, 2, ...)
	int getStationNo() const { return stationNo; };
	void setStationNo( int s ) { stationNo = s; };

	//! return the plane number (0, 1 or 2)
	int getPlaneNo() const { return planeNo; };
	void setPlaneNo( int p ) { planeNo = p; };

	//! return the channel number
	int getChanNo() const { return fiberNo; };
	void setChanNo( int c ) { fiberNo = c; };

	//! return the ADC value for this hit
	int getAdcCounts() const { return adcCounts; };
        void setAdcCounts( int a ) { adcCounts = a; };

	double getNPE() const { return numPE; };

	//! return the ADC value as a number of PE
	double getPhotoElectrons() const { return numPE; };
 	void   setPhotoElectrons( double pe ) { numPE = pe; };

	//! return the TDC value for this hit
        int getTdcCounts() const { return tdcCounts; };
	void setTdcCounts( int t ) { tdcCounts = t; };

	//! Return true if the digit is used in a cluster
	bool		isUsed() const { return m_used; };

	//! Flag this digit as being used
	void		setUsed() { m_used = true; };

        //! Set the "used" status to true or false
        void            setUsed( bool u ) { m_used = u; };

        SciFiHit*	mcHit() 			{ return m_mc_hit; };
        void		setMcHit( SciFiHit* hit ) 	{ m_mc_hit = hit; };

	VlpcHit*	vlpcHit()			{ return m_vlpc_hit; };
	void		setVlpcHit( VlpcHit* hit )	{ m_vlpc_hit = hit; };

	bool		mergeDigit( SciFiDigit* );

private:

	int trackerNo;
	int stationNo;
	int planeNo;
	int fiberNo;
	int adcCounts;
   	double numPE;
	int tdcCounts;

	SciFiHit* m_mc_hit;
	VlpcHit*  m_vlpc_hit;

	bool m_used;
};

#endif
