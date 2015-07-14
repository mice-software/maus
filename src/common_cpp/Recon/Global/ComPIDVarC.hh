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

 /** @class ComPIDVarC
 *  @author Celeste Pidcott, University of Warwick
 *  Commissioning PID variable class, for KL ADC charge product, derived
 *  from PIDBase
 */

#ifndef COMPIDVARC_HH
#define COMPIDVARC_HH

#include <string>
#include <utility>
#include <vector>

#include "TLorentzVector.h"
#include "TH1.h"
#include "TFile.h"

#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "DataStructure/Global/BasePoint.hh"

#include "Recon/Global/PIDBase1D.hh"

namespace MAUS {
namespace recon {
namespace global {

  class ComPIDVarC : public PIDBase1D {
  public:
    /// Constructor to create a PDF
    explicit ComPIDVarC(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    ComPIDVarC(TFile* file, std::string hypothesis, int minD, int maxD);
    /// Destructor
    ~ComPIDVarC();

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

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of bins in PDF
    static const int numBins = 160;
    /// Minimum value of range of PDF
    static const int minBinComC = 0;
    /// Maximum value of range of PDF
    static const int maxBinComC = 8000;
    /// Minimum value of cut for PID
    static const int minComC = 0;
    /// Maximum value of cut for PID
    static const int maxComC = 8000;
  };
}
}
}

#endif
