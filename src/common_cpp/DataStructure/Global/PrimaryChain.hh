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
#include "TRef.h"

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

  /// Constructor setting mapper name and chain type
  PrimaryChain(std::string mapper_name, MAUS::DataStructure::Global::ChainType chain_type);

  /// Destructor
  virtual ~PrimaryChain();

  /// Assignment operator
  PrimaryChain& operator=(const PrimaryChain &primary_chain);


  // Getters & Setters
  /// Get the upstream daughter chain, if current chain is not a through chain,
  /// NULL is returned
  MAUS::DataStructure::Global::PrimaryChain* GetUSDaughter() const;

  /// Set the upstream daughter chain, if current chain is not a through chain,
  /// nothing will be done and an error message sent to Squeak::error
  void SetUSDaughter(MAUS::DataStructure::Global::PrimaryChain* us_daughter);

  /// Get the downstream daughter chain, if current chain is not a through chain,
  /// NULL is returned
  MAUS::DataStructure::Global::PrimaryChain* GetDSDaughter() const;

  /// Set the downstream daughter chain, if current chain is not a through chain,
  /// nothing will be done and an error message sent to Squeak::error
  void SetDSDaughter(MAUS::DataStructure::Global::PrimaryChain* ds_daughter);

  /// Get the matched track with the requested PID. If no such track exists, NULL is returned
  MAUS::DataStructure::Global::Track* GetMatchedTrack(MAUS::DataStructure::Global::PID pid) const;

  /// Get all matched tracks in the chain, for a through chain this will only be through-matched
  /// tracks, the US and DS tracks can be accessed via the daughter chains
  std::vector<MAUS::DataStructure::Global::Track*> GetMatchedTracks() const;

  /// Add a matched track to the chain
  void AddMatchedTrack(MAUS::DataStructure::Global::Track* track);

  /// Retrieve the PID'd track from the chain, if it doesn't exist, NULL is returned
  MAUS::DataStructure::Global::Track* GetPIDTrack() const;

  /// Add the PID track to the chain. Only one can exist for each chain, so if it is already
  /// set, it is overwritten and a warning sent to Squeak::debug
  void SetPIDTrack(MAUS::DataStructure::Global::Track* track);

  /// Retrieve the Fitted track from the chain, if it doesn't exist, NULL is returned
  MAUS::DataStructure::Global::Track* GetFittedTrack() const;

  /// Add the fitted track to the chain. Only one can exist for each chain, so if it is already
  /// set, it is overwritten and a warning sent to Squeak::debug
  void SetFittedTrack(MAUS::DataStructure::Global::Track* track);

  // Getters and setters for serialization
  /// Get chain's tracks as TRefArray. This should only be used for serialization.
  TRefArray* get_tracks() const {return _tracks; }

  /// Set chain's tracks as TRefArray. This should only be used for serialization.
  void set_tracks(TRefArray* tracks) { _tracks = tracks; }

  /// Get chain's upstream daughter as TObject. This should only be used for serialization.
  TObject* get_us_daughter() const { return _us_daughter.GetObject(); }

  /// Set chain's upstream daughter as TObject. This should only be used for serialization.
  void set_us_daughter(TObject* us_daughter) { _us_daughter = us_daughter; }

  /// Get chain's downstream daughter as TObject. This should only be used for serialization.
  TObject* get_ds_daughter() const { return _ds_daughter.GetObject(); }

  /// Set chain's downstream daughter as TObject. This should only be used for serialization.
  void set_ds_daughter(TObject* ds_daughter) { _ds_daughter = ds_daughter; }

  // Getters and Setters for the member variables
  /// Set the name for the mapper which produced the result, #_mapper_name.
  void set_mapper_name(std::string mapper_name) { _mapper_name = mapper_name; }
  /// Get the name for the mapper which produced the result, #_mapper_name.
  std::string get_mapper_name() const { return _mapper_name; }

  /// Get the type of the chain
  MAUS::DataStructure::Global::ChainType get_chain_type() const { return _type; }

  /// Set the type of the chain, options in the enum are
  /// kNoChainType, kUS, kDS, kUSOrphan, kDSOrphan, kThrough
  void set_chain_type(MAUS::DataStructure::Global::ChainType type) { _type = type; }

  /// Get the chain multiplicity
  MAUS::DataStructure::Global::ChainChildMultiplicity get_multiplicity() const {
    return _multiplicity;
  }

  /// Set the chain multiplicity, options in the enum are
  /// kUnique, kMultipleUS, kMultipleDS, kMultipleBoth
  void set_multiplicity(MAUS::DataStructure::Global::ChainChildMultiplicity multiplicity) {
    _multiplicity = multiplicity;
  }

 private:

  /// The name of the mapper that last edited this object
  std::string _mapper_name;

  /// Type of the chain, i.e. whether it is US, DS, Orphan US, OrphanDS, or Through
  MAUS::DataStructure::Global::ChainType _type;

  /// Whether the chain's daughter chains are non-unique (i.e. this and at least one
  /// other chain are mutually exclusive)
  MAUS::DataStructure::Global::ChainChildMultiplicity _multiplicity;

  /// Tracks contained in the chain
  TRefArray* _tracks;

  /// US daughter primary chain (only if this is a through chain)
  TRef _us_daughter;

  /// DS daughter primary chain (only if this is a through chain)
  TRef _ds_daughter;

  MAUS_VERSIONED_CLASS_DEF(PrimaryChain);
}; // ~class PrimaryChain

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
