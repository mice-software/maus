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
  kUndefined    =  0, /**< enum Undefined, used for initialization only */
  kVirtual      =  1, /**< enum For a Virtual point with a geometry path */
  kTOF0         =  2,
  kTOF0_1       =  3,
  kTOF0_2       =  4,
  kCherenkovA   =  5,
  kCherenkovB   =  6,
  kTOF1         =  7,
  kTOF1_1       =  8,
  kTOF1_2       =  9,
  kTracker0     = 10,
  kTracker0_1   = 11,
  kTracker0_2   = 12,
  kTracker0_3   = 13,
  kTracker0_4   = 14,
  kTracker0_5   = 15,
  kTracker1     = 16,
  kTracker1_1   = 17,
  kTracker1_2   = 18,
  kTracker1_3   = 19,
  kTracker1_4   = 20,
  kTracker1_5   = 21,
  kTOF2         = 22,
  kTOF2_1       = 23,
  kTOF2_2       = 24,
  kCalorimeter  = 25,
  kEMR          = 26
  // If you add an entry, remember to update kDetectorPointSize below.
};

/// A constant defining the upper limit of the DetectorPoint enum
/// (i.e. one more than the highest value above).
const int kDetectorPointSize = 27;

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

//////////////////////////////////////////////////////////////////////
///  @brief Defines the type of a primary chain
///
///  An enum to define the type of a primary chain. kUSOrphan and kDSOrphan
///  are upstream and downstream chains whose tracks have not (yet) been matched
///  into a through track. Once matching has occurred, matched chains will be of
///  types kUS and kDS and a chain with type kThrough is created which has the
///  matched US and DS chains as daughters.
enum ChainType {
  kNoChainType = 0,
  kUS          = 1,
  kDS          = 2,
  kUSOrphan    = 3,
  kDSOrphan    = 4,
  kThrough     = 5
};

//////////////////////////////////////////////////////////////////////
///  @brief Defines the multiplicity of chain children, i.e. whether there exist
///  other chains that due to shared children are mutually exclusive with this one.
///
///  This attribute is only set for through chains. Most chains should be of type
///  kUnique, meaning that that their daughter chains are theirs alone. If a chain's
///  US daughter is shared with another chain, they will both have type kMultipleUS,
///  for a shared DS daughter kMultipleDS. If both daughters of a chain are shared
///  with other chains, it will have type kMultipleBoth. Note that constellations
///  are possible where e.g. there are 3 chains, one of type kMultipleUS, one of
///  type kMultipleDS, and the last of type kMultipleBoth.
enum ChainChildMultiplicity {
  kUnique       = 0,
  kMultipleUS   = 1,
  kMultipleDS   = 2,
  kMultipleBoth = 3
};
/** @} */ // end of globalrecon

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS

#endif
