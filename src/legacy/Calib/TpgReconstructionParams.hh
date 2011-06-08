

#ifndef TPGRECONSTRUCTIONPARAMS_HH
#define TPGRECONSTRUCTIONPARAMS_HH

#include <stdio.h>
#include "CLHEP/Vector/ThreeVector.h"



class TpgReconstructionParams
{
public:

  //! Static method to retrieve the instance of this class
  static TpgReconstructionParams* getInstance();

  //! Default constructor
  TpgReconstructionParams();

  //! Default destructor
  ~TpgReconstructionParams();

  //! return the position of the tracker
   CLHEP::Hep3Vector getTrackerPosition(int i)  {return m_trackerPosition[i];}


  //! Return the magnetic field (T)
  double getBField()         {return m_BField;}

  //! Return charge
  double getCharge()	     {return m_charge;}
 
  //! Return the electron drift velocity (mm/mus)
  double getDriftVelocity()  {return m_driftVelocity;}

  //! Return the sampling period (mus)
  double getSamplingPeriod() {return m_samplingPeriod;}

  //! Return the pitch between strip in readout foil (mm)
  double getStripSpace()     {return m_stripSpace;}

  //! Return the transverse diffusion (mm/sgrt(z[cm])
  double getSigmaTrans()     {return m_sigmaTrans;}

  //! Return the longitudinal diffusion (mm/sqrt(z[cm])
  double getSigmaLong()      {return m_sigmaLong;}

  //! Return the cut on R used when building tracks
  double getRCut()           {return m_rCut;}
 
  //! Return the cut on the amplitude of hits
  double getAmplitudeCut()   {return m_amplitudeCut;}

  //! Return the cut on the number of track points 
  double getNPointsCut()     {return m_nPointsCut;}

  //! Return the cut on the internal chi2 of the space point 
  double getChi2Cut()        {return m_chi2Cut;}

  //! Return the cut on the chi2 of the match between extrapolation of the track to the point
  double getPointChi2Cut()   {return m_pointChi2Cut;}

  //! Return the cut on the chi2 of the reconstructed track 
  double getTrackChi2Cut()   {return m_trackChi2Cut;}
 
  //! Return the radius of TPG active gas volume (mm)
  double getGasRadius()      {return m_gasRadius;}
	      
 private:

	static TpgReconstructionParams* m_instance;

  	CLHEP::Hep3Vector m_trackerPosition[2];
	
	double m_BField;
	double m_charge;
        double m_driftVelocity;
        double m_samplingPeriod;
        double m_stripSpace;
        double m_sigmaTrans;
        double m_sigmaLong;
        double m_gasRadius;
	
        double m_rCut;
        double m_amplitudeCut;
        double m_nPointsCut;
        double m_chi2Cut;
        double m_pointChi2Cut;
        double m_trackChi2Cut;
        
};
	

#endif
