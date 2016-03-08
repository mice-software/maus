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
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_

// C++ headers
#include <vector>

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"

namespace MAUS {

/** @class SciFiEvent A container to hold other SciFi containers, representing a particle event
 *
 */

class SciFiEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    SciFiEvent();

    /** Copy constructor - any pointers are deep copied */
    SciFiEvent(const SciFiEvent& _scifievent);

    /** Assignment operator - any pointers are deep copied */
    SciFiEvent& operator=(const SciFiEvent& _scifievent);

    /** Destructor - any member pointers are deleted */
    virtual ~SciFiEvent();

    /** Digits */
    void add_digit(SciFiDigit* digit) { _scifidigits.push_back(digit); }
    void set_digits(SciFiDigitPArray scifidigits) { _scifidigits = scifidigits; }
    SciFiDigitPArray digits() const { return _scifidigits; }
    void clear_digits();

    /** Clusters */
    void add_cluster(SciFiCluster* cluster) { _scificlusters.push_back(cluster); }
    void set_clusters(SciFiClusterPArray scificlusters) { _scificlusters = scificlusters; }
    SciFiClusterPArray clusters() const { return _scificlusters; }
    void clear_clusters();

    /** Spacepoints */
    void add_spacepoint(SciFiSpacePoint* spacepoint) { _scifispacepoints.push_back(spacepoint); }
    void set_spacepoints(SciFiSpacePointPArray scifispacepoints) {
                        _scifispacepoints = scifispacepoints; }
    SciFiSpacePointPArray spacepoints() const { return _scifispacepoints; }
    void set_spacepoints_used_flag(bool flag);
    void clear_spacepoints();

    /** Seeds */
    void add_seeds(SciFiSpacePoint* seed) { _scifiseeds.push_back(seed); }
    SciFiSpacePointPArray seeds() const { return _scifiseeds; }
    void clear_seeds();

    /** Straight Pattern Recognition tracks */
    void add_straightprtrack(SciFiStraightPRTrack* track) {
                             _scifistraightprtracks.push_back(track); }
    void set_straightprtrack(SciFiStraightPRTrackPArray tracks) { _scifistraightprtracks = tracks; }
    SciFiStraightPRTrackPArray straightprtracks() const { return _scifistraightprtracks; }
    void clear_stracks();

    /** Helical Pattern Recognition tracks */
    void add_helicalprtrack(SciFiHelicalPRTrack* track) { _scifihelicalprtracks.push_back(track); }
    void set_helicalprtrack(SciFiHelicalPRTrackPArray tracks) { _scifihelicalprtracks = tracks; }
    SciFiHelicalPRTrackPArray helicalprtracks() const { return _scifihelicalprtracks; }
    void clear_htracks();

    /** Kalman tracks */
    void add_scifitrack(SciFiTrack *a_track) { _scifitracks.push_back(a_track); }
    void set_scifitracks(SciFiTrackPArray tracks) { _scifitracks = tracks; }
    SciFiTrackPArray scifitracks() const { return _scifitracks; }
    void clear_scifitracks();

    /** Clear all data containers */
    void clear_all();

  private:

    /** Digits in an event */
    SciFiDigitPArray                    _scifidigits;

    /** Clusters in an event */
    SciFiClusterPArray                  _scificlusters;

    /** SpacePoints in an event */
    SciFiSpacePointPArray               _scifispacepoints;

    /** Seeds for track fitting */
    SciFiSpacePointPArray               _scifiseeds;

    /** Straight tracks */
    SciFiStraightPRTrackPArray          _scifistraightprtracks;

    /** Helical tracks */
    SciFiHelicalPRTrackPArray           _scifihelicalprtracks;

    /** Kalman tracks */
    SciFiTrackPArray                     _scifitracks;

    MAUS_VERSIONED_CLASS_DEF(SciFiEvent)
};

typedef std::vector<SciFiEvent*> SciFiEventPArray;
typedef std::vector<SciFiEvent> SciFiEventArray;

} // ~namespace MAUS

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_

