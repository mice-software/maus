/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

/** @class MAUSSD
 *  Generic class for MICE special detectors within the MC.
 *
 *  Generic class for MICE special detectors.  Each special
 *  detector will produce a JSON::Value hit and also set
 *  a boolean flag as whether it was hit.
 *
 */
#ifndef _COMMONCPP_DETMODEL_MAUSSD_H_
#define _COMMONCPP_DETMODEL_MAUSSD_H_

#include <vector>
#include <string>
#include <map>

#include "Geant4/G4VSensitiveDetector.hh"

#include "Config/MiceModule.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {

/** MAUSSD is the base class for all G4 sensitive detectors
 *
 *  MAUSSD provides an abstraction layer for sensitive detectors to provide a
 *  common interface; handled by DetectorConstruction
 */
class MAUSSD : public G4VSensitiveDetector {
 public:
  /** Constructor for the MAUSSD
   *
   *  Use the module name to generate the sensitive detector name. Geant4 uses
   *  name as a unique ID for the sensitive detector.
   */
  explicit MAUSSD(MiceModule* mod);

  /** True if the SD has at least one hit in its _hits array */
  virtual bool isHit() = 0;

  /** Returns the number of hits in the Sensitive Detector */
  virtual int GetNHits() = 0;

  /** Clears all hits in the sensitive detector */
  virtual void ClearHits() = 0;

  /** Hand ownership of the detector hits to MCEvent */
  virtual void TakeHits(MCEvent* event) = 0;

  /** Reset the SD list
   *
   *  If the Geant4 SD list has been modified during this geometry iteration,
   *  inactive all SDs from this geometry iteration and increment the iteration
   *  number.
   */
  static void ResetSDs();

 protected:
  MiceModule* _module;
  static std::string namePrefix();

 private:
  static int _uniqueID;
  static std::map<std::string, bool> _hasVolumes;
};
}  //  ends MAUS namespace

#endif  //  _COMMONCPP_DETMODEL_MAUSSD_H_
