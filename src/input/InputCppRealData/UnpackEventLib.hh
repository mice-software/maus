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

#ifndef _MAUS_INPUTCPPREALDATA_UNPACKEVENTLIB_HH_
#define _MAUS_INPUTCPPREALDATA_UNPACKEVENTLIB_HH_

#include <stdint.h>
#include <typeinfo>
#include <string>
#include <vector>
#include <sstream>

#include "json/json.h"

#include "MDprocessor.h"
#include "MDpartEventV1290.h"
#include "MDpartEventV1724.h"

#include "MDpartEventV1731.h"
#include "MDfragmentVLSB_C.h"
#include "MDfragmentV830.h"
#include "MDfragmentDBB.h"
#include "MDequipMap.h"

#include "RawDataProcessing.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////

/** MDarranger
 * Simple class to hold and arrange the Json document
 * containing all the daq spill information.
 */
typedef map<string,Json::Value> JsonDocMap;

class MDarranger
{
 public:
  MDarranger(){}
  virtual ~MDarranger(){}

  void set_JSON_doc(Json::Value *doc) {_docSpill=doc;}
	Json::Value* get_JSON_doc() {return _docSpill;}
 /**
  * This function sets the DAQ map.
  *\param[in,out] pMap The DAQ channel map.
  */
  void set_DAQ_map(DAQChannelMap* map) {_chMap=map;}

 protected:
  /** The JSON node to put the data under.
	* It is created at the beginning of the spill.
	**/
	Json::Value* _docSpill;

  /** The DAQ channel map.
	* It is used to group all channels belonging to a given detector.
	**/
	DAQChannelMap* _chMap;
  string _equipment;
	JsonDocMap _detectors;
};

//////////////////////////////////////////////////////////////////////////////////////////////

/** On Particle Event V1290
 * This class unpacks a V1290 (TDC) board hit.
 */
class V1290DataProcessor : public MDprocessor, public MDarranger
{
 public:
  V1290DataProcessor(){_equipment = "V1290";}
  virtual ~V1290DataProcessor(){}

 /** Unpack a single event to JSON.
  *
  * This function unpacks a single particle event,
	* recorded by equipment CAEN V1290 (TDC)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
	* Will be casted to MDpartEventV1290.
  */
  virtual int Process(MDdataContainer* dc);
};

//////////////////////////////////////////////////////////////////////////////////////////////

/** On Particle Event V1724
 * This class unpacks a V1724 (fADC) board hit.
 */
#define V1724_SAMPLES_PER_WORD   2
class V1724DataProcessor : public MDprocessor, public fADCDataProcessor, public MDarranger
{
 public:
  V1724DataProcessor(){_equipment="V1724";}
  virtual ~V1724DataProcessor(){}

 /** Unpack a single event to JSON.
  *
  * This function unpacks a single particle event,
	* recorded by equipment CAEN V1724 (flash ADC)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
	* Will be casted to MDpartEventV1724.
  */
  virtual int Process(MDdataContainer* dc);
};

//////////////////////////////////////////////////////////////////////////////////////////////

/** On Particle Event V1731
 * This class unpacks a V1731 (fADC) board hit.
 */
#define V1731_SAMPLES_PER_WORD   4
class V1731DataProcessor : public MDprocessor, public fADCDataProcessor, public MDarranger
{
 public:
  V1731DataProcessor(){_equipment="V1731";}
  virtual ~V1731DataProcessor(){}

 /** Unpack a single event to JSON.
  *
  * This function unpacks a single particle event,
	* recorded by equipment CAEN V1731 (flash ADC)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
	* Will be casted to MDpartEventV1731.
  */
  virtual int Process(MDdataContainer* dc);
};

/** On Fragment Event V830
 * This class unpacks a V830 (scaler) board hit.
 */
class V830DataProcessor : public MDprocessor, public MDarranger
{
 public:
  V830DataProcessor(){_equipment="V830";}
  virtual ~V830DataProcessor(){}

 /** Unpack a single event to JSON.
  *
  * This function unpacks a single spill event,
	* recorded by equipment CAEN V830 (scaler)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
	* Will be casted to MDpartEventV830.
  */
  virtual int Process(MDdataContainer* dc);
};

//////////////////////////////////////////////////////////////////////////////////////////////

/** On Fragment Event VLSB
 * This class unpacks a VLSB (tracker) board hit.
 */
class VLSBDataProcessor : public MDprocessor, public MDarranger
{
 public:
  VLSBDataProcessor(){_equipment="VLSB";}
  virtual ~VLSBDataProcessor(){}

 /** Unpack a single event part to JSON.
  *
  * This function unpacks a single particle event,
	* recorded by equipment VLSB (tracker board)
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
	* Will be casted to MDpartEventVLSB_C.
  */
  virtual int Process(MDdataContainer* dc);
};

//////////////////////////////////////////////////////////////////////////////////////////////

/** On Fragment Event DBB
 * This class unpacks a DBB (EMR) board hit.
 */
class DBBDataProcessor : public MDprocessor, public MDarranger
{
 public:
  DBBDataProcessor(){_equipment="DBB";}
  virtual ~DBBDataProcessor(){}

 /** Unpack a single event part to JSON.
  *
  * This function unpacks a single particleevent,
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
	* recorded by equipment DBB (EMR board)
	* Will be casted to MDpartEventVLSB_C.
  */
  virtual int Process(MDdataContainer* dc);
};

#endif  // _MAUS_INPUTCPPREALDATA_UNPACKEVENTLIB_H__



