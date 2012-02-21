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

class ChannelId {
  public:
      virtual ChannelId* Clone() { return NULL; }
};

class Hit {
  public:
    Hit();

    Hit(const Hit& md);
         
    Hit& operator=(const Hit& md);
 
    virtual ~Hit();

    int GetTrackId() const;
    void SetTrackId(int id);

    int GetParticleId() const;
    void SetParticleId(int pid);

    double GetEnergy() const;
    void SetEnergy(double energy);

    double GetCharge() const;
    void SetCharge(double charge);

    double GetTime() const;
    void SetTime(double time);

    double GetEnergyDeposited() const;
    void SetEnergyDeposited(double edep);

    ThreeVector GetPosition() const;
    void SetPosition(ThreeVector pos);

    ThreeVector GetMomentum() const;
    void SetMomentum(ThreeVector mom);

    ChannelId* GetChannelId() const;
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

