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
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
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
#include <map>

// ROOT headers
#include "TObject.h"
#include "TRefArray.h"
#include "Rtypes.h"

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "Interface/Squeal.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TRefTrackPair.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Only elements in the DataStructure should inherit from TObject.
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

  /// Assignment operator - any pointers are deep copied
  PrimaryChain& operator=(const PrimaryChain &primary_chain);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  PrimaryChain* Clone() const;

  // Methods for adding tracks, while tracking the parents

  /// Add a MAUS::DataStructure::Global::Track, tracking the location
  /// of the parent track.
  /// @param[in] track  The track being added to the primary chain. The
  ///                   track pointed to by the pointer must be stored
  ///                   elsewhere in the event.  We are only taking a
  ///                   reference.
  /// @param[in] parent The track's parent.  We require parent to
  ///                   already be a member of the
  ///                   primary_chain. Again, this is only storing a
  ///                   reference, the data must be saved elsewhere.
  bool AddTrack(MAUS::DataStructure::Global::Track* track,
                MAUS::DataStructure::Global::Track* parent);

  /// Add a primary MAUS::DataStructure::Global::Track, with the parent
  /// reference set to NULL.
  /// @param[in] track  The track being added to the primary chain. The
  ///                   track pointed to by the pointer must be stored
  ///                   elsewhere in the event.  We are only taking a
  ///                   reference.
  bool AddPrimaryTrack(MAUS::DataStructure::Global::Track* track);

  /// Checks whether track is stored in the chain.
  bool HasTrack(MAUS::DataStructure::Global::Track* track);

  /// Checks whether track is a parent in the chain.
  bool HasTrackAsParent(MAUS::DataStructure::Global::Track* parent);

  /// Checks if a track is a primary track
  bool IsPrimaryTrack(MAUS::DataStructure::Global::Track* track);

  /// Returns a vector of all of the tracks in this chain, for users
  /// to examine.
  std::vector<MAUS::DataStructure::Global::Track*> GetTracks();

  /// Returns a vector of all of the primary tracks in this chain, for
  /// users to examine.
  std::vector<MAUS::DataStructure::Global::Track*> GetPrimaryTracks();

  /// Returns a track's parent, according to this primary chain
  MAUS::DataStructure::Global::Track* GetTrackParent(
      MAUS::DataStructure::Global::Track* track);

  /// Prepare a list of a track's daughter tracks.
  std::vector<MAUS::DataStructure::Global::Track*>
  GetTrackDaughters(MAUS::DataStructure::Global::Track* track);

  /// Empties the #_tracks object
  void ClearTracks();

  /// Add a preceeding primary chain.  This allows us to record the
  /// full algorithm flow.
  void AddParentChain(MAUS::DataStructure::Global::PrimaryChain* chain);

  /// Get a vector of preceeding primary chains.
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> GetParentChains();

  // Getters and Setters for the member variables
  /// Set the name for the mapper which produced the result, #_mapper_name.
  void set_mapper_name(std::string mapper_name) {
    _mapper_name = mapper_name;
  }
  /// Get the name for the mapper which produced the result, #_mapper_name.
  std::string get_mapper_name() const {
    return _mapper_name;
  }

  /// Set the vector of track/parent references, #_track_parent_pairs.
  /// The PrimaryChain takes ownership of this vector and the
  /// associated TRefTrackPairs.  This is mostly for the Json/Cpp
  /// Processor, in general it would be better for users to use the
  /// AddTrack and AddPrimaryTrack methods.
  void set_track_parent_pairs(
      std::vector<MAUS::DataStructure::Global::TRefTrackPair*>* tracks);

  /// Get the vector of track/parent references, #_track_parent_pairs.
  /// Predominantly to be used by the Json/Cpp Processor.  Users are
  /// encouraged to use the helpful GetTracks and GetPrimaryTracks
  /// methods, then to use the GetTrackParent or GetTrackDaughters
  /// methods.
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*>*
  get_track_parent_pairs() const;

  /// Set the #_goodness_of_fit paramter
  void set_goodness_of_fit(double goodness_of_fit);

  /// Get the #_goodness_of_fit paramter
  double get_goodness_of_fit() const;

  /// Set the #_parent_primary_chains pointer array.  The PrimaryChain
  /// takes ownership of this pointer.  This is mostly for the
  /// Json/Cpp Processor, most users shoudl use AddParentChain.
  void set_parent_primary_chains(TRefArray* parent_primary_chains);

  /// Get the #_parent_primary_chains pointer array
  TRefArray* get_parent_primary_chains() const;

  /// Add a comment to the primary chain, e.g.
  /// AddComment("Error0", "Track took a 10 cm step to the right").
  void AddComment(std::string key, std::string comment);

  /// Clear all comments from the primary chain.
  void ClearComments();

  /// Remove a specific comment from the chain.
  void RemoveComment(std::string key);

  /// Set the #_comments map.
  void set_comments(std::map<std::string, std::string> comments);

  /// Get the #_comments map.
  std::map<std::string, std::string> get_comments() const;

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

  /// An array of pointers to preceeding PrimaryChain, to fully
  /// document the global reconstruction steps which produced this
  /// result.
  TRefArray* _parent_primary_chains;

  /// A map, holding indexed comments.  This is intended to be an
  /// automated interface with the analysis group, providing
  /// potentially important comments on the reconstructed result.
  std::map<std::string, std::string> _comments;

  MAUS_VERSIONED_CLASS_DEF(PrimaryChain);
}; // ~class PrimaryChain

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
