
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

#include <vector>
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/map/MapCppTrackerMCNoise/MapCppTrackerMCNoise.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTrackerMCNoise(void) {
  PyWrapMapBase<MAUS::MapCppTrackerMCNoise>::PyWrapMapBaseModInit
                                      ("MapCppTrackerMCNoise", "", "", "", "");
}


/***** Initialization *****/
MapCppTrackerMCNoise::MapCppTrackerMCNoise()
    : MapBase<Data>("MapCppTrackerMCNoise") {
}


/***** Destructor *****/
MapCppTrackerMCNoise::~MapCppTrackerMCNoise() {
}


/***** Birth *****/
void MapCppTrackerMCNoise::_birth(const std::string& argJsonConfigDocument) {
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  static MiceModule* _mice_modules = Globals::GetMonteCarloMiceModules();
  SF_modules = _mice_modules->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _configJSON = Globals::GetConfigurationCards();
  _vlpc_sim_on = (*_configJSON)["SciFi_BG_VLPCOn"].asInt();
  _rf_sim_on   = (*_configJSON)["SciFi_BG_RFOn"].asInt();

  if (_vlpc_sim_on == 1) {
    _vlpc_rate   = (*_configJSON)["SciFi_BG_VLPCRate"].asDouble();
    vlpc_mean_npe = -log(1-_vlpc_rate);
  }

  if (_rf_sim_on == 1) {
    _rf_ave_act = (*_configJSON)["SciFi_BG_RFAct"].asDouble();
    _rf_mag     = (*_configJSON)["SciFi_BG_RFMag"].asDouble();
    _rf_sdev    = (*_configJSON)["SciFi_BG_RFSDev"].asDouble();
    /***** Total number of events we would expect at the source *****/
    flux = _rf_ave_act / (1/pow(1.95, 2.0)+1/pow(2.15, 2.0)+1/pow(2.4, 2.0)
           +1/pow(2.7, 2.0)+1/pow(3.05, 2.0))*(5.0);
  }
}


/***** Death *****/
void MapCppTrackerMCNoise::_death() {
}


/***** Process *****/
void MapCppTrackerMCNoise::_process(Data* data) const {
  Spill& spill = *(data->GetSpill());

  if ( spill.GetMCEvents() ) {
  } else {
    throw MAUS::Exception(Exception::recoverable,
                          "MC event array not initialised, aborting noise",
                          "MapCppTrackerMCNoise::_process");
  }

  /***** Examine each particle event in a spill individually *****/
  for ( unsigned int event_i = 0; event_i < spill.GetMCEvents()->size(); event_i++ ) {
    int spill_n = spill.GetSpillNumber();
    SciFiNoiseHitArray* noise = new SciFiNoiseHitArray();

    // ===== Noise Implementation ===============================

    /***** Simulates noise from thermal activation of VLPCs ****/
    if (_vlpc_sim_on == 1) {
      VLPC_Thermal(spill, noise, event_i, spill_n);
    }

    /************************************************************ 
    *  Crude attempt to simulate effects of RF generated 
    *  x-rays on tracker background noise
    ************************************************************/
    if (_rf_sim_on == 1) {
      RF_X_Ray(spill, noise, event_i, spill_n);
    }

    // ==========================================================
    spill.GetMCEvents()->at(event_i)->SetSciFiNoiseHits(noise);
  }
}



void MapCppTrackerMCNoise::VLPC_Thermal(Spill& spill, SciFiNoiseHitArray* noise,
                                        unsigned int& event_i, int& spill_n) const {
  std::vector<int> chanArray;
  /**************************************************************************************
  * Ideally time should be a flat distribution in the spill time, however without a
  * trigger simulation this isn't possible.  Some investigation into MC spill time
  * might provide useful, but isn't really a pressing issue
  **************************************************************************************/
  double time1 = 0.;  // Fix: Need to assign a value to this!

  /*************************************************************************************
  * Description:
  * Determines the number of channels activated by background noise in the VLPCs
  * according to a Poisson distribution, where the mean activated channels can be set
  * at run time by command line/configuration file (--SciFiDarkCountChance=x).    
  * Individual channels are then selected and assigned a randomized PE value.
  * Likewise, the NPE is determined then from a Poisson calculation.
  **************************************************************************************/

  /***** Examine each tracker plane individually *****/
  for ( unsigned int mod_i = 0; mod_i < SF_modules.size(); mod_i++ ) {
    int nChannels = static_cast <int>
                (2*((SF_modules[mod_i]->propertyDouble("CentralFibre"))+0.5));
    double mean_channels = static_cast<double>(nChannels*_vlpc_rate);
    int active_channels = CLHEP::RandPoisson::shoot(mean_channels);

    /***** Fills array with all channels *****/
    chanArray.resize(0);
    for (int ich = 0; ich < nChannels; ich++) {
      chanArray.push_back(ich+1);
    }
    if (active_channels == 0) {
      continue;
    }
    for (int fch = 0; fch < active_channels; fch++) {
      /******************************************************
      * Determines and records activated channel then removes 
      * the channel from the list of a possible choices
      ******************************************************/
      int chan_ran = std::rand() % (nChannels - fch);
      int chan_i = chanArray[chan_ran];
      chanArray.erase(chanArray.begin()+chan_ran);

      /***** Setting PE value *****/
      int D_NPE = 0;
      Poisson_Hits(D_NPE, vlpc_mean_npe);

      /***** Writing out result *****/
      int tracker = SF_modules[mod_i]->propertyInt("Tracker");
      int station = SF_modules[mod_i]->propertyInt("Station");
      int plane   = SF_modules[mod_i]->propertyInt("Plane");
      SciFiNoiseHit a_noise_hit(spill_n, event_i,
                                tracker, station, plane,
                                chan_i, D_NPE, time1);
      noise->push_back(a_noise_hit);
    }
  }
}

