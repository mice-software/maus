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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_HIT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_HIT_HH_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/SpecialVirtualChannelId.hh"
#include "src/common_cpp/DataStructure/TOFChannelId.hh"
#include "src/common_cpp/DataStructure/CkovChannelId.hh"
#include "src/common_cpp/DataStructure/KLChannelId.hh"
#include "src/common_cpp/DataStructure/EMRChannelId.hh"
#include "src/common_cpp/DataStructure/SciFiChannelId.hh"

namespace MAUS {

/** @class Hit contains Monte Carlo sensitive detector hit data
 *
 *  Hit holds information pertaining to a particle step in a sensitive detector
 *  Stores information on the Track number, particle type, energy of the
 *  particle, charge, time, energy deposited by this step, position, momentum
 *  and channel hit (channel id)
 *
 *  The template is here to tell developers and users that I expect an extra
 *  parameter, channel id, in actual instantiations of this class, that contains
 *  information about the channel (slab, fiber, etc) that was hit.
 *
 *  ChannelId is required to have a copy constructor
 *
 *  Note that I use a typedef to define specialisations for each detector, like
 *  TOFHit, SciFiHit, ...
 */
template <class ChannelId>
class Hit {
  public:
    /** Initialises everything to 0
     */
    Hit();

    /** Copy constructor; deep copies channel ID
     */
    Hit(const Hit& hit);

    /** Equality operator; deep copies channel ID
     */
    virtual Hit& operator=(const Hit& hit);

    /** Destructor - cleans memory associated with channel ID
     */
    virtual ~Hit();

    /** Returns the track number of the track that made the hit
     */
    virtual int GetTrackId() const;

    /** Sets the track number of the track that made the hit
     */
    virtual void SetTrackId(int id);

    /** Returns the PDG particle type of the track that made the hit
     */
    virtual int GetParticleId() const;

    /** Sets the PDG particle type of the track that made the hit
     */
    virtual void SetParticleId(int pid);

    /** Returns the total energy of the track that made the hit
     */
    virtual double GetEnergy() const;

    /** Sets the total energy of the track that made the hit
     */
    virtual void SetEnergy(double energy);

    // Returns the mass of the track that made the hit
    // Set the mass of the ...
    virtual double GetMass() const;
    virtual void SetMass(double mass);

    /** Returns the charge of the track that made the hit
     */
    virtual double GetCharge() const;

    /** Sets the charge of the track that made the hit
     */
    virtual void SetCharge(double charge);

    /** Returns the time of the track when it made the hit
     */
    virtual double GetTime() const;

    /** Sets the time of the track when it made the hit
     */
    virtual void SetTime(double time);

    /** Returns the energy deposited by the track when it made the hit
     */
    virtual double GetEnergyDeposited() const;

    /** Sets the energy deposited by the track when it made the hit
     */
    virtual void SetEnergyDeposited(double edep);

    /** Add to the energy deposited by the track when it made the hit
     */
    virtual void AddEnergyDeposited(double edep);

    /** Returns the position of the track when it made the hit
     */
    virtual ThreeVector GetPosition() const;

    /** Sets the position of the track when it made the hit
     */
    virtual void SetPosition(ThreeVector pos);

    /** Returns the momentum of the track when it made the hit
     */
    virtual ThreeVector GetMomentum() const;

    /** Sets the momentum of the track when it made the hit
     */
    virtual void SetMomentum(ThreeVector mom);

    /** Returns the channel id that was hit by the track
     */
    virtual ChannelId* GetChannelId() const;

    /** Sets the channel id that was hit by the track
     */
    virtual void SetChannelId(ChannelId* id);

    /** Returns the path length of the track
     */
    virtual double GetPathLength() const;

    /** Sets the path length of the track
     */
    virtual void SetPathLength(double path);

    /** Add the path length to the track
     */
    virtual void AddPathLength(double path);

  private:
    int _track_id;
    int _particle_id;
    double _energy;
    double _mass;
    double _charge;
    double _time;
    double _energy_deposited;
    double _path_length;
    ThreeVector _position;
    ThreeVector _momentum;

    ChannelId* _channel_id;

    MAUS_VERSIONED_CLASS_DEF(Hit);
};

typedef Hit<SciFiChannelId> SciFiHit;
typedef std::vector<SciFiHit*> SciFiHitPArray;
typedef Hit<TOFChannelId> TOFHit;
typedef Hit<KLChannelId> KLHit;
typedef Hit<EMRChannelId> EMRHit;
typedef Hit<SpecialVirtualChannelId> SpecialVirtualHit;
typedef Hit<CkovChannelId> CkovHit;
}

#include "src/common_cpp/DataStructure/Hit-inl.hh"

#endif

