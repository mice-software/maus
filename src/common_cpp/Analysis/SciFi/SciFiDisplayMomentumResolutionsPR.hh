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
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayDataInterface.hh"


namespace MAUS {

class SciFiDisplayMomentumResolutionsPR : public SciFiDisplayDataInterface<SciFiDataMomentumPR> {
  public:
    /** Default constructor */
    SciFiDisplayMomentumResolutionsPR();

    /** Destructor */
    virtual ~SciFiDisplayMomentumResolutionsPR();

    /** Calculate the resolution for a particular MC truth momentum interval,
     *  by plotting a histo of the MC - Recon data for the interval,
     *  fitting a gaussian, and returning the sigma and error on sigma. Will make a cut
     *  for 5 point tracks only if the m5StationOnly variable is set to true.
     */
    bool CalcResolution(const std::string& residual, const TCut cut, double &res, double &res_err);

    /** Make a TCut out of a variable and operator, input as strings, and a value input as double */
    TCut FormTCut(const std::string &var, const std::string &op, double value);

    /** Update the internal data used to make the plots using the pointer to the SciFiData object,
     *  accumulating data into the ROOT member variables
     */
    virtual void Fill();

    /** Return a pointer to the SciFiData object associated with the display */
    virtual SciFiDataMomentumPR* GetData();

    /** Return the cut on reconstructed pz */
    double GetCutPzRec() const { return mCutPzRec; }

    /** Return the number of bins used to make the histos for pt resolution graphs */
    int GetNBbinsPt() const { return mNBinsPt; }

    /** Return the number of bins used to make the histos for pz resolution graphs */
    int GetNBinsPz() const { return mNBinsPz; }

    /** Return the number of points used in the resolution plots */
    int GetNPoints() const { return mNPoints; }

    /** Return the lower limit of each fit used to calc the pt resolution pnts */
    int GetPtFitMin() const { return mPtFitMin; }

    /** Return the upper limit of each fit used to calc the pt resolution pnts */
    int GetPtFitMax() const { return mPtFitMax; }

    /** Return the lower limit of each fit used to calc the pz resolution pnts */
    int GetPzFitMin() const { return mPzFitMin; }

    /** Return the upper limit of each fit used to calc the pz resolution pnts */
    int GetPzFitMax() const { return mPzFitMax; }

    /** Return the lower bound of the PzMC range used in the resolution plots */
    double GetLowerBoundPzMC() const { return mLowerBoundPzMC; }

    /** Return the upper bound of the PzMC range used in the resolution plots */
    double GetUpperBoundPzMC() const { return mUpperBoundPzMC; }

    /** Return the lower bound of the PtMC range used in the resolution plots */
    double GetResolLowerBound() const { return mLowerBoundPtMC; }

    /** Return the upper bound of the PtMC range used in the resolution plots */
    double GetResolUpperBound() const { return mUpperBoundPtMC; }

    /** Create a new SciFiData object of the correct derived type */
    virtual SciFiDataBase* MakeDataObject();

    /** Make pt resolution graphs, as a function of PtMC */
    void MakePtPtResolutions();

    /** Make pz resolution graphs, as a function of PtMC */
    void make_pzpt_resolutions();

    /** Make pt resolution graphs, as a function of PzMC */
    void MakePtPzResolutions();

    /** Make pz resolution graphs, as a function of PzMC */
    void MakePzPzResolutions();

    /** Plot the data currently held */
    virtual void Plot(TCanvas* aCanvas = NULL);

    /** Save data and plots to a ROOT file  */
    void Save();

    /** Set the cut on reconstructed pz using a double, in MeV/c */
    void SetCutPzRec(double cut_pz_rec) { mCutPzRec = cut_pz_rec; }

    /** Set the SciFiData object associated with the display */
    SciFiDataBase* SetData(SciFiDataBase* data);

