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

 /** @class ComPIDVarB
 *  @author Celeste Pidcott, University of Warwick
 *  Commissioning PID variable class using the relation between the TOF1-2
 *  time of flight and the ADC charge product in the KL.
 * 
 */

#ifndef COMPIDVARB_HH
#define COMPIDVARB_HH

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

  class ComPIDVarB : public PIDBase2D {
  public:
    /// Constructor to create a PDF
    explicit ComPIDVarB(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    ComPIDVarB(TFile* file, std::string hypothesis, int XminComB, int XmaxComB,
	       int YminComB, int YmaxComB);
    /// Destructor
    ~ComPIDVarB();

  private:

    /** @brief Calculate value of PID variable
     *
     *  @param track The track for which the variable is calculated
     *
     *	@return Returns value of PID variable
     */
    std::pair<double, double> Calc_Var(MAUS::DataStructure::Global::Track*
				       track);

    /// Track Points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    global_track_points;
    /// KL track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    kl_track_points;
    /// TOF1 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tof1_track_points;
    /// TOF2 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tof2_track_points;

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of X bins in PDF
    static const int XnumBins = 40;
    /// Minimum value of X range of PDF
    static const int XminBinComB = 20;
    /// Maximum value of X range of PDF
    static const int XmaxBinComB = 40;
    /// Number of Y bins in PDF
    static const int YnumBins = 200;
    /// Minimum value of Y range of PDF
    static const int YminBinComB = 0;
    /// Maximum value of Y range of PDF
    static const int YmaxBinComB = 8000;
    /// Minimum value of X cut for PID
    static const int XminComB = 20;
    /// Maximum value of X cut for PID
    static const int XmaxComB = 40;
    /// Minimum value of Y cut for PID
    static const int YminComB = 0;
    /// Maximum value of Y cut for PID
    static const int YmaxComB = 8000;
  };
}
}
}

#endif
