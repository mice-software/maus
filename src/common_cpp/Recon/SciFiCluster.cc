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
#include "src/common_cpp/Recon/SciFiCluster.hh"

SciFiCluster::SciFiCluster(): _used(false) {}

SciFiCluster::SciFiCluster(SciFiDigit *digit) {
  _used = false;

  _tracker    = digit->get_tracker();

  _station    = digit->get_station();

  _plane      = digit->get_plane();

  _channel_w  = digit->get_channel();

  _npe        = digit->get_npe();

  _time       = digit->get_time();

  digit->set_used();
}

SciFiCluster::~SciFiCluster() {}

void SciFiCluster::add_digit(SciFiDigit* neigh) {
  neigh->set_used();

  _npe += neigh->get_npe();
  _channel_w /= 2.0;
  _channel_w += (neigh->get_channel())/2.0;
}

void SciFiCluster::construct(std::vector<const MiceModule*> modules) {
  Hep3Vector perp(-1., 0., 0.);
  Hep3Vector dir(0, 1, 0);

  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); j++ ) {
    // Find the right module
    if ( modules[j]->propertyExists("Tracker", "int") &&
         modules[j]->propertyExists("Station", "int") &&
         modules[j]->propertyExists("Plane", "int")  &&
         modules[j]->propertyInt("Tracker") ==
         _tracker &&
         modules[j]->propertyInt("Station") ==
         _station &&
         modules[j]->propertyInt("Plane") ==
         _plane ) {
         // Save the module
      this_plane = modules[j];
    }
  }

  assert(this_plane != NULL);

  dir  *= this_plane->globalRotation();
  perp *= this_plane->globalRotation();
  double Pitch = this_plane->propertyDouble("Pitch");
  double CentralFibre = this_plane->propertyDouble("CentralFibre");

  double dist_mm = Pitch * 7.0 / 2.0 * (_channel_w - CentralFibre);

  _position  = dist_mm * perp + this_plane->globalPosition();
  _direction = dir;
}
