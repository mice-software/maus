/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "src/map/MapCppEMRMCDigitization/MapCppEMRMCDigitization.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppEMRMCDigitization(void) {
  PyWrapMapBase<MAUS::MapCppEMRMCDigitization>::PyWrapMapBaseModInit
                                                ("MapCppEMRMCDigitization", "", "", "", "");
}

MapCppEMRMCDigitization::MapCppEMRMCDigitization()
    : MapBase<MAUS::Data>("MapCppEMRMCDigitization") {
}

void MapCppEMRMCDigitization::_birth(const std::string& argJsonConfigDocument) {
  _classname = "MapCppEMRMCDigitization";
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    throw MAUS::Exception(Exception::recoverable,
                      "Could not resolve ${MAUS_ROOT_DIR} environment variable",
                      "MapCppEMRMCDigitization::birth");
  }

  //  JsonCpp setup
  Json::Value configJSON;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _secondary_n_low = configJSON["EMRsecondaryNLow"].asInt();
  _secondary_tot_low = configJSON["EMRsecondaryTotLow"].asInt();
  _secondary_bunching_width = configJSON["EMRsecondaryBunchingWidth"].asInt();

  _do_sampling = configJSON["EMRdoSampling"].asInt();
  _nph_per_MeV = configJSON["EMRnphPerMeV"].asInt();
  _seed = configJSON["EMRseed"].asInt();
  _trap_eff = configJSON["EMRtrapEff"].asDouble();
  _QE_MAPMT = configJSON["EMRqeMAPMT"].asDouble();
  _QE_SAPMT = configJSON["EMRqeSAPMT"].asDouble();
  _nADC_per_pe_MAPMT = configJSON["EMRnadcPerPeMAPMT"].asDouble();
  _nADC_per_pe_SAPMT = configJSON["EMRnadcPerPeSAPMT"].asDouble();
  _electronics_response_spread_MAPMT = configJSON["EMRelectronicsResponseSpreadMAPMT"].asDouble();
  _electronics_response_spread_SAPMT = configJSON["EMRelectronicsResponseSpreadSAPMT"].asDouble();
  _dbb_count = configJSON["EMRdbbCount"].asDouble();
  _fadc_count = configJSON["EMRfadcCount"].asDouble();
  _time_response_spread = configJSON["EMRtimeResponseSpread"].asInt();
  _tot_func_p1 = configJSON["EMRtotFuncP1"].asDouble();
  _tot_func_p2 = configJSON["EMRtotFuncP2"].asDouble();
  _tot_func_p3 = configJSON["EMRtotFuncP3"].asDouble();
  _acquisition_window = configJSON["EMRacquisitionWindow"].asInt();
  _signal_integration_window = configJSON["EMRsignalIntegrationWindow"].asInt();
  _arrival_time_shift = configJSON["EMRarrivalTimeShift"].asInt();
  _arrival_time_gaus_width = configJSON["EMRarrivalTimeGausWidth"].asDouble();
  _arrival_time_uniform_width = configJSON["EMRarrivalTimeUniformWidth"].asDouble();
  _pulse_shape_landau_width = configJSON["EMRpulseShapeLandauWidth"].asDouble();
  _fom = configJSON["EMRfom"].asString();
  _birks_constant = configJSON["EMRbirksConstant"].asDouble();
  _signal_energy_threshold = configJSON["EMRsignalEnergyThreshold"].asDouble();
  _baseline_spread = configJSON["EMRbaselineSpread"].asInt();
  _maximum_noise_level = configJSON["EMRmaximumNoiseLevel"].asInt();
  _deltat_shift = configJSON["EMRdeltatShift"].asDouble();
  _baseline_position = configJSON["EMRbaselinePosition"].asDouble();
  _arrival_time_spread = configJSON["EMRarrivalTimeSpread"].asDouble();

  // Generate random pedestals and noise in the SAPMTs
  _rand = new TRandom3(_seed);

  for (size_t iPlane = 0; iPlane < _number_of_planes; iPlane++) {
    _baseline[iPlane] = _baseline_position
                 + static_cast<int>(_rand->Uniform(-_baseline_spread, _baseline_spread));
    _noise_level[iPlane] = static_cast<int>(_rand->Uniform(0, _maximum_noise_level));
    _noise_position[iPlane] = static_cast<int>(_rand->Uniform(0, 1.9999999));
  }

  // Load the EMR calibration map
  bool loaded = _calibMap.InitializeFromCards(configJSON);
  if (!loaded)
    throw(Exception(Exception::recoverable,
          "Could not find EMR calibration map",
          "MapCppEMRMCDigitizer::birth"));

  // Load the EMR attenuation map
  loaded = _attenMap.InitializeFromCards(configJSON);
  if (!loaded)
    throw(Exception(Exception::recoverable,
          "Could not find EMR attenuation map",
          "MapCppEMRMCDigitizer::birth"));
}

