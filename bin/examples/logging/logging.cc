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
#include <sstream>

// MAUS headers
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/Squeak.hh"


/** Example programme for MAUS logging
 *  A. Dobbs
 *  August 2016
 *
 *  If verbose_level is less than or equal to:
 *   - 0 Squeak::debug then mout(Squeak::debug) redirects to std::cout
 *   - 1 Squeak::info then mout(Squeak::info) redirects to std::clog
 *   - 2 Squeak::warning then mout(Squeak::warning) redirects to std::cerr
 *   - 3 Squeak::error then mout(Squeak::error) redirects to std::cerr
 *   - 4 Squeak::fatal then mout(Squeak::fatal) redirects to std::cerr
 *  Note that the redirection is independent of setStandardOutputs status
 * 
 *  log_level defines the amount of output going to the log file:
 *   - 0 no log file is opened
 *   - 1 log file open, Squeak::log goes there
 *   - 2 log file open, Squeak::log goes there and any other streams not going to screen
 *  NB: Streams to MAUS::Squeak::mout(MAUS::Squeak::log) will only write to file if the
 *  log_level is set to 1 or greater (otherwise just /dev/null).
 */

int main(int argc, char *argv[]) {
  // Argument handling first
  if (argc < 2) {
    std::cerr << "Please supply a MAUS file as the first programme argument" << std::endl;
    return 1;
  }
  // First argument to code should be the input ROOT file name (a processed MAUS file)
  std::string filename = std::string(argv[1]);
  // If there is a second argument it should be the verbosity level
  std::stringstream ss1;
  int verbose_level = 0;
  if (argc > 2) {
    ss1 << argv[2];
    ss1 >> verbose_level;
  }
  std::cout << "Verbose level: " << verbose_level << std::endl;
  // If there is a third argument it should be the log level
  std::stringstream ss2;
  int log_level = 0;
  if (argc > 3) {
    ss2 << argv[3];
    ss2 >> log_level;
  }
  std::cout << "Logging level: " << log_level << std::endl;

  // Set the verbosity and logging levels
  MAUS::Squeak::setLogName("the_log.out");
  MAUS::Squeak::setStandardOutputs(verbose_level);
  MAUS::Squeak::setOutputs(verbose_level, log_level);

  // Set up the input file, spill and data objects
  MAUS::Squeak::mout(MAUS::Squeak::info) << "Opening file " << filename << std::endl;
  MAUS::Squeak::mout(MAUS::Squeak::log) << filename << std::endl;
  irstream infile(filename.c_str(), "Spill");
  MAUS::Data data;

  // Loop over all spills
  while ( infile >> readEvent != NULL ) {
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    if (spill == NULL || spill->GetDaqEventType() != "physics_event") {
      MAUS::Squeak::mout(MAUS::Squeak::debug) << "Not a usable spill\n";
      continue;
    }
    MAUS::Squeak::mout(MAUS::Squeak::info) << "Spill: " << spill->GetSpillNumber() << std::endl;
    // MAUS::Squeak::mout(MAUS::Squeak::log) << "Spill: " << spill->GetSpillNumber() << std::endl;
  }
  // Don't forgot to close the log when you are done!!
  MAUS::Squeak::closeLog();
}

