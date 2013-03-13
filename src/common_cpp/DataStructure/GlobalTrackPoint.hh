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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACKPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACKPOINT_HH_

#include <vector>

#include "Rtypes.h" // ROOT

#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class GlobalTrackPoint Stores basic information about a global track point.
 *  This will almost certainly be replaced by something else once the official
 *  global reconstruction data structure is finalized.
 */
class GlobalTrackPoint {
  public:
    /** Initialise to 0/NULL */
    GlobalTrackPoint();

    /** Copy constructor */
    GlobalTrackPoint(const GlobalTrackPoint& track_point);

    /** Destructor */
    virtual ~GlobalTrackPoint();

    /** Assignment operator */
    GlobalTrackPoint& operator=(const GlobalTrackPoint& track_point);

    /** Get the track point time */
    double time() const { return time_; };

    /** Set the track point time */
    void set_time(const double time) { time_ = time; };

    /** Get the track point energy */
    double energy() const { return energy_; };

    /** Set the track point energy */
    void set_energy(const double energy) { energy_ = energy; };

    /** Get the track point position */
    ThreeVector position() const { return position_; };

    /** Set the track point position */
    void set_position(const ThreeVector position) { position_ = position; };

    /** Get the track point momentum */
    ThreeVector momentum() const { return momentum_; };

    /** Set the track point momentum */
    void set_momentum(const ThreeVector momentum) { momentum_ = momentum; };

    /** Get the ID of the detector that produced this track point */
    int detector_id() const { return detector_id_; };

    /** Get the ID of the detector that produced this track point */
    void set_detector_id(const int detector_id) { detector_id_ = detector_id; };

    /** Get the ID of the particle that is represented by this track point */
    int particle_id() const { return particle_id_; };

    /** Get the ID of the particle that is represented by this track point */
    void set_particle_id(const int particle_id) { particle_id_ = particle_id; };

  private:
    double time_;
    double energy_;
    ThreeVector position_;
    ThreeVector momentum_;
    int detector_id_;
    int particle_id_;

    MAUS_VERSIONED_CLASS_DEF(GlobalTrackPoint);
};

typedef std::vector<GlobalTrackPoint> GlobalTrackPointArray;
}

#endif
