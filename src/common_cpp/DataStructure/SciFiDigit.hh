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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIDIGIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIDIGIT_

// C++ headers
#include <vector>

// MAUS Headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class SciFiDigit Represents a channel hit in a tracker station plane. */

class SciFiDigit {
  public:
    /** Default constructor - initialises to 0/NULL */
    SciFiDigit();

    /** Constructor - initialises member variables from arguments */
    SciFiDigit(int spill, int event, int tracker, int station,
               int plane, int channel, double npe, double time );

    /** Copy constructor - any pointers are deep copied */
    SciFiDigit(const SciFiDigit& _scifidigit);

    /** Assignment operator - any pointers are deep copied */
    SciFiDigit& operator=(const SciFiDigit& _scifidigit);

    /** Destructor - any member pointers are deleted */
    virtual ~SciFiDigit();

    // Getters and Setters
    void set_spill(int spill) { _spill = spill; }

    int get_spill()   const { return _spill; }

    void set_event(int event) { _event = event; }

    int get_event() const { return _event; }

    void set_tracker(int trackerNo) { _tracker = trackerNo; }

    int get_tracker() const { return _tracker; }

    void set_station(int stationNo) { _station = stationNo; }

    int get_station() const { return _station; }

    void set_plane(int planeNo) { _plane = planeNo; }

    int get_plane()   const { return _plane; }

    void set_channel(int channelNo) { _channel = channelNo; }

    int get_channel() const { return _channel; }

    void set_npe(double npe) { _npe = npe; }

    double get_npe()     const { return _npe;     }

    void set_time(double time) { _time = time; }

    double get_time()    const { return _time;    }

    void set_used(bool used)   { _used = used; }

    bool is_used()     const { return _used; }

    void set_true_momentum(ThreeVector p) { _p = p; }

    ThreeVector get_true_momentum() const { return _p; }

    void set_true_position(ThreeVector position) { _position = position; }

    ThreeVector get_true_position() const { return _position; }

  private:
    ThreeVector _position, _p;

    int _spill, _event;

    int _tracker, _station, _plane, _channel;

    double _npe, _time;

    bool _used;

    MAUS_VERSIONED_CLASS_DEF(SciFiDigit)
};

typedef std::vector<SciFiDigit*> SciFiDigitPArray;

} // ~namespace MAUS

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIDIGIT_
