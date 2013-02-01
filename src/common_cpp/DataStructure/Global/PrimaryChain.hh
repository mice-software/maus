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

/** @class MAUS::DataStructure::Global::PrimaryChain
 *  @ingroup globalrecon
 *  @brief The reconstructed chain for a single primary particle.
 *
 *  A hypothesised reconstructed chain for a single primary particle.
 *  This consists of a series of tracks, with one primary and the
 *  possibility of multiple daughters.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALPRIMARYCHAIN_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALPRIMARYCHAIN_HH_

// C++ headers
#include <string>
#include <map>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "Interface/Squeal.hh"
#include "DataStructure/GlobalTrack.hh"
#include "DataStructure/TRefTrackPair.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

class PrimaryChain {
 public:

  /// Default constructor - initialises to 0/NULL
  PrimaryChain();

  /// Copy constructor - any pointers are deep copied
  PrimaryChain(const PrimaryChain &primarychain);

  /// Constructor setting mapper name - everything else initialises to
  /// 0/NULL
  PrimaryChain(std::string mapper_name);

  /// Destructor
  virtual ~PrimaryChain();

  /// Assignment operator - any pointers are deep copied
  PrimaryChain& operator=(const PrimaryChain &primarychain);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  PrimaryChain* Clone() const;

  // Methods for adding and removing tracks, while tracking the parents

  /// Add a MAUS::DataStructure::Global::Track, tracking the location of the
  /// parent track.
  /// @param[in] track  The track being added to the primary chain
  /// @param[in] parent The track's parent.  We require parent to
  ///                   already be a member of the primarychain.
  bool AddTrack(MAUS::GlobalTrack* track,
                MAUS::GlobalTrack* parent);

  /// Add a primary MAUS::DataStructure::Global::Track, with the parent
  /// reference set to NULL.
  /// @param[in] track  The track being added to the primary chain
  bool AddPrimaryTrack(MAUS::GlobalTrack* track);

  /// Remove all reference of the provided track.  Returns false if a)
  /// track is not a member of the chain, or b) track has daughter
  /// particles stored in the chain.
  bool RemoveTrack(MAUS::GlobalTrack* track);

  /// Checks whether track is stored in the chain.
  bool HasTrack(MAUS::GlobalTrack* track);

  /// Checks whether track is a parent in the chain.
  bool HasTrackAsParent(MAUS::GlobalTrack* parent);

  /// Checks if a track is a primary track
  bool IsPrimaryTrack(MAUS::GlobalTrack* track);

  /// Returns a track's parent, according to this primary chain
  MAUS::GlobalTrack* GetTrackParent(MAUS::GlobalTrack* track);

  /// Prepare a list of a track's daughter tracks.
  std::vector<MAUS::GlobalTrack*>
  GetTrackDaughters(MAUS::GlobalTrack* track);

  /// Empties the #_tracks object
  void ClearTracks();

  // Getters and Setters for the member variables
  /// Set the name for the mapper which produced the result, #_mapper_name.
  void set_mapper_name(std::string mapper_name) {
    _mapper_name = mapper_name;
  }
  /// Get the name for the mapper which produced the result, #_mapper_name.
  std::string get_mapper_name() const {
    return _mapper_name;
  }

  void set_track_parent_pairs(
      std::vector<MAUS::DataStructure::Global::TRefTrackPair*>* tracks) {
    _tracks = tracks;
  }

  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>*
  get_track_parent_pairs() const {
    return _tracks;
  }

  /// Set the #_goodness_of_fit paramter
  void set_goodness_of_fit(double goodness_of_fit) {
    _goodness_of_fit = goodness_of_fit;
  }
  /// Get the #_goodness_of_fit paramter
  double get_goodness_of_fit() const {
    return _goodness_of_fit;
  }

 private:

  /// The name of the mapper which produced this result. This can be
  /// used to separate different stages of the global reconstruction
  /// if required.
  std::string _mapper_name;
  
  /// Map of tracks associated with this primary chain, and a
  /// reference to their parent tracks if applicable (NULL if track is
  /// believed to be a primary particle).
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*> *_tracks;

  /// The goodness of fit parameter.
  double _goodness_of_fit;

  ClassDef(PrimaryChain, 1)
}; // ~class PrimaryChain

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