void MapCppEMRMCDigitization::_death() {
}

void MapCppEMRMCDigitization::_process(Data *data) const {

  // Routine data checks before processing it
  if ( !data )
      throw Exception(Exception::recoverable, "Data was NULL",
                      "MapCppEMRMCDigitization::_process");

  Spill* spill = data->GetSpill();
  if ( !spill )
      throw Exception(Exception::recoverable, "Spill was NULL",
                      "MapCppEMRMCDigitization::_process");

  if ( spill->GetDaqEventType() != "physics_event" )
      return;

  MCEventPArray* mcEvts = spill->GetMCEvents();
  if ( !mcEvts )
      throw Exception(Exception::recoverable, "MCEventPArray was NULL",
                      "MapCppEMRMCDigitization::_process");

  int nPartEvents = spill->GetMCEventSize();
  if ( !nPartEvents )
      return;

  // Check the Recon event array and EMR Spill Data are initialised. If not make it so
  if ( !spill->GetReconEvents() )
      spill->SetReconEvents(new ReconEventPArray());

  if ( !spill->GetEMRSpillData() )
      spill->SetEMRSpillData(new EMRSpillData());

  // Set primary time window
  std::vector<double> delta_t_array;
  delta_t_array.resize(0);

  for (int iPe = 0; iPe < nPartEvents; iPe++) {

    EMRHitArray *hits = spill->GetAnMCEvent(iPe).GetEMRHits();
    if ( hits ) {

      Primary *primary = spill->GetAnMCEvent(iPe).GetPrimary();
      double pTime = primary->GetTime(); // ns

      for (size_t iHit = 0; iHit < hits->size(); iHit++) {

	EMRHit hit = hits->at(iHit);
	double delta_t = hit.GetTime() - pTime;

        if (delta_t > 0)
	    delta_t_array.push_back(delta_t);
      }
    }
  }

  // Procede if there's no EMR data
  if ( !delta_t_array.size() )
      return;

  // Set the window from the leading time to 20 ADC counts later
  int lt = static_cast<int>
	     (*std::min_element(delta_t_array.begin(), delta_t_array.end())/_dbb_count);
  int deltat_min = lt; // ns
  int deltat_max = lt + 50; // ns, +50 ns

  // Create a temporary array of n+1 BarHitTmp arrays (1 per trigger + decays)
  EMREventPlaneHitVector mc_events_tmp = get_emr_events_tmp(nPartEvents+1);	// EMREvents

  // Fill the temporary array with G4 spill information
  processMC(mcEvts, mc_events_tmp, deltat_min, deltat_max);

  // Create a temporary array of n plane hit arrays (1 per trigger) and a temporary array of bars
  EMREventPlaneHitVector emr_events_tmp = get_emr_events_tmp(nPartEvents);	// EMREvents
  EMRBarHitArray emr_bar_hits_tmp;						// EMRSpillData

  // Digitize the G4 spill digits
  digitize(mc_events_tmp, emr_events_tmp, emr_bar_hits_tmp, nPartEvents);

  // Create a temporary array to harbour n' plane hit arrays (1 per daughter candiate)
  EMREventPlaneHitVector emr_candidates_tmp;

  // Process the secondary hits and merge them timewise into daughter candidates
  process_candidates(emr_bar_hits_tmp, emr_candidates_tmp);
  emr_events_tmp.insert(emr_events_tmp.end(),
			emr_candidates_tmp.begin(), emr_candidates_tmp.end());

  // Fill the Recon event array with spill information (1 per trigger + daughter candidates)
  fill(spill, emr_events_tmp, nPartEvents);
}

