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
  _tot_func_p4 = configJSON["EMRtotFuncP4"].asDouble();
  _acquisition_window = configJSON["EMRacquisitionWindow"].asInt();
  _signal_integration_window = configJSON["EMRsignalIntegrationWindow"].asInt();
  _arrival_time_shift = configJSON["EMRarrivalTimeShift"].asInt();
  _arrival_time_gaus_width = configJSON["EMRarrivalTimeGausWidth"].asDouble();
  _arrival_time_uniform_width = configJSON["EMRarrivalTimeUniformWidth"].asDouble();
  _pulse_shape_landau_width = configJSON["EMRpulseShapeLandauWidth"].asDouble();
  _fom = configJSON["EMRfom"].asString();
  _birks_constant = configJSON["EMRbirksConstant"].asDouble();
  _average_path_length = configJSON["EMRaveragePathLength"].asDouble();
  _signal_energy_threshold = configJSON["EMRsignalEnergyThreshold"].asDouble();
  _baseline_spread = configJSON["EMRbaselineSpread"].asInt();
  _maximum_noise_level = configJSON["EMRmaximumNoiseLevel"].asInt();
  _deltat_shift = configJSON["EMRdeltatShift"].asDouble();
  _baseline_position = configJSON["EMRbaselinePosition"].asDouble();
  _arrival_time_spread = configJSON["EMRarrivalTimeSpread"].asDouble();

  // Generate random noise in each SAPMT
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

  if (spill->GetMCEvents() == NULL)
      return;

  int nPartEvents = spill->GetMCEventSize();

  // Check the Recon event array is initialised, and if not make it so
  if (!spill->GetReconEvents()) {
    ReconEventPArray* revts = new ReconEventPArray();
    spill->SetReconEvents(revts);
  }

  // Set primary hits/noise time windows
  std::vector<double> delta_t_array;
  delta_t_array.resize(0);

  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    EMRHitArray *hits = spill->GetAnMCEvent(iPe).GetEMRHits();

    if (hits) {
      int nHits = hits->size();

      Primary *primary = spill->GetAnMCEvent(iPe).GetPrimary();
      double pTime = primary->GetTime(); // ns

      for (int iHit = 0; iHit < nHits; iHit++) {
	EMRHit hit = hits->at(iHit);

	double time = hit.GetTime(); // ns
	double delta_t = time - pTime;

        if (delta_t > 0) delta_t_array.push_back(delta_t);
      }
    }
  }

  // Break if there's no EMR data
  if (!delta_t_array.size())
      return;

  int lt = static_cast<int>(*std::min_element(delta_t_array.begin(), delta_t_array.end())
			    /_dbb_count);
  int deltat_limits[4] = {lt, lt+20, lt+20, lt+40};

  // Create the DBB and fADC arrays with n events (1 per trigger)
  EMRDBBEventVector emr_dbb_events_tmp = get_dbb_data_tmp(nPartEvents);
  EMRfADCEventVector emr_fadc_events_tmp = get_fadc_data_tmp(nPartEvents);

  // Fill the Recon event array with G4 Spill information
  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    EMRHitArray *EMRhits = spill->GetAnMCEvent(iPe).GetEMRHits();

    if (EMRhits) {
      Primary *primary = spill->GetAnMCEvent(iPe).GetPrimary();
      double pTime = primary->GetTime(); // ns

      processDBB(EMRhits, iPe, pTime, emr_dbb_events_tmp, emr_fadc_events_tmp);
      processFADC(EMRhits, iPe, emr_fadc_events_tmp);
    }
  }

  fill(spill, nPartEvents, emr_dbb_events_tmp, emr_fadc_events_tmp);

  // Reset/Resize DBB and fADC arrays with n+2 events (1 per trigger + noise + decays)
  emr_dbb_events_tmp = get_dbb_data_tmp(nPartEvents+2);
  emr_fadc_events_tmp = get_fadc_data_tmp(nPartEvents+2);

  // Digitize the Recon event array
  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    EMREvent *EMRevt = spill->GetReconEvents()->at(iPe)->GetEMREvent();

    if (EMRevt) {
      digitize(EMRevt, iPe, nPartEvents, deltat_limits, emr_dbb_events_tmp, emr_fadc_events_tmp);
    }
  }

  fill(spill, nPartEvents+2, emr_dbb_events_tmp, emr_fadc_events_tmp);
}

