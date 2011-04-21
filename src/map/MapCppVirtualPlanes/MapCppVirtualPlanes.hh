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
 *
 */

/** @class MapCppVirtualPlanes
 *  Mapper to output particle data at user-specified planes independent of
 *  Geant4 geometry
 *
 *  In accelerator physics we like to study particle distributions as a function
 *  of path length (distance along the accelerator). In straight systems this is
 *  just position in z, but in curved systems we like to work in some rotated
 *  coordinate systems (e.g. in the dipoles)
 *
 *  Additionally, there are some philosophically interesting uses for particle
 *  data as a function of time or proper time. In general, we want particle data
 *  on a surface with some fixed independent variable.
 *
 *  The way we get this data then is to look for steps in the geant4 data that
 *  straddle one of our virtual planes. Then integrate (track) off the plane to
 *  the virtual plane. We track from both the PreStepPoint forwards and the
 *  PostStepPoint backwards. We take the average (thus getting average of any
 *  stochastic processes on the step).
 */

#ifndef _SRC_MAP_MAPCPPVIRTUALPLANES_MAPCPPVIRTUALPLANES_H_
#define _SRC_MAP_MAPCPPVIRTUALPLANES_MAPCPPVIRTUALPLANES_H_

#include <string>

class MapCppVirtualPlanes {
 public:
  /** @brief Set up the virtual planes
   */
  bool birth(std::string argJsonConfigDocument);

  bool death();

  std::string process();

  MapCppVirtualPlanes();

 private:
  VirtualPlaneManager* _planes;
}

#endif

