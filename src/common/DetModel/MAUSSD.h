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
  MAUSSD(MiceModule* , bool dEdxCut);

  bool isHit() { return _isHit; }
  int GetNHits() { return _hits.size(); }
  std::vector<Json::Value> GetHits() { return _hits; }

 protected:
  std::vector<Json::Value> _hits;
  bool _isHit;

  MiceModule* _module;

  bool            _dEdxCut;
};

}  //  ends MAUS namespace

#endif  //  _COMMONCPP_DETMODEL_MAUSSD_H_
