//-----------------------------------------------------------------------------
//
//     File :      TofRec.cc
//     Purpose :   contain the digits/hits for reconstruction of a single event
//     Created :   17-DEC-2002  by Steve Kahn
//
//-----------------------------------------------------------------------------

#include "Recon/TOF/TofRec.hh"

using namespace std;

TofRec::TofRec( MICEEvent& e, MICERun* r )
: theEvent( e ), theRun( r ), DBReader( NULL )
{
  //! Create new TofTrigger object.
  theTrigger = new TofTrigger();

  //!  Get the trigger station from datacards and set this in the TofTrigger object.
  int TriggerSt = theRun->DataCards->fetchValueInt( "TofTriggerStation" );
  theTrigger->SetStation( TriggerSt );

  //! Initialize data members.
  Initialize();

  //! Initialize the channel map by using the datacards.
  ch_map.InitializeFromCards( theRun->DataCards );

  //! If we do not run in calibration mode initialize also the calibration map by using datacards.
  if( !CalibrationMode )
	 calib_map.InitializeFromCards( theRun->DataCards, tofStationModules );
}

TofRec::TofRec( MICEEvent& e, MICERun* r, DataBaseReader* dbreader )
: theEvent( e ), theRun( r ), DBReader( dbreader )
{
  //! Create new TofTrigger object. The trigger station is going to be set later in void OnRunBeginReset;
  theTrigger = new TofTrigger();
  Initialize();

  //! Initialize the channel map by using the datacards. This has to be changed when CBD is ready to hold cabling.
  ch_map.InitializeFromCards( theRun->DataCards );

  //! If we do not run in calibration mode initialize also the calibration map by using CDB.
  if( !CalibrationMode )
	 calib_map.MakePmtKeys( tofStationModules );

  //reader->SetTofRecCall( OnRunBeginReset );
}

TofRec::~TofRec()
{
  delete theTrigger;
	//ME - we don't delete objects here, that is handled globally
}

void TofRec::Initialize()
{
  //! Proper value of CurrentRunNum will be set later in void OnRunBeginReset().
  CurrentRunNum = -1;

  //! If we run in calibration mode the reconstruction is simplified and time is not reconstructed.
  if( theRun->DataCards->fetchValueInt( "TofCalibrationMode" ) == 0) CalibrationMode = false;
  else CalibrationMode = true;

  //int tof_rec_level = theRun->DataCards->fetchValueInt( "TofRecLevel" );

  //! Find in the loaded configuration all TOF detectors and all TOF slabs.
  tofStationModules = theRun->miceModule->findModulesByPropertyString( "Detector", "TOF" );
  tofSlabModules = theRun->miceModule->findModulesByPropertyExists( "Hep3Vector", "Pmt1Pos" );

  //! Create structure to hold all necessary information during the reconstruction of the event.
  int nStation = tofStationModules.size();
  int nTracks = nStation -1;
  SpacePoints.resize(nStation);
  TOFTracks.resize(nTracks);
  SlabHits.resize(nStation);
  Digits.resize(nStation);
  for(int st=0;st<nStation;st++)
  {
	 int nPlanes = tofStationModules[st]->propertyInt( "numPlanes" );
	 int StNum = tofStationModules[st]->propertyInt( "Station" );
	 SlabHits[StNum].resize(nPlanes);
	 Digits[StNum].resize(nPlanes);
	 for(int pl=0;pl<tofStationModules[st]->daughters();pl++)
	 {
		if(tofStationModules[st]->daughter(pl)->propertyExistsThis( "Plane", "int" ))
		{
		  int PlNum = tofStationModules[st]->daughter(pl)->propertyInt( "Plane" );
		  int nSlabs = tofStationModules[st]->daughter(pl)->propertyInt( "numSlabs" );
		  Digits[StNum][PlNum].resize(nSlabs);
		  for(int sl=0;sl<tofStationModules[st]->daughter(pl)->daughters();sl++)
		  {
			 int nPmts = tofStationModules[st]->daughter(pl)->daughter(sl)->propertyInt( "numPMTs" );
			 int SlNum = tofStationModules[st]->daughter(pl)->daughter(sl)->propertyInt( "Slab" );
			 Digits[StNum][PlNum][SlNum].resize(nPmts);
		  }
		}
	 }
  }

  //! Set some constants.
  ScintLightSpeed = theRun->DataCards->fetchValueDouble("TOFscintLightSpeed");

  //! If you want to set different value for MakeSpacePiontCut use void TofRec::SetSpacePiontCut(double).
  MakeSpacePiontCut = 500.;

  //! If you want to set different value for this FindTriggerPixelCut use void TofRec::SetTriggerCut(double).
  FindTriggerPixelCut = 500.;


  ResetEfficiency();
  Print();
}

