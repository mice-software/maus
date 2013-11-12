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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFINOISEHIT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFINOISEHIT_HH_

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** Identifier for the channel where noise was recorded in the SciFi 
 */
class SciFiNoiseHit {
  public:
    /** Constructor - allocate to 0 */
    SciFiNoiseHit();

    /** Constructor - initialises member variables from arguments */
    SciFiNoiseHit(int spill, int event, int tracker, int station,
               int plane, int channel, double npe, double time );

    /** Copy Constructor - copies data from noise*/
    SciFiNoiseHit(const SciFiNoiseHit& noise);

    /** Equality operator - copies data from noise */
    SciFiNoiseHit& operator=(const SciFiNoiseHit& noise);

    /** Destructor (does nothing)*/
    virtual ~SciFiNoiseHit();
	
    /** Get the spill number */
    int GetSpill() const { return _spill; }
    /** Set the spill number */
    void SetSpill(int spill) { _spill = spill; }
	
    /** Get the event number */
    int GetEvent() const {return _event;}
    /** Set the event number */
    void SetEvent(int event) {_event = event;}

    /** Get the station number (3-plane view) */
    int GetStation() const {return _station;}
    /** Set the station number (3-plane view) */
    void SetStation(int station) {_station = station;}

    /** Get the tracker number i.e. upstream or downstream */
    int GetTracker() const {return _tracker;}
    /** Set the tracker number i.e. upstream or downstream */
    void SetTracker(int tracker) {_tracker = tracker;}

    /** Get the plane number */
    int GetPlane() const {return _plane;}
    /** Set the plane number */
    void SetPlane(int plane) {_plane = plane;}
	
    /** Get the channel number */
    int GetChannel() const {return _channel;}
    /** Set the channel number */
    void SetChannel(int channel) {_channel = channel;}
	
	/** Get NPE */
	double GetNPE() const {return _npe;}
	/** Set NPE */
	void SetNPE(double npe) {_npe = npe;}
	
	/** Get Time */
	double GetTime() const {return _time;}
	/** Set Time */
	void SetTime(double time) {_time = time;}
    
    /** Get Used */
    bool GetUsed() const {return _used;}
    /** Set Used */
    void SetUsed(bool used) {_used = used;}
    
    /** Get the Hit-Digit ID */
    int GetID() const {return _digit_id;}
    /** Set the Hit-Digit ID */
    void SetID(int digit_id) {_digit_id = digit_id;}

  private:
	int _spill, _event;
	int _tracker, _station, _plane, _channel;
	double  _npe, _time;
    bool _used;
    int _digit_id;

    MAUS_VERSIONED_CLASS_DEF(SciFiNoiseHit)
};

typedef std::vector<SciFiNoiseHit> SciFiNoiseHitArray;

}

#endif

