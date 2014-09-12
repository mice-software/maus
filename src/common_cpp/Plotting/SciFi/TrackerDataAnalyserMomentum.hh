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
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TCut.h"
#include "TObjArray.h"

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

    /** Calculate the pt resolution for a particular pt_mc interval,
     *  by plotting a histo of the pt_mc - pz for the interval,
     *  fitting a gaussian, and returning the sigma and error on sigma.
     */
    bool calc_pt_resolution(const int trker, const TCut cut, double &res, double &res_err);

    /** Calculate the pz resolution for a particular pt_mc interval,
     *  by plotting a histo of the pz_mc - pz for the interval,
     *  fitting a gaussian, and returning the sigma and error on sigma.
     */
    bool calc_pz_resolution(const int trker, const TCut cut, double &res, double &res_err);

    /** Find the MC momentum of a track cluster */
    bool find_mc_cluster_momentum(const int track_id, const SciFiCluster* clus, SciFiLookup &lkup,
                                  ThreeVector &mom);

    /** Find the MC momentum of a track spacepoint */
    bool find_mc_spoint_momentum(const int track_id, const SciFiSpacePoint* sp,
                                 SciFiLookup &lkup, ThreeVector &mom);

    /** Find the MC track ID number given a vector of spacepoint numbers and their MC track IDs */
    bool find_mc_tid(const std::vector< std::vector<int> > &spoint_mc_tids, int &tid, int &counter);

    /** Make a TCut out of a variable and operator, input as strings, and a value input as double */
    TCut form_tcut(const std::string &var, const std::string &op, double value);

    /** Return the cut on reconstructed pz */
    double get_cut_pz_rec() const { return _cut_pz_rec; }

    /** Return the number of bins used to make the histos for pt resolution graphs */
    int get_n_pt_bins() const { return _n_pt_bins; }

    /** Return the number of bins used to make the histos for pz resolution graphs */
    int get_n_pz_bins() const { return _n_pz_bins; }

    /** Return the number of points used in the resolution plots */
    int get_n_points() const { return _n_points; }

    /** Return the lower limit of each fit used to calc the pt resolution pnts */
    int get_pt_fit_min() const { return _pt_fit_min; }

    /** Return the upper limit of each fit used to calc the pt resolution pnts */
    int get_pt_fit_max() const { return _pt_fit_max; }

    /** Return the lower limit of each fit used to calc the pz resolution pnts */
    int get_pz_fit_min() const { return _pz_fit_min; }

    /** Return the upper limit of each fit used to calc the pz resolution pnts */
    int get_pz_fit_max() const { return _pz_fit_max; }

    /** Return the lower bound of the pt_mc range used in the pz resolution plots */
    double get_resol_lower_bound() const { return _resol_lower_bound; }

    /** Return the upper bound of the pt_mc range used in the pz resolution plots */
    double get_resol_upper_bound() const { return _resol_upper_bound; }

    /** Make all plots and histograms using accumulated data */
    void make_all();

    /** Make pt resolution graphs, as a function of pt_mc */
    void make_pt_resolutions();

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
    void set_cut_pz_rec(double cut_pz_rec) { _cut_pz_rec = cut_pz_rec; }

    /** Set the number of bins used to make the histos for resolution graphs
        Note: If set to 0, the histos will use the number of bins set automatically by ROOT
              when drawn from the TTree.
    */
    void set_n_pt_bins(double n_pt_bins) { _n_pt_bins = n_pt_bins; }

    /** Set the number of bins used to make the histos for pz resolution graphs
        Note: If set to 0, the histos will use the number of bins set automatically by ROOT
              when drawn from the TTree.
    */
    void set_n_pz_bins(double n_pz_bins) { _n_pz_bins = n_pz_bins; }

    /** Set the number of points used in the pz resolution plots */
    void set_n_points(double n_pz_points) { _n_points = n_pz_points; }

    /** Set the lower limit of each fit used to calc the pt resolution pnts  */
    void set_pt_fit_min(double pt_fit_min) { _pt_fit_min = pt_fit_min; }

    /** Set the upper limit of each fit used to calc the pt resolution pnts  */
    void set_pt_fit_max(double pt_fit_max) { _pt_fit_max = pt_fit_max; }

    /** Set the lower limit of each fit used to calc the pz resolution pnts  */
    void set_pz_fit_min(double pz_fit_min) { _pz_fit_min = pz_fit_min; }

    /** Set the upper limit of each fit used to calc the pz resolution pnts  */
    void set_pz_fit_max(double pz_fit_max) { _pz_fit_max = pz_fit_max; }

    /** Set the lower bound of the pt_mc range used in the pz resolution plots */
    void set_resol_lower_bound(double pz_lower_bound) { _resol_lower_bound = pz_lower_bound; }

    /** Set the upper bound of the pt_mc range used in the pz resolution plots */
    void set_resol_upper_bound(double pz_upper_bound) { _resol_upper_bound = pz_upper_bound; }

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
    TGraphErrors* _t1_pt_resol_pt_mc;
    TGraphErrors* _t1_pz_resol_pt_mc;
    TGraphErrors* _t2_pt_resol_pt_mc;
    TGraphErrors* _t2_pz_resol_pt_mc;

    // The ROOT canvases
    TCanvas* _cResiduals;
    TCanvas* _cGraphs;
    TCanvas* _cResolutions;

  private:
    // Cuts
    int _n_pt_bins;           /// Number of bins used to make the histos for pt resolution graphs
    int _n_pz_bins;           /// Number of bins used to make the histos for pz resolution graphs
    int _n_points;            /// Number of points in the resolution plots
    int _pt_fit_min;          /// The lower limit of each fit used to calc the pt resolution pnts
    int _pt_fit_max;          /// The upper limit of each fit used to calc the pt resolution pnts
    int _pz_fit_min;          /// The lower limit of each fit used to calc the pz resolution pnts
    int _pz_fit_max;          /// The upper limit of each fit used to calc the pz resolution pnts
    double _resol_lower_bound;   /// The lower bound of the pt_mc range for the pz resolution graphs
    double _resol_upper_bound;   /// The upper bound of the pt_mc range for the pz resolution graph
    double _cut_pz_rec;         /// Cut on the reconstructed pz
};

} // ~namespace MAUS

#endif
