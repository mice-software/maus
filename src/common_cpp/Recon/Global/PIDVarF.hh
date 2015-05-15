/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

 /** @class ComPIDVarE
 *  @author Celeste Pidcott, University of Warwick
 *  Comissioning PID variable class, for EMR range vs TOF1-TOF2 time of flight,
 *  derivedfrom PIDBase
 */

#ifndef COMPIDVARE_HH
#define COMPIDVARE_HH

#include <string>
#include <utility>

#include "TLorentzVector.h"
#include "TH1.h"
#include "TFile.h"

#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "DataStructure/Global/BasePoint.hh"

#include "Recon/Global/PIDBase2D.hh"

namespace MAUS {
namespace recon {
namespace global {

  class PIDVarF : public PIDBase2D {
  public:
    /// Constructor to create a PDF
    explicit PIDVarF(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    PIDVarF(TFile* file, std::string hypothesis, int XminF, int XmaxF, int YminF, int YmaxF);
    /// Destructor
    ~PIDVarF();

  private:

    /** @brief Calculate value of PID variable
     *
     *  @param track The track for which the variable is calculated
     *
     *	@return Returns value of PID variable
     */
    std::pair<double, double> Calc_Var(MAUS::DataStructure::Global::Track* track);

    /// (Primary) range of particle in EMR
    double EMR_range;
    /// Track Points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    global_track_points;
    /// Tracker 1 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tracker1_track_points;

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of X bins in PDF
    static const int XnumBins = 150;
    /// Minimum value of X range of PDF
    static const int XminBinF = 50;
    /// Maximum value of X range of PDF
    static const int XmaxBinF = 350;
    /// Number of Y bins in PDF
    static const int YnumBins = 100;
    /// Minimum value of Y range of PDF
    static const int YminBinF = 0;
    /// Maximum value of Y range of PDF
    static const int YmaxBinF = 1000;
    /// Minimum value of X cut for PID
    static const int XminF = 50;
    /// Maximum value of X cut for PID
    static const int XmaxF = 350;
    /// Minimum value of Y cut for PID
    static const int YminF = 0;
    /// Maximum value of Y cut for PID
    static const int YmaxF = 1000;
  };
}
}
}

#endif
