/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// C / C++ headers
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

// ROOT headers
#include "TCanvas.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGClient.h"
#include "TCut.h"
#include "TPaveText.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerDataAnalyserMomentum.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"


int main( int argc, char **argv )
{
  std::cout << "Kalman Test Program.\n";

  std::cout << std::endl;
  return 0;
}
