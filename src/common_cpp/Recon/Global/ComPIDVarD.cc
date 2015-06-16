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

#include <string>
#include <vector>
#include "Recon/Global/ComPIDVarD.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string ComPIDVarD::VARIABLE = "CommissioningEMRrange";

  ComPIDVarD::ComPIDVarD(std::string hypothesis, std::string unique_identifier)
    : PIDBase1D(VARIABLE, hypothesis, unique_identifier, minBinComD, maxBinComD,
                numBins) {
    _nonZeroHistEntries = true;
  }

  ComPIDVarD::ComPIDVarD(TFile* file, std::string hypothesis, int minComD,
			 int maxComD)
    : PIDBase1D(file, VARIABLE, hypothesis, minComD, maxComD, minBinComD,
		maxBinComD) {
  }

  ComPIDVarD::~ComPIDVarD() {}

  std::pair<double, double> ComPIDVarD::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    EMR_range = track->get_emr_range_primary();
    if (EMR_range == 0.0) {
      Squeak::mout(Squeak::debug) << "Global track records no range " <<
	" of particle in EMR, Recon::Global::ComPIDVarD::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else if ( minBinComD > EMR_range || EMR_range > maxBinComD ) {
      Squeak::mout(Squeak::debug) << "Range of particle in EMR " <<
	"outside of PDF range, Recon::Global::ComPIDVarD::Calc_Var()" <<
	std::endl;
      return std::make_pair(-1, 0);
    } else {
      return std::make_pair(EMR_range, 0);
    }
  }
}
}
}


