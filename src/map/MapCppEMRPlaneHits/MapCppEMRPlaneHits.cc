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

#include "src/map/MapCppEMRPlaneHits/MapCppEMRPlaneHits.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppEMRPlaneHits(void) {
  PyWrapMapBase<MAUS::MapCppEMRPlaneHits>::PyWrapMapBaseModInit
                                                ("MapCppEMRPlaneHits", "", "", "", "");
}

MapCppEMRPlaneHits::MapCppEMRPlaneHits()
    : MapBase<MAUS::Data>("MapCppEMRPlaneHits") {
}

void MapCppEMRPlaneHits::_birth(const std::string& argJsonConfigDocument) {
  _classname = "MapCppEMRPlaneHits";
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    throw MAUS::Exception(Exception::recoverable,
                      "Could not resolve ${MAUS_ROOT_DIR} environment variable",
                      "MapCppEMRPlaneHits::birth");
  }

  //  JsonCpp setup
  Json::Value configJSON;
  Json::Value map_file_name;
  Json::Value xEnable_V1731_Unpacking;
  Json::Value xEnable_DBB_Unpacking;

  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _primary_deltat_low = configJSON["EMRprimaryDeltatLow"].asInt();
  _primary_deltat_up = configJSON["EMRprimaryDeltatUp"].asInt();
  _secondary_n_low = configJSON["EMRsecondaryNLow"].asInt();
  _secondary_tot_low = configJSON["EMRsecondaryTotLow"].asInt();
  _secondary_bunching_width = configJSON["EMRsecondaryBunchingWidth"].asInt();

  // Load EMR channel map
  std::string xMapFile = std::string(pMAUS_ROOT_DIR)
			 + configJSON["EMR_cabling_file"].asString();
  bool loaded = _emrMap.InitializeFromFile(xMapFile);
  if ( !loaded )
    throw MAUS::Exception(Exception::recoverable,
                          "Failed to load EMR Channel Map File",
                          "MapCppEMRPlaneHits::birth");

  // Enable fADC unpacking
  xEnable_V1731_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1731_Unpacking",
                                                     JsonWrapper::booleanValue);
  if ( !xEnable_V1731_Unpacking.asBool() ) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppEMRPlaneHits::birth. The unpacking of the flashADC V1731 is disabled!!!"
    << " Are you sure you want this?"
    << std::endl;
  }

  // Enable DBB unpacking
  xEnable_DBB_Unpacking   = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_DBB_Unpacking",
                                                     JsonWrapper::booleanValue);
  if ( !xEnable_DBB_Unpacking.asBool() ) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppEMRPlaneHits::birth. The unpacking of the VRBs is disabled!!!"
    << " Are you sure you want this?"
    << std::endl;
  }
}

void MapCppEMRPlaneHits::_death() {
}

void MapCppEMRPlaneHits::_process(Data *data) const {

  // Routine data checks before processing it
  if ( !data )
      throw Exception(Exception::recoverable, "Data was NULL",
                      "MapCppEMRPlaneHits::_process");

  Spill* spill = data->GetSpill();
  if ( !spill )
      throw Exception(Exception::recoverable, "Spill was NULL",
                      "MapCppEMRPlaneHits::_process");

  if ( spill->GetDaqEventType() != "physics_event" )
      return;

  DAQData* daqData = spill->GetDAQData();
  if ( !daqData )
      throw Exception(Exception::recoverable, "DAQData was NULL",
                      "MapCppEMRPlaneHits::_process");

  EMRDaq emrDaq = daqData->GetEMRDaq();
  size_t nPartEvents = emrDaq.GetV1731NumPartEvents();
  if ( !nPartEvents )
      return;

  // Check the Recon event array and EMR Spill Data are initialised. If not, make it so
  if ( !spill->GetReconEvents() )
      spill->SetReconEvents(new ReconEventPArray());

  if ( !spill->GetEMRSpillData() )
      spill->SetEMRSpillData(new EMRSpillData());

  // Create a temporary array of n plane hit arrays (1 per trigger) and a temporary array of bars
  EMREventPlaneHitVector emr_events_tmp = get_emr_events_tmp(nPartEvents);	// EMREvents
  EMRBarHitArray emr_bar_hits_tmp;						// EMRSpillData

  // Fill the temporary arrays with the DAQ information
  process_DBB(emrDaq, emr_events_tmp, emr_bar_hits_tmp, nPartEvents);
  process_fADC(emrDaq, emr_events_tmp, nPartEvents);

  // Create a temporary array to harbour n' plane hit arrays (1 per daughter candiate)
  EMREventPlaneHitVector emr_candidates_tmp;

  // Process the secondary hits and merge them timewise into daughter candidates
  process_candidates(emr_bar_hits_tmp, emr_candidates_tmp);
  emr_events_tmp.insert(emr_events_tmp.end(),
			emr_candidates_tmp.begin(), emr_candidates_tmp.end());

  // Fill the Recon event array with spill information (1 per trigger + daughter candidates)
  fill(spill, emr_events_tmp, nPartEvents);
}

