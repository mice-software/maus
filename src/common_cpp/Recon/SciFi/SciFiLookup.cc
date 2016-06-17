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

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"

namespace MAUS {

SciFiLookup::SciFiLookup() : _hits_map(),
                             _noise_map() {
  // Do nothing
}

SciFiLookup::~SciFiLookup() {
  // Do nothing
}

bool SciFiLookup::make_hits_map(MCEvent* evt) {
  // Check the pointers are valid
  if (!evt) {
    std::cerr << "Invalid MC event pointer supplied, aborting" << std::endl;
    return false;
  }
  if (!evt->GetSciFiHits()) {
    std::cerr << "Invalid MC Hits container pointer supplied, aborting" << std::endl;
    return false;
  }

  // Make the hits map
  std::vector<SciFiHit> *hits = evt->GetSciFiHits();
  for ( size_t i = 0; i < hits->size(); ++i ) {
    SciFiHit *hit = &(hits->at(i));
    uint64_t id = (uint64_t)hit->GetChannelId()->GetID();
    // If id is not in map, make a hits vector for the key, and add the current hit
    if ( _hits_map.find(id) == _hits_map.end() ) {
      std::vector<SciFiHit*> v1;
      _hits_map[id] = v1;
      _hits_map[id].push_back(hit);
    // Otherwise add the current hit to the vector already associate with this id
    } else {
      _hits_map[id].push_back(hit);
    }
  }
  return true;
}

bool SciFiLookup::make_noise_map(MCEvent* evt) {
  // Check the pointer is valid
  if (!evt) {
    std::cerr << "Invalid MC event pointer supplied, aborting" << std::endl;
    return false;
  }

  // Make the noise hits map
  std::vector<SciFiNoiseHit> *hits = evt->GetSciFiNoiseHits();
  for ( size_t i = 0; i < hits->size(); ++i ) {
    SciFiNoiseHit *hit = &(hits->at(i));
    uint64_t id = (uint64_t)hit->GetID();
    // If id is not in map, make a hits vector for the key, and add the current hit
    if ( _noise_map.find(id) == _noise_map.end() ) {
      std::vector<SciFiNoiseHit*> v1;
      _noise_map[id] = v1;
      _noise_map[id].push_back(hit);
    // Otherwise add the current hit to the vector already associate with this id
    } else {
      _noise_map[id].push_back(hit);
    }
  }
  return true;
}

bool SciFiLookup::get_hits(const SciFiDigit* dig, std::vector<SciFiHit*> &hits) {
  if (!dig) {
    std::cerr << "SciFiLookup: Bad digit pointer passed" << std::endl;
    return false;
  } else if (_hits_map.size() < 1) {
    std::cerr << "SciFiLookup: No hits in map, cannot perfom lookup" << std::endl;
    return false;
  } else {
    uint64_t digit_id = (uint64_t)get_digit_id(dig);
    hits = _hits_map[digit_id];
    return true;
  }
}

bool SciFiLookup::get_noise(const SciFiDigit* dig, std::vector<SciFiNoiseHit*> &noise) {
  if (dig && _noise_map.size() > 0) {
    uint64_t digit_id = (uint64_t)get_digit_id(dig);
    noise = _noise_map[digit_id];
    return true;
  } else {
    return false;
  }
}

uint64_t SciFiLookup::get_digit_id(const SciFiDigit* dig) {
  uint64_t digit_id = dig->get_channel() + 1000 * dig->get_plane() + 10000 * dig->get_station()
                                         + 100000 * dig->get_tracker() + 1000000 * dig->get_event();
  return digit_id;
}

} // ~namespace MAUS
