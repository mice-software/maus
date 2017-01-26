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

/** @class SciFiSeed
 *
 *
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISEED_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFISEED_HH_


// C++ headers
#include <string>
#include <vector>

// ROOT headers
#include "TObject.h"
#include "TRefArray.h"
#include "TMatrixD.h"
#include "TRef.h"

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"

namespace MAUS {

class SciFiSeed : public TObject {
 public:

  /** Default constructor - initialises to 0/NULL */
  SciFiSeed();

  /** Copy constructor - any pointers are deep copied */
  SciFiSeed(const SciFiSeed &_scifiseed);

  /** Destructor  */
  virtual ~SciFiSeed();

  /** Assignment operator - any pointers are deep copied */
  SciFiSeed& operator=(const SciFiSeed &_scifiseed);


  void setTracker(int tracker) { _tracker = tracker; }

  int getTracker() const { return _tracker; }


  int getDimension() const { return _state_vector.size(); }


//  ThreeVector getPosition() const { return _position; }
//
//  ThreeVector getMomentum() const { return _momentum; }


  void setStateVector(TMatrixD vector);

  TMatrixD getStateVector() const;


  void setCovariance(TMatrixD covariance);

  TMatrixD getCovariance() const;


  void setPRTrackTobject(TObject* const pr_track) { *_pr_track = pr_track; }

  TObject* getPRTrackTobject() const 
                                 { return static_cast<SciFiBasePRTrack*>(_pr_track->GetObject()); }


  void setAlgorithm(int algo) { _algorithm = algo; }

  int getAlgorithm() const { return _algorithm; }



  void setStateVector_vector(std::vector<double> vector) { _state_vector = vector; }

  std::vector<double> getStateVector_vector() const { return _state_vector; }


  void setCovariance_vector(std::vector<double> covariance) { _covariance = covariance; }

  std::vector<double> getCovariance_vector() const { return _covariance; }

 private:
  int _tracker;

  std::vector<double> _state_vector;

  std::vector<double> _covariance;

  int _algorithm;

  TRef* _pr_track;

  MAUS_VERSIONED_CLASS_DEF(SciFiSeed)
};

typedef std::vector<SciFiSeed*> SciFiSeedPArray;
typedef std::vector< std::vector<SciFiSeed*> > Seed2dPArray;
typedef std::vector<SciFiSeed> SciFiSeedArray;

} // ~namespace MAUS

#endif
