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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_VIRTUALHIT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_VIRTUALHIT_HH_

#include <vector>

#include "Rtypes.h" // ROOT

#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class VirtualHit {
  public:
    VirtualHit();

    VirtualHit(const VirtualHit& md);
         
    VirtualHit& operator=(const VirtualHit& md);
    
    virtual ~VirtualHit() {}

    int GetStationId() const;
    void SetStationId(int id);

    int GetParticleId() const;
    void SetParticleId(int id);

    int GetTrackId() const;
    void SetTrackId(int id);

    double GetTime() const;
    void SetTime(double time);

    double GetMass() const;
    void SetMass(double mass);

    double GetCharge() const;
    void SetCharge(double charge);

    double GetProperTime() const;
    void SetProperTime(double proper_time);

    double GetPathLength() const;
    void SetPathLength(double path_length);

    ThreeVector GetPosition() const;
    void SetPosition(ThreeVector position);

    ThreeVector GetMomentum() const;
    void SetMomentum(ThreeVector momentum);

    ThreeVector GetBField() const;
    void SetBField(ThreeVector b_field);

    ThreeVector GetEField() const;
    void SetEField(ThreeVector e_field);

  private:
    int _station_id;
    int _particle_id;
    int _track_id;
    double _time;
    double _mass;
    double _charge;
    double _proper_time;
    double _path_length;
    ThreeVector _position;
    ThreeVector _momentum;
    ThreeVector _b_field;
    ThreeVector _e_field;  

    ClassDef(VirtualHit, 1)
};

}

#endif

