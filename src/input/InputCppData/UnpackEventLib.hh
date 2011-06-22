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

#ifndef _MAUS_INPUTCPPDATA_UNPACKEVENTLIB_HH_
#define _MAUS_INPUTCPPDATA_UNPACKEVENTLIB_HH_

#include <stdint.h>

#include "json/json.h"

/** \class V830Hit
  * 
  *
  * This class unpacks a V830 (scalar) board hit.
  * This is based on the original MICE Data Module.
  *
 */
class V830Hit {
public
:
/** Unpack a single event part to JSON.
  *
  * This function unpacks a single event part,
  * (of the right type), into a JSON sub-tree.
  *
  * \param[in,out] pPartEvntPtr The event part to process.
  * \param[in,out] pDoc The JSON node to put the data under.
  */
  static void getJSON(void *pPartEvntPtr,
                      Json::Value &pDoc);
};

/** \class V1290Hit
  * 
  *
  * This class unpacks a V1290 (TDC) board hit.
  * This is based on the original MICE Data Module.
  *
 */
class V1290Hit {
public
:
/** Unpack a single event part to JSON.
  *
  * This function unpacks a single event part,
  * (of the right type), into a JSON sub-tree.
  *
  * \param[in,out] pPartEvntPtr The event part to process.
  * \param[in,out] pDoc The JSON node to put the data under.
  */
  static void getJSON(void *pPartEvntPtr,
                      Json::Value &pDoc);
};

/** \class V1724Hit
  * 
  *
  * This class unpacks a V1724 (ADC) board hit.
  * This is based on the original MICE Data Module.
  *
 */
class V1724Hit {
private
:
/** Convert a channel mask to a channel count.
 *
 * This function takes a channel mask and works out
 * how many channels are enabled.
 * Copied from the MICE Data module.
 *
 * \param[in] pMask The mask to process.
 * \return The number of channels enabled in the mask.
 */
  static int getNumberOfChannels(uint32_t pMask);

public
:
/** Unpack a single event part to JSON.
  *
  * This function unpacks a single event part,
  * (of the right type), into a JSON sub-tree.
  *
  * \param[in,out] pPartEvntPtr The event part to process.
  * \param[in,out] pDoc The JSON node to put the data under.
  */
  static void getJSON(void *pPartEvntPtr,
                      Json::Value &pDoc);
};

/** \class V1731Hit
  * 
  *
  * This class unpacks a V1731 (ADC) board hit.
  * This is based on the original MICE Data Module.
  *
 */
class V1731Hit {
public
:
/** Unpack a single event part to JSON.
  *
  * This function unpacks a single event part,
  * (of the right type), into a JSON sub-tree.
  *
  * \param[in,out] pPartEvntPtr The event part to process.
  * \param[in,out] pDoc The JSON node to put the data under.
  */
  static void getJSON(void *pPartEvntPtr,
                      Json::Value &pDoc);
};

#endif  // _MAUS_INPUTCPPDATA_UNPACKEVENTLIB_H__
