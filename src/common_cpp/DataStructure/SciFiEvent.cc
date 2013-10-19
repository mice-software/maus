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
  _scifiseeds.resize(0);
  _scifistraightprtracks.resize(0);
  _scifihelicalprtracks.resize(0);
  _scifitracks.resize(0);
}

SciFiEvent::SciFiEvent(const SciFiEvent& _scifievent) {
    _scifidigits.resize(_scifievent._scifidigits.size());
    for (unsigned int i = 0; i < _scifievent._scifidigits.size(); ++i) {
      _scifidigits[i] = new SciFiDigit(*_scifievent._scifidigits[i]);
    }

    _scificlusters.resize(_scifievent._scificlusters.size());
    for (unsigned int i = 0; i < _scifievent._scificlusters.size(); ++i) {
      _scificlusters[i] = new SciFiCluster(*_scifievent._scificlusters[i]);
    }

    _scifispacepoints.resize(_scifievent._scifispacepoints.size());
    for (unsigned int i = 0; i < _scifievent._scifispacepoints.size(); ++i) {
      _scifispacepoints[i] = new SciFiSpacePoint(*_scifievent._scifispacepoints[i]);
    }

    _scifiseeds.resize(_scifievent._scifiseeds.size());
    for (unsigned int i = 0; i < _scifievent._scifiseeds.size(); ++i) {
      _scifiseeds[i] = new SciFiSpacePoint(*_scifievent._scifiseeds[i]);
    }

    _scifistraightprtracks.resize(_scifievent._scifistraightprtracks.size());
    for (unsigned int i = 0; i < _scifievent._scifistraightprtracks.size(); ++i) {
      _scifistraightprtracks[i] =
          new SciFiStraightPRTrack(*_scifievent._scifistraightprtracks[i]);
    }

    _scifihelicalprtracks.resize(_scifievent._scifihelicalprtracks.size());
    for (unsigned int i = 0; i < _scifievent._scifihelicalprtracks.size(); ++i) {
      _scifihelicalprtracks[i] =
          new SciFiHelicalPRTrack(*_scifievent._scifihelicalprtracks[i]);
    }

    _scifitracks.resize(_scifievent._scifitracks.size());
    for (unsigned int i = 0; i < _scifievent._scifitracks.size(); ++i) {
      _scifitracks[i] = _scifievent._scifitracks[i];
    }
}

SciFiEvent& SciFiEvent::operator=(const SciFiEvent& _scifievent) {
  std::cerr << "IN = operator " << std::endl;
    if (this == &_scifievent) {
        return *this;
    }

    _scifidigits.resize(_scifievent._scifidigits.size());
    for (unsigned int i = 0; i < _scifievent._scifidigits.size(); ++i) {
      _scifidigits[i] = new SciFiDigit(*_scifievent._scifidigits[i]);
    }

        _scifispacepoints.resize(_scifievent._scifispacepoints.size());
    for (unsigned int i = 0; i < _scifievent._scifispacepoints.size(); ++i) {
      _scifispacepoints[i] = new SciFiSpacePoint(*_scifievent._scifispacepoints[i]);
    }

    _scifiseeds.resize(_scifievent._scifiseeds.size());
    for (unsigned int i = 0; i < _scifievent._scifiseeds.size(); ++i) {
      _scifiseeds[i] = new SciFiSpacePoint(*_scifievent._scifiseeds[i]);
    }

    _scifistraightprtracks.resize(_scifievent._scifistraightprtracks.size());
    for (unsigned int i = 0; i < _scifievent._scifistraightprtracks.size(); ++i) {
      _scifistraightprtracks[i] =
          new SciFiStraightPRTrack(*_scifievent._scifistraightprtracks[i]);
    }

    _scifihelicalprtracks.resize(_scifievent._scifihelicalprtracks.size());
    for (unsigned int i = 0; i < _scifievent._scifihelicalprtracks.size(); ++i) {
      _scifihelicalprtracks[i] =
          new SciFiHelicalPRTrack(*_scifievent._scifihelicalprtracks[i]);
    }

    _scifitracks.resize(_scifievent._scifitracks.size());
    for (unsigned int i = 0; i < _scifievent._scifitracks.size(); ++i) {
      _scifitracks[i] = _scifievent._scifitracks[i];
    }

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

  std::vector<SciFiSpacePoint*>::iterator seed;
  for (seed = _scifiseeds.begin(); seed!= _scifiseeds.end(); ++seed) {
    delete (*seed);
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

  std::vector<SciFiTrack*>::iterator track;
  for (track = _scifitracks.begin();
       track!= _scifitracks.end(); ++track) {
    delete (*track);
  }
}

void SciFiEvent::set_spacepoints_used_flag(bool flag) {
  for ( unsigned int i = 0; i < _scifispacepoints.size(); ++i ) {
    _scifispacepoints[i]->set_used(flag);
  }
}

} // ~namespace MAUS