/******************************************************************************
* Uses the chance of an event happening to determine the likelihood that the
* event will happen 1, 2, 3, or 4 times as described by a Poisson distribution.  
* Then uses a random number generator to choose the correct number of events. 
******************************************************************************/ 
void MapCppTrackerMCNoise::Poisson_Hits(int& hits, const double& mean) const {
  double p_one     = mean*exp(-mean);
  double p_two     = pow(mean, 2.0)*exp(-mean)/2;
  double p_three   = pow(mean, 3.0)*exp(-mean)/6;
  double p_four    = pow(mean, 4.0)*exp(-mean)/24;
  double p_sum     = p_one + p_two + p_three + p_four;
  double rand_pick = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX) * p_sum;

  hits = 0;
  if (rand_pick < p_one) {
    hits = 1;
  } else if (rand_pick < p_one+p_two) {
    hits = 2;
  } else if (rand_pick < p_one+p_two+p_three) {
    hits = 3;
  } else {
    hits = 4;
    }
}

void MapCppTrackerMCNoise::RF_X_Ray(Spill& spill, SciFiNoiseHitArray* noise,
                                    unsigned int& event_i, int& spill_n) const {
  std::vector<int> chanArray;
  double time1 = 0.;  // Fix: Need to assign a value to this!

  /*************************************************************************************
  * Description:
  * Written specificity to test a mean X-ray background rate of 10 hits per plane.
  * The per station hit rate varies as 1/r^2, where r comes from the distance from
  * the station (not plane, effect would be minimal) to the center of the RF cavity 
  * as determined from a simplistic model of the MICE Demonstration geometry (r1 = 1.95)
  **************************************************************************************/

	/***** Examine each tracker plane individually *****/
  for ( unsigned int mod_i = 0; mod_i < SF_modules.size(); mod_i++ ) {
    int nChannels = static_cast <int>
                    (2*((SF_modules[mod_i]->propertyDouble("CentralFibre"))+0.5));

    /***** Mean channels activated determined by distance from RF *****/
    double mean_channels = 0.0;
	  if (SF_modules[mod_i]->propertyInt("Station") == 1) {
	   mean_channels = flux/pow(1.95, 2.0);
	  } else if (SF_modules[mod_i]->propertyInt("Station") == 2) {
	   mean_channels = flux/pow(2.15, 2.0);
	  } else if (SF_modules[mod_i]->propertyInt("Station") == 3) {
	   mean_channels = flux/pow(2.4, 2.0);
	  } else if (SF_modules[mod_i]->propertyInt("Station") == 4) {
	   mean_channels = flux/pow(2.7, 2.0);
	  } else if (SF_modules[mod_i]->propertyInt("Station") == 5) {
	   mean_channels = flux/pow(3.05, 2.0);
	  }
    int active_channels = CLHEP::RandPoisson::shoot(mean_channels);

    /***** Fills array with all channels *****/
    chanArray.resize(0);
    for (int ich = 0; ich < nChannels; ich++) {
      chanArray.push_back(ich+1);
    }
    if (active_channels == 0) {
      continue;
    }
    for (int fch = 0; fch < active_channels; fch++) {
      /***********************************************************************
      * Determines and records activated channel then removes the channel from
      * the list of a possible choices in chanArray 
      ***********************************************************************/
      int chan_ran = std::rand() % (nChannels - fch);
      int chan_i = chanArray[chan_ran];
      chanArray.erase(chanArray.begin()+chan_ran);

      /***** Setting PE value, toy values! *****/
      double D_NPE = floor(CLHEP::RandGauss::shoot(_rf_mag, _rf_sdev));

      /***** Creating the noise hit for writing to spill *****/
      int tracker = SF_modules[mod_i]->propertyInt("Tracker");
      int station = SF_modules[mod_i]->propertyInt("Station");
      int plane   = SF_modules[mod_i]->propertyInt("Plane");
      SciFiNoiseHit a_noise_hit(spill_n, event_i,
                                tracker, station, plane,
                                chan_i, D_NPE, time1);
      noise->push_back(a_noise_hit);
    }
  }
}

} // ~namespace MAUS
