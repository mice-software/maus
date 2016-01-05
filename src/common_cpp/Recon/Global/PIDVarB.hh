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

 /** @class PIDVarB
 *  @author Celeste Pidcott, University of Warwick
 *  PID variable class using the relation between the momentum measured
 *  in the upstream tracker and the time of flight between TOF1 and TOF0.
 * 
 */

#ifndef PIDVARB_HH
#define PIDVARB_HH

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

  class PIDVarB : public PIDBase2D {
  public:
    /// Constructor to create a PDF
    explicit PIDVarB(std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    PIDVarB(TFile* file, std::string hypothesis, int XminB, int XmaxB, int YminB, int YmaxB);
    /// Destructor
    ~PIDVarB();

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
    /// TOF0 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tof0_track_points;
    /// TOF1 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tof1_track_points;
    /// Tracker 0 track points from global track
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
    tracker0_track_points;

    /// Name of PID variable
    static const std::string VARIABLE;
    /// Number of X bins in PDF
    static const int XnumBins = 240;
    /// Minimum value of X range of PDF
    static const int XminBinB = 10;
    /// Maximum value of X range of PDF
    static const int XmaxBinB = 250;
    /// Number of Y bins in PDF
    static const int YnumBins = 40;
    /// Minimum value of Y range of PDF
    static const int YminBinB = 20;
    /// Maximum value of Y range of PDF
    static const int YmaxBinB = 40;
    /// Minimum value of X cut for PID
    static const int XminB = 10;
    /// Maximum value of X cut for PID
    static const int XmaxB = 250;
    /// Minimum value of Y cut for PID
    static const int YminB = 20;
    /// Maximum value of Y cut for PID
    static const int YmaxB = 40;
  };
}
}
}

#endif
