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

#include "UnpackEventLib.hpp"

#include <stdint.h>

#include <string>
#include <vector>
#include <sstream>

#include "MDpartEventV830.h"
#include "MDpartEventV1290.h"
#include "MDpartEventV1724.h"
#include "MDpartEventV1731.h"

// do not increment inside following macro! (e.g.: MAX( ++a, ++b );
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void V830Hit::getJSON(void *pPartEvntPtr,
                      Json::Value &pDoc) {
  // Create the event object
  MDpartEventV830 xV830Evnt;
  xV830Evnt.SetDataPtr(pPartEvntPtr);

  // Put static data into the Json
  pDoc["geo"] = Json::Value(xV830Evnt.GetGeo());

  pDoc["channels"] = Json::Value();

  unsigned int xChannels = xV830Evnt.GetWordCount();
  // Last channel is "channel -1", so don't process it!
  for (unsigned int i = 0; i < (xChannels-1); i++) {
    int xChan = xV830Evnt.GetChannel(i);
    int xValue = xV830Evnt.GetMeasurement(i);

    // Convert the channel number into a channel number string!
    std::stringstream xConv;
    xConv << xChan;

    pDoc["channels"][xConv.str()] = Json::Value(xValue);
  }
}

void V1290Hit::getJSON(void *pPartEvntPtr,
                       Json::Value &pDoc) {
  // Create the event object
  MDpartEventV1290 xV1290Evnt;
  xV1290Evnt.SetDataPtr(pPartEvntPtr);

  if (xV1290Evnt.IsValid()) {
    // Put static data into the Json
    pDoc["ttt"] = xV1290Evnt.GetTriggerTimeTag();
    pDoc["geo"] = xV1290Evnt.GetGeo();

    // Loop over all the channels
    for (unsigned int i = 0; i < V1290_NCHANNELS; i++) {
      int xHitsL = xV1290Evnt.GetNHits(i, 'l');
      int xHitsT = xV1290Evnt.GetNHits(i, 't');
      int xMaxHits = MAX(xHitsL, xHitsT);

      // Loop over each possible hit
      int xLT, xTT;  // Lead and Trail times?
      for (unsigned int j = 0; j < xMaxHits; j++) {
        if (j < xHitsL)
          xLT = xV1290Evnt.GetHitMeasurement(j,  // Hit ID
                                             i,  // Channel ID
                                             'l');
        else
          xLT = -99;

        if (j < xHitsT) xTT = xV1290Evnt.GetHitMeasurement(j,  // Hit ID
                                                           i,  // Channel ID
                                                           't');
        else
          xTT = -99;
        // Why Channel ID / 8?, I don't know.
        int xBunchID = xV1290Evnt.GetBunchID(i / 8);

        Json::Value xTdcHit;
        xTdcHit["bid"] = xBunchID;
        xTdcHit["channel"] = i;
        xTdcHit["lt"] = xLT;
        xTdcHit["tt"] = xTT;

        pDoc["hits"].append(xTdcHit);
      }
    }
  } else {
    pDoc = Json::Value();
  }
}

void V1724Hit::getJSON(void *pPartEvntPtr,
                       Json::Value &pDoc) {
  // Create the event object
  MDpartEventV1724 xV1724Evnt;
  xV1724Evnt.SetDataPtr(pPartEvntPtr);

  if (xV1724Evnt.IsValid()) {
    // Put static data into the Json
    pDoc["ttt"] = xV1724Evnt.GetTriggerTimeTag();
    pDoc["geo"] = xV1724Evnt.GetGeo();

    // Now work out various parameters
    //// Number of channels
    unsigned int xChannels =
        V1724Hit::getNumberOfChannels(xV1724Evnt.GetChannelMask());
    //// Word count
    int xWordCount = xV1724Evnt.GetWordCount();
    //// Samples per channel
    //// Magic code from V1724DATEFileOut.cc
    int xSamplesPerChan = ((xWordCount * 2) / xChannels) - 1;

    // Actually process the events
    for (unsigned int i = 0; i < V1724_NCHANNELS; i++) {
      std::vector<int> xData;
      for (unsigned int j = 0; j < xSamplesPerChan; j++) {
        int xSample = xV1724Evnt.GetSampleData(i,  // Channel ID
                                               j);  // Sample ID
        xData.push_back(xSample);
      }
      // Convert the channel number to a string
      std::stringstream xConv;
      xConv << i;

      std::vector<int>::iterator xIt;
      for (xIt = xData.begin(); xIt < xData.end(); xIt++) {
        Json::Value xValue(*xIt);
        pDoc["hits"][xConv.str()].append(xValue);
      }
    }
  } else {
    pDoc = Json::Value();
  }
}

int V1724Hit::getNumberOfChannels(uint32_t pMask) {
  unsigned int n = pMask;
  n = (n & 0x55555555) + ((n & 0xAAAAAAAA) >> 1);
  n = (n & 0x33333333) + ((n & 0xCCCCCCCC) >> 2);
  n = (n + (n >> 4)) & 0x0F0F0F0F;
  n = (n + (n >> 16));
  n = (n + (n >> 8)) & 0x3F;
  return static_cast<int>(n);
}

