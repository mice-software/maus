
#ifndef TOFCHANNELMAP
#define TOFCHANNELMAP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Interface/VmeTdcHit.hh"
#include "Interface/VmefAdcHit.hh"
#include "Interface/dataCards.hh"

using namespace std;


//! TofChannelMap class holds the cabeling information for TOF detectors and provides identification of the VmeTdcHits and VmefAdcHits.
class TofChannelMap
{
 public :
	//! Default constructor. The object is not ready to be used. Apply some of the methods for initialization first.
   TofChannelMap();

	//! Constructor. The map is initialized by using the provided text file.
   TofChannelMap(std::string);

   //~TofChannelMap();

	//! Initializes the map by using the provided text file.
   void Initialize(std::string file);

	//! Initializes the map by using the text file specified in the cards.
	void InitializeFromCards(dataCards* cards);

   //! Dumps some information about this digit to the standard output
   void Print();

	//! Returns true if the TDC channel belongs to the TOF detector.
   bool isThisTofTdc(VmeTdcHit*);

   //! Returns true if the fADC channel belongs to the TOF detector.
   bool isThisTofFadc(VmefAdcHit*);

	enum{
	 //! this channel do not belong to the TOF detector.
	 UNKNOWN = 0,

	 //! this is a trigger channel.
	 TRIGGER = 1,

	 //! this is a trigger request channel.
	 TRIGGER_R = 2,

	 //! this is a TOF channel.
	 TOF = 3,

	 //! this is a GVA channel.
	 GVA = 4
	};

	//! Returns integer flag. Can be UNKNOWN, TRIGGER, TRIGGER_R, TOF or GVA.
	int WhatIsThis(VmeTdcHit*);

	//! Finds the position of the TDC hit channel in the map.
   int findThePosition(VmeTdcHit*);

	//! Finds the position of the TDC hit channel in the map. The provided variables are set according to the coordinates of the channel in the detector space.
   int findThePosition(VmeTdcHit* theHit, int& station, int& plane, int& slab, int& pmt, std::string& name);

	//! Finds the position of the fADC hit channel in the map.
   int findThePosition(VmefAdcHit*);

	//! Finds the position of the fADC hit channel in the map. The provided variables are set according to the coordinates of the channel in the detector space.
   int findThePosition(VmefAdcHit* theHit, int& station, int& plane, int& slab, int& pmt, std::string& name);

	//! Finds the position of the PMT in the map.
   int findByName(std::string);

	//! Returns the number of channels in this map.
   int GetNumOfChannels() const { return numchans; };


   std::string GetName(int i)   { return pmtName[i]; };

 private :
	//! The number of channels in this map.
   int numchans;


	//! TDC coordinates - board.
   std::vector<int> tdc_boards;

	//! TDC coordinates - crate.
   std::vector<int> tdc_crates;

	//! TDC coordinates - channel.
   std::vector<int> tdc_channels;

	//! fADC coordinates - board.
   std::vector<int> fadc_boards;

	//! fADC coordinates -crate.
   std::vector<int> fadc_crates;

	//! fADC coordinates - channel.
   std::vector<int> fadc_channels;

	//! Detector coordinates - station.
   std::vector<int> station;

	//! Detector coordinates - plane.
   std::vector<int> plane;

	//! Detector coordinates - slab.
   std::vector<int> slab;

	//! Detector coordinates - PMT.
   std::vector<int> pmt;

	//! Detector coordinates - PMT number.
   std::vector<int> pmtNum;

	//! Detector coordinates - PMT name.
   std::vector<std::string> pmtName;
};

#endif