void MapCppEMRMCDigitization::processMC(MAUS::MCEventPArray *mcEvts,
					EMREventPlaneHitVector& mc_events_tmp,
		 			int deltat_min,
		 			int deltat_max) const {

  size_t nPartEvents = mcEvts->size();
  for (size_t iPe = 0; iPe < nPartEvents; iPe++) {

    Primary *primary = mcEvts->at(iPe)->GetPrimary();
    double pTime = primary->GetTime(); // ns

    EMRHitArray *EMRhits = mcEvts->at(iPe)->GetEMRHits();

    for (size_t iHit = 0; iHit < EMRhits->size(); iHit++) {

      EMRHit hit = EMRhits->at(iHit);

      // Channel ID
      EMRChannelId *xCh = hit.GetChannelId();
      int g4barid = xCh->GetBar();
      int xPlane = g4barid / 59;
      int xBar = g4barid % 59 + 1;

      // Position
      ThreeVector xPos = hit.GetPosition(); 	// (mm, mm, mm)
      double xPathLength = hit.GetPathLength();	// mm

      // Time
      double time = hit.GetTime(); 		// ns
      int xTime  = static_cast<int>(time); 	// ns, leading time
      int xDeltaT = xTime - pTime; 		// ns

      // Energy deposition
      double edep = hit.GetEnergyDeposited(); 	// MeV
      int xTot = static_cast<int>(edep*pow(10, 6)); // eV, non digitized time-over-threshold

      // Fill the bar hit data (from the EMR sensitive detector)
      EMRBarHitTmp bHitTmp;
      EMRBarHit bHit;
      bHit.SetChannel(xPlane*_number_of_bars+xBar);
      bHit.SetTot(xTot);
      bHit.SetTime(xTime);
      bHit.SetDeltaT(xDeltaT);
      bHitTmp._barhit = bHit;
      bHitTmp._pos = xPos;
      bHitTmp._path_length = xPathLength;

      if ( xDeltaT >= deltat_min && xDeltaT < deltat_max ) {
        mc_events_tmp[iPe][xPlane]._barhits.push_back(bHitTmp);
        mc_events_tmp[iPe][xPlane]._orientation = xPlane % 2;
      } else {
	mc_events_tmp[nPartEvents][xPlane]._barhits.push_back(bHitTmp);
        mc_events_tmp[nPartEvents][xPlane]._orientation = xPlane % 2;
      }
    }
  }
}

