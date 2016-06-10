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

#include "src/utilities/event-viewer/EVEvent.hh"

namespace EventViewer {

EVEvent::EVEvent() {
  // Do Nothing
}

EVEvent::~EVEvent() {
  // Do Nothing
}

void EVEvent::Reset() {
  MAUS::ThreeVector nullVector(5673390, 5673390, 5673390);
  std::vector<std::vector<double>> stdNullVector(2, std::vector<double>(0));

  // - TOF data containers
  for (auto& it : tofPoints)
    it = nullVector;

  // - US tracker data containters
  for (auto& it : scifiUSTrackerPoints)
    it = nullVector;

  for (auto& it : scifiUSTrackerPointsMomenta)
    it = nullVector;

  scifiUSTrackerSpacePoints.clear();

  for (auto& it : scifiUSTrackerStraightTrackPoints)
    it = nullVector;

  for (auto& it : scifiUSTrackerClusters)
    it = stdNullVector;

  // - DS tracker data containers
  for (auto& it : scifiDSTrackerPoints)
    it = nullVector;

  for (auto& it : scifiDSTrackerPointsMomenta)
    it = nullVector;

  scifiDSTrackerSpacePoints.clear();

  for (auto& it : scifiDSTrackerStraightTrackPoints)
    it = nullVector;

  for (auto& it : scifiDSTrackerClusters)
    it = stdNullVector;
}

} // ~namespace EventViewer
