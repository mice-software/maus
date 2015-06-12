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

#ifndef MAUS_RECON_CONSTANTS_HH
#define MAUS_RECON_CONSTANTS_HH

namespace MAUS {
namespace Recon {
namespace Constants {

typedef struct BetheBloch {
  /// Bethe Bloch constant in MeV g-1 cm2 (for A=1gmol-1)
  static const double K() { return 0.307075; }
  /// The electron mass is a parameter of Bethe-Bloch's formula.
  static const double Electron_Mass() { return 0.511; }
} BetheBlochParameters;

/// Bethe Bloch constant in MeV g-1 cm2 (for A=1gmol-1)
const double BetheBlochConstant = 0.307075;
/// For multiple scattering calculations. Unit: MeV
const double HighlandConstant = 13.6;
/// Muon mass in MeV/c2
const double MuonMass = 105.6583715;
/// Electron mass in MeV/c2
const double ElectronMass = 0.510998910;


} // ~namespace Constants
} // ~namespace Recon
} // ~namespace MAUS

#endif // MAUS_RECON_CONSTANTS_HH