void TofRec::Print()
{
  cout<<dec<<"=========== TofRec ============"<<endl;
  cout<<"CalibrationMode "<<CalibrationMode<<endl;
  cout<<"Trigger from - ";
  if( !DBReader || (DBReader && CurrentRunNum>0) )	 cout<<"TOF"<< theTrigger->GetStation() <<endl;
  else cout<<"to be set" <<endl;

  if(!CalibrationMode)
  {
	 cout<<"MakeSpacePiontCut = "<<MakeSpacePiontCut<<" ps"<<endl;
	 cout<<"FindTriggerPixelCut = "<<FindTriggerPixelCut<<" ps"<<endl;
	 //cout<<"Good = "<<NGoodEvents<<"  NoCalib = "<<NBadEvents_NoCalib<<"  NoTrigger = "<<NBadEvents_NoTrigger<<endl;
	 if( GetEfficiency() ){
		cout<<"Reconstruction Efficiency = "<<GetEfficiency()*100.<<"%"<<endl;
		cout<<"Reconstruction Inefficiency - No Calibration = "<<GetInefficiency(NoCalibration)*100.<<"%"<<endl;
		cout<<"Reconstruction Inefficiency - Unknown Trigger pixel = "<<GetInefficiency(UnknownTrigger)*100.<<"%"<<endl;
	 }
  }
  //cout<<"Cabling from "<<fcable<<endl;
  //cout<<"Calibration from "<<endl<<ft0<<endl<<ftw<<endl<<ftr<<endl;
  cout<<"==============================="<<endl;
}

bool TofRec::Process()
{
  //! Check the data type.
  if( theEvent.tofDigits.size() != 0 )
	 DataType = MCData;
  else if( theEvent.tofDigits.size() == 0 && theEvent.vmefAdcHits.size() != 0 && theEvent.vmeAdcHits.size() == 0 )
	 DataType = MICEData;

  //! Prepare to reconstruct the event.
  OnEventBeginReset();

  if(DataType == MICEData)
  {
	 // This is for the real data.
	 if(DBReader)
	 {
		if(DBReader->GetCurrentRunNum() != CurrentRunNum )
		  OnRunBeginReset();

		if( CurrentRunNum<0 )
		  return false;
	 }

	 //! Read the trigger times.
	 theTrigger->Reset();
	 theTrigger->SetTriggerTimes( &theEvent, &ch_map);
	 //theTrigger->Print();

	 makeDigits();
  }

  if(DataType == MCData)
  {
	 // This is for the MonteCarlo data.
	 sortDigits();
  }

  if(theEvent.tofDigits.size() > 1)
	 makeSlabHits();

   if(theEvent.tofSlabHits.size() > 1)
	 makeSpacePoints();

   if(theEvent.tofSpacePoints.size() > 1)
	 makeTracks();

  return true;
}

void TofRec::OnRunBeginReset()
{
  CurrentRunNum = DBReader->GetCurrentRunNum();
  int TriggerSt = DBReader->GetTrigger( CurrentRunNum );
  if(TriggerSt == -99999)
  {
	 cerr << "*** ERROR in TofRec::OnRunBeginReset() ***"<<endl;
	 cerr << "*** Corrupted comunication with BD server ***"<<endl;
	 cerr << "*** Unable to set the trigger ***"<<endl;
	 cerr << "*** exit ***"<< endl;
	 exit(1);
  }
  cout<<"TofRec::OnRunBeginReset() ";
  cout<<"In Run "<<dec<<CurrentRunNum<<" trigger in TOF"<<TriggerSt<<endl;
  theTrigger->SetStation( TriggerSt );

  if( !CalibrationMode )
	 calib_map.InitializeFromDB( DBReader, CurrentRunNum );
}

void TofRec::OnEventBeginReset()
{
  //! Erase the information from the previous event.
  for(unsigned int st=0;st<SpacePoints.size();st++)
  {
	 SpacePoints[st].resize(0);
	 if( st < (SpacePoints.size() -1) )
		TOFTracks[st].resize(0);
	 for(unsigned int pl=0;pl<Digits[st].size();pl++)
	 {
		SlabHits[st][pl].resize(0);
		for(unsigned int sl=0;sl<Digits[st][pl].size();sl++)
		  for(unsigned int pmt=0;pmt<Digits[st][pl][sl].size();pmt++)
			 Digits[st][pl][sl][pmt].resize(0);
	 }
  }
  GVADigits.resize(0);
  GVASpacePoints.resize(0);
}

