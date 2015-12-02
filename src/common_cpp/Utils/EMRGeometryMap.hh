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

#ifndef _MAUS_EMRGEOMETRYMAP_HH_
#define _MAUS_EMRGEOMETRYMAP_HH_


#include <Python.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <functional>

#include "json/json.h"
#include "Utils/EMRChannelMap.hh"
#include "Utils/Exception.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "Config/MiceModule.hh"

namespace MAUS {

/** Complete map of all calibration constants needed in order to reconstruct the energy
 * measured by the EMR detector. This class is used to hold and manage calibration 
 * constants and to calculate the calibration corrections.
 */

class EMRGeometryMap {
 public:

  EMRGeometryMap();
  virtual ~EMRGeometryMap();

 /** Initialize the geometry map by using the text files provided in 
  * ConfigurationDefaults.py
  * \returns true if the text file is loaded successfully.
  */
  bool InitializeFromCards(Json::Value configJSON);

 /** Initialize the map by using the provided file.
  * \param[in] geometryFile name of the text file containing the geometry.
  * \returns true if the text file is loaded successfully.
  */
  bool Initialize(std::string geometryFile);

 /** Returns the local position of the bar coded by the key.
  * \param[in] key of the bar
  * \returns the value of the local position for this bar. If no geometry 
  * for this channel the function returns NOGEO (-99999).
  */
  Hep3Vector LocalPosition(EMRChannelKey key) const;

 /** Returns the global position of the bar coded by the key.
  * \param[in] key of the bar
  * \returns the value of the global position for this bar. If no geometry 
  * for this channel the function returns NOGEO (-99999).
  */
  Hep3Vector GlobalPosition(EMRChannelKey key) const;

 /** Returns the dimensions of the an EMR bar w*h*l.
  */
  Hep3Vector Dimensions() const;

 /** Print the geometry map;
  * To be used only for debugging.
  */
  void Print();

  enum {
   /** This value is returned when the key is not found.
    */
    NOGEO = -99999
  };

 private:

 /** Make one EMRChannelKey for each channel of the detector.
  * All EMRChannelKeys are held in the data member _Ckey.
  */
  int MakeEMRChannelKeys();

 /** Find the position of the channel key in the data member _Ckey.
  */
  int FindEMRChannelKey(EMRChannelKey key) const;

 /** Load geometry map from file
  */
  bool LoadGeometryFile(std::string geometryFile);

 /** This vector holds one EMRChannelKey for each channel of the detector.
  */
  std::vector<EMRChannelKey> _Ckey;

 /** These vectors hold the position vectors. IMPORTANT - the order 
  * of the entries here is the same as the order of the entries in _Ckey. 
  * This is used when the constants are read.
  */
  std::vector<Hep3Vector> _Plkey; // Local
  std::vector<Hep3Vector> _Pgkey; // Global

 /** This vector holds the dimensions vector w*h*l.
  */
  Hep3Vector _D;

 /** EMR characteristics
  */
  int _number_of_planes;
  int _number_of_bars;
  double _bar_width;
  double _bar_height;
  double _bar_length;
  double _gap;
};
}

#endif


