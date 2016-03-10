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

#ifndef _SRC_COMMON_CPP_UTILS_EMRGEOMETRYMAP_HH_
#define _SRC_COMMON_CPP_UTILS_EMRGEOMETRYMAP_HH_

// C++ headers
#include <Python.h>
#include <string>
#include <vector>
#include <iostream>

// MAUS headers
#include "json/json.h"
#include "Utils/EMRChannelMap.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/Exception.hh"
#include "Interface/Squeak.hh"
#include "Config/MiceModule.hh"
#include "DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class Complete map of all bar positions and dimensions in the EMR.
	   This class is used to hold and manage the EMR geometry.
 */

class EMRGeometryMap {
 public:

  EMRGeometryMap();
  virtual ~EMRGeometryMap();

 /** @brief Initialize the geometry map by using the text files provided in ConfigurationDefaults.py
  *
  *  \returns true if the text file is loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON);

 /** @brief Initialize the map by using the provided file.
  *
  *  \param[in] geometryFile 	name of the text file containing the geometry.
  *  \returns 			true if the geometry file is loaded successfully.
  */
  bool Initialize(std::string geometryFile);

 /** @brief Returns the local position of the bar coded by the key.
  *
  *  \param[in] key	channel of the bar.
  *  \returns 		the value of the local position for this bar. If no geometry 
  * 	      		for this channel the function returns NOGEO (-99999).
  */
  ThreeVector LocalPosition(EMRChannelKey key) const;

 /** @brief Returns the global position of the bar coded by the key.
  *
  *  \param[in] key	channel of the bar.
  *  \returns 		the value of the global position for this bar. If no geometry 
  * 	      		for this channel the function returns NOGEO (-99999).
  */
  ThreeVector GlobalPosition(EMRChannelKey key) const;

 /** @brief Returns the global equivalent of the set of local coordinates in the EMR
  *
  *  \param[in] pos	local coordinates of a points
  *  \returns		the value of the global coordinates for this point. If no geometry
  * 	      		for this channel the function returns the coordinates unchanged.
  */
  ThreeVector MakeGlobal(ThreeVector pos) const;

 /** @brief Returns the local start of the fiducial volume in the three axes.
  */
  ThreeVector LocalStart() const { return _localStartEMR; }

 /** @brief Returns the local end of the fiducial volume in the three axes.
  */
  ThreeVector LocalEnd() const   { return _localEndEMR; }

 /** @brief Returns the dimensions of the an EMR bar l*w*h.
  */
  ThreeVector Dimensions() const { return _D; }

 /** @brief Returns the size of the gaps between the bars
  */
  double Gap() const             { return _gap; }

 private:

 /** @brief Make one EMRChannelKey for each channel of the detector.
  * 	   All EMRChannelKeys are held in the data member _Ckey.
  */
  int MakeEMRChannelKeys();

 /** @brief Find the position of the channel key in the data member _Ckey.
  */
  int FindEMRChannelKey(EMRChannelKey key) const;

 /** @brief Load geometry map from file
  */
  bool LoadGeometryFile(std::string geometryFile);

 /** @brief Global position and rotation of the EMR sensitive detector
  */
  Hep3Vector _globalPosEMR;
  HepRotation _globalRotEMR;

 /** @brief Local limits and end of the scintillating volume in (x,y,z)
  */
  ThreeVector _localStartEMR;
  ThreeVector _localEndEMR;

 /** @brief This vector holds the dimensions vector l*w*h
  */
  ThreeVector _D;

 /** @brief EMR characteristics
  */
  int _number_of_planes;
  int _number_of_bars;
  double _bar_width;
  double _bar_height;
  double _bar_length;
  double _gap;
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_UTILS_EMRGEOMETRYMAP_HH_
