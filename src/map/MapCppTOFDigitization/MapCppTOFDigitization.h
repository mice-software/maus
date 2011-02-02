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

/** @class MapCppTOFDigitization 
 *  Digitize events by running TOF electronics simulation.
 *
 *  @authors Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>,
 *  Yordan Karadzhov <yordan@phys.uni-sofia.bg>
 */

#ifndef _COMPONENTS_MAP_MAPCPPTOFDIGITIZATION_H_
#define _COMPONENTS_MAP_MAPCPPTOFDIGITIZATION_H_
// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers  
#include <iostream>
#include <string>
#include <sstream>

// G4MICE from commonCpp
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"

// from old file
//#include "TofDigits.hh"
//#include "TofHit.hh"
//#include "TofDigit.hh"
#include "Config/MiceModule.hh"
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <cmath>

class MapCppTOFDigitization
{
 public: 
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool Birth(std::string argJsonConfigDocument);

  /** Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool Death();
 
  /** Process JSON document
   *
   *  Receive a document with MC hits and return
   *  a document with digits
   */
  std::string Process(std::string document);

 private:
  /// This should be the classname
  std::string _classname;

  double              _conversionFactor;
  double              _pmtTimeResolution;
  double              _attenuationLength;
  double              _adcConversionFactor;
  double              _tdcConversionFactor;
  double              _pmtQuantumEfficiency;
  double              _scintLightSpeed;

  MiceModule* _module;
};  // Don't forget this trailing colon!!!!

#endif  //  _COMPONENTS_MAP_MAPCPPTOFDIGITIZATION_H_
