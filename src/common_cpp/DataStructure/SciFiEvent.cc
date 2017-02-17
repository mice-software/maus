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

#include "src/common_cpp/DataStructure/SciFiEvent.hh"

namespace MAUS {

SciFiEvent::SciFiEvent() {
  _scifidigits.resize(0);
  _scificlusters.resize(0);
  _scifispacepoints.resize(0);
  _scifistraightprtracks.resize(0);
  _scifihelicalprtracks.resize(0);
  _scifiseeds.resize(0);
  _scifitracks.resize(0);
  _mean_Bz_upstream = 0.0;
  _mean_Bz_downstream = 0.0;
  _var_Bz_upstream = 0.0;
  _var_Bz_downstream = 0.0;
  _range_Bz_upstream = 0.0;
  _range_Bz_downstream = 0.0;
}

SciFiEvent::SciFiEvent(const SciFiEvent& old_event) {
  *this = old_event;
}

SciFiEvent& SciFiEvent::operator=(const SciFiEvent& rhs) {
  if (this == &rhs) {
      return *this;
  }

  // Deep copy the digits
  _scifidigits.resize(rhs._scifidigits.size());
  for (unsigned int i = 0; i < rhs._scifidigits.size(); ++i) {
    _scifidigits[i] = new SciFiDigit(*rhs._scifidigits[i]);
  }

  // Deep copy the clusters
  _scificlusters.resize(rhs._scificlusters.size());
  for (unsigned int i = 0; i < rhs._scificlusters.size(); ++i) {
    _scificlusters[i] = new SciFiCluster(*rhs._scificlusters[i]);
    // Now set cross-pointers so they point to correct place in the new copy of the datastructure
    SciFiDigitPArray new_digits(rhs._scificlusters[i]->get_digits()->GetLast() + 1);
    for (unsigned int j = 0; j < new_digits.size(); ++j) {
      new_digits[j] = NULL;
      for (unsigned int k = 0; k < rhs._scifidigits.size(); ++k) {
        if (rhs._scificlusters[i]->get_digits()->At(j) == rhs._scifidigits[k]) {
          new_digits[j] = _scifidigits[k];
          break;
        }
      }
    }
    _scificlusters[i]->set_digits_pointers(new_digits);
  }

  // Deep copy the spacepoints
  _scifispacepoints.resize(rhs._scifispacepoints.size());
  for (unsigned int i = 0; i < rhs._scifispacepoints.size(); ++i) {
    _scifispacepoints[i] = new SciFiSpacePoint(*rhs._scifispacepoints[i]);
    // Now set cross-pointers so they point to correct place in the new copy of the datastructure
    SciFiClusterPArray new_clusters(rhs._scifispacepoints[i]->get_channels()->GetLast() + 1);
    for (unsigned int j = 0; j < new_clusters.size(); ++j) {
      new_clusters[j] = NULL;
      for (unsigned int k = 0; k < rhs._scificlusters.size(); ++k) {
        if (rhs._scifispacepoints[i]->get_channels()->At(j) == rhs._scificlusters[k]) {
          new_clusters[j] = _scificlusters[k];
          break;
        }
      }
    }
    _scifispacepoints[i]->set_channels_pointers(new_clusters);
  }

  // Deep copy the straight pattern recognition tracks
  _scifistraightprtracks.resize(rhs._scifistraightprtracks.size());
  for (unsigned int i = 0; i < rhs._scifistraightprtracks.size(); ++i) {
    _scifistraightprtracks[i] = new SciFiStraightPRTrack(*rhs._scifistraightprtracks[i]);
    // Now set cross-pointers so they point to correct place in the new copy of the datastructure
    SciFiSpacePointPArray new_sps(rhs._scifistraightprtracks[i]->get_spacepoints()->GetLast()+1);
    for (unsigned int j = 0; j < new_sps.size(); ++j) {
      new_sps[j] = NULL;
      for (unsigned int k = 0; k < rhs._scifispacepoints.size(); ++k) {
        if (rhs._scifistraightprtracks[i]->get_spacepoints_pointers()[j] ==
          rhs._scifispacepoints[k]) {
          new_sps[j] = _scifispacepoints[k];
          break;
        }
      }
    }
    _scifistraightprtracks[i]->set_spacepoints_pointers(new_sps);
  }

  // Deep copy the helical pattern recognition tracks
  _scifihelicalprtracks.resize(rhs._scifihelicalprtracks.size());
  for (unsigned int i = 0; i < rhs._scifihelicalprtracks.size(); ++i) {
    _scifihelicalprtracks[i] = new SciFiHelicalPRTrack(*rhs._scifihelicalprtracks[i]);
    // Now set cross-pointers so they point to correct place in the new copy of the datastructure
    SciFiSpacePointPArray new_sps(rhs._scifihelicalprtracks[i]->get_spacepoints()->GetLast() + 1);
    for (unsigned int j = 0; j < new_sps.size(); ++j) {
      new_sps[j] = NULL;
      for (unsigned int k = 0; k < rhs._scifispacepoints.size(); ++k) {
        if (rhs._scifihelicalprtracks[i]->get_spacepoints()->At(j) == rhs._scifispacepoints[k]) {
          new_sps[j] = _scifispacepoints[k];
          break;
        }
      }
    }
    _scifihelicalprtracks[i]->set_spacepoints_pointers(new_sps);
  }

  // Deep copy the seeds
  _scifiseeds.resize(rhs._scifiseeds.size());
  for (unsigned int i = 0; i < rhs._scifiseeds.size(); ++i) {
    _scifiseeds[i] = new SciFiSeed(*rhs._scifiseeds[i]);
  }

  // Deep copy the kalman tracks
  _scifitracks.resize(rhs._scifitracks.size());
  for (unsigned int iTrk = 0; iTrk < rhs._scifitracks.size(); ++iTrk) {
    _scifitracks[iTrk] =  new SciFiTrack(*rhs._scifitracks[iTrk]);

    // Now set the cross-pointer to the PR track within the track so that it points to correct
    // place in the new copy of the datastructure, by searching for the PR track index in rhs
    // event, which matches the pointer address of the PR track in the rhs track. Use this to set
    // new track PR track to pointer to the correct PR track in the new event.
    SciFiBasePRTrack* new_pr_track = NULL;
    if (_scifitracks[iTrk]->GetAlgorithmUsed() == 0) {
      for (unsigned int iRst = 0; iRst < rhs._scifistraightprtracks.size(); ++iRst) {
        if (rhs._scifitracks[iTrk]->pr_track_pointer() == rhs._scifistraightprtracks[iRst]) {
          new_pr_track = _scifistraightprtracks[iRst];
          break;
        }
      }
    } else {
      for (unsigned int iRhe = 0; iRhe < rhs._scifihelicalprtracks.size(); ++iRhe) {
        if (rhs._scifitracks[iTrk]->pr_track_pointer() == rhs._scifihelicalprtracks[iRhe]) {
          new_pr_track = _scifihelicalprtracks[iRhe];
          break;
        }
      }
    }
    _scifitracks[iTrk]->set_pr_track_pointer(new_pr_track);
  }

  this->_mean_Bz_upstream = rhs._mean_Bz_upstream;
  this->_mean_Bz_downstream = rhs._mean_Bz_downstream;

  this->_var_Bz_upstream = rhs._var_Bz_upstream;
  this->_var_Bz_downstream = rhs._var_Bz_downstream;

  this->_range_Bz_upstream = rhs._range_Bz_upstream;
  this->_range_Bz_downstream = rhs._range_Bz_downstream;
  return *this;
}

SciFiEvent::~SciFiEvent() {
  std::vector<SciFiDigit*>::iterator digit;
  for (digit = _scifidigits.begin(); digit!= _scifidigits.end(); ++digit) {
    delete (*digit);
  }

  std::vector<SciFiCluster*>::iterator cluster;
  for (cluster = _scificlusters.begin(); cluster!= _scificlusters.end(); ++cluster) {
    delete (*cluster);
  }

  std::vector<SciFiSpacePoint*>::iterator spoint;
  for (spoint = _scifispacepoints.begin(); spoint!= _scifispacepoints.end(); ++spoint) {
    delete (*spoint);
  }

  std::vector<SciFiStraightPRTrack*>::iterator strack;
  for (strack = _scifistraightprtracks.begin();
       strack!= _scifistraightprtracks.end(); ++strack) {
    delete (*strack);
  }

  std::vector<SciFiHelicalPRTrack*>::iterator htrack;
  for (htrack = _scifihelicalprtracks.begin();
       htrack!= _scifihelicalprtracks.end(); ++htrack) {
    delete (*htrack);
  }

  std::vector<SciFiSeed*>::iterator seed;
  for (seed = _scifiseeds.begin(); seed!= _scifiseeds.end(); ++seed) {
    delete (*seed);
  }

  std::vector<SciFiTrack*>::iterator track;
  for (track = _scifitracks.begin();
       track!= _scifitracks.end(); ++track) {
    delete (*track);
  }
}

void SciFiEvent::clear_all() {
  clear_digits();
  clear_clusters();
  clear_spacepoints();
  clear_seeds();
  clear_stracks();
  clear_htracks();
  clear_scifitracks();
}

void SciFiEvent::clear_digits() {
  std::vector<SciFiDigit*>::iterator it;
  for (it = _scifidigits.begin(); it!= _scifidigits.end(); ++it) {
    delete (*it);
  }
  _scifidigits.resize(0);
}

void SciFiEvent::clear_clusters() {
  std::vector<SciFiCluster*>::iterator it;
  for (it = _scificlusters.begin(); it != _scificlusters.end(); ++it) {
    delete (*it);
  }
  _scificlusters.resize(0);
}

void SciFiEvent::clear_spacepoints() {
  std::vector<SciFiSpacePoint*>::iterator it;
  for (it = _scifispacepoints.begin(); it != _scifispacepoints.end(); ++it) {
    delete (*it);
  }
  _scifispacepoints.resize(0);
}

void SciFiEvent::clear_stracks() {
  std::vector<SciFiStraightPRTrack*>::iterator it;
  for (it = _scifistraightprtracks.begin(); it != _scifistraightprtracks.end(); ++it) {
    delete (*it);
  }
  _scifistraightprtracks.resize(0);
}

void SciFiEvent::clear_htracks() {
  std::vector<SciFiHelicalPRTrack*>::iterator it;
  for (it = _scifihelicalprtracks.begin(); it != _scifihelicalprtracks.end(); ++it) {
    delete (*it);
  }
  _scifihelicalprtracks.resize(0);
}

void SciFiEvent::clear_seeds() {
  std::vector<SciFiSeed*>::iterator it;
  for (it = _scifiseeds.begin(); it != _scifiseeds.end(); ++it) {
    delete (*it);
  }
  _scifiseeds.resize(0);
}

void SciFiEvent::clear_scifitracks() {
  std::vector<SciFiTrack*>::iterator it;
  for (it = _scifitracks.begin(); it != _scifitracks.end(); ++it) {
    delete (*it);
  }
  _scifitracks.resize(0);
}

void SciFiEvent::set_spacepoints_used_flag(bool flag) {
  for ( unsigned int i = 0; i < _scifispacepoints.size(); ++i ) {
    _scifispacepoints[i]->set_used(flag);
  }
}

} // ~namespace MAUS
