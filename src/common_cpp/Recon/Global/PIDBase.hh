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

 /** @class PIDBase
 *  Base class containing constructors and functions required for PID
 *  analysis
 */

#ifndef PIDBASE_HH
#define PIDBASE_HH

#include <stdio.h>
#include <string>

#include "TMath.h"
#include "TMathBase.h"
#include "TTimeStamp.h"
#include "TH1.h"
#include "TFile.h"
#include "TUnixSystem.h"
#include "TSystem.h"

#include "Interface/Squeak.hh"

#include "DataStructure/Global/Track.hh"


namespace MAUS {
namespace recon {
namespace global {

  class PIDBase {

  public:

    /// Constructor to create a PDF
    PIDBase(int minBin, int maxBin, int numBins, std::string variable,
	    std::string hypothesis, std::string unique_identifier);
    /// Constructor to use a PDF to perform PID analysis
    PIDBase(TFile* file, std::string variable, std::string hypothesis);
    /// Destructor
    virtual ~PIDBase();

    /// Get _hyp (hypothesis used by constructor that uses an existing PDF)
    std::string Get_hyp();

    /// Get __var_name (PID variable)
    std::string Get_var_name();

    /// Get filename of file containing PDF created by constructor
    std::string Get_filename();

    /// Get directory of file containing PDF created by constructor
    std::string Get_directory();

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
    void Fill_TH1(MAUS::DataStructure::Global::Track* track);

  protected:
    /// Lower and upper values of range, and number of bins in range, of PDF,
    /// set in derived PID classes
    int _minBin, _maxBin, _numBins;

    /// Check if we want to add one event spread over the entire histogram, to
    /// avoid bins with zero entries
    bool _nonZeroHistEntries;

    double _addToAllBins;

  private:

    /** @brief Virtual function (defined in derived classes) that calculates
     *    value of PID variable
     *
     *  @param track The track for which the variable is calculated
     *
     */
    virtual double Calc_Var(MAUS::DataStructure::Global::Track* track) = 0;

    /// Name of PID variable
    std::string _var_name;

    /// Particle hypothesis
    std::string _hyp;

    /// Unique identifier for naming root files holding PDFs when reducer is run
    std::string _unique_identifier;

    /// _var_name + _hyp;
    std::string _varhyp;

    /// Filename of file containing PDF created by constructor
    std::string _filename;

    /// Directory of file containing PDF created by constructor
    std::string _directory;

    /// Histogram that forms PDF
    TH1F *_hist;

    /// FIle that PDF is written to
    TFile *_writeFile;
  };
}
}
}

#endif