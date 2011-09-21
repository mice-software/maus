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

#include <json/json.h>
#include <G4VSensitiveDetector.hh>
#include <vector>
#include "Config/MiceModule.hh"

namespace MAUS {

class MAUSSD : public G4VSensitiveDetector {
 public:
  explicit MAUSSD(MiceModule* mod);

  bool isHit();
  int GetNHits() { return _hits.size(); }
  void ClearHits() { _hits.clear(); }
  std::vector<Json::Value> GetHits() { return _hits; }


 protected:
  std::vector<Json::Value> _hits;

  MiceModule* _module;
};

}  //  ends MAUS namespace

#endif  //  _COMMONCPP_DETMODEL_MAUSSD_H_
