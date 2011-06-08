

#include <stdlib.h>
#include <stdio.h>
#include "TpgReconstructionParams.hh"
#include "Interface/dataCards.hh"

TpgReconstructionParams* TpgReconstructionParams::m_instance = (TpgReconstructionParams*) NULL;

TpgReconstructionParams* TpgReconstructionParams::getInstance()
{
  if( !m_instance )
   m_instance = new TpgReconstructionParams();

  return m_instance;
}

TpgReconstructionParams::TpgReconstructionParams()
{
  m_BField = MyDataCards.fetchValueDouble("TPGTrackerBField");

  m_charge = MyDataCards.fetchValueDouble("BeamCharge");

  m_driftVelocity = MyDataCards.fetchValueDouble("TPGDriftVelocity");

  m_samplingPeriod = MyDataCards.fetchValueDouble("TPGSamplingPeriod");
  
  m_stripSpace = MyDataCards.fetchValueDouble("TPGStripSpacing");

  m_sigmaTrans = MyDataCards.fetchValueDouble("TPGSigmaTrans");

  m_sigmaLong = MyDataCards.fetchValueDouble("TPGSigmaLong");

  m_rCut = MyDataCards.fetchValueDouble("TPGTrackRecRCut");

  m_gasRadius = MyDataCards.fetchValueDouble("TPGGasRadius");

  m_amplitudeCut = MyDataCards.fetchValueDouble("TPGTrackRecAmplitudeCut");

  m_nPointsCut = MyDataCards.fetchValueDouble("TPGTrackRecNPointsCut");

  m_chi2Cut = MyDataCards.fetchValueDouble("TPGTrackRecChi2Cut");
   
  m_pointChi2Cut = MyDataCards.fetchValueDouble("TPGTrackRecPointChi2Cut");

  m_trackChi2Cut = MyDataCards.fetchValueDouble("TPGTrackRecTrackChi2Cut");
   
  m_trackerPosition[0] = MyDataCards.fetchValue3Vector("TPGTracker0Pos");

  m_trackerPosition[1] = MyDataCards.fetchValue3Vector("TPGTracker1Pos");


}

TpgReconstructionParams::~TpgReconstructionParams()
{
}
