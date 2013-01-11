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
#include <map>

// ROOT headers
#include "Rtypes.h"

// MAUS headers
#include "Interface/Squeal.hh"
#include "DataStructure/GlobalTrack.hh"

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
  /// @param[in] parent The track's parent.  We require parent to
  ///                   already be a member of the primarychain.
  bool AddTrack(MAUS::recon::global::Track* track,
                MAUS::recon::global::Track* parent);

  /// Add a primary MAUS::recon::global::Track, with the parent
  /// reference set to NULL.
  /// @param[in] track  The track being added to the primary chain
  bool AddPrimaryTrack(MAUS::recon::global::Track* track);

  /// Remove all reference of the provided track.  Returns false if a)
  /// track is not a member of the chain, or b) track has daughter
  /// particles stored in the chain.
  bool RemoveTrack(MAUS::recon::global::Track* track);

  /// Checks whether track is stored in the chain.
  bool HasTrack(MAUS::recon::global::Track* track);

  /// Checks whether track is a parent in the chain.
  bool HasTrackAsParent(MAUS::recon::global::Track* parent);

  /// Checks if a track is a primary track
  bool IsPrimaryTrack(MAUS::recon::global::Track* track);

  /// Returns a track's parent, according to this primary chain
  MAUS::recon::global::Track* GetTrackParent(MAUS::recon::global::Track* track);

  /// Prepare a list of a track's daughter tracks.
  std::vector<MAUS::recon::global::Track*>
  GetTrackDaughters(MAUS::recon::global::Track* track);

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

  /// Fills the tracks half of the map #_tracks from a vector of tracks.
  void set_tracks(GlobalTrackPArray* tracks){
    _tracks.clear();
    for(size_t i = 0; i < tracks->size(); ++i){
      // Create a vector entry, but leave the parent TRef to be
      // constructed by the default constructor
      std::pair<TRef, TRef> track_pair;
      track_pair.first = tracks->at(i);
      _tracks.push_back(track_pair);
    }
  }

  /// Fills the parents half of the map #_tracks from a vector of
  /// tracks, requiring the map size to be equal to the vector size.
  void set_parents(GlobalTrackPArray* parents){
    if(_tracks.size() != parents->size()){
      throw(Squeal(Squeal::recoverable,
                   "Trying to set parents using a diff. size vector to tracks",
                   "PrimaryChain::set_parents"));
    }
    for(size_t i = 0; i < parents->size(); ++i){
      _tracks.at(i).second = parents->at(i);
    }
  }

  /// Builds a vector of tracks from the internal #_tracks TRefArray
  GlobalTrackPArray* get_tracks() const {
    GlobalTrackPArray *result =
        new GlobalTrackPArray;
    for(size_t i = 0; i < _tracks.size(); ++i){
      MAUS::recon::global::Track* temp =
          (MAUS::recon::global::Track*) _tracks.at(i).first.GetObject();
      result->push_back(temp);
    }
    
    return result;
  }

  /// Builds a vector of tracks from the internal #_tracks TRefArray
  GlobalTrackPArray* get_parents() const {
    GlobalTrackPArray *result =
        new GlobalTrackPArray;
    for(size_t i = 0; i < _tracks.size(); ++i){
      MAUS::recon::global::Track* temp =
          (MAUS::recon::global::Track*) _tracks.at(i).second.GetObject();
      result->push_back(temp);
    }
    
    return result;
  }

  /// Directly sets the internal #_tracks vector
  void set_tracks_tref(std::vector<std::pair<TRef, TRef> > tracks){
    _tracks = tracks;
  }
  /// Direct access to the internal #_tracks vector
  std::vector<std::pair<TRef, TRef> > get_tracks_tref() const {
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
  std::vector<std::pair<TRef, TRef> > _tracks;

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
