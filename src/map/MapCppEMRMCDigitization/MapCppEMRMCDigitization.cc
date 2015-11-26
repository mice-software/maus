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

#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Interface/dataCards.hh"
#include "DataStructure/MCEvent.hh"
#include "DataStructure/Spill.hh"
#include "API/PyWrapMapBase.hh"
#include "Converter/DataConverters/CppJsonSpillConverter.hh"
#include "Converter/DataConverters/JsonCppSpillConverter.hh"

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

  _tot_noise_up = configJSON["EMRtotNoiseUp"].asInt();
  _tot_noise_low = configJSON["EMRtotNoiseLow"].asInt();

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

  for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
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

  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();
  MCEventPArray *mcEvts = spill->GetMCEvents();
  if ( mcEvts == NULL )
      return;
  int nPartEvents = spill->GetMCEventSize();
  if ( !nPartEvents )
      return;

  // Check the Recon event array and EMR Spill Data are initialised, and if not make it so
  if ( !spill->GetReconEvents() ) {
    ReconEventPArray* recEvts = new ReconEventPArray();
    spill->SetReconEvents(recEvts);
  }
  if ( !spill->GetEMRSpillData() ) {
    EMRSpillData* emrData = new EMRSpillData();
    spill->SetEMRSpillData(emrData);
  }

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
  int deltat_min = lt; // ADC counts
  int deltat_max = lt + 20; // ADC counts, +50 ns

  // Reset DBB and fADC arrays with n events (1 per trigger)
  EMRDBBEventVector emr_dbb_events_tmp = get_dbb_data_tmp(nPartEvents);
  EMRfADCEventVector emr_fadc_events_tmp = get_fadc_data_tmp(nPartEvents);

  // Fill the fADC and DBB event arrays with G4 Spill information
  processMC(mcEvts, emr_dbb_events_tmp, emr_fadc_events_tmp, deltat_min, deltat_max);

  // Reset DBB and fADC arrays with n+1 events (1 per trigger + spill data)
  EMRDBBEventVector emr_dbb_events = get_dbb_data_tmp(nPartEvents + 1);
  EMRfADCEventVector emr_fadc_events = get_fadc_data_tmp(nPartEvents + 1);

  // Digitize the G4 spill information
  digitize(emr_dbb_events_tmp, emr_fadc_events_tmp,
	   emr_dbb_events, emr_fadc_events,
	   deltat_min, deltat_max);

  // Fill the Recon event array with Spill information (1 per trigger + decays)
  fill(spill, emr_dbb_events, emr_fadc_events);
}

void MapCppEMRMCDigitization::processMC(MAUS::MCEventPArray *mcEvts,
					EMRDBBEventVector& emr_dbb_events_tmp,
		 			EMRfADCEventVector& emr_fadc_events_tmp,
		 			int deltat_min,
		 			int deltat_max) const {

  for (size_t iPe = 0; iPe < mcEvts->size(); iPe++) {

    Primary *primary = mcEvts->at(iPe)->GetPrimary();
    double pTime = primary->GetTime(); // ns

    EMRHitArray *EMRhits = mcEvts->at(iPe)->GetEMRHits();
    for (size_t iHit = 0; iHit < EMRhits->size(); iHit++) {

      EMRHit hit = EMRhits->at(iHit);

      // Channel ID
      EMRChannelId *id = hit.GetChannelId();
      int g4barid = id->GetBar();
      int xPlane = g4barid / 59;
      int xBar = g4barid % 59 + 1;

      // Position
      ThreeVector xPos = hit.GetPosition(); // (mm, mm, mm)
      double xPathLength = hit.GetPathLength(); // mm

      // Time
      double time = hit.GetTime(); // ns
      int xHitTime  = static_cast<int>(time); // ns, leading time
      int xDeltaT = xHitTime - pTime; // ns

      // Energy deposition
      double edep = hit.GetEnergyDeposited(); // MeV
      int xTot = static_cast<int>(edep*pow(10, 6)); // eV, non digitized time-over-threshold
      double xCharge = emr_fadc_events_tmp[iPe][xPlane]._charge;
      if ( xDeltaT >= deltat_min && xDeltaT < deltat_max )
          xCharge += edep*pow(10, 6); // eV, non digitized fADC charge

      // Fill DBB data
      EMRBarHit bHit;
      bHit.SetX(xPos.x());
      bHit.SetY(xPos.y());
      bHit.SetZ(xPos.z());
      bHit.SetPathLength(xPathLength);
      bHit.SetTot(xTot);
      bHit.SetHitTime(xHitTime);
      bHit.SetDeltaT(xDeltaT);
      emr_dbb_events_tmp[iPe][xPlane][xBar].push_back(bHit);

      // Fill fADC data
      fADCdata data;
      data._orientation = xPlane % 2;
      data._charge = xCharge;
      data._time = 0;
      emr_fadc_events_tmp[iPe][xPlane] = data;
    }
  }
}

