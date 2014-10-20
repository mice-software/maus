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

/** @class SciFiDisplayMomentumResolutionsPR
 *
 *  Class to pattern recognition momentum residuals as ROOT histograms
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYMOMENTUMRESOLUTIONSPR_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIDISPLAYMOMENTUMRESOLUTIONSPR_

#include <string>

// ROOT headers
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TCut.h"

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDataBase.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDataMomentumPR.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayBase.hh"


namespace MAUS {

class SciFiDisplayMomentumResolutionsPR : public SciFiDisplayBase {
  public:
    /** Default constructor */
    SciFiDisplayMomentumResolutionsPR();

    /** Destructor */
    virtual ~SciFiDisplayMomentumResolutionsPR();

    /** Calculate the resolution for a particular MC truth momentum interval,
     *  by plotting a histo of the MC - Recon data for the interval,
     *  fitting a gaussian, and returning the sigma and error on sigma.
     */
    bool calc_resolution(const std::string& residual, const TCut cut,
                         double &res, double &res_err);

    /** Make a TCut out of a variable and operator, input as strings, and a value input as double */
    TCut form_tcut(const std::string &var, const std::string &op, double value);

    /** Update the internal data used to make the plots using the pointer to the SciFiData object,
     *  accumulating data into the ROOT member variables
     */
    virtual void Fill();

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

    /** Return the lower bound of the pz_mc range used in the resolution plots */
    double get_lower_bound_pzmc() const { return _lower_bound_pzmc; }

    /** Return the upper bound of the pz_mc range used in the resolution plots */
    double get_upper_bound_pzmc() const { return _upper_bound_pzmc; }

    /** Return the lower bound of the pt_mc range used in the resolution plots */
    double get_resol_lower_bound() const { return _resol_lower_bound; }

    /** Return the upper bound of the pt_mc range used in the resolution plots */
    double get_resol_upper_bound() const { return _resol_upper_bound; }

    /** Make pt resolution graphs, as a function of pt_mc */
    void make_ptpt_resolutions();

    /** Make pz resolution graphs, as a function of pt_mc */
    void make_pzpt_resolutions();

    /** Make pt resolution graphs, as a function of pz_mc */
    void make_ptpz_resolutions();

    /** Make pz resolution graphs, as a function of pz_mc */
    void make_pzpz_resolutions();

    /** Plot the data currently held */
    virtual void Plot(TCanvas* aCanvas = NULL);

    /** Save data and plots to a ROOT file  */
    void Save();

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

    /** Set the lower bound of the pz_mc range used in the resolution plots */
    void set_lower_bound_pzmc(double lower_bound_pzmc) { _lower_bound_pzmc = lower_bound_pzmc; }

    /** Set the lower bound of the pz_mc range used in the resolution plots */
    void set_upper_bound_pzmc(double upper_bound_pzmc) { _upper_bound_pzmc = upper_bound_pzmc; }

    /** Set the lower bound of the pt_mc range used in the resolution plots */
    void set_resol_lower_bound(double pz_lower_bound) { _resol_lower_bound = pz_lower_bound; }

    /** Set the upper bound of the pt_mc range used in the resolution plots */
    void set_resol_upper_bound(double pz_upper_bound) { _resol_upper_bound = pz_upper_bound; }

    /** Set up the SciFiData object and ROOT tree */
    virtual SciFiDataBase* SetUp();

 private:

    /** Set up the ROOT TTree, call this after setting up the SciFiData member */
    bool SetUpRoot();

    /** Sets up the SciFiData object needed by the display.
     *  The display does not own the memory, but rather this should be called by
     *  the SciFiAnalysis class which then assumes ownership.
     */
    SciFiDataBase* SetUpSciFiData();

    TFile* mOf1;                      /** The output ROOT file */
    TTree* mTree;                     /** The ROOT tree used to accumulate the reduced data */
    SciFiDataMomentumPR* mSpillData;  /** The reduced data object, covering one spill */
    MomentumDataPR mTrackData;        /** Struct containing reduced data for one track in a spill */

    // Resolution Graphs
    TGraphErrors* _t1_pt_resol_pt_mc;
    TGraphErrors* _t2_pt_resol_pt_mc;
    TGraphErrors* _t1_pz_resol_pt_mc;
    TGraphErrors* _t2_pz_resol_pt_mc;
    TGraphErrors* _t1_pt_resol_pz_mc;
    TGraphErrors* _t2_pt_resol_pz_mc;
    TGraphErrors* _t1_pz_resol_pz_mc;
    TGraphErrors* _t2_pz_resol_pz_mc;

    // Parameters
    int _n_pt_bins;           /// Number of bins used to make the histos for pt resolution graphs
    int _n_pz_bins;           /// Number of bins used to make the histos for pz resolution graphs
    int _n_points;            /// Number of points in the resolution plots
    int _pt_fit_min;          /// The lower limit of each fit used to calc the pt resolution pnts
    int _pt_fit_max;          /// The upper limit of each fit used to calc the pt resolution pnts
    int _pz_fit_min;          /// The lower limit of each fit used to calc the pz resolution pnts
    int _pz_fit_max;          /// The upper limit of each fit used to calc the pz resolution pnts
    double _upper_bound_pzmc;
    double _lower_bound_pzmc;
    double _resol_lower_bound;   /// The lower bound of the pt_mc range for the pz resolution graphs
    double _resol_upper_bound;   /// The upper bound of the pt_mc range for the pz resolution graph
    double _cut_pz_rec;         /// Cut on the reconstructed pz
};

} // ~namespace MAUS

#endif
