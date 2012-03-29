#ifndef SRC_COMMON_CPP_RECONSTRUCTION_TRAJECTORY_FITTER_HH
#define SRC_COMMON_CPP_RECONSTRUCTION_TRAJECTORY_FITTER_HH

#include <vector>

#include "DetectorEvent.hh"
#include "ParticleTrajectory.hh"

namespace MAUS {

class OpticsModel;

class TrajectoryFitter
{
 public:
  virtual void Fit(std::vector<DetectorEvent const *> const * const detector_events
                   ParticleTrajectory * const trajectory) = 0;
 protected:
  OpticsModel const * optics_model_;
  std::vector<double> const * detector_planes_;
  std::vector<CovarianceMatrix> const * detector_uncertainties_;
};

}

#endif
