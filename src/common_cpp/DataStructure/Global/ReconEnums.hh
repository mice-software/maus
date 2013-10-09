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


/** @file GlobalReconEnums.hh
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
 * 
 *  This file provides enumerators for: a) keeping track of the
 *  locations of space- and track_points within the global
 *  reconstruction and b) the particle ID hypotheses of reconstructed
 *  objects.
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALRECONENUMS_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALRECONENUMS_HH_

namespace MAUS {
namespace DataStructure {
namespace Global {

/** @defgroup globalrecon Global Reconstruction
 *  The global reconstruction software, including both the associated
 *  data structures used for storage and the mappers containing
 *  reconstruction algorithms.
 *  @{
 */

/*! \public
 *  \brief Defines detector element, or kVirtual for non-detector
 *  point.

 * An enum to define the detector or geometry element for the
 * SpacePoint.  The kVirtual entry allows for the definition of
 * arbitrary space- or track_points, by filling a separate member
 * variable with a geometry path string.  For the code to work, we
 * need to keep the values consecutive and constant.  I hope no new
 * values will be needed, but if so they should be added at the end,
 * with an incremental step and the `kDetectorPointSize' constant
 * updated.
 */
enum DetectorPoint {
  kUndefined   =  0, /**< enum Undefined, used for initialization only */
  kVirtual     =  1, /**< enum For a Virtual point with a geometry path */
  kTOF0_1      =  2,
  kTOF0_2      =  3,
  kCherenkov1  =  4,
  kTOF1_1      =  5,
  kTOF1_2      =  6,
  kTracker1    =  7,
  kTracker1_1  =  8,
  kTracker1_2  =  9,
  kTracker1_3  = 10,
  kTracker1_4  = 11,
  kTracker1_5  = 12,
  kTracker2    = 13,
  kTracker2_1  = 14,
  kTracker2_2  = 15,
  kTracker2_3  = 16,
  kTracker2_4  = 17,
  kTracker2_5  = 18,
  kTOF2_1      = 19,
  kTOF2_2      = 20,
  kCherenkov2  = 21,
  kCalorimeter = 22,
  kEMR         = 23
  // If you add an entry, remember to update kDetectorPointSize below.
};

/// A constant defining the upper limit of the DetectorPoint enum
/// (i.e. one more than the highest value above).
const int kDetectorPointSize = 24;

//////////////////////////////////////////////////////////////////////
///  @brief Defines the PID hypothesis for reconstruction objects,
///  matches the PDG codes.
///
///  An enum to define the particle hypotheses used for track fitting or
///  particle ID.  The values match the PDG codes, allowing for
///  consistent expansion as necessary.  The full possible list is
///  available at:
///  http://pdg.lbl.gov/2012/reviews/rpp2012-rev-monte-carlo-numbering.pdf
///  Particles and antiparticles are considered identically here.
enum PID {
  kNoPID                =  0,
  kEMinus               =  11,
  kEPlus                = -11,
  kElectronNeutrino     =  12,
  kElectronAntineutrino = -12,
  kMuMinus              =  13,
  kMuPlus               = -13,
  kMuonNeutrino         =  14,
  kMuonAntineutrino     = -14,
  kPhoton               =  22,
  kPiMinus              = -211,
  kPi0                  =  111,
  kPiPlus               =  211,
  kKMinus               = -321,
  kK0                   =  311,
  kKLong                =  130,
  kKShort               =  310,
  kKPlus                =  321,
  kNeutron              =  2112,
  kAntineutron          = -2112,
  kProton               =  2212,
  kAntiproton           = -2212,
  kLambda               =  3122,
  kAntilambda           = -3122,
  kDeuterium            =  1000010020,
  kTritium              =  1000010030,
  kHelium3              =  1000020030,
  kHelium4              =  1000020040
};
/** @} */ // end of globalrecon

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif