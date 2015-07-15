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


/** @class MapCppCkovDigits
 *  Reconstruct events by running over Ckov real data.
 *
 */

#ifndef _MAP_MAPCPPCKOVDIGITS_H_
#define _MAP_MAPCPPCKOVDIGITS_H_
// C headers
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <numeric>
#include <algorithm>
#include "API/MapBase.hh"

namespace MAUS {

typedef struct fCkovAdc {
  int ckovindex; /* ckova or ckovb */
  int partnum; /* event number */
  int arTime[4]; /* arrival time */
  int position[4]; /* position */
  int posmin[4]; /* mininum position */
  int pulse[4]; /* pulse area */
  int totpulse[4]; /* integrated pulse over threshold */
  double pes[4]; /* number of photoelectrons */
  int ncoin; /* number of coincidences */
  int totcharge; /* total charge */
  double totnpe; /* total number of photoelectrons */
} CkovAdcHits;

typedef std::vector<CkovAdcHits> CkovTmpDigs; /* 2 counters */

class MapCppCkovDigits : public MapBase<MAUS::Data> {

 public:
  MapCppCkovDigits();

 private:
 /** @brief Sets up the worker
  *  @param argJsonConfigDocument a JSON document with
  *         the configuration.
  */
  void _birth(const std::string& argJsonConfigDocument);

 /** @brief Shutdowns the worker
  *  This takes no arguments and does nothing
  */
  void _death();

 /**  @brief Receive raw data pointer and set digits
 */
  void _process(MAUS::Data* data) const;

 private:
  /* these parameters should be from CDB. for now they are set at birth */
  int _position_threshold; /* threshold for finding coincidences */
  int _pulse_area_threshold; /* threshold for photoelectron counting */
  int _window_min; /* pulse integration window min */
  int _window_max; /* pulse integration window max */
  int _maxCoincidences; /* maximum number of pulse coincidences/peaks */

  /** @brief temporary storage for ckov digits accumulated from flash ADCs and peakfinding
   */
  CkovTmpDigs getAdc(V1731Array &adcHits) const;

  /** @brief find peaks and set coincidences, total npe, total charge
   */
  void getSignal(CkovAdcHits* ckdigs) const;

  /** @brief Set the digits for the event for CkovA and B 
   */
  void setDigits(CkovTmpDigs &tmpDigs, CkovDigitArray* theDigits) const;
};
}
#endif

