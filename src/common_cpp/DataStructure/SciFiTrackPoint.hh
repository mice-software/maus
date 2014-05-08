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

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/Recon/Kalman/KalmanState.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class SciFiTrackPoint {
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
  void set_f_chi2(double f_chi2)                { _f_chi2 = f_chi2;     }

  /** @brief  Sets the smoothed chi2 for this track point.
   */
  void set_s_chi2(double s_chi2)                { _s_chi2 = s_chi2;     }

  /** @brief  Sets the x coordinate.
   */
  void set_x(double x)                          { _x      = x;          }

  /** @brief  Sets the x momentum component.
   */
  void set_px(double px)                        { _px     = px;         }

  /** @brief  Sets the y coordinate.
   */
  void set_y(double y)                          { _y      = y;          }

  /** @brief  Sets the y momentum component.
   */
  void set_py(double py)                        { _py     = py;         }

  /** @brief  Sets the z momentum component.
   */
  void set_pz(double pz)                        { _pz     = pz;         }

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

  void set_mc_x(double mc_x)                    { _mc_x   = mc_x;       }
  void set_mc_px(double mc_px)                  { _mc_px  = mc_px;      }
  void set_mc_y(double mc_y)                    { _mc_y   = mc_y;       }
  void set_mc_py(double mc_py)                  { _mc_py  = mc_py;      }
  void set_mc_pz(double mc_pz)                  { _mc_pz  = mc_pz;      }

  /** @brief Set the mother cluster
   */
  void set_cluster(SciFiCluster* cluster) { _cluster = cluster; }

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
  double f_chi2()            const { return _f_chi2;   }

  /** @brief  Returns the smoothed chi2 value.
   */
  double s_chi2()            const { return _s_chi2;   }

  /** @brief  Returns the x position.
   */
  double x()                 const { return _x;        }

  /** @brief  Returns the x momentum component.
   */
  double px()                const { return _px;       }

  /** @brief  Returns the y position.
   */
  double y()                 const { return _y;        }

  /** @brief  Returns the y momentum component.
   */
  double py()                const { return _py;       }

  /** @brief  Returns the z momentum component.
   */
  double pz()                const { return _pz;       }

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

  double mc_x()              const { return _mc_x;     }
  double mc_px()             const { return _mc_px;    }
  double mc_y()              const { return _mc_y;     }
  double mc_py()             const { return _mc_py;    }
  double mc_pz()             const { return _mc_pz;    }

  int spill()                const { return _spill; }

  int event()                const { return _event; }

  /** @brief  Returns the mother cluster.
   */
  SciFiCluster* cluster()    const { return _cluster; }

 private:
  /** @brief The tracker the trackpoint belongs to.
   */
  int _spill;

  /** @brief The tracker the trackpoint belongs to.
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
  double _f_chi2;

  /** @brief smoothed chi2
   */
  double _s_chi2;

  /** @brief x position
   */
  double _x;

  /** @brief x momentum component
   */
  double _px;

  /** @brief y position
   */
  double _y;

  /** @brief y momentum component
   */
  double _py;

  /** @brief z momentum component
   */
  double _pz;

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

  /// The TRUTH state vector.
  double _mc_x;
  double _mc_px;
  double _mc_y;
  double _mc_py;
  double _mc_pz;

  /** @brief A pointer to the cluster used to form the state - does not assume control of memory
   */
  SciFiCluster* _cluster;

  MAUS_VERSIONED_CLASS_DEF(SciFiTrackPoint)
};

typedef std::vector<SciFiTrackPoint*> SciFiTrackPointPArray;

} // ~namespace MAUS

#endif