void TofRec::sortDigits()
{
  //! In oreder to reconstruct MC data first we have to know how the digits are disributet over the detector.
  //! The equivalent of this function for the real data is TofRec::makeDigits().
  int digits = theEvent.tofDigits.size();
  for(int i=0;i<digits;i++)
  {
	 //theEvent.tofDigits[i]->Print();
	 int station = theEvent.tofDigits[i]->stationNumber();
	 int plane = theEvent.tofDigits[i]->planeNumber();
	 int slab = theEvent.tofDigits[i]->slab();
	 int pmt = theEvent.tofDigits[i]->pmt();
	 Digits[station][plane][slab][pmt].push_back(i);
	 //! Later this information will be used to make TofSlabHits.
  }
}

void TofRec::makeDigits()
{
  //! This method matches a VmefAdcHit to every VmeTdcHit and makes a TofDigit
  int tdcHits = theEvent.vmeTdcHits.size();
  int fadcHits = theEvent.vmefAdcHits.size();
  int DigitNum = 0;
  for(int i=0; i<tdcHits; i++)
  {
	 VmeTdcHit* tdchit = theEvent.vmeTdcHits[i];
	 int hitType = ch_map.WhatIsThis(tdchit);
	 if( hitType == TofChannelMap::TOF )
	 {
		int st, plane, slab, pmt, pmtNum;
		std::string pmtName;
		// find the position of the hit in the channel map
		pmtNum = ch_map.findThePosition(tdchit, st, plane, slab, pmt, pmtName);
		PmtKey pmtKey( st,plane,slab,pmt );
		if(pmtNum == 99 || pmtNum == -99) continue;
		//tdchit->Print();
		for(int k=0; k<fadcHits;k++)
		  if( pmtNum == ch_map.findThePosition(theEvent.vmefAdcHits[k]) )
		  {
			 VmefAdcHit* fadchit = theEvent.vmefAdcHits[k];
			 //fadchit->Print();

			 double triggerTime = theTrigger->GetTriggerReq( tdchit->board());
			 //!Create a TofDigit.
			 TofDigit* tofDigit =
				  new TofDigit(fadchit, tdchit, st, plane, slab, pmt, triggerTime, pmtName);
			 //tofDigit->Print();
			 //! This TofDigit will be good only if triggerTime is found (the value is positive).
			 if( tofDigit->isGood() ){
				theEvent.tofDigits.push_back( tofDigit );

				if( st < (int)Digits.size()
				    || plane < (int)Digits[st].size()
				//  || slab << (int)Digits[st][plane].size()
				  )
				  Digits[st][plane][slab][pmt].push_back( DigitNum );
				  //! Later this information will be used to make TofSlabHits.
				  else
				  {
					 cerr << "*** ERROR in TofRec::makeDigits() ***"<<endl;
					 cerr << "*** Conflict between the configuration file ";
					 cerr << theRun->DataCards->fetchValueString( "MiceModel" ) <<" and the reconstructed data. ***"<<endl;
					 cerr << "*** Counter with signature ";
					 cerr << "(St "<<st<<", Pl "<<plane<<", Slab "<<slab<<") is not presented in the current configuration. ***"<<endl;
					 cerr << "*** exit ***"<< endl;
					 exit(1);
				  }
				DigitNum ++;
			 }
		  }
	 }
	 else if( hitType == TofChannelMap::GVA )
	 {
		//cout<<dec;
		//tdchit->Print();
		TofDigit* tofDigit = new TofDigit(NULL, tdchit, -1,0,0,0, theTrigger->GetTriggerReq( tdchit->board() ), "GVA");
		theEvent.tofDigits.push_back( tofDigit );
		GVADigits.push_back( DigitNum );
		DigitNum ++;
	 }
  }
}

