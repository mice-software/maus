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

 /** @class PIDBase1D
 *  @author Celeste Pidcott, University of Warwick
 *  Class containing constructors and functions required for PID
 *  analysis (1D case)
 */

#ifndef PIDBASE1D_HH
#define PIDBASE1D_HH

#include <stdio.h>
#include <string>

#include "TMath.h"
#include "TMathBase.h"
#include "TTimeStamp.h"
#include "TH1.h"
#include "TFile.h"
#include "TUnixSystem.h"
#include "TSystem.h"

#include "Utils/Squeak.hh"

#include "DataStructure/Global/Track.hh"
#include "Recon/Global/PIDBase.hh"


namespace MAUS {
namespace recon {
namespace global {

  class PIDBase1D : public PIDBase {

  public:

    /// Constructor to create a PDF
    PIDBase1D(std::string variable, std::string hypothesis,
           std::string unique_identifier, int XminBin, int XmaxBin,
           int XnumBins);
    /// Constructor to use a PDF to perform PID analysis
    PIDBase1D(TFile* file, std::string variable, std::string hypothesis,
	      int Xmin, int Xmax, int XminBin, int XmaxBin);
    /// Destructor
    virtual ~PIDBase1D();

    // Get histogram
    TH1F* Get_hist();

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
    TH1F *_hist;

    /// For 1D hist, Y bins and values set to 0
    static const int YnumBins = 0;
    static const int YminBin = 0;
    static const int YmaxBin = 0;
    static const int Ymin = 0;
    static const int Ymax = 0;
  };
}
}
}

#endif
