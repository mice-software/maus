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
#include <algorithm>

#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4RotationMatrix.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"

namespace MAUS {

SciFiClusterRec::SciFiClusterRec():_size_exception(0.),
                                   _min_npe(0.) {}

// To be removed soon.
SciFiClusterRec::SciFiClusterRec(int cluster_exception, double min_npe,
                std::vector<const MiceModule*> modules):
                                   _size_exception(0.),
                                   _min_npe(0.),
                                   _modules(modules) {}

SciFiClusterRec::~SciFiClusterRec() {}

bool sort_by_npe(SciFiDigit *a, SciFiDigit *b ) {
  return ( a->get_npe() > b->get_npe() );
}

void SciFiClusterRec::initialise() {
  Json::Value *json = Globals::GetConfigurationCards();

  std::string filename;
  filename = (*json)["reconstruction_geometry_filename"].asString();
  MiceModule* module;
  module = new MiceModule(filename);

  _size_exception = (*json)["SciFiClustExcept"].asInt();
  _min_npe = (*json)["SciFiNPECut"].asDouble();
  _modules = module->findModulesByPropertyString("SensitiveDetector", "SciFi");
}

void SciFiClusterRec::process(SciFiEvent &evt) {
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

std::vector<SciFiDigit*> SciFiClusterRec::get_seeds(SciFiEvent &evt) {
  std::vector<SciFiDigit*> seeds_in_event;
  for ( unsigned int dig = 0; dig < evt.digits().size(); dig++ ) {

    if ( evt.digits()[dig]->get_npe() > _min_npe/2.0 )
      seeds_in_event.push_back(evt.digits()[dig]);
  }
  return seeds_in_event;
}

void SciFiClusterRec::make_clusters(SciFiEvent &evt, std::vector<SciFiDigit*>   &seeds) {
  int seeds_size = seeds.size();
  for ( int i = 0; i < seeds_size; i++ ) {
    if ( !(seeds[i]->is_used()) ) {
      SciFiDigit* neigh = NULL;
      SciFiDigit* seed = seeds[i];

      double pe = seed->get_npe();
      // Look for a neighbour.
      for ( int j = i+1; j < seeds_size; j++ ) {
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

void SciFiClusterRec::process_cluster(SciFiCluster *clust) {
  // Get the MiceModule of the plane...
  int tracker = clust->get_tracker();
  int station = clust->get_station();
  int plane   = clust->get_plane();
  const MiceModule* this_plane = NULL;
  this_plane = find_plane(tracker, station, plane);
  assert(this_plane != NULL);
  // compute it's direction & position in TRF...
  ThreeVector trf_dir(0., 1., 0.);
  ThreeVector trf_pos(0., 0., 0.);
  double alpha;
  construct(clust, this_plane, trf_dir, trf_pos, alpha);

  clust->set_direction(trf_dir);
  clust->set_position(trf_pos);

  clust->set_alpha(alpha);
  int id = 15*tracker + 3*(station-1) + (plane);
  clust->set_id(id);
}

void SciFiClusterRec::construct(SciFiCluster *clust,
                                const MiceModule* this_plane,
                                ThreeVector &dir,
                                ThreeVector &tracker_ref_frame_pos,
                                double &alpha) {
  ThreeVector perp(-1., 0., 0.);

  CLHEP::HepRotation zflip;
  const Hep3Vector rowx(-1., 0., 0.);
  const Hep3Vector rowy(0., 1., 0.);
  const Hep3Vector rowz(0., 0., -1.);
  zflip.setRows(rowx, rowy, rowz);
  G4RotationMatrix trot(this_plane->globalRotation());
  // Rotations of the planes in the Tracker Reference Frame.
  if ( clust->get_tracker() == 0 ) {
    trot= trot*zflip;
    dir  *= trot;
    perp *= trot;
  } else if ( clust->get_tracker() == 1 ) {
    dir  *= trot;
    perp *= trot;
  }

  double Pitch = this_plane->propertyDouble("Pitch");
  double CentralFibre = this_plane->propertyDouble("CentralFibre");
  double dist_mm = Pitch * 7.0 / 2.0 * (clust->get_channel() - CentralFibre);
  ThreeVector plane_position = this_plane->globalPosition();
  ThreeVector position = dist_mm * perp + plane_position;
  ThreeVector reference = get_reference_frame_pos(clust->get_tracker());

  tracker_ref_frame_pos = position - reference;
  tracker_ref_frame_pos.setX(0.);
  tracker_ref_frame_pos.setY(0.);
  // ThreeVector tracker_ref_frame_pos;
  if ( clust->get_tracker() == 0 ) {
    tracker_ref_frame_pos = - (position - reference);
  } else if ( clust->get_tracker() == 1 ) {
    tracker_ref_frame_pos = position - reference;
  }

  alpha = clust->get_channel() - CentralFibre;
  if ( clust->get_tracker() == 1 ) {
    alpha = -alpha;
  }
}

const MiceModule* SciFiClusterRec::find_plane(int tracker, int station, int plane) {
  const MiceModule* scifi_plane = NULL;
  for ( unsigned int j = 0; !scifi_plane && j < _modules.size(); j++ ) {
    // Find the right module
    if ( _modules[j]->propertyExists("Tracker", "int") &&
         _modules[j]->propertyExists("Station", "int") &&
         _modules[j]->propertyExists("Plane", "int")  &&
         _modules[j]->propertyInt("Tracker") ==
         tracker &&
         _modules[j]->propertyInt("Station") ==
         station &&
         _modules[j]->propertyInt("Plane") ==
         plane ) {
         // Save the module
      scifi_plane = _modules[j];
    }
  }
  return scifi_plane;
}

ThreeVector SciFiClusterRec::get_reference_frame_pos(int tracker) {
  // Reference plane is plane 0, station 1 of current tracker.
  int station = 1;
  int plane   = 0;
  const MiceModule* reference_plane = NULL;
  reference_plane = find_plane(tracker, station, plane);

  assert(reference_plane != NULL);
  ThreeVector reference_pos =  reference_plane->globalPosition();

  return reference_pos;
}

bool SciFiClusterRec::are_neighbours(SciFiDigit *seed_i, SciFiDigit *seed_j) {
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
