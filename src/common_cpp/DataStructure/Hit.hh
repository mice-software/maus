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

#include "Rtypes.h" // ROOT

#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class ChannelId contains information about the detector channel hit
 */
class ChannelId {
  public:
      virtual ChannelId* Clone() { return new ChannelId(); }
};

/** @class Hit contains Monte Carlo sensitive detector hit data
 *
 *  This is really a placeholder - needs a bit more work on individual
 *  detectors. Probably it needs to be a template class in the end.
 *
 *  Hit holds information pertaining to a particle step in a sensitive detector
 *  Stores information on the Track number, particle type, energy of the 
 *  particle, charge, time, energy deposited by this step, position, momentum
 *  and channel hit (channel id) 
 */
class Hit {
  public:
    /** Initialises everything to 0
     */
    Hit();

    /** Copy constructor; deep copies channel ID
     */
    Hit(const Hit& md);
         
    /** Equality operator; deep copies channel ID
     */
    Hit& operator=(const Hit& md);
 
    /** Destructor - cleans memory associated with channel ID
     */
    virtual ~Hit();

    /** Returns the track number of the track that made the hit
     */
    int GetTrackId() const;

    /** Sets the track number of the track that made the hit
     */
    void SetTrackId(int id);

    /** Returns the PDG particle type of the track that made the hit
     */
    int GetParticleId() const;

    /** Sets the PDG particle type of the track that made the hit
     */
    void SetParticleId(int pid);

    /** Returns the total energy of the track that made the hit
     */
    double GetEnergy() const;

    /** Sets the total energy of the track that made the hit
     */
    void SetEnergy(double energy);

    /** Returns the charge of the track that made the hit
     */
    double GetCharge() const;

    /** Sets the charge of the track that made the hit
     */
    void SetCharge(double charge);

    /** Returns the time of the track when it made the hit
     */
    double GetTime() const;

    /** Sets the time of the track when it made the hit
     */
    void SetTime(double time);

    /** Returns the energy deposited by the track when it made the hit
     */
    double GetEnergyDeposited() const;

    /** Sets the energy deposited by the track when it made the hit
     */
    void SetEnergyDeposited(double edep);

    /** Returns the position of the track when it made the hit
     */
    ThreeVector GetPosition() const;

    /** Sets the position of the track when it made the hit
     */
    void SetPosition(ThreeVector pos);

    /** Returns the momentum of the track when it made the hit
     */
    ThreeVector GetMomentum() const;

    /** Sets the momentum of the track when it made the hit
     */
    void SetMomentum(ThreeVector mom);

    /** Returns the channel id that was hit by the track
     */
    ChannelId* GetChannelId() const;

    /** Sets the channel id that was hit by the track
     */
    void SetChannelId(ChannelId* id);

  private:
    int _track_id;
    int _particle_id;
    double _energy;
    double _charge;
    double _time;
    double _energy_deposited;
    ThreeVector _position;
    ThreeVector _momentum;

    ChannelId* _channel_id;

    ClassDef(Hit, 1)
};



}

#endif