void MapCppEMRPlaneHits::process_DBB(MAUS::EMRDaq EMRdaq,
		  		    EMREventPlaneHitVector& emr_events_tmp,
				    EMRBarHitArray& emr_bar_hits_tmp,
		  	 	    size_t nPartEvents) const {

//  std::cerr << "DBBArraySize: " << EMRdaq.GetDBBArraySize() << std::endl;
  for (size_t iDBB = 0; iDBB < EMRdaq.GetDBBArraySize(); iDBB++) {
    DBBSpillData dbb = EMRdaq.GetDBBArrayElement(iDBB);

    if ( static_cast<size_t>(dbb.GetTriggerCount()) != nPartEvents ) {
      Squeak::mout(Squeak::error)
      << "ERROR in  MapCppEMRPlaneHits::process_DBB: number of triggers mismatch ("
      << dbb.GetTriggerCount() << "!=" << nPartEvents << ")" << std::endl;
      return;
    }

    int xLDC    = dbb.GetLdcId();
    int xGeo    = dbb.GetDBBId();
    size_t nHits   = dbb.GetDBBHitsArraySize();

    for (size_t iHit = 0; iHit < nHits; iHit++) {
      DBBHit this_hit = dbb.GetDBBHitsArrayElement(iHit);
      int xCh = this_hit.GetChannel();

      DAQChannelKey daqKey(xLDC, xGeo, xCh, 141, "emr");
//      std::cerr << daqKey << std::endl;

      EMRChannelKey *emrKey = _emrMap.Find(&daqKey);
      if ( emrKey ) {

        int xPlane = emrKey->GetPlane();
        int xBar   = emrKey->GetBar();
        int xLTime  = this_hit.GetLTime();
        int xTTime  = this_hit.GetTTime();
	if ( !xLTime || !xTTime )
	    continue;
        int xTot = xTTime - xLTime;
//        std::cerr << *emrKey << " --> xLTime: " << xLTime
//                             << "  xTTime: " << xTTime << std::endl;

	// Loop over the trigger and try to associate the hit to one of them
	bool matched = false;
        for (size_t iPe = 0; iPe < nPartEvents; iPe++) {
          DBBHit this_trigger = dbb.GetDBBTriggersArrayElement(iPe);
          int tr_lt = this_trigger.GetLTime();
//          int tr_tt = this_trigger.GetTTime();
//          int xCh   = this_trigger.GetChannel();

          if ( !iHit ) // Set the fADC time only when processing the first hit
            emr_events_tmp[iPe][xPlane]._time  = tr_lt;

          int xDeltaT = xLTime - tr_lt;

	  // Set bar hit
          EMRBarHit bHit;
	  bHit.SetChannel(xPlane*_number_of_bars+xBar);
          bHit.SetTot(xTot);
	  bHit.SetTime(xLTime);

	  // Discriminate primary hits (close to the trigger) from the rest
          if ( xDeltaT > _primary_deltat_low && xDeltaT < _primary_deltat_up ) {
            bHit.SetDeltaT(xDeltaT - _primary_deltat_low);
            emr_events_tmp[iPe][xPlane]._barhits.push_back(bHit);
	    matched = true;
//            std::cerr << "*---> " << *emrKey << " --> trigger_Id: " << iPe
//            	        << "  xTot: " << xTot
//            	        << "  xDeltaT: " << xDeltaT - _primary_deltat_low
//            	        << "(" << xDeltaT << ")" << std::endl;
          } else if ( iPe == nPartEvents-1 && !matched ) {
            bHit.SetDeltaT(0);
	    if ( xTot > _secondary_tot_low )
	    	emr_bar_hits_tmp.push_back(bHit);
	  }
        }
      }
    }
  }
}

