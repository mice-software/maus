// Y.Karadzhov 15.10.2009

#include "Recon/TOF/TofTrigger.hh"

TofTrigger::TofTrigger()
{
  theKey = NULL;
  p_trigger.resize(10);
  p_trigger_request.resize(10);
  //Station = 0;
}

TofTrigger::~TofTrigger()
{
  if(theKey) delete theKey;
}

void TofTrigger::Reset()
{
  for(unsigned int i=0;i<p_trigger.size();i++)
	 p_trigger[i] = 0;

  for(unsigned int j=0;j<p_trigger_request.size();j++)
  {
	 p_trigger_request[j].resize(0);
  }
  TriggerTime = -99999;
  if(theKey) delete theKey;
  theKey = NULL;
}

void TofTrigger::SetTriggerTimes( MICEEvent* theEvent, TofChannelMap* tofmap)
{
  int tdcHits = theEvent->vmeTdcHits.size();
  int nTr = 0;
  for(int i=0; i<tdcHits; i++)
  {
	 VmeTdcHit* tdchit = theEvent->vmeTdcHits[i];
	 if( tofmap->isThisTofTdc(tdchit) )
	 {
		int st, plane, slab, pmt, pmtNum;
		std::string pmtName;

      //! find the position of the hit in the channel map and check is this a trigger.
		pmtNum = tofmap->findThePosition(tdchit, st, plane, slab, pmt, pmtName);
		if(pmtName == "trigger" ) p_trigger[ tdchit->board() ]= tdchit->tdc();
		if(pmtName == "triggerRequest" ) {p_trigger_request[ tdchit->board() ].push_back( tdchit->tdc() ); nTr++;}
	 }
  }
}

TriggerKey* TofTrigger::ProcessTriggerKey(MICEEvent* theEvent,TofCalibrationMap*calib_map,vector< vector<int> > TrHits,double tCut,bool CalibMode)
{
  //! The function searches for the pixel that is giving the trigger.
  //! TrHits argument holds the serial numbers in the MICEEvent of the TofSlabHits in the trigger station.
  //! The first index of TrHits corresponds to the plane number of the hit.
  //! The second index of TrHits corresponds to the plane number of the hit.
  //! tCut is used to recognize the trigger pixel. After the calibration corrections the reconstructed
  //! time of the trigger hit have to be 0 +- tCut.
  //! In Calibration Mode the time of the trigger hit is not reconstructed.

  //if(TrSlabHits[0].size()>1 && TrSlabHits[1].size()>1)
  //cout<<"trigger SlHits size "<<dec<<TrHits[0].size()<<" "<<TrHits[1].size()<<" Digits = "<<theEvent->tofDigits.size()<<endl;
  //cout<<"CalibMode "<<CalibMode<<endl;
  if( CalibMode )
  {
	 //! If more than one TofSlabHit in plane 0 and plane 1 return NULL.
	 if(TrHits[0].size()==1 && TrHits[1].size()==1)
	 {
		TofSlabHit *hitA = theEvent->tofSlabHits[ TrHits[0][0] ];
		TofSlabHit *hitB = theEvent->tofSlabHits[ TrHits[1][0] ];
		theKey = new TriggerKey(Station, hitA->slabNumber(), hitB->slabNumber());
		return theKey;
	 }else return NULL;
  }

  for(unsigned int m=0;m<TrHits[0].size();m++)
	 for(unsigned int n=0;n<TrHits[1].size();n++)
	 {
		TofSlabHit *hitA = theEvent->tofSlabHits[ TrHits[0][m] ];
		TofSlabHit *hitB = theEvent->tofSlabHits[ TrHits[1][n] ];

		theKey = new TriggerKey(Station, hitA->slabNumber(), hitB->slabNumber());

      //! If no calibration for this pixel return NULL.
		if( !hitA->CalibrateTime(calib_map,theKey) ) {
		  delete theKey;
		  theKey = NULL;
		  continue;
		}
		if( !hitB->CalibrateTime(calib_map,theKey) ) {
		  delete theKey;
		  theKey = NULL;
		  continue;
		}

		double tSlA = hitA->time();
		double tSlB = hitB->time();
		//cout<<"trigger Time  "<<dec<<tSlA<<" "<<tSlB<<" "<<fabs( (tSlA+tSlB)/2. )<<" "<<(tSlA-tSlB)<<endl;

		//! If the reconstructed time of the trigger hit is not inside the cut return NULL.
		if (fabs( (tSlA+tSlB)/2. ) < tCut)
		{
		  TriggerTime = (tSlA+tSlB)/2.;
		  return theKey;
		}
		else
		{
		  delete theKey;
		  theKey = NULL;
		}
	 }

  return NULL;
}

void TofTrigger::Print()
{
  cout << "____________________________" << endl;
  cout << "  TofTrigger " << endl;
  cout << "  Station Number : " << Station << endl;
  for(unsigned int i=0;i<10;i++)
	 if(p_trigger[i])
	 {
		cout << "  Board " << dec <<i<<" Particle Trigger : " << p_trigger[i] <<endl;
		cout << "  Board " << dec <<i<<" Particle Trigger Requests : ";
		for(unsigned int j=0;j<p_trigger_request[i].size();j++)
		   cout << p_trigger_request[i][j] <<" ";
		cout<<endl;
	 }
  if(theKey)
  {
	 cout << "  Slab A : "<< theKey->slabA() << endl;
	 cout << "  Slab B : "<< theKey->slabB() << endl;
  } else
  {
	 cout << "  Slab A : ---"<< endl;
	 cout << "  Slab B : ---"<< endl;
  }
  if(TriggerTime>-99990) cout << "  Trigger Time : " << TriggerTime << endl;
  else cout << "  Trigger Time : ---" << endl;
  cout << "____________________________" << endl;
}

int TofTrigger::GetTrigger(int boardNum)
{
  if( p_trigger_request[boardNum].size() )
	 return p_trigger[ boardNum ];

  else {
	 cout<<"WARENING : No Trigger recorded for TDC board "<<boardNum<<endl;
	 return -1;
  }
}

int TofTrigger::GetTriggerReq(int boardNum)
{
  if( p_trigger_request[boardNum].size() )
	 return p_trigger_request[ boardNum ][0];

  else {
	 cout<<"WARENING : No Trigger Request recorded for TDC board "<<boardNum<<endl;
	 return -1;
  }
}