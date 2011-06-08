//-----------------------------------------------------------------------------
//
//     File :      TofRec.cc
//     Purpose :   contain the digits/hits for reconstruction of a single event
//     Created :   17-DEC-2002  by Steve Kahn
//
//-----------------------------------------------------------------------------

#include "TofRec.hh"

using namespace std;

TofRec::TofRec( MICEEvent& e ) : theEvent( e ), theRun( NULL )
{}

TofRec::TofRec( MICEEvent& e, MICERun* r ) : theEvent( e ), theRun( r )
{
  tofStationModules = theRun->miceModule->findModulesByPropertyString( "Detector", "TOF" );
  tofSlabModules = theRun->miceModule->findModulesByPropertyExists( "Hep3Vector", "Pmt1Pos" );

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

  char* MICEFILES = getenv( "MICEFILES" );
  string calibfile = theRun->DataCards->fetchValueString( "TofCalib" );
  string cablingfile = theRun->DataCards->fetchValueString( "TofCable" );

  if( MyDataCards.fetchValueInt( "TofCalibrationMode" ) == 0) CalibrationMode = false;
  else CalibrationMode = true;

  string fcable  = string( MICEFILES ) + "/Cabling/TOF/" + cablingfile;
  ch_map.Initialize(fcable);
  //ch_map.Print();
  string ft0, ftw, ftr;
  ftw = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "TW.txt";

  int TriggerSt = MyDataCards.fetchValueInt( "TofTriggerStation" );
  if(TriggerSt==0)
  {
	 ft0 = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "T0_trTOF0.txt";
	 ftr = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "Trigger_trTOF0.txt";
  }
  if(TriggerSt==1)
  {
	 ft0 = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "T0_trTOF1.txt";
	 ftr = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "Trigger_trTOF1.txt";
  }

  theTrigger = new TofTrigger();
  if( !CalibrationMode )
  {
	 calib_map.Initialize(ft0, ftw, ftr);
	 if( TriggerSt == calib_map.GetTriggerStation() )
	 {
	 theTrigger->SetStation( TriggerSt );
	 }else
	 {
		cerr << "*** ERROR in TofRec::TofRec ***"<<endl;
		cerr <<"Trigger in TOF"<< TriggerSt <<endl;
		cerr <<"Wrong trigger delays file" << ftr << endl;
		exit(1);
	 }
  } else theTrigger->SetStation( TriggerSt );

  MakeSpacePiontCut = 500.;
  FindTriggerPixelCut = 500.;
  ResetEfficiency();
  Print();
}

TofRec::~TofRec()
{
  delete theTrigger;
	//ME - we don't delete objects here, that is handled globally
}

void TofRec::Print()
{
  cout<<dec<<"=========== TofRec ============"<<endl;
  cout<<"CalibrationMode "<<CalibrationMode<<endl;
  cout<<"Trigger from TOF"<< theTrigger->GetStation() <<endl;
  if(!CalibrationMode)
  {
	 cout<<"MakeSpacePiontCut = "<<MakeSpacePiontCut<<" ps"<<endl;
	 cout<<"FindTriggerPixelCut = "<<FindTriggerPixelCut<<" ps"<<endl;
	 //cout<<"Good = "<<NGoodEvents<<"  NoCalib = "<<NBadEvents_NoCalib<<"  NoTrigger = "<<NBadEvents_NoTrigger<<endl;
	 cout<<"Reconstruction Efficiency = "<<GetEfficiency()*100.<<"%"<<endl;
	 cout<<"Reconstruction Inefficiency - No Calibration = "<<GetInefficiency(NoCalibration)*100.<<"%"<<endl;
	 cout<<"Reconstruction Inefficiency - Unknown Trigger pixel = "<<GetInefficiency(UnknownTrigger)*100.<<"%"<<endl;
  }
  //cout<<"Cabling from "<<fcable<<endl;
  //cout<<"Calibration from "<<endl<<ft0<<endl<<ftw<<endl<<ftr<<endl;
  cout<<"==============================="<<endl;
}