void TofRec::makeSlabHits()
{
  int SlHitNum = 0;
  for( unsigned int i = 0; i < tofSlabModules.size(); ++i )
  {
	 if( tofSlabModules[i]->propertyExistsThis( "numPMTs", "int" ) )
	 {
		int station = tofSlabModules[i]->propertyInt( "Station" );
		int plane = tofSlabModules[i]->propertyInt( "Plane" );
		int slab = tofSlabModules[i]->propertyInt( "Slab" );
		int numPmts = tofSlabModules[i]->propertyInt( "numPMTs" );

		if( numPmts == 2 )
		{
		  if( Digits[station][plane][slab][0].size() && Digits[station][plane][slab][1].size())
		  {
			 int nDigit0 = Digits[station][plane][slab][0][0];
			 int nDigit1 = Digits[station][plane][slab][1][0];
			 TofSlabHit* hit = new TofSlabHit(tofSlabModules[i], theEvent.tofDigits[nDigit0], theEvent.tofDigits[ nDigit1 ] );

			 if( hit->isGood() )
			 {
				hit->digit1()->setSlabHit( hit );
				hit->digit2()->setSlabHit( hit );
				theEvent.tofSlabHits.push_back( hit );
				SlabHits[station][plane].push_back( SlHitNum );
				SlHitNum ++;
			 } else delete hit;
		  }
		}
	 }
  }
}

void TofRec::makeSpacePoints()
{
  int SpPointNum = 0;
  TriggerKey* TrKey;
  if( DataType == MICEData ){
	 TrKey = theTrigger->ProcessTriggerKey(&theEvent, &calib_map, SlabHits[ theTrigger->GetStation() ], FindTriggerPixelCut, CalibrationMode);
	 //theTrigger->Print();
	 //cout<<GetSlabHits( theTrigger->GetStation() ).size()<<endl;
	 if(!TrKey)
	 {
		if(SlabHits[ theTrigger->GetStation() ][0].size()==1 &&
		   SlabHits[ theTrigger->GetStation() ][1].size()==1)
		  AddBadEvent( NoCalibration );
		else  AddBadEvent( UnknownTrigger );
	 }
  }

  if( DataType == MCData || DataType == KEKData || (DataType == MICEData && TrKey) )
  {
	 //cout<<*TrKey<<endl;
	 for( unsigned int m = 0; m < tofStationModules.size(); ++m )
	 if( tofStationModules[m]->propertyExistsThis( "Station", "int" ) )
	 {
		int station = tofStationModules[m]->propertyInt( "Station" );
		int numPlanes = tofStationModules[m]->propertyInt( "numPlanes" );

		if( numPlanes == 2 )
		{
		  //! Get number of slab hits in  plane 0.
		  int nx = SlabHits[station][0].size();

		  //! Get number of slab hits in  plane 1.
		  int ny = SlabHits[station][1].size();

		  //! Loop over all possible combinations.
		  for(int x=0;x<nx;x++)
		  for(int y=0;y<ny;y++)
		  {
			 TofSlabHit *slA = theEvent.tofSlabHits[ SlabHits[station][0][x] ];
			 TofSlabHit *slB = theEvent.tofSlabHits[ SlabHits[station][1][y] ];

			 //! If we run using real data.
			 if( DataType == MICEData && !CalibrationMode)
			 {
				//! Try to calibrate the value of time in the slab hits.
				slA->CalibrateTime(&calib_map,TrKey);
				slB->CalibrateTime(&calib_map,TrKey);
				//! If the calibration is successful TofSlabHit::m_calibrated is set to be true.
			 }

			 //! If we run using Monte Carlo data.
			 if( DataType == MCData )
			 {
				//! Calibration is always successful in this case.
				slA->CalibrateTime(ScintLightSpeed);
				slB->CalibrateTime(ScintLightSpeed);
			 }
          //slA->Print();slB->Print();
			 //! Create Space Point.
			 TofSpacePoint* point = new TofSpacePoint(tofStationModules[m], slA, slB);
			 //point->Print();
			 //! This Space Point will be good only if both slab hits are good end calibrated.
			 //! If we do not run in calibration mode:
			 //! Check the difference of the time measured in the slab hits. This difference should not exceed the cut.
			 //! If we run in calibration mode: just go ahead.
			 if( CalibrationMode || ( point->isGood() && ( fabs(point->dt()) < MakeSpacePiontCut) ) )
			 {
				//! If everything is OK store the space point.
				slA->SetSpacePoint( point );
				slB->SetSpacePoint( point );
				theEvent.tofSpacePoints.push_back( point );
				SpacePoints[station].push_back( SpPointNum );
				SpPointNum ++;
			 }
			 else delete point;
		  }
		}
	 }
	 if( SpPointNum ) AddGoodEvent();
	 else AddBadEvent( UnknownTrigger );
  }
}