void MapCppEMRMCDigitization::digitize(EMRDBBEventVector emr_dbb_events_tmp,
				       EMRfADCEventVector emr_fadc_events_tmp,
				       EMRDBBEventVector& emr_dbb_events,
				       EMRfADCEventVector& emr_fadc_events,
				       int deltat_min,
				       int deltat_max) const {

  unsigned int nPartEvents = emr_fadc_events_tmp.size();
  for (size_t iPe = 0; iPe < nPartEvents; iPe++) {
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      // Skip empty planes
      int nBars(0);
      for (int iBar = 0; iBar < _number_of_bars; iBar++)
	if ( emr_dbb_events_tmp[iPe][iPlane][iBar].size() )
	  nBars++;
       if ( !nBars )
	  continue;

      // Find PMT gain correction coefficient
      EMRChannelKey xAverageKey(iPlane, iPlane%2, -1, "emr");
      double epsilon_MA = _calibMap.Eps(xAverageKey, "MA");
      double epsilon_SA = _calibMap.Eps(xAverageKey, "SA");

      int naph_SAPMT(0); // Number of attenuated PE reaching the SAPMT
      for (int iBar = 0; iBar < _number_of_bars; iBar++) {

        size_t nBarHits = emr_dbb_events_tmp[iPe][iPlane][iBar].size();
        for (size_t iBarHit = 0; iBarHit < nBarHits; iBarHit++) {

	  EMRBarHit barHit = emr_dbb_events_tmp[iPe][iPlane][iBar].at(iBarHit);
	  EMRChannelKey xKey(iPlane, iPlane%2, iBar, "emr");
          double x  = barHit.GetX(); // mm
	  double y  = barHit.GetY();  // mm
	  int xTot  = barHit.GetTot(); // eV
	  int xDeltaT = barHit.GetDeltaT(); // ns
	  int xHitTime = barHit.GetHitTime(); // ns
          double xPathLength = barHit.GetPathLength(); // mm

	  //--------- MAPMT signal simulation -------//
	  // Simulate electronic noise and MAPMT dark current
	  // !!! TODO !!!

	  // Geant4 information: edep, time and path length
	  double energy = static_cast<double>(xTot)/pow(10, 6); // MeV
	  int time = xDeltaT; // ns
	  if ( energy < _signal_energy_threshold )
	      continue;

	  // Convert energy to the number of scintillation photons (nsph) according to Birks's law
	  int nsph = static_cast<int>
		       ((_nph_per_MeV*energy)/(1.0+_birks_constant*energy/xPathLength));
	  if ( _do_sampling )
	      nsph = _rand->Poisson(nsph);

	  // Convert nsph to the number of trapped photons (ntph)
	  int ntph = static_cast<int>(nsph*_trap_eff);
	  if ( _do_sampling )
	      ntph = _rand->Poisson(ntph);

	  // Attenuate ntph according to fibre length (naph) and loss in the connectors
	  // MAPMT (half the PEs)
	  int naph_MAPMT = static_cast<int>
			     (static_cast<double>(ntph)/2
			      *_attenMap.fibreAtten(xKey, x, y, "MA")
			      *_attenMap.connectorAtten(xKey, "MA"));
	  if ( _do_sampling )
	      naph_MAPMT = _rand->Poisson(naph_MAPMT);

	  // SAPMT (other half of the PEs)
	  int naph_SAPMT_hit = static_cast<int>
			     (static_cast<double>(ntph)/2
			      *_attenMap.fibreAtten(xKey, x, y, "SA")
			      *_attenMap.connectorAtten(xKey, "SA"));
	  if ( time >= deltat_min && time < deltat_max )
	    naph_SAPMT += naph_SAPMT_hit;

	  // Simulate crosstalk and misalignment
	  // !!! TODO !!!

	  // Convert naph to the number of photoelectrons (npe)
	  int npe = static_cast<int>(naph_MAPMT*_QE_MAPMT);
	  if ( _do_sampling )
	      npe = _rand->Poisson(npe);

	  // Correct npe for the photocathode non-uniformity
	  // !!! TODO !!!

	  // Correct npe for the gain difference between MAPMTs
	  npe = static_cast<int>(npe*epsilon_MA);
	  if ( !npe )
	      continue;

	  // Convert npe to the number of ADC counts
	  int nADC = static_cast<int>(npe*_nADC_per_pe_MAPMT);
	  if ( _do_sampling )
	      nADC = static_cast<int>(_rand->Gaus(nADC, _electronics_response_spread_MAPMT));

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
	  int xHitTimeDigi = static_cast<int>(static_cast<double>(xHitTime)/_dbb_count);

	  // Set bar hit
	  EMRBarHit bHit;
	  bHit.SetTot(xTotDigi);
	  bHit.SetDeltaT(xDeltaTDigi);
	  bHit.SetHitTime(xHitTimeDigi);

	  // Discriminate primary hits (close to the trigger) from the rest
	  if (xDeltaTDigi >= deltat_min && xDeltaTDigi < deltat_max) {
	    emr_dbb_events[iPe][iPlane][iBar].push_back(bHit);
	  } else {
            bHit.SetDeltaT(0);
	    emr_dbb_events[nPartEvents][iPlane][iBar].push_back(bHit);
	  }
        }
      }

      //------------ SAPMT signal simulation -------//
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
      fADCdata data;
      data._orientation = xOri;
      data._charge = xAreaDigi; // ADC
      data._pedestal_area = xPedestalAreaDigi; // ADC
      data._time = xArrivalTimeDigi; // ADC
      data._samples = xSamplesDigi;
      emr_fadc_events[iPe][iPlane] = data;

      delete pulse_shape;
    }
  }
}

