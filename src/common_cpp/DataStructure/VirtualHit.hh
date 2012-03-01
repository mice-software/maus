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

/** @class VirtualHit stores data for a particle crossing a VirtualPlane
 *
 *  VirtualPlanes enable users to get readout whenever particles cross a
 *  particular boundary, typically a spatial plane, but potentially a particular
 *  time or proper time.
 *
 *  The VirtualHit stores the ID of the station where the hit was recorded, the
 *  PDG type of the particle making the hit, the track ID, time, mass, charge,
 *  proper time, path length of the track at the point where the hit was made,
 *  position, momentum and EM field of the particle.
 */
class VirtualHit {
  public:
    /**Initialise to 0*/
    VirtualHit();

    /**Copy constructor*/
    VirtualHit(const VirtualHit& md);
         
    /**Equality operator*/
    VirtualHit& operator=(const VirtualHit& md);
    
    /**Destructor does nothing*/
    virtual ~VirtualHit() {}

    /**Return the station id of the plane making the hit*/
    int GetStationId() const;
    /**Set the station id of the plane making the hit*/
    void SetStationId(int id);

    /**Return the PDG particle type of the particle making the hit*/
    int GetParticleId() const;

    /**Set the PDG particle type of the particle making the hit*/
    void SetParticleId(int id);

    /**Return the Geant4 track number of the track making the hit*/
    int GetTrackId() const;

    /**Set the Geant4 track number of the track making the hit*/
    void SetTrackId(int id);

    /**Return the time at which the hit was made*/
    double GetTime() const;

    /**Set the time at which the hit was made*/
    void SetTime(double time);

    /**Return the mass of the particle*/
    double GetMass() const;

    /**Set the mass of the particle*/
    void SetMass(double mass);

    /**Return the charge of the particle*/
    double GetCharge() const;

    /**Set the charge of the particle*/
    void SetCharge(double charge);

    /**Return the proper time of the particle making the hit*/
    double GetProperTime() const;

    /**Set the proper time of the particle making the hit*/
    void SetProperTime(double proper_time);

    /**Return the distance travelled by the particle making the hit*/
    double GetPathLength() const;

    /**Set the distance travelled by the particle making the hit*/
    void SetPathLength(double path_length);

    /** Get the position of the hit (either global position or position
     *  relative to the detector, depending on detector settings)
     */
    ThreeVector GetPosition() const;

    /** Set the position of the hit (either global position or position
     *  relative to the detector, depending on detector settings)
     */
    void SetPosition(ThreeVector position);

    /** Get the momentum of the hit (either momentum in the Lab frame or
     *  momentum rotated relative to the detector, depending on detector
     *  settings)
     */
    ThreeVector GetMomentum() const;

    /** Set the momentum of the hit (either momentum in the Lab frame or
     *  momentum rotated relative to the detector, depending on detector
     *  settings)
     */
    void SetMomentum(ThreeVector momentum);

    /** Get the magnetic field at the point where the hit was registered
     */
    ThreeVector GetBField() const;

    /** Set the magnetic field at the point where the hit was registered
     */
    void SetBField(ThreeVector b_field);

    /** Get the electric field at the point where the hit was registered
     */
    ThreeVector GetEField() const;

    /** Set the electric field at the point where the hit was registered
     */
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

