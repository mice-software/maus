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

#include "src/common_cpp/Recon/SciFiHit.hh"

SciFiHit::SciFiHit(): _used(false) {}

SciFiHit::SciFiHit(int tracker, int station, int plane, int fibre, double edep, double time)
                  :_tracker(tracker), _station(station), _plane(plane),
                   _fibre(fibre), _edep(edep), _time(time), _used(false) {
}

SciFiHit::~SciFiHit() {}
