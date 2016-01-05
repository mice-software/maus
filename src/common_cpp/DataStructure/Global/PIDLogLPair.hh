/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

/** @class MAUS::DataStructure::Global::PIDLogLPair
 *  @ingroup globalrecon
 *  @brief A container class, for a pid and it's associated log-likelihood.
 *  @author Celeste Pidcott, University of Warwick
 *  @date 2015/03/10
 *
 *  A container class, for a pid and it's associated log-likelihood,
 *  to be used in the Global PID when considering potential particle
 *  hypotheses for a given track, creating a record of the
 *  hypothesis and the log-likelihood that that hypothesis is correct.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_PIDLOGLPAIR_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_PIDLOGLPAIR_HH_

#include <utility>

namespace MAUS {
namespace DataStructure {
namespace Global {

class PIDLogLPair : public std::pair<int, double> {
 public:
  /// Standard Constructor
  PIDLogLPair();

  /// Constructor from values
  PIDLogLPair(int pid, double logL);

  /// Destructor
  virtual ~PIDLogLPair();

  /// Setter for first element, for PIDLogLPairrProcessor
  void set_PID(int PID);

  /// Getter for first element, for PIDLogLPairProcessor
  int get_PID() const;

  /// Setter for second element, for PIDLogLPairProcessor
  void set_logL(double logL);

  /// Getter for second element, for PIDLogLPairProcessor
  double get_logL() const;
};
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
