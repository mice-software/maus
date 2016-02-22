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

 /** @class PIDVarI
 *  @author Celeste Pidcott, University of Warwick
 *  PID variable class, for CkovA PES vs momentum measured in the upstream
 *  tracker, derivedfrom PIDBase
 */

#ifndef PIDVARI_HH
#define PIDVARI_HH

#include <string>
#include <utility>
#include <vector>

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

  class PIDVarI : public PIDBase2D {
  public:
    /// Constructor to create a PDF
    explicit PIDVarI(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    PIDVarI(TFile* file, std::string hypothesis, int XminF, int XmaxF, int YminF, int YmaxF);
    /// Destructor
    ~PIDVarI();

  private:

    /** @brief Calculate value of PID variable
     *
     *  @param track The track for which the variable is calculated
     *
     *	@return Returns value of PID variable
     */
    std::pair<double, double> Calc_Var(MAUS::DataStructure::Global::Track* track);

    /// # of photoelectrons in CkovA
    double CkovA_pes;
    /// Track Points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    global_track_points;
    /// Tracker 1 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tracker0_track_points;

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of X bins in PDF
    static const int XnumBins = 150;
    /// Minimum value of X range of PDF
    static const int XminBinI = 50;
    /// Maximum value of X range of PDF
    static const int XmaxBinI = 350;
    /// Number of Y bins in PDF
    static const int YnumBins = 100;
    /// Minimum value of Y range of PDF
    static const int YminBinI = 0;
    /// Maximum value of Y range of PDF
    static const int YmaxBinI = 140;
    /// Minimum value of X cut for PID
    static const int XminI = 50;
    /// Maximum value of X cut for PID
    static const int XmaxI = 350;
    /// Minimum value of Y cut for PID
    static const int YminI = 0;
    /// Maximum value of Y cut for PID
    static const int YmaxI = 140;
  };
}
}
}

#endif
