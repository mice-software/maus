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

 /** @class PIDVarC
 *  @author Celeste Pidcott, University of Warwick
 *  PID variable class using the relation between the momentum measured
 *  in the downstream tracker and the ADC charge product in the KL.
 * 
 */

#ifndef PIDVARC_HH
#define PIDVARC_HH

#include <string>
#include <utility>
#include <vector>

#include "TLorentzVector.h"
#include "TH2.h"
#include "TFile.h"

#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "DataStructure/Global/BasePoint.hh"
#include "DataStructure/ReconEvent.hh"

#include "Recon/Global/PIDBase2D.hh"

namespace MAUS {
namespace recon {
namespace global {

  class PIDVarC : public PIDBase2D {
  public:
    /// Constructor to create a PDF
    explicit PIDVarC(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    PIDVarC(TFile* file, std::string hypothesis);
    /// Destructor
    ~PIDVarC();

  private:

    /** @brief Calculate value of PID variable
     *
     *  @param track The track for which the variable is calculated
     *
     *	@return Returns value of PID variable
     */
    std::pair<double, double> Calc_Var(MAUS::DataStructure::Global::Track* track);

    /// Track Points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    global_track_points;
    /// KL track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    kl_track_points;
    /// Tracker 1 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tracker1_track_points;

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of X bins in PDF
    static const int XnumBins = 100;
    /// Minimum value of X range of PDF
    static const int XminBinC = 50;
    /// Maximum value of X range of PDF
    static const int XmaxBinC = 350;
    /// Number of Y bins in PDF
    static const int YnumBins = 400;
    /// Minimum value of Y range of PDF
    static const int YminBinC =0;
    /// Maximum value of Y range of PDF
    static const int YmaxBinC = 8000;
  };
}
}
}

#endif
