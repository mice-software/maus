/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#include <stdlib.h>
#include <stdio.h>
#include "Calib/SciFiReconstructionParams.hh"

#include "Interface/dataCards.hh"

SciFiReconstructionParams* SciFiReconstructionParams::_instance = (SciFiReconstructionParams*) NULL;

SciFiReconstructionParams* SciFiReconstructionParams::getInstance()
{
  if( ! _instance )
    _instance = new SciFiReconstructionParams();

  return _instance;
}

SciFiReconstructionParams::SciFiReconstructionParams()
{
// We are not using Reconstruction at the moment, and will be phasing this object out
/*
        activeRadius = MyDataCards.fetchValueDouble("SciFiActiveRadius");
	tdcFactor = MyDataCards.fetchValueDouble("SciFitdcFactor");
	adcFactor = MyDataCards.fetchValueDouble("SciFiadcFactor");
        combineWidth = MyDataCards.fetchValueDouble("SciFiCombineWidth");
        combineTime = MyDataCards.fetchValueDouble("SciFiCombineTime");
        combineRadius = MyDataCards.fetchValueDouble("SciFiCombineRadius");
        charge = MyDataCards.fetchValueDouble("BeamCharge");
        planeThickness = MyDataCards.fetchValueDouble("SciFiPlaneThickness");
        statusCut = MyDataCards.fetchValueDouble("SciFiTrackRecStatusCut");
        rCut = MyDataCards.fetchValueDouble("SciFiTrackRecRCut");
        nHitCut = MyDataCards.fetchValueDouble("SciFiTrackRecNHitCut");
        nPECut = MyDataCards.fetchValueDouble("SciFiTrackRecNPECut");
        chi2Cut = MyDataCards.fetchValueDouble("SciFiTrackRecChi2Cut");
        tchi2Cut = MyDataCards.fetchValueDouble("SciFiTripletChi2Cut");
	MUXNum = MyDataCards.fetchValueInt("SciFiMUXNum");

        stationFiberSpacing[0] = 
		MyDataCards.fetchValueDouble("SciFiStation0FiberSpacing");
        stationFiberSpacing[1] = 
		MyDataCards.fetchValueDouble("SciFiStation1FiberSpacing");
        stationFiberSpacing[2] = 
		MyDataCards.fetchValueDouble("SciFiStation2FiberSpacing");
        stationFiberSpacing[3] = 
		MyDataCards.fetchValueDouble("SciFiStation3FiberSpacing");
        stationFiberSpacing[4] = 
		MyDataCards.fetchValueDouble("SciFiStation4FiberSpacing");
        BField = MyDataCards.fetchValueDouble("SciFiTrackerBField");

	trackerPosition[0] = MyDataCards.fetchValue3Vector("SciFiTracker0Pos");
	stationPosition[0][0] =
		MyDataCards.fetchValue3Vector("SciFiTracker0Station0Pos");
	stationPosition[0][1] = 
		MyDataCards.fetchValue3Vector("SciFiTracker0Station1Pos");
	stationPosition[0][2] = 
		MyDataCards.fetchValue3Vector("SciFiTracker0Station2Pos");
	stationPosition[0][3] = 
		MyDataCards.fetchValue3Vector("SciFiTracker0Station3Pos");
	stationPosition[0][4] = 
		MyDataCards.fetchValue3Vector("SciFiTracker0Station4Pos");

	trackerPosition[1] = MyDataCards.fetchValue3Vector("SciFiTracker1Pos");
	stationPosition[1][0] = 
		MyDataCards.fetchValue3Vector("SciFiTracker1Station0Pos");
	stationPosition[1][1] = 
		MyDataCards.fetchValue3Vector("SciFiTracker1Station1Pos");
	stationPosition[1][2] = 
		MyDataCards.fetchValue3Vector("SciFiTracker1Station2Pos");
	stationPosition[1][3] = 
		MyDataCards.fetchValue3Vector("SciFiTracker1Station3Pos");
	stationPosition[1][4] = 
		MyDataCards.fetchValue3Vector("SciFiTracker1Station4Pos");
*/
}

SciFiReconstructionParams::~SciFiReconstructionParams()
{
}
