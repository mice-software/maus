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

/** @class SciFiEvent
 *  D...
 *
 */

#ifndef SCIFIEVENT_HH
#define SCIFIEVENT_HH

// C headers

// C++ headers
#include <vector>
#include <string>
#include <iterator>

// Third party headers
#include "TH1D.h"
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SciFiHit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"
#include "src/common_cpp/Recon/SciFi/SciFiHelicalPRTrack.hh"
// #include "src/common_cpp/Recon/SciFi/SciFiPRTrack.hh"

// namespace MAUS {

class SciFiEvent {
 public:

  /// Constructor
  SciFiEvent();

  /// Copy constructor
  SciFiEvent(const SciFiEvent& _scifievent);

  /// Destructor
  virtual ~SciFiEvent();

  /// Equality operator
  SciFiEvent& operator=(const SciFiEvent& _scifievent);

  /// Residual histograms
  std::vector< std::vector<int> > residuals() const { return _residuals; }
  void set_residuals(std::vector< std::vector<int> > residuals) { _residuals = residuals; }

  /// Hits
  void add_hit(SciFiHit* hit) { _scifihits.push_back(hit); }
  std::vector<SciFiHit*> hits() const { return _scifihits; }

  /// Digits
  void add_digit(SciFiDigit* digit) { _scifidigits.push_back(digit); }
  std::vector<SciFiDigit*> digits() const { return _scifidigits; }

  /// Clusters
  void add_cluster(SciFiCluster* cluster) { _scificlusters.push_back(cluster); }
  std::vector<SciFiCluster*> clusters() const { return _scificlusters; }

  /// Spacepoints
  void add_spacepoint(SciFiSpacePoint* spacepoint) { _scifispacepoints.push_back(spacepoint); }
  std::vector<SciFiSpacePoint*> spacepoints() const { return _scifispacepoints; }

  /// Seeds
  void add_seeds(SciFiSpacePoint* seed) { _scifiseeds.push_back(seed); }
  std::vector<SciFiSpacePoint*> seeds() const { return _scifiseeds; }

  /// Straight Pattern Recognition tracks
  void add_straightprtrack(SciFiStraightPRTrack track) { _scifistraightprtracks.push_back(track); }
  void set_straightprtrack(std::vector<SciFiStraightPRTrack> tracks) {
    _scifistraightprtracks = tracks; }
  std::vector<SciFiStraightPRTrack> straightprtracks() const { return _scifistraightprtracks; }

  /// Helical Pattern Recognition tracks
  void add_helicalprtrack(SciFiHelicalPRTrack track) { _scifihelicalprtracks.push_back(track); }
  void set_helicalprtrack(std::vector<SciFiHelicalPRTrack> tracks) {
                          _scifihelicalprtracks = tracks; }
  std::vector<SciFiHelicalPRTrack> helicalprtracks() const {return _scifihelicalprtracks; }


 private:

  /// Residuals histograms
  std::vector< std::vector<int> >  _residuals;

  /// Hits in an event.
  std::vector<SciFiHit*>              _scifihits;

  /// Digits in an event.
  std::vector<SciFiDigit*>            _scifidigits;

  /// Clusters in an event.
  std::vector<SciFiCluster*>          _scificlusters;

  /// Spacepoints in an event.
  std::vector<SciFiSpacePoint*>       _scifispacepoints;

  /// Seeds for track fitting.
  std::vector<SciFiSpacePoint*>       _scifiseeds;

  /// Straight tracks.
  std::vector<SciFiStraightPRTrack>   _scifistraightprtracks;

  /// Helical tracks.
  std::vector<SciFiHelicalPRTrack>    _scifihelicalprtracks;

  /// Class def for MAUS data structure
  // ClassDef(SciFiEvent, 1)
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