void MapCppEMRMCDigitization::processDBB(MAUS::EMRHitArray *EMRhits,
					 int xPe,
					 double pTime,
					 EMRDBBEventVector& emr_dbb_events_tmp,
					 EMRfADCEventVector& emr_fadc_events_tmp) const {

  int nHits = EMRhits->size();

  for (int iHit = 0; iHit < nHits; iHit++) {

    EMRHit hit = EMRhits->at(iHit);

    // Channel ID
    EMRChannelId *id = hit.GetChannelId();

    int g4barid = id->GetBar();
    int planeid = g4barid / 59;
    int barid = g4barid % 59 + 1;

    // Energy Deposition
    double edep = hit.GetEnergyDeposited(); /*MeV*/

    // Position
    ThreeVector pos = hit.GetPosition();
    double pozx = pos.x(); /*mm*/
    double pozy = pos.y(); /*mm*/
    double pozz = pos.z(); /*mm*/

    // Time
    double time = hit.GetTime(); /*ns*/

    int lt  = static_cast<int>(time);/*ns*/
    int tot = static_cast<int>(edep*1000000);/*eV*/

    int xPlane = planeid;
    int xBar = barid;
    int delta_t = lt-pTime;

    // Fill EMRDBBEvent
    EMRBarHit bHit;
    bHit.SetX(pozx);
    bHit.SetY(pozy);
    bHit.SetZ(pozz);
    bHit.SetTot(tot);
    bHit.SetHitTime(lt);
    bHit.SetDeltaT(delta_t);
    emr_dbb_events_tmp[xPe][xPlane][xBar].push_back(bHit);
  }
}

void MapCppEMRMCDigitization::processFADC(MAUS::EMRHitArray *EMRhits,
					  int xPe,
					  EMRfADCEventVector& emr_fadc_events_tmp) const {

  int nHits = EMRhits->size();

  for (int iHit = 0; iHit < nHits; iHit++) {

    EMRHit hit = EMRhits->at(iHit);

    // Plane ID
    EMRChannelId *id = hit.GetChannelId();

    int g4barid = id->GetBar();
    int planeid = g4barid / 59;

    // Energy Deposition
    double edep = hit.GetEnergyDeposited(); /*MeV*/

    double xCharge = edep*1000000;/*eV*/
    int xPlane = planeid;

    xCharge += emr_fadc_events_tmp[xPe][xPlane]._charge;
    fADCdata data;
    data._orientation = planeid % 2;
    data._charge = xCharge;
    data._time = 0;
    emr_fadc_events_tmp[xPe][xPlane] = data;
  }
}

