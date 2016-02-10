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

 /** @class PIDVarE
 *  @author Celeste Pidcott, University of Warwick
 *  PID variable class, for EMR range, derived
 *  from PIDBase
 */

#ifndef PIDVARE_HH
#define PIDVARE_HH

#include <string>
#include <utility>

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

  class PIDVarE : public PIDBase1D {
  public:
    /// Constructor to create a PDF
    explicit PIDVarE(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    PIDVarE(TFile* file, std::string hypothesis, int minD, int maxD);
    /// Destructor
    ~PIDVarE();

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

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of bins in PDF
    static const int numBins = 100;
    /// Minimum value of range of PDF
    static const int minBinE = 0;
    /// Maximum value of range of PDF
    static const int maxBinE = 1000;
    /// Minimum value of cut for PID
    static const int minE = 0;
    /// Maximum value of cut for PID
    static const int maxE = 1000;
  };
}
}
}

#endif
