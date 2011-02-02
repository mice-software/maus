/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#ifndef SCIFIRECONSTRUCTIONPARAMS_HH
#define SCIFIRECONSTRUCTIONPARAMS_HH

#include <stdio.h>
#include "CLHEP/Vector/ThreeVector.h"

/**
* SciFiReconstructionParams holds information needed to perform the reconstruction
* of tracks in the Scintillating Fibre trackers.
* This class is implemented as a singleton, the instance of this class is accesed
* through the static member function getInstance().
**/

class SciFiReconstructionParams
{
public:

        //! Static method to retrieve the instance of this class
	static SciFiReconstructionParams* getInstance();

private:

        //! Default constructor
	SciFiReconstructionParams();

public:

        //! Default destructor
	~SciFiReconstructionParams();

	//! Return the position of the center of the tracker
        CLHEP::Hep3Vector getTrackerPosition(int i)  {return trackerPosition[i];}

	//! Return the x position of the center of the station st within the tracker tr (mm)
	double getXPos(int tr, int st)  {return stationPosition[tr][st].x();}

	//! Return the y position of the center of the station st within the tracker tr (mm)
	double getYPos(int tr, int st)  {return stationPosition[tr][st].y();}

	//! Return the z position of the center of the station st within the tracker tr (mm)
	double getZPos(int tr, int st)  {return stationPosition[tr][st].z();}

	//! Return the radius of the active material (mm)
        double getActiveRadius()  {return activeRadius;}

	//! Return the spacing of the fibres (?units)
	double getSpacing(int i)  {return stationFiberSpacing[i];}

	//! Return the TDC factor
	double getTdcFactor()     {return tdcFactor;}

	//! Return the ADC factor
	double getAdcFactor()     {return adcFactor;}

	//! Return the width used to combine hits (mm)
	double getCombineWidth()  {return combineWidth;}

	//! Return the time used to combine hits (ns)
	double getCombineTime()   {return combineTime;}

	//! Return the radius used to combine hits (mm)
	double getCombineRadius() {return combineRadius;}

	//! Return the magnetic field (T)
	double getBField()        {return BField;}

	//! Return the charge
	double getCharge()	  {return charge;}

	//! Return the thickness of a Sci Fi plane (mm)
	double getPlaneThickness(){return planeThickness;}

	//! Return the cut on status used in processing
	double getStatusCut()     {return statusCut;}

	//! Return the cut on R used when building tracks
	double getRCut()          {return rCut;}

	//! Return the cuts on the number of hits in an event
	double getNHitCut()       {return nHitCut;}

	//! Return the cut on the number of photo electrons
	double getNPECut()       {return nPECut;}

	//! Return the cut on chi2
	double getChi2Cut()       {return chi2Cut;}

	//! Return the cut on chi2 ???
	double getTChi2Cut()       {return tchi2Cut;}

	//! Return the level of multiplexing (or ganging)
	int getMUXNum()           {return MUXNum;}
	      
private:

	static SciFiReconstructionParams* _instance;

	CLHEP::Hep3Vector trackerPosition[2];
	CLHEP::Hep3Vector stationPosition[2][5];

	double activeRadius;
	double stationFiberSpacing[5];
	double tdcFactor;
	double adcFactor;
	double BField;
	double charge;
	double planeThickness;

	double combineWidth;
	double combineTime;
	double combineRadius;

	double statusCut, rCut, nHitCut, nPECut, chi2Cut, tchi2Cut;

	int MUXNum;
};

#endif
