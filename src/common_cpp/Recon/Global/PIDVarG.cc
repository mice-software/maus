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
#include "Recon/Global/PIDVarG.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string PIDVarG::VARIABLE = "EMRPlaneDensity";

  PIDVarG::PIDVarG(std::string hypothesis, std::string unique_identifier)
    : PIDBase1D(VARIABLE, hypothesis, unique_identifier, minBinG, maxBinG,
                numBins) {
    _nonZeroHistEntries = true;
  }

  PIDVarG::PIDVarG(TFile* file, std::string hypothesis, int minG, int maxG)
    : PIDBase1D(file, VARIABLE, hypothesis, minG, maxG, minBinG, maxBinG) {
  }

  PIDVarG::~PIDVarG() {}

  std::pair<double, double> PIDVarG::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    EMR_density = track->get_emr_plane_density();
    if (EMR_density == -1) {
      Squeak::mout(Squeak::debug) << "Global track was passed an EMR event " <<
	" with plane density -1,Recon::Global::PIDVarG::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else if ( minBinG > EMR_density || EMR_density > maxBinG ) {
      Squeak::mout(Squeak::debug) << "Plane density in EMR " <<
	"outside of PDF range, Recon::Global::PIDVarG::Calc_Var()" <<
	std::endl;
      return std::make_pair(-1, 0);
    } else {
      return std::make_pair(EMR_density, 0);
    }
  }
}
}
}


