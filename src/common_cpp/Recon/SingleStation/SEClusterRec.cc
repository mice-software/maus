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
#include "src/common_cpp/Recon/SingleStation/SEClusterRec.hh"

// namespace MAUS {

SEClusterRec::SEClusterRec() {}

SEClusterRec::SEClusterRec(int cluster_exception, double min_npe)
                                 :_size_exception(cluster_exception), _min_npe(min_npe) {}

SEClusterRec::~SEClusterRec() {}

void SEClusterRec::process(SEEvent &evt, std::vector<const MiceModule*> modules) {
  // Create and fill the seeds vector.
  std::vector<SEDigit*>   seeds;
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
      SEDigit* neigh = NULL;
      SEDigit* seed = seeds[i];

      int plane   = seed->get_plane();
      int fibre   = seed->get_channel();
      double pe   = seed->get_npe();
      // Look for a neighbour.
      for ( int j = i+1; j < seeds_size; j++ ) {
        if ( !seeds[j]->is_used() && seeds[j]->get_plane() == plane &&
             abs(seeds[j]->get_channel() - fibre) < 2 ) {
          neigh = seeds[j];
        }
      }
      // If there is a neighbour, sum npe contribution.
      //if ( neigh ) {
      //  pe += neigh->get_npe();
      //}
      // Save cluster if it's above npe cut.
      if ( pe > _min_npe ) {
        SECluster* clust = new SECluster(seed);
        //if ( neigh ) {
        //  clust->add_digit(neigh);
        //}
        construct(clust, modules);
        evt.add_cluster(clust);
      }
    }
  } // ends loop over seeds
}

void SEClusterRec::construct(SECluster *clust, std::vector<const MiceModule*> modules) {
  Hep3Vector perp(-1., 0., 0.);
  Hep3Vector dir(0, 1, 0);
  const MiceModule* this_plane = NULL;
  // std::cout << "Modules size: "  << modules.size() << std::endl;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); j++ ) {
    // Find the right module
    if ( modules[j]->propertyInt("Plane") ==
         clust->get_plane() ) {
         // Save the module
      this_plane = modules[j];
    }
  }
  // std::cout << clust->get_plane() << std::endl;

  assert(this_plane != NULL);

  dir  *= this_plane->globalRotation();
  perp *= this_plane->globalRotation();
  double Pitch = this_plane->propertyDouble("Pitch");
  double CentralFibre = this_plane->propertyDouble("CentralFibre");

  double dist_mm = Pitch * 7.0 / 2.0 * (clust->get_channel() - CentralFibre);

  Hep3Vector position = dist_mm * perp;// + this_plane->globalPosition();

  // The position in the (x, y) plane.
  clust->set_position(position);
  clust->set_direction(dir);
}

// } // ~namespace MAUS
