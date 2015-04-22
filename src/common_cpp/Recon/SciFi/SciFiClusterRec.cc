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
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"

namespace MAUS {

SciFiClusterRec::SciFiClusterRec():_size_exception(0),
                                   _min_npe(0.) {}

SciFiClusterRec::SciFiClusterRec(int cluster_exception,
                                 double min_npe,
                                 const SciFiTrackerMap& geometry_map):
                                   _size_exception(cluster_exception),
                                   _min_npe(min_npe),
                                   _geometry_map(geometry_map) {}

SciFiClusterRec::~SciFiClusterRec() {}

bool sort_by_npe(SciFiDigit *a, SciFiDigit *b ) {
  return ( a->get_npe() > b->get_npe() );
}

void SciFiClusterRec::process(SciFiEvent &evt) const {
  // Create and fill the seeds vector.
  std::vector<SciFiDigit*> seeds = get_seeds(evt);

  // Get the number of clusters. If too large, abort reconstruction.
  int seeds_size = seeds.size();
  if ( seeds_size > _size_exception ) {
    return;
  }

  // Sort seeds so that we use higher npe first.
  std::sort(seeds.begin(), seeds.end(), sort_by_npe);

  make_clusters(evt, seeds);
}

std::vector<SciFiDigit*> SciFiClusterRec::get_seeds(SciFiEvent &evt) const {
  std::vector<SciFiDigit*> seeds_in_event;
  for ( size_t dig = 0; dig < evt.digits().size(); ++dig ) {
    if ( evt.digits()[dig]->get_npe() > _min_npe/2.0 )
      seeds_in_event.push_back(evt.digits()[dig]);
  }
  return seeds_in_event;
}

void SciFiClusterRec::make_clusters(SciFiEvent &evt, std::vector<SciFiDigit*> &seeds) const {
  size_t seeds_size = seeds.size();
  for ( size_t i = 0; i < seeds_size; i++ ) {
    if ( !(seeds[i]->is_used()) ) {
      SciFiDigit* neigh = NULL;
      SciFiDigit* seed = seeds[i];

      double pe = seed->get_npe();
      // Look for a neighbour.
      for ( size_t j = i+1; j < seeds_size; ++j ) {
        if ( are_neighbours(seeds[i], seeds[j]) ) {
          neigh = seeds[j];
        }
      }
      // If there is a neighbour, sum npe contribution.
      if ( neigh ) {
        pe += neigh->get_npe();
      }
      // Save cluster if it's above npe cut.
      if ( pe > _min_npe ) {
        SciFiCluster* clust = new SciFiCluster(seed);
        if ( neigh ) {
          clust->add_digit(neigh);
        }
        process_cluster(clust);
        evt.add_cluster(clust);
      }
    }
  } // ends loop over seeds
}

void SciFiClusterRec::process_cluster(SciFiCluster *clust) const {
  int tracker = clust->get_tracker();
  int station = clust->get_station();
  int plane   = clust->get_plane();

  int id =  3*(station-1) + (plane+1);
  SciFiPlaneMap::const_iterator iterator = _geometry_map.find(tracker)->second.Planes.find(id);
//  iterator = _geometry_map.find(id);
  // Throw if the plane isn't found.
//  if ( iterator == _geometry_map.end() ) {
  if ( iterator == _geometry_map.find(tracker)->second.Planes.end() ) {
    throw(Exception(Exception::nonRecoverable,
    "Failed to find SciFi plane in _geometry_map.",
    "SciFiClusterRec::process_cluster"));
  }
  id = ( tracker == 0 ? -id : id );
  clust->set_id(id);
  SciFiPlaneGeometry this_plane = (*iterator).second;
  ThreeVector plane_direction   = this_plane.Direction;
  ThreeVector plane_position    = this_plane.Position;
  double Pitch                  = this_plane.Pitch;
  double CentralFibre           = this_plane.CentralFibre;
  // alpha is the distance to the central fibre.
  double alpha   = clust->get_channel() - CentralFibre;
  double dist_mm = Pitch * (7.0 / 2.0) * alpha;

  ThreeVector perp = plane_direction.Orthogonal();
  ThreeVector position = dist_mm * perp + plane_position;

  clust->set_direction(plane_direction);
  clust->set_position(position);
  clust->set_alpha(dist_mm);
}

bool SciFiClusterRec::are_neighbours(SciFiDigit *seed_i, SciFiDigit *seed_j) const {
  bool neigh = false;

  if ( !(seed_j->is_used()) && // seed is unused
       seed_j->get_spill() == seed_i->get_spill() && // same spill
       seed_j->get_event() == seed_i->get_event() && // same event
       seed_j->get_tracker() == seed_i->get_tracker() && // same tracker
       seed_j->get_station() == seed_i->get_station() && // same station
       seed_j->get_plane() == seed_i->get_plane() && // seeds belong to same plane
       abs(seed_j->get_channel() - seed_i->get_channel()) < 2.0 ) { // and are neighbours
    neigh = true;
  }

  return neigh;
}

} // ~namespace MAUS
