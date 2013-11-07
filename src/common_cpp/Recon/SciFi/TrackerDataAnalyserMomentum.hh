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

/** @class TrackerDataAnalyserMomentum
 *
 *  Produce tracker Pattern Recognition momentum plots using MC and Recon data
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAANALYSERMOMENTUM_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAANALYSERMOMENTUM_

// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

class TrackerDataAnalyserMomentum {
  public:

    /** Default constructor, initialise the abstract base class, and set pointers to NULL. */
    TrackerDataAnalyserMomentum();

    /** Destructor */
    virtual ~TrackerDataAnalyserMomentum();

    /** Takes in the data, one spill at a time */
    void accumulate(Spill* spill);

    /** Set up ROOT objects (too much work for constructor) */
    void setUp();

    /** Produce the final plots using the accumulated data */
    void analyse();

    /** Make pz resolution graphs, as a function of pt_mc */
    void make_pz_resolutions();

    /** Calculate the pz resolution for a particular pt_mc interval,
     *  by plotting a histo of the pz_mc - pz for the interval,
     *  fitting a gaussian, and returning the sigma and error on sigma.
     */
    void calc_pz_resolution(const int trker, const TCut cut,
                            double &res, double &res_err);

    /** Save the plots with the input file type */
    bool save_graphics(std::string save_type = "pdf");

    /** Save the root objects to the input root file*/
    void save_root();

  protected:
    int _tracker_num;
    int _charge;
    double _pt;
    double _pz;
    double _pt_mc;
    double _pz_mc;

    std::vector<double> _t1_vPtMc;
    std::vector<double> _t1_vPt_res;
    std::vector<double> _t1_vPz;
    std::vector<double> _t1_vPz_res;
    std::vector<double> _t2_vPtMc;
    std::vector<double> _t2_vPt_res;
    std::vector<double> _t2_vPz;
    std::vector<double> _t2_vPz_res;

    TFile* _out_file;
    TTree* _tree;

    TH1D* _t1_pt_res;
    TH1D* _t1_pz_res;
    TH1D* _t1_pz_res_log;
    TGraph* _t1_pt_res_pt;
    TGraph* _t1_pz_res_pt;

    TH1D* _t2_pt_res;
    TH1D* _t2_pz_res;
    TH1D* _t2_pz_res_log;
    TGraph* _t2_pt_res_pt;
    TGraph* _t2_pz_res_pt;

    TGraphErrors* _t1_pz_resol;
    TGraphErrors* _t2_pz_resol;

    TCanvas* _cResiduals;
    TCanvas* _cGraphs;
};

} // ~namespace MAUS

#endif