void MapCppEMRMCDigitization::digitize(MAUS::EMREvent *EMRevt,
				       int xPe,
				       int nPartEvents,
				       int *deltat_limits,
				       EMRDBBEventVector& emr_dbb_events_tmp,
				       EMRfADCEventVector& emr_fadc_events_tmp) const {

  int nPlHits = EMRevt->GetEMRPlaneHitArray().size();

  for (int iPlane = 0; iPlane < nPlHits; iPlane++) {
    EMRPlaneHit *plHit = EMRevt->GetEMRPlaneHitArray().at(iPlane);
    int xPlane = plHit->GetPlane();

    // Find PMT gain correction coefficient
    EMRChannelKey xAverageKey(xPlane, xPlane%2, -1, "emr");
    double epsilon_MA = _calibMap.Eps(xAverageKey, "MA");
    double epsilon_SA = _calibMap.Eps(xAverageKey, "SA");

    int nBars = plHit->GetEMRBarArray().size();
    int naph_SAPMT = 0;

    for (int iBar = 0; iBar < nBars; iBar++) {
      EMRBar *bar = plHit->GetEMRBarArray().at(iBar);
      int xBar = bar->GetBar();
      int nBarHits = bar->GetEMRBarHitArray().size();
      bool matched = false;

      for (int iBarHit = 0; iBarHit < nBarHits; iBarHit++) {
	EMRBarHit barHit = bar->GetEMRBarHitArray().at(iBarHit);
	EMRChannelKey xKey(xPlane, xPlane%2, xBar, "emr");
        double x  = barHit.GetX();/*mm*/
	double y  = barHit.GetY();/*mm*/
	int xTot  = barHit.GetTot();/*eV*/
	int xDeltaT = barHit.GetDeltaT();/*ns*/
	int xHitTime = barHit.GetHitTime();/*ns*/

	/*--------- MAPMT signal simulation -------*/

	// simulate electronics noise and PMT dark current
	// !!! TO DO !!!

	// set g4 information
	double energy = xTot/1000000.0;/*MeV*/
	int time = xDeltaT;/*ns*/

	if (energy < _signal_energy_threshold) continue;

	// convert energy to the number of scintillation photons (nsph) according to Birks's law
	// !!! TO DO !!! calculate path length
	int nsph = static_cast<int>(static_cast<double>(_nph_per_MeV*energy)  // MeV
		 /(1.0+_birks_constant  // mm/MeV
		 /_average_path_length*energy));  // MeV

	// sample nsph with Poisson
	if (_do_sampling) nsph = _rand->Poisson(nsph);

	// covert nsph to the number of trapped photons (ntph)
	int ntph = static_cast<int>(static_cast<double>(nsph*_trap_eff));

	// sample ntph with Poisson
	if (_do_sampling) ntph = _rand->Poisson(ntph);

	// reduce ntph according to fibre length (naph)
	// for MAPMT
	int naph_MAPMT = static_cast<int>(static_cast<double>(ntph)
		       /2.0*_attenMap.fibreAtten(xKey, x, y, "MA"));

	// for SAPMT
	int naph_SAPMT_hit = static_cast<int>(static_cast<double>(ntph)
			   /2.0*_attenMap.fibreAtten(xKey, x, y, "SA"));

	// apply channel attenuation map
	// for MAPMT
	naph_MAPMT = static_cast<int>(static_cast<double>(naph_MAPMT)
		   *_attenMap.connectorAtten(xKey, "MA"));

	// for SAPMT
	naph_SAPMT_hit = static_cast<int>(static_cast<double>(naph_SAPMT_hit)
		       *_attenMap.connectorAtten(xKey, "SA"));

	if (time > deltat_limits[0] && time < deltat_limits[1]) naph_SAPMT += naph_SAPMT_hit;

	// sample naph with Poisson
	if (_do_sampling) naph_MAPMT = _rand->Poisson(naph_MAPMT);

	// simulate cross-talk and misalignment
	// !!! TO DO !!!

	// convert naph to the number of photoelectrons (npe)
	int npe = static_cast<int>(static_cast<double>(naph_MAPMT)*_QE_MAPMT);

	// sample npe with Poisson
	if (_do_sampling) npe = _rand->Poisson(npe);

	// correct npe for the photocathode non-uniformity
	// !!! TO DO !!!

	// correct npe for pmt gain difference
	npe = static_cast<int>(static_cast<double>(npe)*epsilon_MA);

	if (npe == 0) continue;

	// convert npe to the number of ADC counts
	int nADC = static_cast<int>(static_cast<double>(npe)*_nADC_per_pe_MAPMT);

	if (nADC == 0) continue;

	// simulate electronics response
	if (_do_sampling) nADC = static_cast<int>(_rand->Gaus(nADC, _electronics_response_spread_MAPMT));

	// convert nADC to Tot
	int xTotDigi = static_cast<int>(_tot_func_p1+_tot_func_p2
		     *log(static_cast<double>(nADC)
		     /_tot_func_p4+_tot_func_p3));

	if (xTotDigi <= 0) continue;

	// convert g4 time to deltaT time in ADC counts
	int xDeltaTDigi = static_cast<int>(static_cast<double>(time)/_dbb_count);

	// sample deltaT
	if (_do_sampling) xDeltaTDigi = static_cast<int>(_rand->Gaus(xDeltaTDigi,
								     _time_response_spread));
	if (xDeltaTDigi < 0) xDeltaTDigi = 0;

	// correct deltaT with fibre length
	xDeltaTDigi += static_cast<int>(static_cast<double>(_attenMap.fibreDelay(xKey, x, y, "MA"))
					/_dbb_count);

	// set hit time
	int xHitTimeDigi = static_cast<int>(static_cast<double>(xHitTime)/_dbb_count);

	// set bar hit
	EMRBarHit bHit;
	bHit.SetTot(xTotDigi);
	bHit.SetDeltaT(xDeltaTDigi);
	bHit.SetHitTime(xHitTimeDigi);

	// discriminate noise and decays from signal events
	if (xDeltaTDigi >= deltat_limits[0] && xDeltaTDigi < deltat_limits[1]) {
	  emr_dbb_events_tmp[xPe][xPlane][xBar].push_back(bHit);
	  matched = true;
	} else if (xDeltaTDigi >= deltat_limits[2] && xDeltaTDigi < deltat_limits[3] &&
		   xTotDigi > _tot_noise_low && xTotDigi < _tot_noise_up ) {
	  emr_dbb_events_tmp[nPartEvents][xPlane][xBar].push_back(bHit);
	  matched = true;
	} else if (iBarHit == nBarHits-1 && !matched) {
	  bHit.SetDeltaT(0);
	  emr_dbb_events_tmp[nPartEvents+1][xPlane][xBar].push_back(bHit);
	}
      }
    }

    int xOri = plHit->GetOrientation();

    /*------------ SAPMT signal simulation -------*/

    // simulate PMT dark current
    // !!! TO DO !!!

    // sample naph with Poisson
    if (_do_sampling) naph_SAPMT = _rand->Poisson(naph_SAPMT);

    // convert naph to the number of photoelectrons (npe)
    int npe = static_cast<int>(static_cast<double>(naph_SAPMT)*_QE_SAPMT);

    // sample npe with Poisson
    if (_do_sampling) npe = _rand->Poisson(npe);

    // correct npe for pmt gain difference
    npe = static_cast<int>(static_cast<double>(npe)*epsilon_SA);

    // convert npe to the number of ADC counts
    int nADC = static_cast<int>(static_cast<double>(npe)*_nADC_per_pe_SAPMT);

    // simulate electronics response
    if (_do_sampling) nADC = static_cast<int>(_rand->Gaus(nADC,
							  _electronics_response_spread_SAPMT));

    // correct non physical values
    if (nADC < 0) nADC = 0;

    // set signal baseline (8bit ADC)
    int baseline = _baseline[xPlane];

    // set electronics noise level - number of fluctuations within acquisition window
    int noise_level = _noise_level[xPlane];

    // set electronics noise position (upwards/downwards fluctuations)
    int noise_position = _noise_position[xPlane];

    // simulate negative voltage pulse with random electronics noise
    TH1F *pulse_shape = new TH1F("pulse_shape", "pulse_shape",
				 _acquisition_window, 0, _acquisition_window);

    double arrival_time = _signal_integration_window*2 + _arrival_time_shift;

    if (static_cast<int>(_rand->Uniform(0, 1.9999999))) arrival_time += _arrival_time_spread;

    arrival_time += _rand->Gaus(0, _arrival_time_gaus_width)
		 +_rand->Uniform(-_arrival_time_uniform_width, _arrival_time_uniform_width);

    for (int i = 0; i < noise_level; i++) {
      double noise = _rand->Uniform(0, _acquisition_window);
      int bin = static_cast<Int_t>(noise+1);
      if (pulse_shape->GetBinContent(bin) != 1) pulse_shape->Fill(noise);
    }

    if (noise_position) for (Int_t i = 1; i <= _acquisition_window; i++) {
      pulse_shape->SetBinContent(i, -pulse_shape->GetBinContent(i));}

    for (int i = 0; i < nADC; i++) {
      pulse_shape->Fill(_rand->Landau(arrival_time, _pulse_shape_landau_width));}

    for (int i = 1; i <= _acquisition_window; i++) {
      pulse_shape->SetBinContent(i, baseline-pulse_shape->GetBinContent(i));}

    double pedestal_baseline = pulse_shape->Integral(1, _signal_integration_window)
			     /static_cast<double>(_signal_integration_window);
    double pedestal_area = pedestal_baseline*static_cast<double>(_signal_integration_window)
			 - pulse_shape->Integral(_signal_integration_window+1,
			   _signal_integration_window*2);
    double signal_area = pedestal_baseline*static_cast<double>(_signal_integration_window)
		       - pulse_shape->Integral(arrival_time-10+1,
					       arrival_time+_signal_integration_window-10);

    // set pulse area
    double xAreaDigi = signal_area;

    // correct non physical values
    if (nADC == 0) xAreaDigi = 0;
    if (xAreaDigi < 0) xAreaDigi = 0;

    // set pedestal area
    double xPedestalAreaDigi = pedestal_area;

    // set hit arrival time
    int xArrivalTimeDigi = arrival_time;

    // simulate time delay in cables
    // !!! TO DO !!!

    // set pulse shape
    std::vector<int> xSamplesDigi;
    for (Int_t i = 1; i <= _acquisition_window; i++) {
      xSamplesDigi.push_back(pulse_shape->GetBinContent(i));}

    fADCdata data;
    data._orientation = xOri;
    data._charge = xAreaDigi;/*ADC*/
    data._pedestal_area = xPedestalAreaDigi;/*ADC*/
    data._time = xArrivalTimeDigi;/*ADC*/
    data._samples = xSamplesDigi;/*#*/
    emr_fadc_events_tmp[xPe][xPlane] = data;

    delete pulse_shape;
  }
}

