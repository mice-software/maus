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

//////////////////////////////////////////////////////////////////////
/// \file GlobalReconEnums.hh
///
/// This file provides enumerators for: a) keeping track of the
/// locations of space- and trackpoints within the global
/// reconstruction and b) the particle ID hypotheses of reconstructed
/// objects.

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALRECONENUMS_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALRECONENUMS_HH_

namespace MAUS {
namespace recon {
namespace global {

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
 * spacepoint.  The kVirtual entry allows for the definition of
 * arbitrary space- or trackpoints, by filling a separate member
 * variable with a geometry path string.  For the code to work, we
 * need to keep the values consecutive and constant.  I hope no new
 * values will be needed, but if so they should be added at the end,
 * with an incremental step and the `kDetectorPointSize' constant
 * updated.
 */
enum DetectorPoint {
  kUndefined  =  0, /**< enum Undefined, used for initialization only */
  kVirtual    =  1, /**< enum For a Virtual point with a geometry path */
  kTOF0       =  2,
  kTOF1       =  3,
  kTOF2       =  4,
  kCkovA      =  5,
  kCkovB      =  6,
  kKL         =  7,
  kEMR        =  8,
  kTracker1   =  9,
  kTracker1S1 = 10,
  kTracker1S2 = 11,
  kTracker1S3 = 12,
  kTracker1S4 = 13,
  kTracker1S5 = 14,
  kTracker2   = 15,
  kTracker2S1 = 16,
  kTracker2S2 = 17,
  kTracker2S3 = 18,
  kTracker2S4 = 19,
  kTracker2S5 = 20,
  kGVA        = 21,
  kBPM        = 22
  // If you add an entry, remember to update kDetectorPointSize below.
};

/// A constant defining the upper limit of the DetectorPoint enum
/// (i.e. one more than the highest value above).
const int kDetectorPointSize = 23;

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
  kNoPID    = 0,
  kElectron = 11,
  kMuon     = 13,
  kPion     = 211,
  kProton   = 2212
};
/** @} */ // end of globalrecon

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS

#endif
