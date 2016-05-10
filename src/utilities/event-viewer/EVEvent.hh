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

/* Container class to hold all information of interest from one particular event */

#ifndef _EVEvent_hh
#define _EVEvent_hh

#include <vector>

#include "DataStructure/ThreeVector.hh"

namespace EventViewer {

class EVEvent {
 public:
  EVEvent();
  virtual ~EVEvent();

  void Reset();

  // all these structures could be set to private with addition of appropriate getters and setters
  int runNumber;
  int spillNumber;
  int eventNumber;

  // all these structures could be set to private with addition of appropriate getters and setters
  // also they could be changed to vector< vector<double> > to acommodate more than one track(?)
  std::vector<MAUS::ThreeVector> tofPoints = std::vector<MAUS::ThreeVector>(3);

  std::vector<MAUS::ThreeVector> scifiUSTrackerPoints = std::vector<MAUS::ThreeVector>(5);
  std::vector<MAUS::ThreeVector> scifiUSTrackerPointsMomenta = std::vector<MAUS::ThreeVector>(5);
  std::vector<MAUS::ThreeVector> scifiUSTrackerSpacePoints;
  std::vector<MAUS::ThreeVector> scifiUSTrackerStraightTrackPoints =
    std::vector<MAUS::ThreeVector>(2);
  std::vector<std::vector<std::vector<double> > > scifiUSTrackerClusters =
    std::vector<std::vector<std::vector<double> > >(3, std::vector<std::vector<double> >(2));

  std::vector<MAUS::ThreeVector> scifiDSTrackerPoints = std::vector<MAUS::ThreeVector>(5);
  std::vector<MAUS::ThreeVector> scifiDSTrackerPointsMomenta = std::vector<MAUS::ThreeVector>(5);
  std::vector<MAUS::ThreeVector> scifiDSTrackerSpacePoints;
  std::vector<MAUS::ThreeVector> scifiDSTrackerStraightTrackPoints =
    std::vector<MAUS::ThreeVector>(2);
  std::vector<std::vector<std::vector<double> > > scifiDSTrackerClusters =
    std::vector<std::vector<std::vector<double> > >(3, std::vector<std::vector<double> >(2));
};

} // ~namespace EventViewer

#endif
