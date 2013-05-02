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

// C++ headers
#include <string>
#include <vector>
#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class SciFiTrackPoint {
 public:
  SciFiTrackPoint();

  virtual ~SciFiTrackPoint();

  explicit SciFiTrackPoint(const KalmanSite *kalman_site);

  SciFiTrackPoint(const SciFiTrackPoint &site);

  SciFiTrackPoint& operator=(const SciFiTrackPoint& site);

  void set_id(int id)                           { _id     = id;         }
  void set_f_chi2(double f_chi2)                { _f_chi2 = f_chi2;     }
  void set_s_chi2(double s_chi2)                { _s_chi2 = s_chi2;     }
  void set_x(double x)                          { _x      = x;          }
  void set_px(double px)                        { _px     = px;         }
  void set_y(double y)                          { _y      = y;          }
  void set_py(double py)                        { _py     = py;         }
  void set_mc_x(double mc_x)                    { _mc_x   = mc_x;       }
  void set_mc_px(double mc_px)                  { _mc_px  = mc_px;      }
  void set_mc_y(double mc_y)                    { _mc_y   = mc_y;       }
  void set_mc_py(double mc_py)                  { _mc_py  = mc_py;      }
  void set_pull(double pull)                    { _pull   = pull;       }
  void set_residual(double residual)            { _residual = residual; }
  void set_smoothed_residual(double s_residual) { _smoothed_residual = s_residual; }

  int id()                   const { return _id;       }
  double f_chi2()            const { return _f_chi2;   }
  double s_chi2()            const { return _s_chi2;   }
  double x()                 const { return _x;        }
  double px()                const { return _px;       }
  double y()                 const { return _y;        }
  double py()                const { return _py;       }
  double mc_x()              const { return _mc_x;     }
  double mc_px()             const { return _mc_px;    }
  double mc_y()              const { return _mc_y;     }
  double mc_py()             const { return _mc_py;    }
  double pull()              const { return _pull;     }
  double residual()          const { return _residual; }
  double smoothed_residual() const { return _smoothed_residual; }

 private:
  /// Site id.
  int _id;
  /// The Chi2 at this site.
  double _f_chi2;
  double _s_chi2;

  /// The state vector.
  double _x;
  double _px;
  double _y;
  double _py;
  /// The TRUTH state vector.
  double _mc_x;
  double _mc_px;
  double _mc_y;
  double _mc_py;

  /// The residuals.
  double _pull;
  double _residual;
  double _smoothed_residual;

  MAUS_VERSIONED_CLASS_DEF(SciFiTrackPoint)
};

typedef std::vector<SciFiTrackPoint*> SciFiTrackPointPArray;

} // ~namespace MAUS

#endif
