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
 */

/** @class MapCppPrint
 *  Prints out the JSON input and returns input
 *
 *  This class is meant to be a debugging and simple example class
 *  to illustrate using JSON from within C++.    
 */

#ifndef _COMPONENTS_MAP_MAPCPPPRINT_H_
#define _COMPONENTS_MAP_MAPCPPPRINT_H_

#include <json/json.h>
#include <iostream>
#include <string>
#include <sstream>

class MapCppPrint {
 public:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool death();

  /** process and print JSON input
   *
   *  \param document JSON spill to send to std::out
   */
  std::string process(std::string document);

 private:
  std::string _classname;
};  // Don't forget this trailing colon!!!!

#endif  //  _COMPONENTS_MAP_MAPCPPPRINT_H_
