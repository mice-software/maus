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

/** @class MAUS::recon::global::PrimaryChain
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
#include <vector>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "GlobalTrack.hh"

namespace MAUS {
namespace recon {
namespace global {

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

  /// Add a MAUS::recon::global::Track, tracking the location of the
  /// parent track.
  /// @param[in] track  The track being added to the primary chain
  /// @param[in] parent The index of track's parent
  ///                   MAUS::recon::global::Track. -1 for a primary track.
  int AddTrack(MAUS::recon::global::Track* track, int parent);

  /// Remove all reference of the provided track.  Returns false if a)
  /// track is not a member of the chain, or b) track has daughter
  /// particles stored in the chain.
  bool RemoveTrack(MAUS::recon::global::Track* track);

  /// Checks whether track is stored in the chain.
  bool HasTrack(MAUS::recon::global::Track* track);

  /// Checks whether trackid is a valid index for a valid track.
  bool HasTrackFromInt(int trackid);

  /// Determines the index for a given track pointer, returns -2 if
  /// track not found.
  int GetTrackId(MAUS::recon::global::Track* track);

  /// Determines a track's parent, according to this primary chain
  int GetTrackParentId(MAUS::recon::global::Track* track);

  /// Determines a track's parent, according to this primary chain  
  int GetTrackParentIdFromInt(int index);

  /// Prepare a list of a track's daughter tracks.
  std::vector<int> GetTrackDaughterIds(MAUS::recon::global::Track* track);

  /// Prepare a list of a track's daughter tracks.
  std::vector<int> GetTrackDaughterIdsFromInt(int index);

  /// Returns the track matching a specified index.
  const MAUS::recon::global::Track* GetTrackFromInt(int index);

  /// Rearranges the tracks in the chain, in case tracks have been
  /// removed.  Correctly handles indices internally, but provides no
  /// functionality for replacing external indices.  Really, it would
  /// be a lot better if we just never deleted Tracks from a
  /// PrimaryChain.
  bool CleanUpTracks();

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

  /// Directly sets the internal #_tracks vector
  void set_tracks(std::vector<MAUS::GlobalTrack*> * tracks){
    _tracks = tracks;
  }
  /// Direct access to the internal #_tracks vector
  std::vector<MAUS::GlobalTrack*> * get_tracks() const {
    return _tracks;
  }

  /// Directly sets the internal #_parents vector
  void set_parents(std::vector<int> parents){
    _parents = parents;
  }
  /// Direct access to the #_parents vector
  std::vector<int> get_parents() const {
    return _parents;
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
  
  /// List of tracks associated with this primary chain, to be used
  /// with #_parents, which contains the location of the parent
  /// track.
  MAUS::GlobalTrackPArray * _tracks;

  /// The indices of each track's parent track.
  std::vector<int> _parents;

  /// The goodness of fit parameter.
  double _goodness_of_fit;

  ClassDef(PrimaryChain, 1)
}; // ~class PrimaryChain

} // ~namespace global
} // ~namespace recon

typedef MAUS::recon::global::PrimaryChain GlobalPrimaryChain;

typedef std::vector<MAUS::GlobalPrimaryChain*> GlobalPrimaryChainPArray; 

typedef std::vector<const MAUS::GlobalPrimaryChain*>
ConstGlobalPrimaryChainPArray; 

} // ~namespace MAUS

#endif
