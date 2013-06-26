/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 *
 */

/** @class TrackerDataManager
 *
 * A class used to draw tracker data on a spill-by-spill basis using ROOT.
 * TrackerData considers TrackerDataManager a friend, but not vice-versa.
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAMANAGER_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAMANAGER_

// C++ headers
#include <vector>

// ROOT headers
#include "TArc.h"
#include "TF1.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Recon/SciFi/TrackerData.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterBase.hh"


namespace MAUS {

class TrackerDataManager {
  public:
    /** Constructor */
    TrackerDataManager();

    /** Destructor */
    ~TrackerDataManager();

    /** Clear data for this spill only */
    void clear_spill();

    /**  Clear all data */
    void clear_run();

    /**  Draw data using TrackerDataPlotter classes */
    void draw(std::vector<TrackerDataPlotterBase*> plotters);

    /** Make a circle using ROOT TArc function */
    TArc make_circle(double x0, double y0, double rad);

    /** Make a straight line using ROOT TF1 */
    TF1 make_str_track(double c, double m, double zmin, double zmax);

    /** Make a function for the z-x projection of the helix */
    TF1 make_xz(double circle_x0, double rad, double dsdz, double sz_c, double zmin, double zmax);

    /** Make a function for the z-y projection of the helix */
    TF1 make_yz(double circle_y0, double rad, double dsdz, double sz_c, double zmin, double zmax);

    /** Main control function. spill is a const pointer to const data. */
    void process(const Spill *spill);

    /** Print info about the track */
    void print_track_info(const SciFiHelicalPRTrack * const trk, int trk_num);

    /** Print info about the seeds in a track */
    void print_seed_info(const SciFiHelicalPRTrack * const trk, int seed_num);

    /** Print info for whole run */
    void print_run_info();

  private:
    // Bit of physics, factor translating radius to transverse momentum, pt = _RtoPt * rad
    static const double _RtoPt = 1.2;

    // Variables to hold running totals over multiple spills
    int _t1_sp;
    int _t2_sp;

    int _t1_seeds;
    int _t2_seeds;

    int _t1_5pt_strks;
    int _t1_4pt_strks;
    int _t1_3pt_strks;
    int _t2_5pt_strks;
    int _t2_4pt_strks;
    int _t2_3pt_strks;

    int _t1_5pt_htrks;
    int _t1_4pt_htrks;
    int _t1_3pt_htrks;
    int _t2_5pt_htrks;
    int _t2_4pt_htrks;
    int _t2_3pt_htrks;

    // Some plotting parameters
    static const double _zmin = 0.0;
    static const double _zmax = 1200.0;

    // The data associated with each tracker for one spill
    TrackerData _t1;
    TrackerData _t2;
};

} // ~namespace MAUS

#endif