    /** Set the number of bins used to make the histos for resolution graphs
        Note: If set to 0, the histos will use the number of bins set automatically by ROOT
              when drawn from the TTree.
    */
    void SetNBinsPt(double n_pt_bins) { mNBinsPt = n_pt_bins; }

    /** Set the number of bins used to make the histos for pz resolution graphs
        Note: If set to 0, the histos will use the number of bins set automatically by ROOT
              when drawn from the TTree.
    */
    void SetNBinsPz(double n_pz_bins) { mNBinsPz = n_pz_bins; }

    /** Set the number of points used in the pz resolution plots */
    void SetNPoints(double n_pz_points) { mNPoints = n_pz_points; }

    /** Set the lower limit of each fit used to calc the pt resolution pnts  */
    void SetPtFitMin(double pt_fit_min) { mPtFitMin = pt_fit_min; }

    /** Set the upper limit of each fit used to calc the pt resolution pnts  */
    void SetPtFitMax(double pt_fit_max) { mPtFitMax = pt_fit_max; }

    /** Set the lower limit of each fit used to calc the pz resolution pnts  */
    void SetPzFitMin(double pz_fit_min) { mPzFitMin = pz_fit_min; }

    /** Set the upper limit of each fit used to calc the pz resolution pnts  */
    void SetPzFitMax(double pz_fit_max) { mPzFitMax = pz_fit_max; }

    /** Set the lower bound of the PzMC range used in the resolution plots */
    void SetLowerBoundPzMC(double lower_bound_pzmc) { mLowerBoundPzMC = lower_bound_pzmc; }

    /** Set the lower bound of the PzMC range used in the resolution plots */
    void SetUpperBoundPzMC(double upper_bound_pzmc) { mUpperBoundPzMC = upper_bound_pzmc; }

    /** Set the lower bound of the PtMC range used in the resolution plots */
    void SetResolLowerBound(double pz_lower_bound) { mLowerBoundPtMC = pz_lower_bound; }

    /** Set the upper bound of the PtMC range used in the resolution plots */
    void SetResolUpperBound(double pz_upper_bound) { mUpperBoundPtMC = pz_upper_bound; }

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

    TFile* mOf1;                   /** The output ROOT file */
    TTree* mTree;                  /** The ROOT tree used to accumulate the reduced data */
    MomentumDataPR mTrackData;     /** Struct containing reduced data for 1 track in a spill */

    // Resolution Graphs
    TGraphErrors* mT1PtResolPtMC;
    TGraphErrors* mT2PtResolPtMC;
    TGraphErrors* mT1PzResolPtMC;
    TGraphErrors* mT2PzResolPtMC;
    TGraphErrors* mT1PtResolPzMC;
    TGraphErrors* mT2PtResolPzMC;
    TGraphErrors* mT1PzResolPzMC;
    TGraphErrors* mT2PzResolPzMC;

    // Parameters
    int mNBinsPt;            /// Number of bins used to make the histos for pt resolution graphs
    int mNBinsPz;            /// Number of bins used to make the histos for pz resolution graphs
    int mNPoints;            /// Number of points in the resolution plots
    int mPtFitMin;           /// The lower limit of each fit used to calc the pt resolution pnts
    int mPtFitMax;           /// The upper limit of each fit used to calc the pt resolution pnts
    int mPzFitMin;           /// The lower limit of each fit used to calc the pz resolution pnts
    int mPzFitMax;           /// The upper limit of each fit used to calc the pz resolution pnts
    double mLowerBoundPtMC;  /// The lower bound of the PtMC range for the resolution graphs
    double mUpperBoundPtMC;  /// The upper bound of the PtMC range for the resolution graphs
    double mUpperBoundPzMC;  /// The upper bound of the PzMC range used for the resolution graphs
    double mLowerBoundPzMC;  /// The lower bound of the PzMC range used for the resolution graphs
    double mCutPzRec;        /// Cut on the reconstructed pz
};

} // ~namespace MAUS

#endif
