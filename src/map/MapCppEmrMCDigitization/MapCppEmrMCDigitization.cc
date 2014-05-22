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
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/map/MapCppEmrMCDigitization/MapCppEmrMCDigitization.hh"

namespace MAUS {

////////////////////////////////////////////////////////////////////////////
bool MapCppEmrMCDigitization::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppEmrMCDigitization";

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  try {
    //  JsonCpp setup
    Json::Value configJSON;
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

    // Load constants and hit preselection cuts
    Json::Value *json = Globals::GetConfigurationCards();

    _number_of_planes = (*json)["EMRnumberOfPlanes"].asInt();
    _number_of_bars = (*json)["EMRnumberOfBars"].asInt();

    _tot_noise_up = (*json)["EMRtotNoiseUp"].asInt();
    _tot_noise_low = (*json)["EMRtotNoiseLow"].asInt();

    _do_sampling = (*json)["EMRdoSampling"].asInt();
    _nph_per_MeV = (*json)["EMRnphPerMeV"].asInt();
    _seed = (*json)["EMRseed"].asInt();
    _trap_eff = (*json)["EMRtrapEff"].asDouble();
    _QE_MAPMT = (*json)["EMRqeMAPMT"].asDouble();
    _QE_SAPMT = (*json)["EMRqeSAPMT"].asDouble();
    _nADC_per_pe_MAPMT = (*json)["EMRnadcPerPeMAPMT"].asDouble();
    _nADC_per_pe_SAPMT = (*json)["EMRnadcPerPeSAPMT"].asDouble();
    _electronics_response_spread_MAPMT = (*json)["EMRelectronicsResponseSpreadMAPMT"].asDouble();
    _electronics_response_spread_SAPMT = (*json)["EMRelectronicsResponseSpreadSAPMT"].asDouble();
    _atten_WLSf = (*json)["EMRattenWLSf"].asDouble();
    _atten_CLRf = (*json)["EMRattenCLRf"].asDouble();
    _bar_length = (*json)["EMRbarLength"].asInt();
    _nbars = (*json)["EMRnBars"].asInt();
    _dbb_count = (*json)["EMRdbbCount"].asDouble();
    _fadc_count = (*json)["EMRfadcCount"].asDouble();
    _spill_width = (*json)["EMRspillWidth"].asInt();
    _time_response_spread = (*json)["EMRtimeResponseSpread"].asInt();
    _tot_func_p1 = (*json)["EMRtotFuncP1"].asDouble();
    _tot_func_p2 = (*json)["EMRtotFuncP2"].asDouble();
    _tot_func_p3 = (*json)["EMRtotFuncP3"].asDouble();
    _tot_func_p4 = (*json)["EMRtotFuncP4"].asDouble();
    _acquisition_window = (*json)["EMRacquisitionWindow"].asInt();
    _signal_integration_window = (*json)["EMRsignalIntegrationWindow"].asInt();
    _arrival_time_shift = (*json)["EMRarrivalTimeShift"].asInt();
    _arrival_time_gaus_width = (*json)["EMRarrivalTimeGausWidth"].asDouble();
    _arrival_time_uniform_width = (*json)["EMRarrivalTimeUniformWidth"].asDouble();
    _pulse_shape_landau_width = (*json)["EMRpulseShapeLandauWidth"].asDouble();
    _fom = (*json)["EMRfom"].asString();
    _birks_constant = (*json)["EMRbirksConstant"].asDouble();
    _average_path_length = (*json)["EMRaveragePathLength"].asDouble();
    _signal_energy_threshold = (*json)["EMRsignalEnergyThreshold"].asDouble();
    _baseline_spread = (*json)["EMRbaselineSpread"].asInt();
    _maximum_noise_level = (*json)["EMRmaximumNoiseLevel"].asInt();
    _deltat_shift = (*json)["EMRdeltatShift"].asDouble();
    _baseline_position = (*json)["EMRbaselinePosition"].asDouble();
    _arrival_time_spread = (*json)["EMRarrivalTimeSpread"].asDouble();

    // Generate random noise in each SAPMT
    _rand = new TRandom3(_seed);

    for (int i = 0; i < 48; i++) {
      _baseline[i] = _baseline_position
		   + static_cast<int>(_rand->Uniform(-_baseline_spread, _baseline_spread));
      _noise_level[i] = static_cast<int>(_rand->Uniform(0, _maximum_noise_level));
      _noise_position[i] = static_cast<int>(_rand->Uniform(0, 1.9999999));
    }

    // Check calibration file
    _calibfilename = std::string(pMAUS_ROOT_DIR)
		   + (*json)["EMR_calibration_file"].asString();
    _calibfile = fopen(_calibfilename.Data(), "r");
    if ( _calibfile == NULL ) {
       std::cerr << "EMR calibration data file not found..." << std::endl;
       exit(1);
    }
    fclose(_calibfile);

    // Check clear fiber length map
    _cflengthfilename = std::string(pMAUS_ROOT_DIR)
			+ (*json)["EMR_clear_fiber_length_map"].asString();
    _cflengthfile = fopen(_cflengthfilename.Data(), "r");
    if ( _cflengthfile == NULL ) {
       std::cerr << "EMR clear fibre length map not found..." << std::endl;
       exit(1);
    }
    fclose(_cflengthfile);

    // Check clear fiber length map
    _cattenfilename = std::string(pMAUS_ROOT_DIR)
		    + (*json)["EMR_connector_attenuation_map"].asString();
    _cattenfile = fopen(_cattenfilename.Data(), "r");
    if ( _cattenfile == NULL ) {
       std::cerr << "EMR connector attenuation map not found..." << std::endl;
       exit(1);
    }
    fclose(_cattenfile);

    // Reset spillCount
    _spillCount = 0;

    return true;
  } catch (Exception exc) {
    CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
  } catch (std::exception exc) {
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////
bool MapCppEmrMCDigitization::death() {
  return true;
}

////////////////////////////////////////////////////////////////////////////
std::string MapCppEmrMCDigitization::process(std::string document) {

  //==================== INPUT ====================//
  Json::Value spill_json;
  try {
    spill_json = JsonWrapper::StringToJson(document);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

  Data* spill_cpp;
  try {
    spill_cpp = JsonCppSpillConverter().convert(&spill_json);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to convert the input document into Data";
    errors["bad_json_document"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

  //==================== DIGITIZATION ====================//

  // Get spill, break if there's no trigger
  _spill = spill_cpp->GetSpill();
  int nPartEvents = _spill->GetMCEventSize();
  if (!nPartEvents) return document;

  // Check the Recon event array is initialised, and if not make it so
  if ( !_spill->GetReconEvents() ) {
    ReconEventPArray* revts = new ReconEventPArray();
    _spill->SetReconEvents(revts);
  }

  // Set primary hits/noise time windows
  std::vector<double> delta_t_array;

  for (int xPe = 0; xPe < nPartEvents; xPe++) {
    EMRHitArray *hits = _spill->GetAnMCEvent(xPe).GetEMRHits();

    if (hits) {
      int nHits = hits->size();

      Primary *primary = _spill->GetAnMCEvent(xPe).GetPrimary();
      int pTime = primary->GetTime(); // ns

      for (int ihit = 0; ihit < nHits; ihit++) {
	EMRHit hit = hits->at(ihit);

	int time = static_cast<int>(hit.GetTime()); /*ns*/
	double delta_t = time-pTime;

        delta_t_array.push_back(delta_t);
      }
    }
  }

  int lt = static_cast<int>(*std::min_element(delta_t_array.begin(), delta_t_array.end())
			    /_dbb_count);

  _deltat_signal_up = lt + 15;
  _deltat_signal_low = lt - 5;
  _deltat_noise_up = lt + 40;
  _deltat_noise_low = lt + 15;

  // Reset/Resize DBB and fADC arrays with n events (1 per trigger)
  reset_data_tmp(nPartEvents);

  // Fill the Recon event array with G4 Spill information

  for (int xPe = 0; xPe < nPartEvents; xPe++) {
    EMRHitArray *hits = _spill->GetAnMCEvent(xPe).GetEMRHits();

    if (hits) {
      Primary *primary = _spill->GetAnMCEvent(xPe).GetPrimary();
      int pTime = primary->GetTime(); // ns

      processDBB(hits, xPe, pTime);
      processFADC(hits, xPe, pTime);
    }
  }

  fill(nPartEvents);

  // Reset/Resize DBB and fADC arrays with n+2 events (1 per trigger + noise + decays)
  reset_data_tmp(nPartEvents+2);

  // Digitize the Recon event array
  for (int xPe = 0; xPe < nPartEvents; xPe++) {
    EMREvent *evt = _spill->GetReconEvents()->at(xPe)->GetEMREvent();

    if (evt) {
      digitize(evt, xPe, nPartEvents);
    }
  }

  fill(nPartEvents+2);

  // Increment Spill Count
  _spillCount++;

  // Fill data with digitized spill
  Data *spill_cpp_out = new MAUS::Data;
  spill_cpp_out->SetEventType("Spill");
  spill_cpp_out->SetSpill(_spill);

  //==================== OUTPUT ====================//

  Json::Value* spill_json_out;
  try {
    spill_json_out = CppJsonSpillConverter().convert(spill_cpp_out);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to convert the output Data into Json";
    errors["bad_cpp_data"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }
  document = JsonWrapper::JsonToString(*spill_json_out);

  delete spill_cpp;
  delete spill_json_out;
  return document;
}

////////////////////////////////////////////////////////////////////////////
void MapCppEmrMCDigitization::reset_data_tmp(int nPartEvts) {

  _emr_dbb_events_tmp.resize(0);
  _emr_dbb_events_tmp.resize(nPartEvts);

  for (int ipe = 0; ipe < nPartEvts; ipe++) {
    _emr_dbb_events_tmp[ipe].resize(_number_of_planes);  // number of planes
    for (int iplane = 0; iplane < _number_of_planes; iplane++) {
      _emr_dbb_events_tmp[ipe][iplane].resize(_number_of_bars); // number of bars in a plane
    }
  }

  _emr_fadc_events_tmp.resize(0);
  _emr_fadc_events_tmp.resize(nPartEvts);

  for (int ipe = 0; ipe < nPartEvts; ipe++) {
    _emr_fadc_events_tmp[ipe].resize(_number_of_planes);
    for (int iplane = 0; iplane < _number_of_planes; iplane++) {
      fADCdata data;
      data._orientation = iplane % 2;
      data._charge = 0;
      data._pedestal_area = 0;
      data._arrival_time = 0;
      std::vector<int> xSamples;
      for (int isample = 0; isample < 1; isample++) xSamples.push_back(0);
      data._samples = xSamples;
      _emr_fadc_events_tmp[ipe][iplane] = data;
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void MapCppEmrMCDigitization::processFADC(EMRHitArray *hits, int xPe, int ptime) {

  int nHits = hits->size();

  for (int ihit = 0; ihit < nHits; ihit++) {

    EMRHit hit = hits->at(ihit);

    // Plane ID
    EMRChannelId *id = hit.GetChannelId();

    int g4barid = id->GetBar();
    int planeid = g4barid / 59;

    // Energy Deposition
    double edep = hit.GetEnergyDeposited(); /*MeV*/

    double xCharge = edep*1000000;/*eV*/
    int xPlane = planeid;

    xCharge += _emr_fadc_events_tmp[xPe][xPlane]._charge;
    fADCdata data;
    data._orientation = planeid % 2;
    data._charge = xCharge;
    data._arrival_time = 0;
    _emr_fadc_events_tmp[xPe][xPlane] = data;
  }
}

////////////////////////////////////////////////////////////////////////////
void MapCppEmrMCDigitization::processDBB(EMRHitArray *hits, int xPe, int ptime) {

  int nHits = hits->size();

  for (int ihit = 0; ihit < nHits; ihit++) {

    EMRHit hit = hits->at(ihit);

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
    int delta_t = lt-ptime;

    // Fill EMRDBBEvent
    EMRBarHit bHit;
    bHit.SetX(pozx);
    bHit.SetY(pozy);
    bHit.SetZ(pozz);
    bHit.SetTot(tot);
    bHit.SetHitTime(lt);
    bHit.SetDeltaT(delta_t);
    bHit.SetHitTime(lt);
    _emr_dbb_events_tmp[xPe][xPlane][xBar].push_back(bHit);
  }
}

////////////////////////////////////////////////////////////////////////////
void MapCppEmrMCDigitization::fill(int nPartEvents) {

  int recPartEvents = _spill->GetReconEventSize();
  ReconEventPArray *recEvts =  _spill->GetReconEvents();

  // Resize the recon event to harbour all the EMR noise+decays
  if (recPartEvents == 0) { // No recEvts yet
      for (int ipe = 0; ipe < nPartEvents; ipe++) {
        recEvts->push_back(new ReconEvent);
    }
  }

  if (nPartEvents == recPartEvents+2) { // Regular size recEvts already created
    for (int ipe = 0; ipe < 2; ipe++) {
      recEvts->push_back(new ReconEvent);
    }
  }

  // Fill it with DBB and fADC arrays
  for (int ipe = 0; ipe < nPartEvents; ipe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int iplane = 0; iplane < _number_of_planes; iplane++) {
      int xOri  = _emr_fadc_events_tmp[ipe][iplane]._orientation;
      double xCharge = _emr_fadc_events_tmp[ipe][iplane]._charge;
      int xPos = _emr_fadc_events_tmp[ipe][iplane]._arrival_time;
      double xPedestalArea = _emr_fadc_events_tmp[ipe][iplane]._pedestal_area;
      std::vector<int> xSamples = _emr_fadc_events_tmp[ipe][iplane]._samples;

      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(iplane);
      plHit->SetTrigger(ipe);
      plHit->SetRun(0);
      plHit->SetOrientation(xOri);
      plHit->SetCharge(xCharge);
      plHit->SetDeltaT(xPos);
      plHit->SetSpill(_spillCount);
      plHit->SetPedestalArea(xPedestalArea);
      plHit->SetSamples(xSamples);

      EMRBarArray barArray;
      for (int ibar = 0; ibar < _number_of_bars; ibar++) {
        int nHits = _emr_dbb_events_tmp[ipe][iplane][ibar].size();
        if (nHits) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(ibar);
          bar->SetEMRBarHitArray(_emr_dbb_events_tmp[ipe][iplane][ibar]);
          barArray.push_back(bar);
        }
      }

      plHit->SetEMRBarArray(barArray);
      if (barArray.size() || xCharge) {
        plArray.push_back(plHit);
      }
    }

    if (ipe < 1) {evt->SetInitialTrigger(true);
    } else {evt->SetInitialTrigger(false);}

    evt->SetEMRPlaneHitArray(plArray);
    recEvts->at(ipe)->SetEMREvent(evt);
  }

  _spill->SetReconEvents(recEvts);
}

////////////////////////////////////////////////////////////////////////////
void MapCppEmrMCDigitization::digitize(EMREvent *evt, int xPe, int nPartEvents) {

  int nPlHits = evt->GetEMRPlaneHitArray().size();
  int nBarsTotal = 0;

  for (int ipl = 0; ipl < nPlHits; ipl++) {
    EMRPlaneHit *plHit = evt->GetEMRPlaneHitArray().at(ipl);
    nBarsTotal += plHit->GetEMRBarArray().size();
  }

  for (int ipl = 0; ipl < nPlHits; ipl++) {
    EMRPlaneHit *plHit = evt->GetEMRPlaneHitArray().at(ipl);
    int xPlane = plHit->GetPlane();

    // Find PMT gain correction coefficient
    _calibfile = fopen(_calibfilename.Data(), "r");
    double epsilon_MA = 0.0;
    double epsilon_SA = 0.0;
    char line[100];
    while (fgets(line, 100, _calibfile)) {
      char pmt[10], fom[10];
      int pl(-1), ch(-1);
      float corr(-1.0);
      sscanf(line, "%s %s %d %d %f", pmt, fom, &pl, &ch, &corr);
      if (strcmp(pmt, "MA") == 0 &&
	  strcmp(fom, _fom.c_str()) == 0 &&
	  pl == xPlane &&
	  ch == -1) {epsilon_MA = corr;}
      if (strcmp(pmt, "SA") == 0 &&
	  strcmp(fom, _fom.c_str()) == 0 &&
	  pl == xPlane &&
	  ch == -1) {epsilon_SA = corr;}
      if (epsilon_SA && epsilon_MA) break;
    }
    fclose(_calibfile);

    int nBars = plHit->GetEMRBarArray().size();
    int naph_SAPMT = 0;

    for (int ibar = 0; ibar < nBars; ibar++) {

      EMRBar *bar = plHit->GetEMRBarArray().at(ibar);
      int xBar = bar->GetBar();
      int nBarHits = bar->GetEMRBarHitArray().size();

      for (int ibh = 0; ibh < nBarHits; ibh++) {

	EMRBarHit barHit = bar->GetEMRBarHitArray().at(ibh);
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
		       /2.0*fiber_atten_map(xPlane, xBar, x, y, 0));

	// for SAPMT
	int naph_SAPMT_hit = static_cast<int>(static_cast<double>(ntph)
			   /2.0*fiber_atten_map(xPlane, xBar, x, y, 1));

	// apply channel attenuation map
	// for MAPMT
	int global_barid = 59*xPlane+xBar-1;
	naph_MAPMT = static_cast<int>(static_cast<double>(naph_MAPMT)
		   *connector_atten_map(global_barid, 0));

	// for SAPMT
	naph_SAPMT_hit = static_cast<int>(static_cast<double>(naph_SAPMT_hit)
		       *connector_atten_map(global_barid, 1));

	if (time < 10/*!!! TO DO !!! use a parameter*/) naph_SAPMT += naph_SAPMT_hit;

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

	// correct deltaT with fiber length
	xDeltaTDigi += static_cast<int>(static_cast<double>(fiber_delay_map(xPlane, xBar, x, y, 0))
					/_dbb_count);

	// set hit time
	int xHitTimeDigi = static_cast<int>(static_cast<double>(xHitTime)/_dbb_count);

	// set bar hit
	EMRBarHit bHit;
	bHit.SetTot(xTotDigi);
	bHit.SetDeltaT(xDeltaTDigi);
	bHit.SetHitTime(xHitTimeDigi);

	// select signal and noise events
	if (xDeltaTDigi > _deltat_signal_low && xDeltaTDigi < _deltat_signal_up) {
	  _emr_dbb_events_tmp[xPe][xPlane][xBar].push_back(bHit);
	} else if (xDeltaTDigi > _deltat_noise_low && xDeltaTDigi < _deltat_noise_up &&
		   xTotDigi > _tot_noise_low && xTotDigi < _tot_noise_up ) {
	  _emr_dbb_events_tmp[nPartEvents][xPlane][xBar].push_back(bHit);
	} else {
	  bHit.SetDeltaT(0);
	  _emr_dbb_events_tmp[nPartEvents+1][xPlane][xBar].push_back(bHit);
	}
      }
    }

    int xOri = plHit->GetOrientation();
    double xCharge = plHit->GetCharge();/*eV*/
    double energy = xCharge/1000000.0;/*MeV*/

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
    data._arrival_time = xArrivalTimeDigi;/*ADC*/
    data._samples = xSamplesDigi;/*#*/
    data._g4_edep_mev = energy;/*MeV*/
    _emr_fadc_events_tmp[xPe][xPlane] = data;

    delete pulse_shape;
  }
}

////////////////////////////////////////////////////////////////////////////
double MapCppEmrMCDigitization::fiber_atten_map(int planeid, int barid,
						double x, double y, int pmt) {

  /*------------ WLS fiber ---------*/
  double WLSf_length;

  if (planeid%2 == 0) { // X plane - light travels along Y direction
    if (pmt == 0) {WLSf_length = _bar_length/2./100.0 - y/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 + y/1000.0;} // SAPMT
  } else { // Y plane - light travels along X direction
    if (pmt == 0) {WLSf_length = _bar_length/2./100.0 + x/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 - x/1000.0;} // SAPMT
  }
  double atten_wlsf = pow(10, -_atten_WLSf*WLSf_length/10.0);

  /*---------- Clear fiber ---------*/
  double clearf_length = clear_fiber_length_map(barid, pmt)/1000.0;
  double atten_clrf = pow(10, -_atten_CLRf*clearf_length/10.0);

  return atten_wlsf*atten_clrf;
}

////////////////////////////////////////////////////////////////////////////
int MapCppEmrMCDigitization::fiber_delay_map(int planeid, int barid,
						double x, double y, int pmt) {

  int c = 299792458; // m/s
  double n = 1.59; // Polystyrene refractive index

  /*------------ WLS fiber ---------*/
  double WLSf_length;

  if (planeid%2 == 0) { // X plane - light travels along Y direction
    if (pmt == 0) {WLSf_length = _bar_length/2./100.0 - y/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 + y/1000.0;} // SAPMT
  } else { // Y plane - light travels along X direction
    if (pmt == 0) {WLSf_length = _bar_length/2./100.0 + x/1000.0; // MAPMT
    } else {WLSf_length = _bar_length/2./100.0 - x/1000.0;} // SAPMT
  }
  double delay_wlsf = WLSf_length*n*1000000000/c; // ns

  /*---------- Clear fiber ---------*/
  double clearf_length = clear_fiber_length_map(barid, pmt)/1000.0;
  double delay_clrf = clearf_length*n*1000000000/c; // ns

  return static_cast<int>(delay_wlsf+delay_clrf);
}

////////////////////////////////////////////////////////////////////////////
double MapCppEmrMCDigitization::clear_fiber_length_map(int barid, int pmt) {

  double l = -1.0;
  char line[100];

  _cflengthfile = fopen(_cflengthfilename.Data(), "r");
  while (fgets(line, 100, _cflengthfile)) {
    int pmid(-1), chid(-1);
    float length(-1.0);
    sscanf(line, "%d  %d  %f", &pmid, &chid, &length);
    if (pmid == pmt && chid == barid) {
      l = length;
      break;
    }
  }
  fclose(_cflengthfile);

  if (l == -1.) {
    std::cerr << "Wrong pmt or barid in clear_fiber_length_map" << std::endl;
    exit(1);
  }

  return l;
}

////////////////////////////////////////////////////////////////////////////
double MapCppEmrMCDigitization::connector_atten_map(int barid, int pmt) {

  double a = -999.9;
  char line[100];

  _cattenfile = fopen(_cattenfilename.Data(), "r");
  while (fgets(line, 100, _cattenfile)) {
    int pmid(-1), chid(-1);
    float attenuation(-1.0);
    sscanf(line, "%d  %d  %f", &pmid, &chid, &attenuation);
    if (pmid == pmt && chid == barid) {
      a = attenuation;
      break;
    }
  }
  fclose(_cattenfile);

  a += 1.0;
  if (a == -998.9) {
    std::cerr << "Wrong pmt or barid in connector_atten_map()" << std::endl;
    exit(1);
  }

  return a;
}
} // Namespace MAUS

