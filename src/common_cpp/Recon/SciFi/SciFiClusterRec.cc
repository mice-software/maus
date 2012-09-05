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
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4RotationMatrix.hh"

namespace MAUS {

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

      // int tracker = seed->get_tracker();
      // int station = seed->get_station();
      // int plane   = seed->get_plane();
      // int fibre   = seed->get_channel();
      double pe   = seed->get_npe();
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
        construct(clust, modules);
        evt.add_cluster(clust);
      }
    }
  } // ends loop over seeds
}

void SciFiClusterRec::construct(SciFiCluster *clust, std::vector<const MiceModule*> modules) {
  ThreeVector perp(-1., 0., 0.);
  ThreeVector dir(0, 1, 0);
  int tracker = clust->get_tracker();
  int station = clust->get_station();
  int plane   = clust->get_plane();
/*
  const MiceModule* tracker_solenoid_0 = NULL;
  for ( unsigned int j = 0; !tracker_solenoid_0 && j < modules.size(); j++ ) {
    // Find the right module
    if ( modules[j]->propertyExists("KalmanSolenoid", "int") &&
         modules[j]->propertyExists("KalmanSolenoidNumber", "int") &&
         modules[j]->propertyInt("KalmanSolenoid") ==1 &&
         modules[j]->propertyInt("KalmanSolenoidNumber") ==0 ) {
         // Save the module
      tracker_solenoid_0 = modules[j];
    }
  }
  assert(tracker_solenoid_0 != NULL);
  const MiceModule* tracker_solenoid_1 = NULL;
  for ( unsigned int j = 0; !tracker_solenoid_1 && j < modules.size(); j++ ) {
    // Find the right module
    if ( modules[j]->propertyExists("KalmanSolenoid", "int") &&
         modules[j]->propertyExists("KalmanSolenoidNumber", "int") &&
         modules[j]->propertyInt("KalmanSolenoid") ==1 &&
         modules[j]->propertyInt("KalmanSolenoidNumber") ==1 ) {
         // Save the module
      tracker_solenoid_1 = modules[j];
    }
  }
  assert(tracker_solenoid_1 != NULL);
*/

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

  //dir  *= this_plane->globalRotation();
  //perp *= this_plane->globalRotation();
  //ThreeVector plane_position;
/*
  if ( clust->get_tracker() == 0 ) {
    dir  *= this_plane->relativeRotation(tracker_solenoid_0);
    perp *= this_plane->relativeRotation(tracker_solenoid_0);
    plane_position = this_plane->relativePosition(tracker_solenoid_0);
  } else if ( clust->get_tracker() == 1 ) {
    dir  *= this_plane->relativeRotation(tracker_solenoid_1);
    perp *= this_plane->relativeRotation(tracker_solenoid_1);
    plane_position = this_plane->relativePosition(tracker_solenoid_1);
  }
*/
  CLHEP::HepRotation zflip;
  const Hep3Vector rowx(-1., 0, 0);
  const Hep3Vector rowy(0, 1., 0);
  const Hep3Vector rowz(0, 0, -1.);
  zflip.setRows(rowx, rowy, rowz);

  G4RotationMatrix trot(this_plane->globalRotation());

/*
  size_t found;
  found=doubletName.find("Tracker1");
  if (found!=G4String::npos)
    (*trot) = (*trot)*zflip;
*/

  // this is the rotation of the fibre array
  //(*trot) = (*trot)*zflip;
  // G4RotationMatrix* trot = new G4RotationMatrix();
  //Hep3Vector dir(0, 1, 0);
  //dir *= *(trot);

  if ( tracker == 0 ) {
    trot= trot*zflip;
    dir  *= trot;
    perp *= trot;
    // dir.rotateY(pi*rad);
    // perp.rotateY(pi*rad);
    //plane_position = this_plane->globalPosition();
  } else if ( tracker == 1 ) {
    dir  *= trot;
    perp *= trot;
    //dir.rotateY(pi*rad);
    //perp.rotateY(pi*rad);
    //plane_position = this_plane->globalPosition();
  }

  double Pitch = this_plane->propertyDouble("Pitch");
  double CentralFibre = this_plane->propertyDouble("CentralFibre");

  double dist_mm = Pitch * 7.0 / 2.0 * (clust->get_channel() - CentralFibre);

  ThreeVector plane_position = this_plane->globalPosition();
  ThreeVector position = dist_mm * perp + plane_position;

  ThreeVector reference = get_reference_frame_pos(clust->get_tracker(), modules);

  ThreeVector tracker_ref_frame_pos = position - reference;

  //ThreeVector tracker_ref_frame_pos;
  if ( clust->get_tracker() == 0 ) {
    tracker_ref_frame_pos = - (position - reference);
  } else {
    tracker_ref_frame_pos = position - reference;
  }

  clust->set_position(tracker_ref_frame_pos);
  clust->set_direction(dir);
  clust->set_relative_position(tracker_ref_frame_pos);

  //clust->set_position(position);
  //clust->set_relative_position(position);
  // Set relative position & channel number for the Kalman Filter.
  // This is the position of the cluster relatively to station 1 of the tracker (0 or 1)
  // with the displacement of the station centre subtracted.
  // ThreeVector relative_position = position - this_plane->globalPosition();
  double alpha = clust->get_channel() - CentralFibre;
  // clust->set_relative_position(relative_position);
  if ( tracker == 1 ) {
    alpha = -alpha;
  }
  clust->set_alpha(alpha);
  int id = 15*tracker + 3*(station-1) + (plane);
  clust->set_id(id);
/*
   std::cerr << "----------Clustering--------- \n"
            << "Site ID: " << id << "\n"
            << "Tracker " << tracker << ", station " << station << ", plane " << plane << "\n"
            << "Fibre direction: " << dir << "\n"
            << "Position: " << tracker_ref_frame_pos << "\n";
*/
}

ThreeVector SciFiClusterRec::get_reference_frame_pos(int tracker,
                                                    std::vector<const MiceModule*> modules) {
  const MiceModule* reference_plane = NULL;

  // Reference plane is plane 0, station 1 of current tracker.
  int station = 1;
  int plane   = 0;

  for ( unsigned int j = 0; !reference_plane && j < modules.size(); j++ ) {
    // Find the right module
    if ( modules[j]->propertyExists("Tracker", "int") &&
         modules[j]->propertyExists("Station", "int") &&
         modules[j]->propertyExists("Plane", "int")  &&
         modules[j]->propertyInt("Tracker") == tracker &&
         modules[j]->propertyInt("Station") == station &&
         modules[j]->propertyInt("Plane")   == plane ) {
         // Save the module
      reference_plane = modules[j];
    }
  }
  assert(reference_plane != NULL);
  ThreeVector reference_pos =  reference_plane->globalPosition();

  return reference_pos;
}

bool SciFiClusterRec::are_neighbours(SciFiDigit *seed_i, SciFiDigit *seed_j) {
  bool neigh = false;

  if ( !seed_j->is_used() && // seed is unused
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