bool TofRec::Process()
{
  //int tof_rec_level = theRun->DataCards->fetchValueInt( "TofRecLevel" );

  if( theEvent.tofDigits.size() != 0 )
	 DataType = MCData;
  else if( theEvent.tofDigits.size() == 0 && theEvent.vmeAdcHits.size() != 0 && theEvent.vmefAdcHits.size() == 0)
	 DataType = KEKData;
  else if( theEvent.tofDigits.size() == 0 && theEvent.vmefAdcHits.size() != 0 && theEvent.vmeAdcHits.size() == 0 )
	 DataType = MICEData;

  Reset();

  if(DataType == KEKData)
  {
	 TofCable* cable = theRun->tofCable;
	 TofCalib* calib = theRun->tofCalib;
	 if( calib && cable ) makeDigits( calib, cable );
  }

  if(DataType == MICEData)
  {
	 theTrigger->Reset();
	 theTrigger->SetTriggerTimes( theEvent, &ch_map);
	 makeDigits();
  }

  if(DataType == MCData)
  {
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

void TofRec::Reset()
{
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
}

void TofRec::sortDigits()
{
  int digits = theEvent.tofDigits.size();
  for(int i=0;i<digits;i++)
  {
	 //theEvent.tofDigits[i]->Print();
	 int station = theEvent.tofDigits[i]->stationNumber();
	 int plane = theEvent.tofDigits[i]->planeNumber();
	 int slab = theEvent.tofDigits[i]->slab();
	 int pmt = theEvent.tofDigits[i]->pmt();
	 Digits[station][plane][slab][pmt].push_back(i);
  }
}

//This method matches a VmefAdcHit to every VmeTdcHit and makes a TofDigit
void TofRec::makeDigits()
{
  int tdcHits = theEvent.vmeTdcHits.size();
  int fadcHits = theEvent.vmefAdcHits.size();
  int DigitNum = 0;
  for(int i=0; i<tdcHits; i++)
  {
	 VmeTdcHit* tdchit = theEvent.vmeTdcHits[i];
	 if( ch_map.isThisTofTdc(tdchit) )
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
			 //correct the tdc according the trigger time and create a TofDigit
			 TofDigit* tofDigit =
				  new TofDigit(fadchit,tdchit,st,plane,slab,pmt,theTrigger->GetTriggerReq( tdchit->board() ),pmtName);
			 //tofDigit->Print();
			 theEvent.tofDigits.push_back( tofDigit );
			 if( st < (int)Digits.size()
				  || plane < (int)Digits[st].size()
				//  || slab << (int)Digits[st][plane].size()
				)
				Digits[st][plane][slab][pmt].push_back( DigitNum );
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
}

//This method matches a VmeAdcHit to every VmeTdcHit and makes a TofDigit
void TofRec::makeDigits(TofCalib* m_calib, TofCable* m_cable) {
   int DigitNum = 0;
	//Loop over each VME TDC hit
	for( unsigned int tdc = 0; tdc < theEvent.vmeTdcHits.size(); ++tdc ) {

	    	// loop over TofCable and see if we find this channel in the list
		VmeTdcHit* tdchit = theEvent.vmeTdcHits[tdc];
		bool found   = false;
		int TFLN     = -1; // Text File Line Number = (line of cabling text file with this PMT's TDC cable in it) - 1
		int stat     = -1;
		int plane    = -1;
		int pmt      = -1;
		int slab     = -1;
		//Loop over each cable (look at 1st line of cabling text file to see how many)
		for( int i = 0; ! found && i < m_cable->variables(); ++i ) {

			//If you're a TDC cable with the correct module number and channel...
			if ( m_cable->modType     ( i ) == 1 //i.e. a TDC module
			  && m_cable->crateNumber ( i ) == tdchit->crate()
			  && m_cable->moduleNumber( i ) == tdchit->board()
			  && m_cable->channel     ( i ) == tdchit->channel()
			) {
				TFLN = i;
				found    = true;
				stat     = m_cable->stationNumber( TFLN );
				plane    = m_cable->planeNumber  ( TFLN );
				pmt      = m_cable->pmt          ( TFLN );
				slab     = m_cable->slab         ( TFLN );
			}

			if( found ) {
				//This VmeTdcHit is on a known TDC channel, so find the matching VmeAdcHit
				// `hit' is a Vme...A...dcHit* --- match it to it's previously found Vme...T...dcHit*
				VmeAdcHit * hit = NULL;

				//Loop over VME ADC hits and see if one matches
				for( unsigned int adc = 0; adc < theEvent.vmeAdcHits.size(); ++adc )
					for( int i = 0; i < m_cable->variables(); ++i )
						if( m_cable->modType      ( i ) == 2 //i.e. an ADC module
						 && m_cable->slab         ( i ) == slab
						 && m_cable->pmt          ( i ) == pmt
						 && m_cable->stationNumber( i ) == stat
						 && m_cable->planeNumber  ( i ) == plane
						 && m_cable->crateNumber  ( i ) == theEvent.vmeAdcHits[adc]->crate()
						 && m_cable->moduleNumber ( i ) == theEvent.vmeAdcHits[adc]->board()
						 && m_cable->channel      ( i ) == theEvent.vmeAdcHits[adc]->channel()
						)
							hit = theEvent.vmeAdcHits[adc];

				//We have a matching VmeAdc and Tdc hit on a TOF PMT
				if( hit ) {
					double ped = m_calib->ped( stat, plane, slab, pmt );
					double gain = m_calib->gain( stat, plane, slab, pmt );
					double t2t = m_calib->tdc2time( stat, plane, slab, pmt );
					std::vector<double> tw = m_calib->timeWalk( stat, plane, slab, pmt );
					double adcped = (double)hit->adc() - ped;
					if( adcped > 100. && adcped < 3500 && tdchit->tdc() * t2t < 500. ) {
						TofDigit* digit = new TofDigit( hit, tdchit, stat, plane, pmt, slab, ped, gain, t2t, tw );
						theEvent.tofDigits.push_back( digit );
			         Digits[stat][plane][slab][pmt].push_back( DigitNum );
			         DigitNum ++;
					}
				}

				//No ADC hit was found to match the TDC hit, so make a TofDigit with just the TDC hit
				else {
					double ped = m_calib->ped( stat, plane, slab, pmt );
					double gain = m_calib->gain( stat, plane, slab, pmt );
					double t2t = m_calib->tdc2time( stat, plane, slab, pmt );
					std::vector<double> tw = m_calib->timeWalk( stat, plane, slab, pmt );
					TofDigit* digit = new TofDigit( tdchit, stat, plane, pmt, slab, ped, gain, t2t, tw );
					theEvent.tofDigits.push_back( digit );
			      Digits[stat][plane][slab][pmt].push_back( DigitNum );
			      DigitNum ++;
				}
			}
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
			 TofSlabHit* hit = new TofSlabHit(theRun, tofSlabModules[i], theEvent.tofDigits[nDigit0], theEvent.tofDigits[ nDigit1 ] );

			 if( hit->isGood() )
			 {
				hit->digit1()->setSlabHit( hit );
				hit->digit2()->setSlabHit( hit );
				theEvent.tofSlabHits.push_back( hit );
				SlabHits[station][plane].push_back( SlHitNum );
				SlHitNum ++;
			 } else delete hit;
		  }
		}  else if( numPmts == 1 )
		{
		  if( Digits[station][plane][slab][0].size() )
		  {
			 int nDigit0 = Digits[station][plane][slab][0][0];
			 TofSlabHit* hit = new TofSlabHit(theRun, tofSlabModules[i], theEvent.tofDigits[nDigit0] );

			 if( hit->isGood() )
			 {
				hit->digit1()->setSlabHit( hit );
				theEvent.tofSlabHits.push_back( hit );
			 }
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
	 TrKey = theTrigger->ProcessTriggerKey(theEvent, &calib_map, SlabHits[ theTrigger->GetStation() ], FindTriggerPixelCut, CalibrationMode);
	 //if( theTrigger->GetNumberOfRequests() > 1 )theTrigger->Print();
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

		if( numPlanes == 1 )
		{
		  int nx = SlabHits[station][0].size();
		  for(int x=0;x<nx;x++)
		  {
			 TofSlabHit *slA = theEvent.tofSlabHits[ SlabHits[station][0][x] ];

			 if( DataType == MICEData )
				if( !slA->CalibrateTime(&calib_map,TrKey) ) {slA->Print(); continue;}

			 if( DataType == MCData )
				if( !slA->CalibrateTime() ) {slA->Print(); continue;}

			 TofSpacePoint* point = new TofSpacePoint(tofStationModules[m], slA);
			 if( point->isGood() )
			 {
				theEvent.tofSpacePoints.push_back( point );
				SpacePoints[station].push_back( SpPointNum );
				SpPointNum ++;
			 }
		  }
		}
		else if( numPlanes == 2 )
		{
		  int nx = SlabHits[station][0].size();
		  int ny = SlabHits[station][1].size();
		  for(int x=0;x<nx;x++)
		  for(int y=0;y<ny;y++)
		  {
			 TofSlabHit *slA = theEvent.tofSlabHits[ SlabHits[station][0][x] ];
			 TofSlabHit *slB = theEvent.tofSlabHits[ SlabHits[station][1][y] ];

			 if( DataType == MICEData && !CalibrationMode)
			 {
				if( !slA->CalibrateTime(&calib_map,TrKey) ) {
				  //AddBadEvent( NoCalibration );
				  continue;
				}
				if( !slB->CalibrateTime(&calib_map,TrKey) ){
				  //AddBadEvent( NoCalibration );
				  continue;
				}
				if( fabs(slA->time() - slB->time()) > MakeSpacePiontCut ){
				   //AddBadEvent( UnknownTrigger );
					continue;
				}
			 }

			 if( DataType == MCData )
			 {
				if( !slA->CalibrateTime() ) continue;
				if( !slB->CalibrateTime() ) continue;
			 }

			 TofSpacePoint* point = new TofSpacePoint(tofStationModules[m], slA, slB);
			 if( point->isGood() )
			 {
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

vector<int> TofRec::GetSlabHits(int Station)
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

