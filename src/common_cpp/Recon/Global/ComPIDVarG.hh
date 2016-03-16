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

 /** @class ComPIDVarG
 *  @author Celeste Pidcott, University of Warwick
 *  Comissioning PID variable class, for EMR density vs TOF1-TOF2 time of flight,
 *  derived from PIDBase
 */

#ifndef COMPIDVARG_HH
#define COMPIDVARG_HH

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

  class ComPIDVarG : public PIDBase2D {
  public:
    /// Constructor to create a PDF
    explicit ComPIDVarG(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    ComPIDVarG(TFile* file, std::string hypothesis, int XminComG,
	       int XmaxComG, int YminComG, int YmaxComG);
    /// Destructor
    ~ComPIDVarG();

  private:

    /** @brief Calculate value of PID variable
     *
     *  @param track The track for which the variable is calculated
     *
     *	@return Returns value of PID variable
     */
    std::pair<double, double> Calc_Var(MAUS::DataStructure::Global::Track* track);

    /// (Primary) range of particle in EMR
    double EMR_density;

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of X bins in PDF
    static const int XnumBins = 60;
    /// Minimum value of X range of PDF
    static const int XminBinComG = 20;
    /// Maximum value of X range of PDF
    static const int XmaxBinComG = 50;
    /// Number of Y bins in PDF
    static const int YnumBins = 100;
    /// Minimum value of Y range of PDF
    static const int YminBinComG = 0;
    /// Maximum value of Y range of PDF
    static const int YmaxBinComG = 1;
    /// Minimum value of X cut for PID
    static const int XminComG = 20;
    /// Maximum value of X cut for PID
    static const int XmaxComG = 50;
    /// Minimum value of Y cut for PID
    static const int YminComG = 0;
    /// Maximum value of Y cut for PID
    static const int YmaxComG = 1;
  };
}
}
}

#endif