void MapCppEMRMCDigitization::fill(MAUS::Spill *spill,
				   int nPartEvents,
				   EMRDBBEventVector emr_dbb_events_tmp,
				   EMRfADCEventVector emr_fadc_events_tmp) const {

  int recPartEvents = spill->GetReconEventSize();
  ReconEventPArray *recEvts =  spill->GetReconEvents();
  int xSpill = spill->GetSpillNumber();

  // Resize the recon event to harbour all the EMR noise+decays
  while (recPartEvents < nPartEvents) {
    recEvts->push_back(new ReconEvent);
    recPartEvents++;
  }

  // Fill it with DBB and fADC arrays
  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      int xOri  = emr_fadc_events_tmp[iPe][iPlane]._orientation;
      double xCharge = emr_fadc_events_tmp[iPe][iPlane]._charge;
      int xPos = emr_fadc_events_tmp[iPe][iPlane]._time;
      double xPedestalArea = emr_fadc_events_tmp[iPe][iPlane]._pedestal_area;
      std::vector<int> xSamples = emr_fadc_events_tmp[iPe][iPlane]._samples;

      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(iPlane);
      plHit->SetTrigger(iPe);
      plHit->SetRun(0);
      plHit->SetOrientation(xOri);
      plHit->SetCharge(xCharge);
      plHit->SetDeltaT(xPos);
      plHit->SetSpill(xSpill);
      plHit->SetPedestalArea(xPedestalArea);
      plHit->SetSamples(xSamples);

      EMRBarArray barArray;
      for (int iBar = 0; iBar < _number_of_bars; iBar++) {
        int nHits = emr_dbb_events_tmp[iPe][iPlane][iBar].size();
        if (nHits) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(iBar);
          bar->SetEMRBarHitArray(emr_dbb_events_tmp[iPe][iPlane][iBar]);
          barArray.push_back(bar);
        }
      }

      plHit->SetEMRBarArray(barArray);
      if (barArray.size() || xCharge) {
        plArray.push_back(plHit);
      }
    }

    if (iPe < 1) {evt->SetInitialTrigger(true);
    } else {evt->SetInitialTrigger(false);}

    evt->SetEMRPlaneHitArray(plArray);
    recEvts->at(iPe)->SetEMREvent(evt);
    recEvts->at(iPe)->SetPartEventNumber(iPe);
  }

  spill->SetReconEvents(recEvts);
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
