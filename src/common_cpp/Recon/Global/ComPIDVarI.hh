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

 /** @class ComPIDVarI
 *  @author Celeste Pidcott, University of Warwick
 *  Comissioning PID variable class, for CkovB PES vs TOF1-TOF2 time of flight,
 *  derived from PIDBase
 */

#ifndef COMPIDVARI_HH
#define COMPIDVARI_HH

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

  class ComPIDVarI : public PIDBase2D {
  public:
    /// Constructor to create a PDF
    explicit ComPIDVarI(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    ComPIDVarI(TFile* file, std::string hypothesis, int XminComI,
	       int XmaxComI, int YminComI, int YmaxComI);
    /// Destructor
    ~ComPIDVarI();

  private:

    /** @brief Calculate value of PID variable
     *
     *  @param track The track for which the variable is calculated
     *
     *	@return Returns value of PID variable
     */
    std::pair<double, double> Calc_Var(MAUS::DataStructure::Global::Track* track);

    /// # of photoelectrons in CkovB
    double CkovB_pes;

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of X bins in PDF
    static const int XnumBins = 80;
    /// Minimum value of X range of PDF
    static const int XminBinComI = 30;
    /// Maximum value of X range of PDF
    static const int XmaxBinComI = 70;
    /// Number of Y bins in PDF
    static const int YnumBins = 140;
    /// Minimum value of Y range of PDF
    static const int YminBinComI = 0;
    /// Maximum value of Y range of PDF
    static const int YmaxBinComI = 140;
    /// Minimum value of X cut for PID
    static const int XminComI = 20;
    /// Maximum value of X cut for PID
    static const int XmaxComI = 40;
    /// Minimum value of Y cut for PID
    static const int YminComI = 0;
    /// Maximum value of Y cut for PID
    static const int YmaxComI = 140;
  };
}
}
}

#endif