void MapCppEMRMCDigitization::digitize(EMREventPlaneHitVector mc_events_tmp,
				       EMREventPlaneHitVector& emr_events_tmp,
				       EMRBarHitArray& emr_bar_hits_tmp,
				       size_t nPartEvents) const {

  for (size_t iPe = 0; iPe < mc_events_tmp.size(); iPe++) {
    for (size_t iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      if ( !mc_events_tmp[iPe][iPlane]._barhits.size() )
	  continue;

      // Find PMT gain correction coefficient
      EMRChannelKey xAverageKey(iPlane, iPlane%2, -1, "emr");
      double epsilon_MA = _calibMap.Eps(xAverageKey, "MA");
      double epsilon_SA = _calibMap.Eps(xAverageKey, "SA");

      int naph_SAPMT(0); // Number of attenuated photons reaching the SAPMT
      for (size_t iHit = 0; iHit < mc_events_tmp[iPe][iPlane]._barhits.size(); iHit++) {

	EMRBarHitTmp bHitTmp = mc_events_tmp[iPe][iPlane]._barhits[iHit];
	EMRBarHit bHit = bHitTmp._barhit;
        int xBar = bHit.GetChannel()%_number_of_bars;
	EMRChannelKey xKey(iPlane, iPlane%2, xBar, "emr");

	int xTot  = bHit.GetTot(); // eV
	int xTime = bHit.GetTime(); // ns
	int xDeltaT = bHit.GetDeltaT(); // ns
        double x  = bHitTmp._pos.x(); // mm
	double y  = bHitTmp._pos.y();  // mm
        double xPathLength = bHitTmp._path_length; // mm

	//--------- MAPMT signal simulation -------//
	// Simulate electronic noise and MAPMT dark current
	// !!! TODO !!!

	// Geant4 information: edep, time and path length
	double energy = static_cast<double>(xTot)/pow(10, 6); // MeV
	int time = xDeltaT; // ns
	if ( energy < _signal_energy_threshold )
	    continue;

	// Convert energy to the number of scintillation photons (nsph) according to Birks's law
	double nsph = (_nph_per_MeV*energy)/(1.0+_birks_constant*energy/xPathLength);
	if ( _do_sampling )
	    nsph = _rand->Poisson(nsph);

	// Convert nsph to the number of trapped photons (ntph)
	double ntph = nsph*_trap_eff;
	if ( _do_sampling )
	    ntph = _rand->Poisson(ntph);

	// Attenuate ntph according to fibre length (naph) and loss in the connectors
	// MAPMT (half the PEs)
	double naph_MAPMT = ntph/2 *_attenMap.fibreAtten(xKey, x, y, "MA")
			      	   *_attenMap.connectorAtten(xKey, "MA");
	if ( _do_sampling )
  	    naph_MAPMT = _rand->Poisson(naph_MAPMT);

	// SAPMT (other half of the PEs)
	double naph_SAPMT_hit = ntph/2 *_attenMap.fibreAtten(xKey, x, y, "SA")
			               *_attenMap.connectorAtten(xKey, "SA");
	naph_SAPMT += naph_SAPMT_hit;

	// Simulate crosstalk and misalignment
	// !!! TODO !!!

	// Convert naph to the number of photoelectrons (npe)
	double npe = naph_MAPMT*_QE_MAPMT;
	if ( _do_sampling )
	    npe = _rand->Poisson(npe);

	// Correct npe for the photocathode non-uniformity
	// !!! TODO !!!

	// Correct npe for the gain difference between MAPMTs
	npe = npe*epsilon_MA;
	if ( !npe )
	    continue;

	// Convert npe to the number of ADC counts
	double nADC = npe*_nADC_per_pe_MAPMT;
	if ( _do_sampling )
	    nADC = _rand->Gaus(nADC, _electronics_response_spread_MAPMT);

	// Convert nADC to a time-over-threshold
	int xTotDigi = static_cast<int>
			   (_tot_func_p1*log(_tot_func_p2*nADC+_tot_func_p3));
	if (xTotDigi <= 0)
	    continue;

	// Convert Geant4 hit time to deltaT in ADC counts
	int xDeltaTDigi = static_cast<int>(static_cast<double>(time)/_dbb_count);
	if ( _do_sampling )
	    xDeltaTDigi = static_cast<int>(_rand->Gaus(xDeltaTDigi, _time_response_spread));
        if (xDeltaTDigi < 0) xDeltaTDigi = 0;

	// Correct deltaT with fibre length
	xDeltaTDigi += static_cast<int>
		           (_attenMap.fibreDelay(xKey, x, y, "MA")/_dbb_count);

	// Set hit time
	int xTimeDigi = static_cast<int>(static_cast<double>(xTime)/_dbb_count);

	// Set bar hit
        EMRBarHitTmp barHitTmp;
        EMRBarHit barHit;
        barHit.SetChannel(bHit.GetChannel());
	barHit.SetTot(xTotDigi);
	barHit.SetTime(xTimeDigi);

	// Discriminate primary hits (close to the trigger) from the rest
	if ( iPe < nPartEvents ) {
	  barHit.SetDeltaT(xDeltaTDigi);
          barHitTmp._barhit = barHit;
	  emr_events_tmp[iPe][iPlane]._barhits.push_back(barHitTmp);
	} else {
          barHit.SetDeltaT(0);
	  emr_bar_hits_tmp.push_back(barHit);
	}
      }

      //------------ SAPMT signal simulation -------//
      if ( iPe == nPartEvents )
	  continue;
      int xOri = iPlane % 2;

      // Simulate SAPMT dark current
      // !!! TODO !!!

      // Sample naph with Poisson
      if ( _do_sampling )
	  naph_SAPMT = _rand->Poisson(naph_SAPMT);

      // Convert naph to the number of photoelectrons (npe)
      int npe = static_cast<int>(naph_SAPMT*_QE_SAPMT);
      if ( _do_sampling )
	  npe = _rand->Poisson(npe);

      // Correct npe for the gain difference between SAPMTs
      npe = static_cast<int>(npe*epsilon_SA);

      // Convert npe to the number of ADC counts
      int nADC = static_cast<int>(npe*_nADC_per_pe_SAPMT);
      if ( _do_sampling )
	  nADC = static_cast<int>(_rand->Gaus(nADC, _electronics_response_spread_SAPMT));
      if (nADC < 0) nADC = 0;

      // Set signal baseline (8bit ADC)
      int baseline = _baseline[iPlane];

      // Set electronics noise level - number of fluctuations within acquisition window
      int noise_level = _noise_level[iPlane];

      // Set electronics noise position (upwards/downwards fluctuations)
      int noise_position = _noise_position[iPlane];

      // Generate negative voltage pulse with random electronics noise
      TH1F *pulse_shape = new TH1F("pulse_shape", "pulse_shape",
				   _acquisition_window, 0, _acquisition_window);

      double arrival_time = _signal_integration_window*2 + _arrival_time_shift;
      if ( static_cast<int>(_rand->Uniform(0, 1.9999999)) )
	  arrival_time += _arrival_time_spread;

      arrival_time += _rand->Gaus(0, _arrival_time_gaus_width)
		      +_rand->Uniform(-_arrival_time_uniform_width, _arrival_time_uniform_width);

      for (int i = 0; i < noise_level; i++) {
        double noise = _rand->Uniform(0, _acquisition_window);
        int bin = static_cast<Int_t>(noise+1);
        if ( pulse_shape->GetBinContent(bin) != 1 ) pulse_shape->Fill(noise);
      }

      if ( noise_position )
	for (Int_t i = 1; i <= _acquisition_window; i++)
          pulse_shape->SetBinContent(i, -pulse_shape->GetBinContent(i));

      for (int i = 0; i < nADC; i++)
        pulse_shape->Fill(_rand->Landau(arrival_time, _pulse_shape_landau_width));

      for (int i = 1; i <= _acquisition_window; i++)
        pulse_shape->SetBinContent(i, baseline-pulse_shape->GetBinContent(i));

      double pedestal_baseline = pulse_shape->Integral(1, _signal_integration_window)
			         /_signal_integration_window;

      // Set pedestal area
      double xPedestalAreaDigi = pedestal_baseline*_signal_integration_window
			          - pulse_shape->Integral(_signal_integration_window+1,
			       			          _signal_integration_window*2);

      // Set pulse area
      double xAreaDigi = pedestal_baseline*_signal_integration_window
		         - pulse_shape->Integral(arrival_time-10+1,
			     			 arrival_time+_signal_integration_window-10);
      if (nADC == 0) xAreaDigi = 0;
      if (xAreaDigi < 0) xAreaDigi = 0;

      // Set hit arrival time
      int xArrivalTimeDigi = arrival_time;

      // Simulate time delay in cables
      // !!! TODO !!!

      // Sample pulse shape every 2ns (500 MHz clock)
      std::vector<int> xSamplesDigi;
      for (int iBin = 0; iBin < _acquisition_window; iBin++)
        xSamplesDigi.push_back(pulse_shape->GetBinContent(iBin+1));

      // Fill the fADC information
      emr_events_tmp[iPe][iPlane]._orientation = xOri;
      emr_events_tmp[iPe][iPlane]._charge = xAreaDigi;			// ADC
      emr_events_tmp[iPe][iPlane]._pedestal_area = xPedestalAreaDigi; 	// ADC
      emr_events_tmp[iPe][iPlane]._time = xArrivalTimeDigi;		// ADC
      emr_events_tmp[iPe][iPlane]._samples = xSamplesDigi; 		// ADC

      delete pulse_shape;
    }
  }
}

