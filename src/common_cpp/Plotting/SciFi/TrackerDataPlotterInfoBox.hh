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

/** @class TrackerDataPlotterInfoBox
 *
 *  Display tracker info such as number of clusters, spacepoints, etc, in a ROOT TPave
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERINFOBOX_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAPLOTTERINFOBOX__

// C++ headers
#include <string>

// Google test headers
#include "gtest/gtest_prod.h"

// ROOT headers
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLine.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerData.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterBase.hh"


namespace MAUS {

class TrackerDataPlotterInfoBox : public TrackerDataPlotterBase {
  public:
    // Macros to allow friendship with the gtests
    FRIEND_TEST(TrackerDataManagerTest, TestDraw);
    FRIEND_TEST(TrackerDataPlotterInfoBoxTest, TestConstructor);
    FRIEND_TEST(TrackerDataPlotterInfoBoxTest, TestBrackets);

    /** Default constructor, initialise the abstract base class, and set pointers to NULL. */
    TrackerDataPlotterInfoBox();

    /** Constructor allowing setting of some of the box parameters */
    TrackerDataPlotterInfoBox(int canvas_width, int canvas_height,
                              double line1_pos, double line2_pos);

    /** Destructor  */
    virtual ~TrackerDataPlotterInfoBox();

    /** Overloaded brackets operator, takes in the data, does all the work. */
    TCanvas* operator() ( TrackerData &t1, TrackerData &t2, TCanvas* aCanvas = NULL);

    /** Setup the TPaves, can't be done in constructor as need to have chosen a canvas beforehand */
    void setup_paves(TCanvas* aCanvas);

    /** Convert an int to a const char* */
    const char* chr(int i);

  protected:
    bool _setup_true;

    int _tot_digits_t1;
    int _tot_clusters_t1;
    int _tot_spoints_t1;
    int _tot_5htracks_t1;
    int _tot_4htracks_t1;
    int _tot_3htracks_t1;
    int _tot_5stracks_t1;
    int _tot_4stracks_t1;
    int _tot_3stracks_t1;
    int _tot_pos_tracks_t1;
    int _tot_neg_tracks_t1;

    int _tot_digits_t2;
    int _tot_clusters_t2;
    int _tot_spoints_t2;
    int _tot_5htracks_t2;
    int _tot_4htracks_t2;
    int _tot_3htracks_t2;
    int _tot_5stracks_t2;
    int _tot_4stracks_t2;
    int _tot_3stracks_t2;
    int _tot_pos_tracks_t2;
    int _tot_neg_tracks_t2;

    /** Parameters of the TCanvas */
    int _canvas_width;
    int _canvas_height;

    /** Position of the seperator lines in the info box */
    double _line1_pos;
    double _line2_pos;

    TPaveText *_p_label;
    TPaveText *_p_t1;
    TPaveText *_p_t2;

    TLine *_line1;
    TLine *_line2;
};

} // ~namespace MAUS

#endif
