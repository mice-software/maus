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
#include "Recon/Global/PIDVarE.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string PIDVarE::VARIABLE = "EMRrange";

  PIDVarE::PIDVarE(std::string hypothesis, std::string unique_identifier)
    : PIDBase1D(VARIABLE, hypothesis, unique_identifier, minBinE, maxBinE,
                numBins) {
    _nonZeroHistEntries = true;
  }

  PIDVarE::PIDVarE(TFile* file, std::string hypothesis, int minE, int maxE)
    : PIDBase1D(file, VARIABLE, hypothesis, minE, maxE, minBinE, maxBinE) {
  }

  PIDVarE::~PIDVarE() {}

  std::pair<double, double> PIDVarE::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    EMR_range = track->get_emr_range_primary();
    if (EMR_range == 0.0) {
      Squeak::mout(Squeak::debug) << "Global track records no range " <<
	" of particle in EMR, Recon::Global::PIDVarE::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else if ( minBinE > EMR_range || EMR_range > maxBinE ) {
      Squeak::mout(Squeak::debug) << "Range of particle in EMR " <<
	"outside of PDF range, Recon::Global::PIDVarE::Calc_Var()" <<
	std::endl;
      return std::make_pair(-1, 0);
    } else {
      return std::make_pair(EMR_range, 0);
    }
  }
}
}
}