void MapCppEMRMCDigitization::process_candidates(EMRBarHitArray emr_bar_hits_tmp,
						 EMREventPlaneHitVector& emr_candidates_tmp) const {

  // Sort the vector in ascending order of hit time (lambda sorting function)
  sort(emr_bar_hits_tmp.begin(), emr_bar_hits_tmp.end(),
       [] (const EMRBarHit& a, const EMRBarHit& b) {
	   return a.GetTime() < b.GetTime();
       });

  // Find groups of hits defined by mat
  std::vector<EMRBarHitArray> barHitGroupVector;
  EMRBarHitArray barHitGroup;
  int xHitTime = 0;

  for (size_t iHit = 1; iHit < emr_bar_hits_tmp.size()+1; iHit++) {

    int xDeltaT = _secondary_bunching_width + 1;
    if ( !xHitTime ) xHitTime = emr_bar_hits_tmp[iHit-1].GetTime();
    if (iHit < emr_bar_hits_tmp.size()) xDeltaT = emr_bar_hits_tmp[iHit].GetTime() - xHitTime;

    if (xDeltaT > _secondary_bunching_width) {
      barHitGroup.push_back(emr_bar_hits_tmp[iHit-1]);
      if (barHitGroup.size() >= _secondary_n_low) {
        barHitGroupVector.push_back(barHitGroup);
      }
      barHitGroup.resize(0);
      xHitTime = 0;
    } else {
      barHitGroup.push_back(emr_bar_hits_tmp[iHit-1]);
    }
  }
  size_t nSeconPartEvents = barHitGroupVector.size();

  // Resize the event array to accomodate one extra event per secondary track (n')
  emr_candidates_tmp = get_emr_events_tmp(nSeconPartEvents);

  // Associate each barHitGroup to a secondary trigger (daughter candidate)
  for (size_t iGroup = 0; iGroup < nSeconPartEvents; iGroup++) {
    EMRBarHitArray barHitArray = barHitGroupVector[iGroup];
    for (size_t iHit = 0; iHit < barHitArray.size(); iHit++) {
      int xPlane = barHitArray[iHit].GetChannel()/_number_of_bars;
      EMRBarHitTmp barHitTmp;
      barHitTmp._barhit = barHitArray[iHit];
      emr_candidates_tmp[iGroup][xPlane]._barhits.push_back(barHitTmp);
    }
  }
}

