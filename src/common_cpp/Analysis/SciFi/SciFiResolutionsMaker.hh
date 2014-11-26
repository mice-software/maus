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

/** @class SciFiResolutionsMaker
 *
 *   Calculate the resolutions of scifi track parameters
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_SCIFIRESOLUTIONSMAKER_
#define _SRC_COMMON_CPP_RECON_SCIFI_SCIFIRESOLUTIONSMAKER_

#include <string>

// ROOT headers
#include "TTree.h"
#include "TGraphErrors.h"
#include "TCut.h"

// MAUS headers

namespace MAUS {

class SciFiResolutionsMaker {
  public:
    /** Default constructor */
    SciFiResolutionsMaker();

    /** Destructor */
    virtual ~SciFiResolutionsMaker();

    /** Calculate the resolution for a particular MC truth interval,
     *  by plotting a histo of the MC - Recon data for the interval,
     *  fitting a gaussian, and returning the sigma and error on sigma.
     */
    bool CalcResolution(TTree* aTree, const std::string& aResidual,
                        const TCut& aCut, double& aRes, double &aResError);

    /** Return the cuts */
    TCut GetCuts() const { return mCuts; }

    /** Return the lower bound of the x axis range used in the resolution plots */
    double GetGraphLowerBound() const { return mGraphLowerBound; }

    /** Return the upper bound of the x axis range used in the resolution plots */
    double GetGraphUpperBound() const { return mGraphUpperBound; }

    /** Return the lower bound of the fit range used in the resolution histos */
    double GetFitMin() const { return mFitMin; }

    /** Return the upper bound of the fit range used in the resolution histos */
    double GetFitMax() const { return mFitMax; }

    /** Return the number of points used in the resolution graph */
    int GetNGraphPoints() const { return mNGraphPoints; }

    /** Return the number of bins used in the resolution histos */
    int GetNHistoBins() const { return mNHistoBins; }

    /** Make the resolution graph.
     *  @param aTree A pointer to the ROOT holding the data
     *  @param aXParam The variable used for the x axis of the graph e.g. PtMC, PzMC
     *  @param aYResidualT1 The expression used to get the residual of the y variable in
     *                      tracker 1 e.g. "PzMc+Charge*PzRec"
     *  @param aYResidualT2 The expression used to get the residual of the y variable in
     *                      tracker 2 e.g. "PzMc-Charge*PzRec"
     *  @param aT1Graph The resolution graph for tracker 1
     *  @param aT2Graph The resolution graph for tracker 2
     */
    void MakeGraph(TTree* aTree, const std::string& aXParam,
                   const std::string&  aYResidualT1,
                   const std::string&  aYResidualT2,
                   TGraphErrors* aT1Graph, TGraphErrors* aT2Graph);

    /** Set the cuts */
    void SetCuts(TCut aCuts) { mCuts = aCuts; }

    /** Set the lower bound of the x axis range used in the resolution plots */
    void SetGraphLowerBound(double aGraphLowerBound) { mGraphLowerBound = aGraphLowerBound; }

    /** Set the upper bound of the x axis range used in the resolution plots */
    void SetGraphUpperBound(double aGraphUpperBound) { mGraphUpperBound = aGraphUpperBound; }

    /** Set the lower bound of the fit range used in the resolution histos */
    void SetFitMin(double aFitMin) { mFitMin = aFitMin; }

    /** Set the upper bound of the fit range used in the resolution histos */
    void SetFitMax(double aFitMax) { mFitMax = aFitMax; }

    /** Set the number of points used in the resolution graph */
    void SetNGraphPoints(double aNGraphPoints) { mNGraphPoints = aNGraphPoints; }

    /** Set the number of bins used in the resolution histos */
    void SetNHistoBins(double aNHistoBins) { mNHistoBins = aNHistoBins; }

  private:
    int mNGraphPoints;       // Number of points in the resolution graph
    int mNHistoBins;         /// Number of bins used to make the histos for the resolution graph
    double mGraphLowerBound; /// The lower bound of the "x" range for the resolution graph
    double mGraphUpperBound; /// The upper bound of the "x" range for the resolution graph
    double mFitMin;          /// The lower limit of each fit used to calc the resolution pnts
    double mFitMax;          /// The upper limit of each fit used to calc the resolution pnts
    TCut mCuts;              /// Additional cuts to be applied
};

} // ~namespace MAUS

#endif
