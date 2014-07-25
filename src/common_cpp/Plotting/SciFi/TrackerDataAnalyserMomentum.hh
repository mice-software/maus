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
#include "TCut.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"

namespace MAUS {

class TrackerDataAnalyserMomentum {
  public:

    /** Default constructor, initialise the abstract base class, and set pointers to NULL. */
    TrackerDataAnalyserMomentum();

    /** Destructor */
    virtual ~TrackerDataAnalyserMomentum();

    /** Takes in the data, one spill at a time */
    void accumulate(Spill* spill);

    /** Calculate pattern recognition efficiency parameters */
    void calc_pat_rec_efficiency(MCEvent *mc_evt, SciFiEvent* evt);

    /** Calculate the pz resolution for a particular pt_mc interval,
     *  by plotting a histo of the pz_mc - pz for the interval,
     *  fitting a gaussian, and returning the sigma and error on sigma.
     */
    void calc_pz_resolution(const int trker, const TCut cut,
                            double &res, double &res_err);

    /** Find the MC momentum of a track spacepoint */
    bool find_mc_spoint_momentum(const int track_id, const SciFiSpacePoint* sp,
                                 SciFiLookup &lkup, ThreeVector &mom);

    /** Find the MC track ID number given a vector of spacepoint numbers and their MC track IDs */
    bool find_mc_tid(const std::vector< std::vector<int> > &spoint_mc_tids, int &tid, int &counter);

    TCut formTCut(const std::string &var, const std::string &op, double value);

    /** Return the cut on reconstructed pz */
    double get_cutPzRec() const { return _cutPzRec; }

    /** Make all plots and histograms using accumulated data */
    void make_all();

    /** Make pz resolution graphs, as a function of pt_mc */
    void make_pz_resolutions();

    /** Produce the final residual histograms using the accumulated data */
    void make_residual_histograms();

    /** Produce the final residual graphs using the accumulated data */
    void make_residual_graphs();

    /** Produce the final resolution graphs using the accumulated data */
    void make_resolution_graphs();

    /** Save the plots with the input file type */
    bool save_graphics(std::string save_type = "pdf");

    /** Save the root objects to the input root file*/
    void save_root();

    /** Set the cut on reconstructed pz using a double, in MeV/c */
    void set_cutPzRec(double cutPzRec) { _cutPzRec = cutPzRec; }

    /** Set up ROOT objects (too much work for constructor) */
    void setUp();

  protected:
    int _spill_num;
    int _tracker_num;
    int _charge;
    int _num_points;
    int _n_bad_tracks;
    int _mc_track_id;
    int _mc_pid;
    int _n_matched;
    int _n_mismatched;
    int _n_missed;

    double _pt_rec;
    double _pz_rec;
    double _pt_mc;
    double _pz_mc;

    // Vectors used to store momentum data used to produce graphs
    std::vector<double> _vec_t1_pt_mc;
    std::vector<double> _vec_t1_pt_res;
    std::vector<double> _vec_t1_pz;
    std::vector<double> _vec_t1_pz_res;
    std::vector<double> _vec_t2_ptMc;
    std::vector<double> _vec_t2_pt_res;
    std::vector<double> _vec_t2_pz;
    std::vector<double> _vec_t2_pz_res;

    TFile* _out_file;  // The output ROOT file
    TTree* _tree;      // The output ROOT tree

    // Residual histograms
    TH1D* _t1_pt_res;
    TH1D* _t1_pz_res;
    TH1D* _t1_pz_res_log;
    TH1D* _t2_pt_res;
    TH1D* _t2_pz_res;
    TH1D* _t2_pz_res_log;

    // Residual graphs
    TGraph* _t1_pt_res_pt;
    TGraph* _t1_pz_res_pt;
    TGraph* _t2_pt_res_pt;
    TGraph* _t2_pz_res_pt;

    // Resolution graphs
    TGraphErrors* _t1_pz_resol;
    TGraphErrors* _t2_pz_resol;

    // The ROOT canvases
    TCanvas* _cResiduals;
    TCanvas* _cGraphs;
    TCanvas* _cResolutions;

  private:
    // Cuts
    double _cutPzRec;  /// Cut on the reconstructed pz
};

} // ~namespace MAUS

#endif