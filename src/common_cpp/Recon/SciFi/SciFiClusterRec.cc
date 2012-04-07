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

// namespace MAUS {

SciFiClusterRec::SciFiClusterRec() {}

SciFiClusterRec::SciFiClusterRec(int cluster_exception, double min_npe)
                                 :_size_exception(cluster_exception), _min_npe(min_npe) {}

SciFiClusterRec::~SciFiClusterRec() {}

void SciFiClusterRec::process(SciFiEvent &evt, std::vector<const MiceModule*> modules) {
  // Create and fill the seeds vector.
  std::vector<SciFiDigit*>   seeds;
  for ( unsigned int dig = 0; dig < evt.digits().size(); dig++ ) {
    if ( evt.digits()[dig]->get_npe() > _min_npe/2.0 )
      seeds.push_back(evt.digits()[dig]);
  }

  // Get the number of clusters. If too large, abort reconstruction.
  int seeds_size = seeds.size();
  if ( seeds_size > _size_exception ) {
    return;
  }

  // Sort seeds so that we use higher npe first.

  for ( int i = 0; i < seeds_size; i++ ) {
    if ( !seeds[i]->is_used() ) {
      SciFiDigit* neigh = NULL;
      SciFiDigit* seed = seeds[i];

      int tracker = seed->get_tracker();
      int station = seed->get_station();
      int plane   = seed->get_plane();
      int fibre   = seed->get_channel();
      double pe   = seed->get_npe();
      // Look for a neighbour.
      for ( int j = i+1; j < seeds_size; j++ ) {
        if ( !seeds[j]->is_used() && seeds[j]->get_tracker() == tracker &&
             seeds[j]->get_station() == station && seeds[j]->get_plane()   == plane &&
             abs(seeds[j]->get_channel() - fibre) < 2 ) {
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
        construct(clust, modules);
        evt.add_cluster(clust);
      }
    }
  } // ends loop over seeds
}

void SciFiClusterRec::construct(SciFiCluster *clust, std::vector<const MiceModule*> modules) {
  Hep3Vector perp(-1., 0., 0.);
  Hep3Vector dir(0, 1, 0);
  int tracker = clust->get_tracker();
  int station = clust->get_station();
  int plane   = clust->get_plane();

  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); j++ ) {
    // Find the right module
    if ( modules[j]->propertyExists("Tracker", "int") &&
         modules[j]->propertyExists("Station", "int") &&
         modules[j]->propertyExists("Plane", "int")  &&
         modules[j]->propertyInt("Tracker") ==
         tracker &&
         modules[j]->propertyInt("Station") ==
         station &&
         modules[j]->propertyInt("Plane") ==
         plane ) {
         // Save the module
      this_plane = modules[j];
    }
  }

  assert(this_plane != NULL);

  dir  *= this_plane->globalRotation();
  perp *= this_plane->globalRotation();
  double Pitch = this_plane->propertyDouble("Pitch");
  double CentralFibre = this_plane->propertyDouble("CentralFibre");

  double dist_mm = Pitch * 7.0 / 2.0 * (clust->get_channel() - CentralFibre);

  Hep3Vector position = dist_mm * perp + this_plane->globalPosition();

  Hep3Vector reference = get_reference_frame_pos(clust->get_tracker(), modules);

  Hep3Vector tracker_ref_frame_pos;
  if ( clust->get_tracker() == 0 ) {
    tracker_ref_frame_pos = position - reference;
  } else {
    tracker_ref_frame_pos = - (position - reference);
  }

  clust->set_position(tracker_ref_frame_pos);
  clust->set_direction(dir);
  // Set relative position & channel number for the Kalman Filter.
  Hep3Vector relative_position = position - this_plane->globalPosition();
  double channel = clust->get_channel() - CentralFibre;
  clust->set_relative_position(relative_position);
  clust->set_alpha(channel);
  int id = 15*tracker + 3*(station-1) + (plane);
  clust->set_id(id);
}

Hep3Vector SciFiClusterRec::get_reference_frame_pos(int tracker,
                                                    std::vector<const MiceModule*> modules) {
  const MiceModule* reference_plane = NULL;
  for ( unsigned int j = 0; !reference_plane && j < modules.size(); j++ ) {
    // Find the right module
    if ( modules[j]->propertyExists("Tracker", "int") &&
         modules[j]->propertyExists("Station", "int") &&
         modules[j]->propertyExists("Plane", "int")  &&
         modules[j]->propertyInt("Tracker") == tracker &&
         modules[j]->propertyInt("Station") == 1 &&
         modules[j]->propertyInt("Plane")   == 0 ) {
         // Save the module
      reference_plane = modules[j];
    }
  }
  assert(reference_plane != NULL);
  Hep3Vector reference_pos =  reference_plane->globalPosition();
  return reference_pos;
}
// } // ~namespace MAUS