void TofRec::makeTracks() {
	if(true) {
		// Maps one space point to each TOF station number for which a space point has been found in the event so far
		map <int,TofSpacePoint*> tsp;

		// To deal with corner clippers...
		// Summed number of photoelectrons (reconstructed from ADC hits to make a double) from either the four PMTs in a 2-orthogonal-plane TOF, or the two PMTs in a single plane TOF, for the TofSpacePoint mapped by tsp. Mapped to TOF station numbers.
		map <int,double> peSum;

		// Sum over space points in the event
		vector <TofSpacePoint*> ::iterator tspIt;
		for( tspIt = theEvent.tofSpacePoints.begin(); tspIt != theEvent.tofSpacePoints.end(); ++tspIt )
		{
		int statNum = (*tspIt)->station();
			double peSumTmp = addUpPe( *tspIt );

		// If this is the first space point found at this TOF station, add it to the map
			if ( peSum.find( statNum ) == peSum.end() )
		{
			peSum[ statNum ] = peSumTmp;
			tsp[ statNum ] = *tspIt;
		}

		// If this isn't the first, but it has more photoelectrons replace the previous incumbent with this.
			else {
				if ( peSumTmp > peSum[ statNum ] ) {
					peSum[ statNum ] = peSumTmp;
					tsp[ statNum ] = *tspIt;
				}
			}
		}

	        // Make TofTrack's in all permutations of space points from different stations and push them back to the MICEEvent.
		if (tsp.find(0) != tsp.end() && tsp.find(1) != tsp.end())
			theEvent.tofTracks.push_back(new TofTrack(tsp[0], tsp[1]));
		if (tsp.find(1) != tsp.end() && tsp.find(2) != tsp.end())
			theEvent.tofTracks.push_back(new TofTrack(tsp[1], tsp[2]));
		if (tsp.find(0) != tsp.end() && tsp.find(2) != tsp.end())
			theEvent.tofTracks.push_back(new TofTrack(tsp[0], tsp[2]));

		// Criticisms - Mark Rayner
		// * Doesn't deal with the scenario that, say, a muon decays before TOF2, but a stray electron makes a space point and a TofTrack is created.
	}

	if(false) {
		int TrackNum = 0;
		if( (DataType == MICEData || DataType == MCData) && !CalibrationMode) {
			//for(unsigned int t=0; t < (SpacePoints.size() - 1); t++)
			for(unsigned int t=0; t < SpacePoints.size(); t++)
			for(unsigned int i=0;i<SpacePoints[t].size();i++)
			for(unsigned int j=0;j<SpacePoints[t+1].size();j++) {
				int first_sp = SpacePoints[t][i];
				int second_sp = SpacePoints[t+1][j];
				TofTrack *tr = new TofTrack( theEvent.tofSpacePoints[first_sp], theEvent.tofSpacePoints[second_sp]);
				theEvent.tofTracks.push_back( tr );
				TOFTracks[t].push_back( TrackNum );
				TrackNum++;
			}

		}
	}


}

// Used to determine which TofSpacePoint to use in a TofTrack in the case of multiple space points in the same TOF
double TofRec::addUpPe( TofSpacePoint* tsp ) {
	double s = 0;
	if ( tsp->hitA() ) s += tsp->hitA()->pe();
	if ( tsp->hitB() ) s += tsp->hitB()->pe();
	return s;
}

vector<int> TofRec::GetSlabHits(int Station) const
{
  vector<int> slhits = SlabHits[Station][0];
  for(unsigned int i=0;i<SlabHits[Station][1].size();i++)
	 slhits.push_back( SlabHits[Station][1][i] );

  return slhits;
}

void TofRec::AddGoodEvent()
{
  //cout<<"AddGooDdEvent"<<endl;
  NGoodEvents++;
}

void TofRec::AddBadEvent(int flag)
{
  //cout<<"AddBadEvent  "<<flag<<endl;
  if( flag == NoCalibration )NBadEvents_NoCalib++;
  if( flag == UnknownTrigger )NBadEvents_NoTrigger++;
}

double TofRec::GetEfficiency()
{
  double allEvents = NGoodEvents + NBadEvents_NoCalib + NBadEvents_NoTrigger;
  if( DataType == MICEData && allEvents ) return NGoodEvents/allEvents;
  else return 0.;
}

double TofRec::GetInefficiency(int flag)
{
  double allEvents = NGoodEvents + NBadEvents_NoCalib + NBadEvents_NoTrigger;
  if( DataType == MICEData &&  flag == NoCalibration && allEvents )
	 return NBadEvents_NoCalib/allEvents;

  if( DataType == MICEData && flag ==  UnknownTrigger && allEvents )
	 return NBadEvents_NoTrigger/allEvents;

  return 0.;
}

void TofRec::ResetEfficiency()
{
  NGoodEvents = 0;
  NBadEvents_NoTrigger = 0;
  NBadEvents_NoCalib = 0;
}

