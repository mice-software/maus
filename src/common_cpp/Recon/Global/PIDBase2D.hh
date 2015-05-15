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

 /** @class PIDBase2D
 *  @author Celeste Pidcott, University of Warwick
 *  Class containing constructors and functions required for PID
 *  analysis (2D case)
 */

#ifndef PIDBASE2D_HH
#define PIDBASE2D_HH

#include <stdio.h>
#include <string>

#include "TMath.h"
#include "TMathBase.h"
#include "TTimeStamp.h"
#include "TH2.h"
#include "TFile.h"
#include "TUnixSystem.h"
#include "TSystem.h"

#include "Interface/Squeak.hh"

#include "DataStructure/Global/Track.hh"
#include "Recon/Global/PIDBase.hh"


namespace MAUS {
namespace recon {
namespace global {

  class PIDBase2D : public PIDBase {

  public:

    /// Constructor to create a PDF
    PIDBase2D(std::string variable, std::string hypothesis,
           std::string unique_identifier, int XminBin, int XmaxBin,
           int XnumBins, int YminBin, int YmaxBin, int YnumBins);
    /// Constructor to use a PDF to perform PID analysis
    PIDBase2D(TFile* file, std::string variable, std::string hypothesis,
	      int Xmin, int Xmax, int Ymin, int Ymax);
    /// Destructor
    virtual ~PIDBase2D();

    // Get histogram
    TH2F* Get_hist();

    /** @brief Calculate log likelihood of incoming track corresponding to 
     *   a particle hypothesis
     *
     *  @param track The track for which the log likelihood is calculated
     *
     *  @return Returns the log likelihood
     */
    double logL(MAUS::DataStructure::Global::Track* track);

    /** @brief Fill histogram with value of PID variable of global track
     *
     *  @param track The track to have its PID variable value added to PDF
     */
    void Fill_Hist(MAUS::DataStructure::Global::Track* track);


  protected:

    /// If _nonZeroHistEntries is true, fraction of event to add to all bins
    double _addToAllBins;

  private:

    /// _var_name + _hyp;
    std::string _varhyp;

    /// Histogram that forms PDF
    TH2F *_hist;
  };
}
}
}

#endif
