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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFICHANNELID_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFICHANNELID_HH_

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** Identifier for the channel where a hit was recorded in the SciFi 
 */
class SciFiChannelId {
  public:
    /** Constructor - allocate to 0 */
    SciFiChannelId();

    /** Copy Constructor - copies data from channel*/
    SciFiChannelId(const SciFiChannelId& channel);

    /** Equality operator - copies data from channel */
    SciFiChannelId& operator=(const SciFiChannelId& channel);

    /** Destructor (does nothing)*/
    virtual ~SciFiChannelId();

    /** Get index of the fibre that was hit */
    int GetFibreNumber() const;

    /** Set index of the fibre that was hit */
    void SetFibreNumber(int fibre);

    /** Get the station number (3-plane view) */
    int GetStationNumber() const;

    /** Set the station number (3-plane view) */
    void SetStationNumber(int station);

    /** Get the tracker number i.e. upstream or downstream */
    int GetTrackerNumber() const;

    /** Set the tracker number i.e. upstream or downstream */
    void SetTrackerNumber(int tracker);

    /** Get the plane number */
    int GetPlaneNumber() const;

    /** Set the plane number */
    void SetPlaneNumber(int plane);

    /** Get the used flag */
    bool GetUsed() const;

    /** Set the used flag */
    void SetUsed(bool used);

    /** Get the Hit-Digit ID */
    double GetID() const;

    /** Set the Hit-Digit ID */
    void SetID(double digit_id);

  private:
    int _fibre;
    int _station;
    int _tracker;
    int _plane;

    bool _used;
    double _digit_id;

    MAUS_VERSIONED_CLASS_DEF(SciFiChannelId)
};
}

#endif