void MapCppEMRMCDigitization::fill(MAUS::Spill *spill,
				   EMREventPlaneHitVector& emr_events_tmp,
				   size_t nPartEvents) const {

  // Get the EMR recon events and the spill data
  ReconEventPArray *recEvts =  spill->GetReconEvents();
  EMRSpillData* emrSpill = spill->GetEMRSpillData();

  for (size_t iPe = 0; iPe < emr_events_tmp.size(); iPe++) {

    EMREventTrack* evtTrack = new EMREventTrack;

    EMRPlaneHitArray plArray;
    for (size_t iPlane = 0; iPlane < _number_of_planes; iPlane++) {

      EMRPlaneHit *plHit = new EMRPlaneHit;
      EMRBarHitVector barHitVector = emr_events_tmp[iPe][iPlane]._barhits;

      plHit->SetPlane(iPlane);
      for (size_t iHit = 0; iHit < barHitVector.size(); iHit++)
        plHit->AddEMRBarHit(barHitVector[iHit]._barhit);
      plHit->SetOrientation(emr_events_tmp[iPe][iPlane]._orientation);
      plHit->SetDeltaT(emr_events_tmp[iPe][iPlane]._time);
      plHit->SetCharge(emr_events_tmp[iPe][iPlane]._charge);
      plHit->SetPedestalArea(emr_events_tmp[iPe][iPlane]._pedestal_area);
      plHit->SetSampleArray(emr_events_tmp[iPe][iPlane]._samples);

      if ( barHitVector.size() || plHit->GetCharge() > 0 ) {
        plArray.push_back(plHit);
	for ( size_t iHit = 0; iHit < barHitVector.size(); iHit++ )
	  emrSpill->AddEMRBarHit(barHitVector[iHit]._barhit);
      } else {
        delete plHit;
      }
    }
    evtTrack->SetEMRPlaneHitArray(plArray);

    if ( iPe < nPartEvents ) {
      evtTrack->SetType("mother");
      evtTrack->SetTrackId(0);
      EMREvent* evt = new EMREvent;
      if ( plArray.size() ) {
        evt->AddEMREventTrack(evtTrack);
      } else {
	delete evtTrack;
      }

      size_t nRecEvents = spill->GetReconEventSize();
      if ( nRecEvents > iPe ) {
        recEvts->at(iPe)->SetEMREvent(evt);
      } else {
        ReconEvent *recEvt = new ReconEvent;
        recEvt->SetPartEventNumber(iPe);
        recEvt->SetEMREvent(evt);
        recEvts->push_back(recEvt);
      }
    } else {
      evtTrack->SetType("candidate");
      evtTrack->SetTrackId(iPe-nPartEvents);
      emrSpill->AddEMREventTrack(evtTrack);
    }
  }

  spill->SetReconEvents(recEvts);
  spill->SetEMRSpillData(emrSpill);
}

EMREventPlaneHitVector MapCppEMRMCDigitization::get_emr_events_tmp(size_t nPartEvts) const {
  EMREventPlaneHitVector emr_events_tmp;
  emr_events_tmp.resize(nPartEvts);
  for (size_t iPe = 0; iPe < nPartEvts; iPe++) {
    emr_events_tmp[iPe].resize(_number_of_planes);  // number of planes
    for (size_t iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      EMRPlaneHitTmp data;
      EMRBarHitVector barhits;
      std::vector<int> xSamples;
      data._barhits = barhits;
      data._orientation = iPlane%2;
      data._time = -1;
      data._deltat = -1;
      data._charge = -1;
      data._pedestal_area = -1;
      data._samples = xSamples;
      emr_events_tmp[iPe][iPlane] = data;
    }
  }
  return emr_events_tmp;
}
} // namespace MAUS
