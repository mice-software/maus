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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFITRACKPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFITRACKPOINT_HH_

// C headers
#include <assert.h>
#include <iostream>
#include <cmath>

// C++ headers
#include <string>
#include <vector>
#include "TMath.h"
#include "TMatrixD.h"

// ROOT headers
#include "TObject.h"
#include "TRef.h"

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/Recon/Kalman/KalmanState.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class SciFiTrackPoint : public TObject {
 public:
  /** @brief  Constructor.
   */
  SciFiTrackPoint();

  /** @brief  Destructor.
   */
  virtual ~SciFiTrackPoint();

  /** @brief  Constructs a SciFiTrackPoint from a KalmanState.
   */
  explicit SciFiTrackPoint(const KalmanState *kalman_site);

  /** @brief  Copy constructor.
   */
  SciFiTrackPoint(const SciFiTrackPoint &site);

  /** @brief  Assignment operator.
   */
  SciFiTrackPoint& operator=(const SciFiTrackPoint& site);

  /** @brief  Sets spill number.
   */
  void set_spill(int spill)                     { _spill   = spill;   }

  /** @brief  Sets event number.
   */
  void set_event(int event)                     { _event   = event;   }

  /** @brief  Sets tracker number.
   */
  void set_tracker(int tracker)                 { _tracker = tracker; }

  /** @brief  Sets station number.
   */
  void set_station(int station)                 { _station = station; }

  /** @brief  Sets plane number.
   */
  void set_plane(int plane)                     { _plane   = plane;   }

  /** @brief  Sets channel number.
   */
  void set_channel(double channel)              { _channel = channel; }

  /** @brief  Sets the filtered chi2 for this track point.
   */
  void set_chi2(double chi2)                    { _chi2 = chi2;     }

  /** @brief  Sets the x coordinate.
   */
  void set_pos(ThreeVector pos)                 { _pos   = pos;      }

  /** @brief  Sets the x momentum component.
   */
  void set_mom(ThreeVector mom)                 { _mom  = mom;         }

  /** @brief  Sets the covariance matrix.
   */
  void set_covariance(std::vector<double> covariance)     { _covariance = covariance; }

  /** @brief  Sets pull (residual of the projected state).
   */
  void set_pull(double pull)                    { _pull   = pull;       }

  /** @brief  Sets residual for the filtered state.
   */
  void set_residual(double residual)            { _residual = residual; }

  /** @brief  Sets residual for the smoothed state.
   */
  void set_smoothed_residual(double s_residual) { _smoothed_residual = s_residual; }

  /** @brief  Returns the tracker number.
   */
  int tracker()              const { return _tracker;  }

  /** @brief  Returns the station number.
   */
  int station()              const { return _station;  }

  /** @brief  Returns the plane number.
   */
  int plane()                const { return _plane;    }

  /** @brief  Returns the channel number.
   */
  double channel()           const { return _channel;  }

  /** @brief  Returns filtered chi2 value.
   */
  double chi2()            const { return _chi2;   }

  /** @brief  Returns the x position.
   */
  ThreeVector pos()                 const { return _pos; }

  /** @brief  Returns the x momentum component.
   */
  ThreeVector mom()                const { return _mom;       }

  /** @brief  Returns the covariance matrix.
   */
  std::vector<double> covariance()        const { return _covariance; }

  /** @brief  Returns the residual of the projected state.
   */
  double pull()              const { return _pull;     }

  /** @brief  Returns the residual of the filtered state.
   */
  double residual()          const { return _residual; }

  /** @brief  Returns the residual of the smoothed state.
   */
  double smoothed_residual() const { return _smoothed_residual; }

  /** @brief  Returns the spill number.
   */
  int spill()                const { return _spill; }

  /** @brief  Returns the event number.
   */
  int event()                const { return _event; }

  /** @brief  Returns the mother clusters as a TRef*.
   */
  TRef* get_cluster() const { return _cluster; }

   /** @brief Set the mother cluster using a TRef*.
   */
  void set_cluster(TRef* const cluster) { _cluster = cluster; }


  /** @brief  Returns the mother cluster as a TObject pointer
   */
  TObject* get_cluster_tobject() const { return _cluster->GetObject(); }

  /** @brief Set the mother cluster using a SciFiCluster*.
   */
  void set_cluster_tobject(TObject* const cluster) { *_cluster = cluster; }

  /** @brief Set the mother cluster using a SciFiCluster*.
   */
  void set_cluster_pointer(SciFiCluster* const cluster) { *_cluster = cluster; }

  /** @brief  Returns the mother clusters as an array of pointers.
   */
  SciFiCluster* get_cluster_pointer() const
                { return static_cast<SciFiCluster*>(_cluster->GetObject()); }

 private:
  /** @brief The number of the spill the trackpoint belongs to.
   */
  int _spill;

  /** @brief The number of the event the trackpoint belongs to.
   */
  int _event;

  /** @brief The tracker the trackpoint belongs to.
   */
  int _tracker;

  /** @brief The station the trackpoint belongs to.
   */
  int _station;

  /** @brief The plane the trackpoint belongs to.
   */
  int _plane;

  /** @brief The channel measurement corresponding to the trackpoint.
   */
  double _channel;

  /** @brief filtered chi2
   */
  double _chi2;

  /** @brief position
   */
  ThreeVector _pos;

  /** @brief momentum
   */
  ThreeVector _mom;

  /** @brief Covariance matrix for the state vector [x, px, y, py, pz]
   */
  std::vector<double> _covariance;

  /** @brief projected residual
   */
  double _pull;

  /** @brief filtered residual
   */
  double _residual;

  /** @brief smoothed residual
   */
  double _smoothed_residual;

  /** @brief A pointer to the cluster used to form the state - does not assume control of memory
   */
  TRef* _cluster;

  MAUS_VERSIONED_CLASS_DEF(SciFiTrackPoint)
};

typedef std::vector<SciFiTrackPoint*> SciFiTrackPointPArray;

} // ~namespace MAUS

#endif