void MapCppEMRPlaneHits::process_fADC(MAUS::EMRDaq EMRdaq,
		  		      EMREventPlaneHitVector& emr_events_tmp,
		  		      size_t nPartEvents) const {

//  std::cerr << "GetV1731NumPartEvents: " << EMRdaq.GetV1731NumPartEvents() << std::endl;
  for (size_t iPe = 0; iPe < nPartEvents; iPe++) {

    V1731HitArray fADChits = EMRdaq.GetV1731PartEvent(iPe);
    size_t nHits = fADChits.size();
//    std::cerr << "PartEvent " << iPe << "  --> " << nHits << " hits\n" << std::endl;

    for (size_t iHit = 0; iHit < nHits; iHit++) {
      V1731 fADChit = fADChits[iHit];
      int xLDC  = fADChit.GetLdcId();
      int xGeo  = fADChit.GetGeo();
      int xCh   = fADChit.GetChannel();
      int xEqType = fADChit.GetEquipType();

      DAQChannelKey daqKey(xLDC, xGeo, xCh, xEqType, "emr");
//      std::cerr << daqKey << std::endl;
      EMRChannelKey *emrKey = _emrMap.Find(&daqKey);
      if ( emrKey ) {
        int xPlane = emrKey->GetPlane();
        int xOri   = emrKey->GetOrientation();
        int xArea = fADChit.GetPulseArea();
        int xPos  = fADChit.GetPositionMin();
        std::vector<int> xSamples = fADChit.GetSampleArray();
//        std::cerr << iPe << "  " << *emrKey << " --> pos: "
//                  << xPos << "  area: " << xArea << std::endl;
        emr_events_tmp[iPe][xPlane]._orientation  = xOri;
        emr_events_tmp[iPe][xPlane]._charge       = xArea;
        emr_events_tmp[iPe][xPlane]._deltat       = xPos;
        emr_events_tmp[iPe][xPlane]._samples      = xSamples;
      }
    }
  }
}

void MapCppEMRPlaneHits::process_candidates(EMRBarHitArray emr_bar_hits_tmp,
					    EMREventPlaneHitVector& emr_candidates_tmp) const {

  // Sort the vector in ascending order of hit time (lambda sorting function)
  sort(emr_bar_hits_tmp.begin(), emr_bar_hits_tmp.end(),
       [] (const EMRBarHit& a, const EMRBarHit& b) {
	   return a.GetTime() < b.GetTime();
       });

  // Find groups of hits defined by the secondary bunching width
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
      emr_candidates_tmp[iGroup][xPlane]._barhits.push_back(barHitArray[iHit]);
    }
  }
}

void MapCppEMRPlaneHits::fill(MAUS::Spill *spill,
		  	      EMREventPlaneHitVector emr_events_tmp,
		  	      size_t nPartEvents) const {

  // Get the EMR recon events and the spill data
  ReconEventPArray *recEvts =  spill->GetReconEvents();
  EMRSpillData *emrSpill = spill->GetEMRSpillData();

  for (size_t iPe = 0; iPe < emr_events_tmp.size(); iPe++) {

    EMREventTrack* evtTrack = new EMREventTrack;

    EMRPlaneHitArray plArray;
    for (size_t iPlane = 0; iPlane < _number_of_planes; iPlane++) {

      EMRPlaneHit *plHit = new EMRPlaneHit;
      EMRBarHitArray barHitArray = emr_events_tmp[iPe][iPlane]._barhits;

      plHit->SetPlane(iPlane);
      plHit->SetEMRBarHitArray(barHitArray);
      plHit->SetOrientation(emr_events_tmp[iPe][iPlane]._orientation);
      plHit->SetTime(emr_events_tmp[iPe][iPlane]._time);
      plHit->SetDeltaT(emr_events_tmp[iPe][iPlane]._deltat);
      plHit->SetCharge(emr_events_tmp[iPe][iPlane]._charge);
      plHit->SetSampleArray(emr_events_tmp[iPe][iPlane]._samples);

      if ( barHitArray.size() || plHit->GetCharge() > 0 ) {
        plArray.push_back(plHit);
	for ( size_t iHit = 0; iHit < barHitArray.size(); iHit++ )
	  emrSpill->AddEMRBarHit(barHitArray[iHit]);
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

EMREventPlaneHitVector MapCppEMRPlaneHits::get_emr_events_tmp(size_t nPartEvts) const {
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
      data._samples = xSamples;
      emr_events_tmp[iPe][iPlane] = data;
    }
  }
  return emr_events_tmp;
}
} // namespace MAUS