void MapCppEMRMCDigitization::fill(MAUS::Spill *spill,
				   EMRDBBEventVector emr_dbb_events,
				   EMRfADCEventVector emr_fadc_events) const {

  // Set the EMR recon events and the spill data
  ReconEventPArray *recEvts =  spill->GetReconEvents();
  EMRSpillData *emrData = spill->GetEMRSpillData();

  unsigned int nPartEvents = emr_fadc_events.size()-1;
  for (size_t iPe = 0; iPe < nPartEvents + 1; iPe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      int xOri  = emr_fadc_events[iPe][iPlane]._orientation;
      double xCharge = emr_fadc_events[iPe][iPlane]._charge;
      int xPos = emr_fadc_events[iPe][iPlane]._time;
      double xPedestalArea = emr_fadc_events[iPe][iPlane]._pedestal_area;
      std::vector<int> xSamples = emr_fadc_events[iPe][iPlane]._samples;

      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(iPlane);
      plHit->SetTrigger(iPe);
      plHit->SetOrientation(xOri);
      plHit->SetCharge(xCharge);
      plHit->SetDeltaT(xPos);
      plHit->SetPedestalArea(xPedestalArea);
      plHit->SetSamples(xSamples);

      EMRBarArray barArray;
      for (int iBar = 0; iBar < _number_of_bars; iBar++) {
        int nHits = emr_dbb_events[iPe][iPlane][iBar].size();
        if (nHits) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(iBar);
          bar->SetEMRBarHitArray(emr_dbb_events[iPe][iPlane][iBar]);
          barArray.push_back(bar);
        }
      }

      plHit->SetEMRBarArray(barArray);
      if (barArray.size() || xCharge) {
        plArray.push_back(plHit);
      } else {
        delete plHit;
      }
    }

    if (iPe < nPartEvents) {
      evt->SetEMRPlaneHitArray(plArray);

      unsigned int nRecEvents = spill->GetReconEventSize();
      if (nRecEvents > iPe) {
        recEvts->at(iPe)->SetEMREvent(evt);
      } else {
        ReconEvent *recEvt = new ReconEvent;
        recEvt->SetPartEventNumber(iPe);
        recEvt->SetEMREvent(evt);
        recEvts->push_back(recEvt);
      }
    } else {
      emrData->SetEMRPlaneHitArray(plArray);
    }
  }

  spill->SetReconEvents(recEvts);
  spill->SetEMRSpillData(emrData);
}

EMRDBBEventVector MapCppEMRMCDigitization::get_dbb_data_tmp(int nPartEvts) const {
  EMRDBBEventVector emr_dbb_events_tmp;
  emr_dbb_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts; iPe++) {
    emr_dbb_events_tmp[iPe].resize(_number_of_planes);  // number of planes
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      emr_dbb_events_tmp[iPe][iPlane].resize(_number_of_bars); // number of bars in a plane
    }
  }
  return emr_dbb_events_tmp;
}

EMRfADCEventVector MapCppEMRMCDigitization::get_fadc_data_tmp(int nPartEvts) const {
  EMRfADCEventVector emr_fadc_events_tmp;
  emr_fadc_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts ;iPe++) {
    emr_fadc_events_tmp[iPe].resize(_number_of_planes);
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      fADCdata data;
      data._orientation = iPlane%2;
      data._charge = 0.0;
      data._pedestal_area = 0.0;
      data._time = 0;
      std::vector<int> xSamples;
      data._samples = xSamples;
      emr_fadc_events_tmp[iPe][iPlane] = data;
    }
  }
  return emr_fadc_events_tmp;
}
} // Namespace MAUS
