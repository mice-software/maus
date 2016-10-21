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
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALPRIMARYCHAIN_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALPRIMARYCHAIN_HH_

// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "TObject.h"
#include "TRefArray.h"
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "Utils/Exception.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/SpacePoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

class PrimaryChain : public TObject {
 public:

  /// Default constructor - initialises to 0/NULL
  PrimaryChain();

  /// Copy constructor - any pointers are deep copied
  PrimaryChain(const PrimaryChain &primary_chain);

  /// Constructor setting mapper name - everything else initialises to
  /// 0/NULL
  explicit PrimaryChain(std::string mapper_name);

  /// Destructor
  virtual ~PrimaryChain();

  /// Assignment operator
  PrimaryChain& operator=(const PrimaryChain &primary_chain);

  void AddLRSpacePoint(MAUS::DataStructure::Global::SpacePoint* spacepoint);

  void AddLRTrack(MAUS::DataStructure::Global::Track* track);

  void AddTrack(MAUS::DataStructure::Global::Track* track);

  std::vector<MAUS::DataStructure::Global::SpacePoint*> GetLRSpacePoints() const;

  std::vector<MAUS::DataStructure::Global::Track*> GetLRTracks() const;

  std::vector<MAUS::DataStructure::Global::Track*> GetTracks() const;

  std::vector<MAUS::DataStructure::Global::Track*> GetTracks(std::string mapper_name) const;

  TRefArray* get_lr_spacepoints() const { return _lr_spacepoints; }

  void set_lr_spacepoints(TRefArray* lr_spacepoints) {
    _lr_spacepoints = lr_spacepoints; }

  TRefArray* get_lr_tracks() const { return _lr_tracks; }

  void set_lr_tracks(TRefArray* lr_tracks) { _lr_tracks = lr_tracks; }

  TRefArray* get_tracks() const { return _tracks; }

  void set_tracks(TRefArray* tracks) { _tracks = tracks; }

  // Getters and Setters for the member variables
  /// Set the name for the mapper which produced the result, #_mapper_name.
  void set_mapper_name(std::string mapper_name);
  /// Get the name for the mapper which produced the result, #_mapper_name.
  std::string get_mapper_name() const;


 private:

  /// The name of the mapper that last edited this object
  std::string _mapper_name;

  /// All SpacePoints in the Event from Local Reconstruction (TOFs, CKovs, KL)
  TRefArray* _lr_spacepoints;

  /// All Tracks in the Event from Local Reconstruction (Trackers, EMR)
  TRefArray* _lr_tracks;

  /// All Tracks created in the track-matching, PID, and track-fitting
  TRefArray* _tracks;


  MAUS_VERSIONED_CLASS_DEF(PrimaryChain);
}; // ~class PrimaryChain

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
